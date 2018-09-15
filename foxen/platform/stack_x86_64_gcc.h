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
	    /* Store stack_ptr in ebx */
	    "mov %1, %%rbx\n\t"

	    /* Exchange rbx and rsp, effectively making stack_ptr the new stack
	       pointer. */
	    "xchg %%rsp, %%rbx\n\t"

	    /* Pass "data" to the function as an argument */
	    "mov %2, %%rdi\n\t"

	    /* Call the function "cback" */
	    "call *%3\n\t"

	    /* Backup the return value in "result" */
	    "mov %%rax, %0\n\t"

	    /* Restore the original stack pointer */
	    "mov %%rbx, %%rsp\n\t"
	    : "=r"(result)
	    : "r"(stack_ptr), "r"(data), "r"(cback)
	    : "memory", "rax", "rbx", "rdi", "rsp");

	return result;
}

