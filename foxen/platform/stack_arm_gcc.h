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

__attribute__((noinline)) static void *_fx_stack_switch(void *stack_ptr,
                                                        fx_stack_cback cback,
                                                        void *data) {
	void *result;

	__asm__ __volatile__(
	    /* Store the stack pointer register in r4 and load the stack pointer
	       from stack_ptr. */
	    "mov %%r4, %%sp\n\t"
	    "mov %%sp, %1\n\t"

	    /* Call the callback function. r0 = callback, r1 = data */
	    "mov %%r0, %2\n\t"
	    "mov %%r1, %3\n\t"
	    "blx %%r1\n\t"

	    /* Write r0 to the target register */
	    "mov %0, %%r0\n\t"

	    /* Restore the original stack pointer */
	    "mov %%sp, %%r4\n\t"

	    : "=r"(result)
	    : "r"(stack_ptr), "r"(data), "r"(cback)
	    : "memory", "r0", "r1", "r2", "r3", "r4");

	return result;
}

