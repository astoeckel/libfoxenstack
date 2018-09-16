# libfoxenstack ― Library for switching user-space stacks

[![Build Status](https://travis-ci.org/astoeckel/libfoxenstack.svg?branch=master)](https://travis-ci.org/astoeckel/libfoxenstack)

`libfoxenstack` provides a convenient C API for executing functions in the context of
a different stack. This is particularly useful for user-space threading
libraries.

## Features

* *Multi-platform.*
  Supports x86_64, i386 and ARM32 (tested on ARMv6 upwards). Patches for other platforms are welcome!
* *C++ exception support.*
  Correctly propagates C++ exceptions across stack boundaries―even on ARM.
  C++ exception support can be deactivated for a smaller footprint; use
  `meson configure -Dwith_cpp_exceptions=false`. Note that C++ exception
  propagation works just fine on x86 without explicit support.
* *Support for `valgrind`.* If so desired, `libfoxenstack` can inform valgrind about
  the new stack memory region. Activate with
  `meson configure -Dwith_valgrind=true`.
* *Thoroughly tested.* Comes with a CI script that tests the code in emulation on
  hundreds of combinations of compilers, CPU architecture versions, and
  configuration flags.

## How to use

Just call `fx_stack_switch` with the desired stack address range and a pointer
at the function that should be executed within the new stack.

```c
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <foxen/stack.h>

/* Number of bytes to use for the stack */
#define STACK_LEN 4096U

/* Callback function that is running inside the new stack */
static void *cback(void *data) {
	int b;
	printf("Hello World from another stack at address %p!\n", &b);
	printf("Given data pointer is %p.\n", data);
	return (void *)0xDEADBEAFU;
}

int main() {
	/* Allocate memory for the stack */
	void *stack_start = malloc(STACK_LEN);
	void *stack_end = (void *)((uintptr_t)stack_start + STACK_LEN);

	/* Start at the end of the stack */
	void *stack_ptr = stack_end;

	/* Call fx_stack_switch; pass a pointer to "a" as user-defined data to
	   the callback function cback. */
	int a;
	void *res = fx_stack_switch(stack_start, stack_end, stack_ptr, cback, &a);

	/* Free the reserved memory */
	free(stack_start);
}
```

 For now, supported platforms are Linux (i386, x86_64, ARM32).

Patches for other platforms are welcome.

## How to compile

`libfoxenstack` uses `meson` for compilation, which can be installed using `pip` and depends on the `ninja` build system.

```
git clone https://github.com/astoeckel/libfoxenstack
cd libfoxenstack
mkdir build
cd build
meson ..
ninja
```

Alternatively you can define the `FX_NO_CONFIG` compiler flag and just add
`foxen/stack.c` to your project.

## Unit tests

Execute `ninja test` to run the unit tests for your current CPU architecture.

To run the unit-tests thoroughly on all supported platforms, execute
`test/run_platform_tests.py` from the project root directory. This script
builds and executes the unit tests in emulation for a variety of target
architectures.

If you do not want spend ages to install a bunch of cross-compilers, you can use
the provided `Dockerfile` to run `test/run_platform_tests.py` in a stable
environment. Just run
```
docker build -t libfoxenstack .
docker run -it libfoxenstack
```

## FAQ about the *Foxen* series of C libraries

**Q: What's with the name?**

**A:** [*Foxen*](http://kingkiller.wikia.com/wiki/Foxen) is a mysterious glowing object guiding Auri through the catacumbal “Underthing”. The *Foxen* software libraries are similar in key aspects: mysterious and catacumbal. Probably less useful than an eternal sympathy lamp though.

**Q: What is the purpose and goal of these libraries?**

**A:** The *Foxen* libraries are extremely small C libraries that rely on the [Meson](https://mesonbuild.com/) build system for dependency management. One common feature is that the libraries do not use [heap memory allocations](https://github.com/astoeckel/libfoxenmem).

**Q: Why?**

**A:** Excellent question! The author mainly created these libraries because he grew tired of copying his own source code files between projects all the time.

**Q: Would you recommend to use these libraries in my project?**

**A:** That depends. Some of the code is fairly specialized according to my own needs and might not be intended to be general. If what you are going to use these libraries for aligns with their original purpose, then sure, go ahead. Otherwise, I'd probably advise against using these libraries, and as explained below, I'm not super open to expanding their scope.

**Q: Can you licence these libraries under a something more permissive than AGPLv3?**

**A:** Maybe, if you ask nicely. I'm not a fan of giving my work away “for free” (i.e., allowing inclusion of my code in commercial or otherwise proprietary software) without getting something back (in particular public access to the source code of the things other people built with it). That being said, some of the `foxen` libraries may be too trivial to warrant the use of a strong copyleft licence. Correspondingly, I might reconsider this decision for individual libraries. See “[Why you shouldn't use the Lesser GPL for your next library](https://www.gnu.org/licenses/why-not-lgpl.en.html)” for more info.

**Q: Can I contribute?**

**A:** Sure! Feel free to open an issue or a PR. However, be warned that since I've mainly developed these libraries for use in my own stuff, I might be a little picky about what I'm going to include and what not.

## Licence

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.
