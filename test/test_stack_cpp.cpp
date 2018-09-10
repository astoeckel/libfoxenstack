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

#include <stdexcept>

#include <stdint.h>
#include <stdlib.h>

/******************************************************************************
 * UNITTESTS                                                                  *
 ******************************************************************************/

#define STACK_LEN 4096U

namespace {

class TestSimpleCbackUnwind {
private:
	int *m_data;

public:
	TestSimpleCbackUnwind(int *data) : m_data(data) {}

	~TestSimpleCbackUnwind() { *((int *)m_data) *= 12; }
};

}  // namespace

static void *test_simple_cback(void *data) {
	TestSimpleCbackUnwind obj((int *)data);
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

static void *test_exception_cback(void *data) {
	TestSimpleCbackUnwind obj((int *)data);
	throw std::runtime_error("foobar");
	return (void *)0xAFFEAFFEU;
}

static void test_exception(void) {
	void *stack_start = malloc(STACK_LEN);
	void *stack_end = (void *)((uintptr_t)stack_start + STACK_LEN);

	int data = 4813;
	bool did_catch = false;
	try {
		fx_stack_switch(stack_start, stack_end, stack_end, test_exception_cback,
		                &data);
	} catch (std::runtime_error &e) {
		EXPECT_TRUE(std::string(e.what()) == "foobar");
		did_catch = true;
	}
	EXPECT_TRUE(did_catch);
	EXPECT_EQ(57756, data);

	free(stack_start);
}

/******************************************************************************
 * MAIN                                                                       *
 ******************************************************************************/

int main() {
	RUN(test_simple);
	RUN(test_exception);
	DONE;
}

