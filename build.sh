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

rootdir=$(dirname $0)
orgdir=$(pwd)
echo "-- Entering $rootdir..."
cd $rootdir

closure_url="https://repo1.maven.org/maven2/com/google/javascript/"\
"closure-compiler/v20250820/closure-compiler-v20250820.jar"

help="USAGE: $0 [-d] [-f]\n"\
"A small tool to compile Phosphor Engine games.\n\n"\
"Options:\n"\
"-d  Debug build (JS files aren't minified with closure)\n"\
"-f  Force downloads\n"\
"-i  Use the image loader, which loads the code from an image"

name=phosphor.zip

bin=game/main
srcdir=js
builddir=build
tooldir=tools
data=game/src/data/data.bin
dataname=ph_data

debug=false
force=false

imageloader=false

while getopts "dfih" flag; do
    case "${flag}" in
        d) debug=true ;;
        f) force=true ;;
        i) imageloader=true ;;
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
    rm -f $closure
    wget $closure_url -O $closure
fi

if [ $force = true ]; then
    game/src/libgcc_parts/fetch.sh -f
else
    game/src/libgcc_parts/fetch.sh
fi

# Compile the data generation tool

echo "-- Compiling the datagen tool..."

if [ $debug = true ]; then
    datagen/build.sh -d
else
    datagen/build.sh
fi

# Generate the text adventure data

mkdir -p $(dirname $data)

for i in $(find texts -type f); do
    echo "-- Converting text adventure data $i to $data..."
    # TODO: Improve this
    datagen/main $i -o $data
    xxd  -n $dataname -i $data > $data.c
done

# Compile the game

echo "-- Compiling the game..."

if [ $debug = true ]; then
    game/build.sh -d
else
    game/build.sh
fi

# Create the ZIP

compile() {
    out=$builddir/$2
    if [ $debug = true ]; then
        echo "-- Copying $1 to $out..."
        mkdir -p $(dirname $out)
        cp $1 $out
    else
        echo "-- Compiling $1 to $out..."
        mkdir -p $(dirname $out)
        java -jar $closure --js $1 --js_output_file $out
    fi
}

for i in $(find $srcdir -type f ! -name "loader_*.js"); do
    compile $i $i
done

if [ $imageloader = true ]; then
    compile "js/loader_img.js" "js/loader.js"
else
    compile "js/loader_bin.js" "js/loader.js"
fi

echo "-- Copying index.html..."
cp index.html $builddir/index.html

echo "-- Copying styles.css..."
cp styles.css $builddir/styles.css

echo "-- Copying the binary..."
if [ $imageloader = true ]; then
    rm -f $builddir/main
    cp $bin.png $builddir/main.png
else
    rm -f $builddir/main.png
    cp $bin $builddir/main
fi

for i in $(find assets -type f ! -name "favicon.png"); do
    echo "-- Copying $i to $builddir..."
    cp $i $builddir
done

echo "-- Creating the ZIP file..."
rm -f $name

currentdir=$(pwd)
cd $builddir
zip -9r $currentdir/$name .
cd $currentdir

echo "-- Exiting $rootdir..."
cd $orgdir

echo "-- Done!"
