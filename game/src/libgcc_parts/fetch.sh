#!/bin/sh

# A small text adventure.
#
# by Mibi88
#
# This software is licensed under the BSD-3-Clause license:
#
# Copyright 2025 Mibi88
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its
# contributors may be used to endorse or promote products derived from this
# software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

# Files to download
files=(divmod.c udivhi3.c udivmod.c udivmodhi4.c udivmodsi4.c \
       config/riscv/muldi3.S config/riscv/riscv-asm.h)

baseurl="https://raw.githubusercontent.com/gcc-mirror/gcc/"\
"1b306039ac49f8ad91ca71d3de3150a3c9fa792a/libgcc/"
help="USAGE: $0 [-f]\n"\
"Get some libgcc files.\n\n"\
"Options:\n"\
"-f  Force"

rootdir=$(dirname $0)
orgdir=$(pwd)

force=false

while getopts "fh" flag; do
    case "${flag}" in
        f) force=true ;;
        h) echo -e $help
           exit 0 ;;
    esac
done

echo "-- Entering $rootdir..."
cd $rootdir

for i in ${files[@]}; do
    if [ $force = true ] || [ ! -f $i ]; then
        echo "-- Downloading $baseurl/$i..."
        #curl $baseurl/$i > $i
        rm -f $i
        mkdir -p $(dirname $i)
        wget $baseurl/$i -O $i
    fi
done

echo "-- Exiting $rootdir..."
cd $orgdir
