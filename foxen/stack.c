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

#include "config.h"
#ifdef FX_WITH_VALGRIND
#include <valgrind.h>
#endif

#include <foxen/stack.h>

/* Include the platform-specific _fx_stack_switch function */
#include "platform/stack_platformselect.h"

void *fx_stack_switch(void *stack_start, void *stack_end, void *stack_ptr,
                      fx_stack_cback cback, void *data)
{
	/* Make sure the given stack pointer is sane */
	assert((stack_start < stack_ptr) && (stack_ptr <= stack_end));

#ifdef FX_WITH_VALGRIND
	/* Inform valgrind that the given memory region is a new stack */
	const int stack_id = VALGRIND_STACK_REGISTER(stack_start, stack_end);
#endif

	/* Call the platform-specific assembly function */
	void *result =
	    _fx_stack_switch(stack_start, stack_end, stack_ptr, cback, data);

#ifdef FX_WITH_VALGRIND
	/* Inform valgrind that this memory region is no longer used as a stack */
	VALGRIND_STACK_DEREGISTER(stack_id);
#endif

	return result;
}

