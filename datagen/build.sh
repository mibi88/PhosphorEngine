#!/bin/bash

# Phosphor Engine: A small but quite special game engine to create text
#                  adventures.
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

help="USAGE: $0 [-d]\n"\
"A small tool to compile the Phosphor Engine game "\
"binary.\n\n"\
"Options:\n"\
"-d  Debug build (-O0 and no LTO)"

cc=clang
ld=clang

cflags=(-ansi -Wall -Wextra -Wpedantic -Isrc -I../shared)
ldflags=()

builddir=build

name=main
srcdir=src

debug=false

while getopts "dh" flag; do
    case "${flag}" in
        d) debug=true ;;
        h) echo -e $help
           exit 0 ;;
    esac
done

if [ $debug = true ]; then
    echo "-- Debug build..."
    cflags+=(-O0 -g)
    ldflags+=(-g)
else
    echo "-- Release build..."
    cflags+=(-Os)
    ldflags+=(-flto=auto)
fi

rootdir=$(dirname $0)
orgdir=$(pwd)
echo "-- Entering $rootdir..."
cd $rootdir

mkdir -p $builddir

objfiles=()

for i in $(find $srcdir -mindepth 1 -type f \( -name "*.c" -o -name "*.s" \
           -o -name "*.S" \)); do
    obj=$builddir/${i#$srcdir*}.o
    echo "-- Compiling ${i} to ${obj}..."
    mkdir -p $(dirname $obj)
    $cc -c $i -o $obj ${cflags[@]}
    if [ $? -ne 0 ]; then
        echo "-- Build failed with exit code $?!"
        echo "-- Exiting $rootdir..."
        cd $orgdir
        exit $?
    fi
    objfiles+=($obj)
done

# Linking
echo "-- Linking $name..."
$ld ${objfiles[@]} -o $name ${ldflags[@]}

if [ $? -ne 0 ]; then
    echo "-- Build failed with exit code $?!"
    echo "-- Exiting $rootdir..."
    cd $orgdir
    exit $?
fi

echo "-- Exiting $rootdir..."
cd $orgdir
echo "-- Build succeeded!"
