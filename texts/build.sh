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

textdir=.
data=../game/src/data/data.bin
datagen=../datagen/main
dataname=ph_data

rootdir=$(dirname $0)
orgdir=$(pwd)
echo "-- Entering $rootdir..."
cd $rootdir

errorcheck() {
    rc=$?
    if [ $rc -ne 0 ]; then
        echo "-- Build failed with exit code $rc!"
        echo "-- Exiting $rootdir..."
        cd $orgdir
        exit $rc
    fi
}

mkdir -p $(dirname $data)

objlist=()

for i in $(find $textdir -type f ! -name "build.sh"); do
    obj=$(dirname $data)/${i#$textdir}.obj

    echo "-- Converting text adventure data $i to $obj..."
    $datagen -c $i -o $obj

    errorcheck

    objlist+=($obj)
done

echo "-- Linking text adventure data..."
$datagen -l ${objlist[@]} -o $data
errorcheck

xxd -n $dataname -i $data > $data.c
errorcheck

echo "-- Exiting $rootdir..."
cd $orgdir
