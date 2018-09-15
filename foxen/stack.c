/*
 *  libfoxenstack -- Library for switching user-space stacks
 *  Copyright (C) 2018  Andreas Stöckel
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include <stdint.h>

#ifndef FX_NO_CONFIG
#include "config.h"
#endif

#include <foxen/stack.h>

/* Include the platform-specific _fx_stack_switch function */
#include "platform/stack_platformselect.h"

/*****************************************************************************
 * Support for C++ exception propagation                                     *
 *****************************************************************************/

#ifdef FX_WITH_CPP_EXCEPTIONS

#include <exception>

/**
 * Data passed to the _fx_stack_exception_stub function. Contains the
 * user-defined data originally passed to fx_stack_switch as well as
 * data structures used for exception management.
 */
typedef struct {
	/**
	 * Original user-provided callback passed to fx_stack_switch().
	 */
	fx_stack_cback cback;

	/**
	 * Original user-provided data to be passed to the callback function.
	 */
	void *data;

	/**
	 * Exception-pointer instance holding any exception that may be thrown in
	 * the user-defined function. Note that std::exception_ptr essentially is
	 * a std::unique_ptr; this allows to transport the exception outside of the
	 * original scope and to rethrow it.
	 */
	std::exception_ptr eptr;
} fx_stack_exception_stub_data_t;

static __attribute__((noinline)) void *_fx_stack_exception_stub(void *data_) {
	/* Type-convert the callback data. */
	fx_stack_exception_stub_data_t *data =
	    (fx_stack_exception_stub_data_t *)data_;

	/* Call the callback within an explicit try-catch block. This will make sure
	   that exceptions are caught, even on weird architectures such as ARM which
	   do special exception handling voodoo. */
	try {
		return data->cback(data->data);
	} catch (...) {
		/* There was an exception! Move the exception into the exception_ptr
		   instance. The calling code rethrows the exception. */
		data->eptr = std::current_exception();
	}
	return nullptr;
}

#endif /* FX_WITH_CPP_EXCEPTIONS */

/*****************************************************************************
 * Common stack switching code                                               *
 *****************************************************************************/

/*
 * Define valgrind-related macros
 */
#ifdef FX_WITH_VALGRIND

#include <valgrind.h>

#define FX_VALGRIND_STACK_REGISTER                                    \
	/* Inform valgrind that the given memory region is a new stack */ \
	const int stack_id = VALGRIND_STACK_REGISTER(stack_start, stack_end);

#define FX_VALGRIND_STACK_UNREGISTER                                  \
	/* Inform valgrind that the given memory region is a new stack */ \
	VALGRIND_STACK_DEREGISTER(stack_id);

#else /* FX_WITH_VALGRIND */

#define FX_VALGRIND_STACK_REGISTER
#define FX_VALGRIND_STACK_UNREGISTER

#endif /* FX_WITH_VALGRIND */

void *fx_stack_switch(void *stack_start, void *stack_end, void *stack_ptr,
                      fx_stack_cback cback, void *data) {
	/* Make sure the given stack pointer is sane */
	assert((stack_start < stack_ptr) && (stack_ptr <= stack_end));

#ifdef FX_WITH_CPP_EXCEPTIONS
	/* Call the platform-specific assembly function wrapped in the given
	   stub function */
	fx_stack_exception_stub_data_t stub_data{cback, data, nullptr};

	FX_VALGRIND_STACK_REGISTER
	void *result =
	    _fx_stack_switch(stack_ptr, _fx_stack_exception_stub, &stub_data);
	FX_VALGRIND_STACK_UNREGISTER

	/* Re-throw any C++ exception that was thrown by the code inside the
	   alternative stack. */
	if (stub_data.eptr) {
		std::rethrow_exception(stub_data.eptr);
	}
#else  /* FX_WITH_CPP_EXCEPTIONS */
	/* Call the platform-specific assembly function */
	FX_VALGRIND_STACK_REGISTER
	void *result = _fx_stack_switch(stack_ptr, cback, data);
	FX_VALGRIND_STACK_UNREGISTER
#endif /* FX_WITH_CPP_EXCEPTIONS */

	return result;
}

