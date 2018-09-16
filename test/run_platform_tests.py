#!/usr/bin/env python3

#  libfoxenstack -- Library for switching user-space stacks
#  Copyright (C) 2018  Andreas Stöckel
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Affero General Public License as
#  published by the Free Software Foundation, either version 3 of the
#  License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Affero General Public License for more details.
#
#  You should have received a copy of the GNU Affero General Public License
#  along with this program.  If not, see <https://www.gnu.org/licenses/>.

"""
This Python script implements a test runner that compiles libfoxenstack for
various target platform and executes them via qemu.
"""

import copy
import os
import subprocess
import sys
import tempfile

################################################################################
# COLOURS                                                                      #
################################################################################

ANSI_RED = "\33[38;5;166;1m"
ANSI_GREEN = "\33[38;5;40;1m"
ANSI_ORANGE = "\33[38;5;220;1m"
ANSI_RESET = "\33[0m"

################################################################################
# PLATFORM AND COMPILER FLAG DEFINITIONS                                       #
################################################################################

#
# Define all compilers and platforms that should be tested here
#

platforms = {
    "x86_64-gnu": {
        "compilers": {
            "c": ["gcc", "clang"],
            "cpp": ["g++", "clang++"]
        },
        "flags": [],
        "wrapper": None
    },
    "x86-mlib-gnu": {
        "compilers": {
            "c": ["gcc", "clang"],
            "cpp": ["g++", "clang++"]
        },
        "flags":
        [["-m32"], ["-I/usr/i686-linux-gnu/include/c++/8/i686-linux-gnu"],
         ["-I/usr/i686-linux-gnu/include"]],
        "wrapper":
        None
    },
    "i386-gnu": {
        "compilers": {
            "c": ["i686-linux-gnu-gcc"],
            "cpp": ["i686-linux-gnu-g++"]
        },
        "flags": [[None, "-march=i386"]],
        "wrapper": "qemu-i386-static"
    },
    "arm-gnueabi": {
        "compilers": {
            "c": ["arm-linux-gnueabi-gcc"],
            "cpp": ["arm-linux-gnueabi-g++"]
        },
        "flags": [[None, "-march=armv6", "-march=armv7", "-march=armv8-a"]],
        "wrapper": "qemu-arm-static"
    },
    "arm-gnueabihf": {
        "compilers": {
            "c": ["arm-linux-gnueabihf-gcc"],
            "cpp": ["arm-linux-gnueabihf-g++"]
        },
        "flags": [[None, "-march=armv7", "-march=armv8-a"]],
        "wrapper": "qemu-arm-static"
    },
}

#
# Compiler flags. If multiple flags are specified within a sub-list, all
# combinations of these flags with other flags will be tested.
#
flags = [["-static"], ["-DFX_NO_CONFIG"], ["-I/usr/include/valgrind"],
         ["-I/usr/local/include"], ["-I."], [None, "-DFX_WITH_VALGRIND"],
         ["-O0", "-O3"]]

################################################################################
# COMPILER SANITY TEST PROGRAMS                                                #
################################################################################

# These programs are used to test the compiler/emulator toolchain. Before
# executing any of the "real" tests for the compiler/language/flags combination
# this code tries to compile and execute the below programs. The programs should
# output "Hello World".

test_programs = {
    "c":
    """
#include <stdio.h>

int main() {
	printf("Hello World!");
	return 0;
}
""",
    "cpp":
    """
#include <iostream>

int main() {
	std::cout << "Hello World!";
	return 0;
}
"""
}

################################################################################
# TARGET DEFINITIONS                                                           #
################################################################################

targets = {
    "c": {
        "files": [
            ["foxen/stack.c", "test/test_stack.c"],
        ],
        "flags": []
    },
    "cpp": {
        "files": [
            ["foxen/stack.cpp", "test/test_stack.c"],
            ["foxen/stack.cpp", "test/test_stack_cpp.cpp"],
        ],
        "flags": [["-DFX_WITH_CPP_EXCEPTIONS"]]
    },
}

################################################################################
# HELPER FUNCTIONS                                                             #
################################################################################


def gen_flag_combinations(flags):
    n = len(flags)
    idcs = [0] * n
    while True:
        # Generate the current flag combination, ignore "None" entries
        res = map(lambda p: flags[p[0]][p[1]], zip(range(n), idcs))
        yield list(filter(lambda x: not x is None, res))

        # Go to the next index combination
        for i in range(len(idcs)):
            idcs[i] += 1
            if idcs[i] < len(flags[i]):
                break
            idcs[i] = 0
        if not any(idcs):
            return  # All elements were reset to zero again! We're done


