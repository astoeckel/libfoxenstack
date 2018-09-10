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

#ifndef FX_FOXEN_STACK_H
#define FX_FOXEN_STACK_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Function pointer type used for the callback function passed to
 * fx_stack_switch().
 *
 * @param data is a user-defined pointer.
 * @return an arbitrary user-defined value that will be returned from
 * fx_stack_switch().
 */
typedef void *(*fx_stack_cback)(void *data);

/**
 * Executes the function "cback" on a separate stack. Note that `stack_start`
 * and `stack_end` are ignored on most platforms. However, these pointers will
 * be used to inform valgrind that this memory region is indeed a stack (if
 * valgrind support is compiled in by setting the `with_valgrind` flag in meson
 * and/or defining the FX_WITH_VALGRIND pre-processor flag.). Furthermore, there
 * is an assertion in place that makes sure that
 * `stack_start < stack_ptr <= stack_end`.
 *
 * @param stack_start is the low-address of the memory region that should be
 * used as a stack.
 * @param stack_end is the high-address of the memory region that should be used
 * as a stack.
 * @param stack_ptr is a the actual memory address at which the called function
 * should be executed. Normally you want to set this to stack_end.
 * @param cback is the callback function that should be executed within the new
 * stack.
 * @param data is a user-defined pointer that should be passed to the callback
 * function.
 * @return the return value returned by the callback function.
 */
void *fx_stack_switch(void *stack_start, void *stack_end, void *stack_ptr,
                      fx_stack_cback cback, void *data);

#ifdef __cplusplus
}
#endif

#endif /* FX_FOXEN_STACK_H */
