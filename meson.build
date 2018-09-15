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

project(
    'libfoxenstack', ['c'],
    default_options : ['c_std=c99', 'cpp_std=c++11', 'b_ndebug=if-release'])

compiler = meson.get_compiler('c')
if compiler.get_id() == 'gcc' or compiler.get_id() == 'clang'
    add_global_arguments('-Wc++-compat', language : 'c')
endif

# Configuration
conf_data = configuration_data()
conf_data.set('FX_WITH_VALGRIND', get_option('with_valgrind'))
conf_data.set('FX_WITH_CPP_EXCEPTIONS', get_option('with_cpp_exceptions'))
configure_file(input : 'config.h.in',
               output : 'config.h',
               configuration : conf_data)

# Include directory
inc_foxen = include_directories('.')

# Add the search path for the valgrind headers if requested
dep_valgrind = []
if get_option('with_valgrind')
    dep_valgrind = dependency('valgrind', required: true)
endif

# Either compile the code as C++ or C, depending on the with_cpp_exceptions
# flag
if get_option('with_cpp_exceptions')
    add_languages('cpp')
    lib_foxenstack_src = ['foxen/stack.cpp']
else
    lib_foxenstack_src = ['foxen/stack.c']
endif

# Define the contents of the actual library
lib_foxenstack = library(
    'foxenstack',
    lib_foxenstack_src,
    include_directories: inc_foxen,
    dependencies: [dep_valgrind],
    install: true)

# Compile and register the unit tests
dep_foxenunit = dependency(
    'libfoxenunit',
    fallback:['libfoxenunit', 'dep_foxenunit'])
exe_test_stack = executable(
    'test_stack',
    'test/test_stack.c',
    include_directories: inc_foxen,
    link_with: lib_foxenstack,
    dependencies: dep_foxenunit,
    install: false)
test('test_stack', exe_test_stack)

# Compile the C++-specific unit tests if C++ support is enabled
if get_option('with_cpp_exceptions')
    exe_test_stack_cpp = executable(
        'test_stack_cpp',
        'test/test_stack_cpp.cpp',
        include_directories: inc_foxen,
        link_with: lib_foxenstack,
        dependencies: dep_foxenunit,
        install: false)
    test('test_stack_cpp', exe_test_stack_cpp)
endif

# Install the header file
install_headers(
    ['foxen/stack.h'],
    subdir: 'foxen')

# Generate a Pkg config file
pkg = import('pkgconfig')
pkg.generate(
    libraries: [lib_foxenstack],
    subdirs: [],
    name: 'libfoxenstack',
    version: '1.0',
    filebase: 'libfoxenstack',
    description: 'Library for switching user-space stacks')

# Export the dependency
dep_foxenstack = declare_dependency(
    link_with: [lib_foxenstack],
    include_directories: inc_foxen)
