#!/bin/bash

# A small RV32I emulator written in JS.
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

rootdir=$(dirname $0)
orgdir=$(pwd)
echo "-- Entering $rootdir..."
cd $rootdir

closure_url="https://repo1.maven.org/maven2/com/google/javascript/"\
"closure-compiler/v20250820/closure-compiler-v20250820.jar"

help="USAGE: $0 [-d] [-f]\n"\
"A small tool to compile MTAE (Mibi's Text Adventure Engine) games.\n\n"\
"Options:\n"\
"-d  Debug build (JS files aren't minified with closure)\n"\
"-f  Force downloads"

name=mtae.zip

bin=game/main
srcdir=js
builddir=build
tooldir=tools

debug=false
force=false

while getopts "dfh" flag; do
    case "${flag}" in
        d) debug=true ;;
        f) force=true ;;
        h) echo -e $help
           exit 0 ;;
    esac
done

mkdir -p $builddir
mkdir -p $tooldir

# Get some stuff

closure=$tooldir/closure.jar

if [ $force = true ] || [ ! -f $closure ]; then
    echo "-- Downloading closure..."
    #curl $closure_url > $closure
    rm $closure
    wget $closure_url -O $closure
fi

if [ $force = true ]; then
    game/src/libgcc_parts/fetch.sh -f
else
    game/src/libgcc_parts/fetch.sh
fi

# Compile the game

echo "-- Compiling the game..."

if [ $debug = true ]; then
    game/build.sh -d
else
    game/build.sh
fi

# Create the ZIP

for i in $(find $srcdir -type f); do
    out=$builddir/$i
    if [ $debug = true ]; then
        echo "-- Copying $i to $out..."
        mkdir -p $(dirname $out)
        cp $i $out
    else
        echo "-- Compiling $i to $out..."
        mkdir -p $(dirname $out)
        java -jar $closure --js $i --js_output_file $out
    fi
done

echo "-- Copying index.html..."
cp index.html $builddir/index.html

echo "-- Copying the binary..."
cp $bin $builddir/main

echo "-- Copying favicon.ico..."
cp favicon.ico $builddir

echo "-- Creating the ZIP file..."
rm $name
zip -9r $name $builddir

echo "-- Exiting $rootdir..."
cd $orgdir

echo "-- Done!"