def compile_and_run_executable(compiler,
                               files,
                               flags=None,
                               output=None,
                               wrapper=None,
                               info=None):
    # Use sane defaults
    if flags is None:
        flags = []
    if output is None:
        output = tempfile.mktemp()
    if info is None:
        info = {}

    # Compile the executable
    info["compile_cmd"] = [compiler] + flags + files + ["-o", output]
    p = subprocess.Popen(
        info["compile_cmd"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = p.communicate()
    if p.returncode != 0:
        raise Exception(
            "Error while invoking the compiler, returncode={}, stderr=\"{}\"".
            format(p.returncode, str(stderr, "utf-8")))

    # Execute the executable
    if wrapper is None:
        wrapper = []
    else:
        wrapper = [wrapper]

    info["execute_cmd"] = wrapper + [output]
    p = subprocess.Popen(
        info["execute_cmd"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = p.communicate()
    if p.returncode != 0:
        raise Exception(
            "Error while executing the program, returncode={}, stderr=\"{}\"".
            format(p.returncode,
                   str(stderr, "utf-8"), " ".join(info["compile_cmd"])))

    # Delete the temporary output file
    if os.path.exists(output):
        os.unlink(output)

    return str(stdout, "utf-8"), str(stderr, "utf-8")


def compile_and_run_test_program(compiler,
                                 prog_str,
                                 lang,
                                 flags=None,
                                 output=None,
                                 wrapper=None,
                                 info=None):
    # Write the program to a temporary file and compile it
    with tempfile.NamedTemporaryFile(suffix="." + lang) as f:
        f.write(prog_str.encode("utf-8"))
        f.seek(0)
        f.flush()
        return compile_and_run_executable(
            compiler, [f.name],
            flags=flags,
            output=output,
            wrapper=wrapper,
            info=info)


def gen_execution_plans():
    """
    Iterates over all the platforms and targets and generates a flat list with
    all the tests that are being performed.
    """
    for lang, defn in targets.items():
        # Fetch language-specific flags
        flags_target = defn["flags"]

        # Fetch the programs that should be compiled
        for files in defn["files"]:
            # Iterate over all target platforms
            for platform_name, platform in platforms.items():
                # Fetch the execution wrapper
                wrapper = platform["wrapper"]

                # Add the architectures to the flags
                flags_platform = platform["flags"]

                # Iterate over the compilers for each platform
                for compiler in platform["compilers"][lang]:
                    # Iterate over all compinations of flags
                    for flags_ in gen_flag_combinations(
                            flags + flags_target + flags_platform):
                        yield {
                            "lang": lang,
                            "compiler": compiler,
                            "platform": platform_name,
                            "flags": flags_,
                            "wrapper": wrapper,
                            "files": files
                        }


def get_checkmark(ok, known=True):
    if ok:
        return ANSI_GREEN + "✔" + ANSI_RESET
    if not known:
        return ANSI_ORANGE + "?" + ANSI_RESET
    return ANSI_RED + "✖" + ANSI_RESET


def execute_plan(plan):
    # Fetch the execution plan
    platform_name = plan["platform"]
    lang = plan["lang"]
    compiler = plan["compiler"]
    flags_ = plan["flags"]
    wrapper = plan["wrapper"]
    files = plan["files"]

    # Prepare the information structure
    info = {"config": plan}

    # Make sure we can execute a dummy program with the given flags
    def run():
        try:
            stdout, _ = compile_and_run_test_program(
                compiler=compiler,
                prog_str=test_programs[lang],
                lang=lang,
                flags=flags_,
                wrapper=wrapper,
                info=info)
            if stdout != "Hello World!":
                raise Exception("Sanity test program output mismatch")
        except Exception as e:
            return plan, False, False, str(e)

        # Execute the actual program
        try:
            compile_and_run_executable(
                compiler=compiler,
                files=files,
                flags=flags_,
                wrapper=wrapper,
                info=info)
        except Exception as e:
            return plan, True, False, str(e)

        return plan, True, True, None

    plan, success_sanity_check, success_test, msg = run()

    # Print a real-time message
    colours = [ANSI_ORANGE, ANSI_RED, ANSI_GREEN]

    print("{:15} [{:3}] {:25} {:5} {:5} {}".format(
        platform_name.upper(),
        lang.upper(),
        compiler.upper(),
        get_checkmark(success_sanity_check),
        get_checkmark(success_test, success_sanity_check), flags_))

    info["success_sanity_check"] = success_sanity_check
    info["success_test"] = success_test
    info["msg"] = msg

    return info


# Run all tests in parallel
import multiprocessing
p = multiprocessing.Pool(multiprocessing.cpu_count())
res = p.map(execute_plan, gen_execution_plans())

# Count the number of failed/successful tests
n_total = len(res)
n_failed_sanity = 0
n_failed = 0
for r in res:
    if not r["success_sanity_check"]:
        n_failed_sanity += 1
    elif not r["success_test"]:
        n_failed += 1

# All tests passed, print a success message
if n_failed == 0 and n_failed_sanity == 0:
    print(
        "{}[OK ]{} Success! All tests passed.".format(ANSI_GREEN, ANSI_RESET))
    sys.exit(0)

import json
from datetime import datetime
fn = datetime.strftime(datetime.now(), "test_results_%Y-%m-%d-%H-%M-%S.json")
with open(fn, 'w') as f:
    json.dump(res, f, sort_keys=True, indent=4)
print("{}[---]{} Wrote test information to {}".format(ANSI_ORANGE, ANSI_RESET,
                                                      fn))

if n_failed == 0 and n_failed_sanity < n_total:
    print("{}[OK ]{} All {} tests passing the sanity check were successful.".
          format(ANSI_GREEN, ANSI_RESET, n_total - n_failed_sanity))

# No tests really failed, but some tests did not pass the sanity check, i.e. the
# platform or compiler are not setup correctly
if n_failed == 0:
    print(
        "{}[WRN]{} {} test(s) failed the sanity check (platforms not setup correctly?)".
        format(ANSI_ORANGE, ANSI_RESET, n_failed_sanity))
    sys.exit(1)

# Tests failed.
print(
    "{}[ERR]{} {} out of {} tests failed. {} test(s) failed the sanity check".
    format(ANSI_RED, ANSI_RESET, n_failed, n_total - n_failed_sanity,
           n_failed_sanity))
sys.exit(1)

