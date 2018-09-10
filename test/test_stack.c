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

#include <foxen/stack.h>
#include <foxen/unittest.h>

#include <stdint.h>
#include <stdlib.h>

/******************************************************************************
 * UNITTESTS                                                                  *
 ******************************************************************************/

#define STACK_LEN (4096U * 4U)

/******************************************************************************
 * Unit test test_simple()                                                     *
 ******************************************************************************/

static void *test_simple_cback(void *data) {
	*((int *)data) *= 12;
	return (void *)0xAFFEAFFEU;
}

static void test_simple(void) {
	void *stack_start = malloc(STACK_LEN);
	void *stack_end = (void *)((uintptr_t)stack_start + STACK_LEN);

	int data = 4813;
	void *res = fx_stack_switch(stack_start, stack_end, stack_end,
	                            test_simple_cback, &data);
	EXPECT_EQ((void *)0xAFFEAFFEU, res);
	EXPECT_EQ(57756, data);

	free(stack_start);
}

/******************************************************************************
 * Unit test test_recursive()                                                 *
 ******************************************************************************/

static void _fib_recursive_priv(int n, int *ra, int *rb) {
	if (n <= 0) {
		*ra = 0;
		*rb = 0;
	} else if (n == 1) {
		*ra = 1;
		*rb = 0;
	} else {
		int a, b;
		_fib_recursive_priv(n - 1, &a, &b);
		*ra = a + b;
		*rb = a;
		printf(
		    "fib_(n=%2d) = %10d,    fib_(n=%2d) = %10d,    fib_(n=%2d) = "
		    "%10d\n",
		    n, *ra, n - 1, a, n - 2, b);
	}
}

static int _fib_recursive(int n) {
	int a, b;
	_fib_recursive_priv(n, &a, &b);
	return a;
}

void *test_recursive_cback(void *data) {
	(void)data;
	EXPECT_EQ(1134903170, _fib_recursive(45));
	return NULL;
}

static void test_recursive(void) {
	void *stack_start = malloc(STACK_LEN);
	void *stack_end = (void *)((uintptr_t)stack_start + STACK_LEN);

	EXPECT_EQ(NULL, fx_stack_switch(stack_start, stack_end, stack_end,
	                                test_recursive_cback, NULL));

	free(stack_start);
}

/******************************************************************************
 * Unit test test_check_addr_cback()                                          *
 ******************************************************************************/

static void *test_check_addr_cback(void *data) {
	(void)data;

	int a;
	volatile uintptr_t a_addr = (uintptr_t)(&a);
	return (void *)a_addr;
}

static void test_check_addr(void) {
	void *stack_start = malloc(STACK_LEN);
	void *stack_end = (void *)((uintptr_t)stack_start + STACK_LEN);

	void *res = fx_stack_switch(stack_start, stack_end, stack_end,
	                            test_check_addr_cback, NULL);

	/* Make sure the address of "a" in test_check_addr_cback() is located in
	   the memory region allocated above. */
	EXPECT_GT(res, stack_start);
	EXPECT_LE(res, stack_end);

	free(stack_start);
}

/******************************************************************************
 * MAIN                                                                       *
 ******************************************************************************/

int main() {
	RUN(test_simple);
	RUN(test_recursive);
	RUN(test_check_addr);
	DONE;
}

