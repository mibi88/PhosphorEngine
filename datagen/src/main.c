/* Phosphor Engine: A small but quite special game engine to create text
 *                  adventures.
 * by Mibi88
 *
 * This software is licensed under the BSD-3-Clause license:
 *
 * Copyright 2025 Mibi88
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>

#include <conv.h>

int main(int argc, char **argv) {
    FILE *in;
    FILE *out;

    PHConv conv;

    /* TODO: Allow multiple input files for a single output file. */

    if(argc < 3){
        fprintf(stderr, "USAGE: %s [INPUT] [OUTPUT]\n"
                        "Phosphore Engine data conversion tool\n", argv[0]);

        return EXIT_FAILURE;
    }

    in = fopen(argv[1], "rb");
    if(in == NULL){
        fprintf(stderr, "%s: Failed to open %s!\n", argv[0], argv[1]);

        return EXIT_FAILURE;
    }

    out = fopen(argv[2], "wb");
    if(out == NULL){
        fprintf(stderr, "%s: Failed to open %s!\n", argv[0], argv[2]);
        fclose(in);

        return EXIT_FAILURE;
    }

    if(ph_conv_init(&conv, out)){
        fprintf(stderr, "%s: Internal error!\n", argv[0]);
        fclose(in);
        fclose(out);

        return EXIT_FAILURE;
    }

    if(ph_conv_convert(&conv, in)){
        fprintf(stderr, "%s:%lu: Error: %s\n", argv[1], conv.line,
                ph_conv_get_error(&conv));
    }

    fclose(out);

    ph_conv_free(&conv);

    return EXIT_SUCCESS;
}
