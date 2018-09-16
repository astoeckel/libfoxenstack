#  libfoxenunit -- Minimal unit test library for C
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


# This Dockerfile describes a docker container that runs the
# "test/run_platform_tests.py" script in a Debian environment with
# cross-compilers and system emulators installed. The container
# is used for continuous integration testing on Travis CI.

FROM debian:sid-slim

#
# Installation steps
#

RUN apt-get update

# Utilities: Python and curl
RUN apt-get -y install \
	python3-minimal curl tar bzip2

# libfoxenunit
RUN mkdir /usr/local/include/foxen \
	&& curl https://raw.githubusercontent.com/astoeckel/libfoxenunit/master/foxen/unittest.h \
	 > /usr/local/include/foxen/unittest.h

# valgrind.h
RUN mkdir /usr/include/valgrind \
	&& cd /tmp \
	&& curl ftp://sourceware.org/pub/valgrind/valgrind-3.13.0.tar.bz2 \
	 > valgrind.tar.bz2 \
	&& tar -xf valgrind.tar.bz2 \
	&& install valgrind-3.13.0/include/valgrind.h /usr/include/valgrind/valgrind.h

# Qemu
RUN apt-get -y install \
	qemu-user-static

# Default C++/C compilers (x86_64)
RUN apt-get -y install \
	gcc \
	g++ \
	clang

# Multilib version of the i686 platform
RUN apt-get -y install \
	libc6-dev-i386 \
	gcc-multilib \
	g++-multilib

# Cross-compiler for ARM without hardfloat
RUN apt-get -y install \
	gcc-arm-linux-gnueabi \
	g++-arm-linux-gnueabi

# Cross-compiler for ARM with hardfloat
RUN apt-get -y install \
	gcc-arm-linux-gnueabihf \
	g++-arm-linux-gnueabihf

# Cross-compiler for i686 (x86)
RUN apt-get -y install \
	gcc-i686-linux-gnu \
	g++-i686-linux-gnu

# Setup the locale
RUN apt-get -y install locales \
	&& echo "en_US.UTF-8 UTF-8" > /etc/locale.gen \
	&& locale-gen

#
# Run the actual unit test file
#

ENV LANG en_US.UTF-8
ENV LANGUAGE en_US:en
ENV LC_ALL en_US.UTF-8
COPY . /app
CMD cd /app && python3 /app/test/run_platform_tests.py
