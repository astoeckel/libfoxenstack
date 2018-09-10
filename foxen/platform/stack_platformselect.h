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

#if   defined(__MINGW32__)
#include "stack_x86_gcc.h" /* gcc on X86 */
#elif defined(_M_IX86)
#error "Platform not supported yet!"
/*#include "stack_x86_msvc.h"*/ /* MS Visual Studio on X86 */
#elif defined(_M_X64)
#error "Platform not supported yet!"
/*#include "stack_x64_msvc.h"*/ /* MS Visual Studio on X64 */
#elif defined(__GNUC__) && defined(__amd64__)
#include "stack_x86_64_gcc.h" /* gcc on amd64 */
#elif defined(__GNUC__) && defined(__i386__)
#include "stack_x86_gcc.h" /* gcc on X86 */
#elif defined(__GNUC__) && defined(__arm__)
#include "stack_arm_gcc.h" /* gcc on arm */
#elif defined(__GNUC__) && defined(__PPC64__)
#error "Platform not supported yet!"
/*#include "stack_ppc64_gcc.h"*/ /* gcc on ppc64 */
#elif defined(__GNUC__) && defined(__mips__) && defined(_ABI64)
#error "Platform not supported yet!"
/*#include "stack_mips64_gcc.h"*/ /* gcc on mips64 */
#elif defined(__GNUC__) && defined(__s390x__)
#error "Platform not supported yet!"
/*#include "stack_s390x_gcc.h"*/
#else
#error "Unsupported platform!"
#endif
