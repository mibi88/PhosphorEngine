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

#include <string.h>

#include <getopt.h>

#include <conv.h>
#include <link.h>

#include <commands.h>

static const char help_str[] = (
    "USAGE: %s [-c] [-l] [-o OUTPUT_FILE] [-s START_LABEL] [-h] "
        "[INPUT_FILES...]\n"
    "Phosphore Engine data conversion tool\n"
    "\n"
    "Options:\n"
    "  -c   Compile\n"
    "  -l   Link\n"
    "  -o   Specify the output file\n"
    "  -s   Specify the starting label\n"
    "  -h   Show this help message\n"
);

static FILE *in;
static FILE *out;

static PHConv conv;
static PHLinker linker;

static void compile_file(char *argv0, char *in_path, char *out_path,
                        unsigned char compiled_file) {
    if(strcmp(in_path, "-")){
        in = fopen(in_path, "rb");
        if(in == NULL){
            fprintf(stderr, "%s: Failed to open %s!\n", argv0, in_path);

            exit(EXIT_FAILURE);
        }
    }else{
        in = stdin;
    }

    if(strcmp(out_path, "-")){
        out = fopen(out_path, compiled_file ? "ab" : "wb");
        if(out == NULL){
            fprintf(stderr, "%s: Failed to open %s!\n", argv0, out_path);
            if(strcmp(in_path, "-")) fclose(in);

            exit(EXIT_FAILURE);
        }
    }else{
        out = stdout;
    }

    if(ph_conv_init(&conv, &ph_commands, NULL)){
        fprintf(stderr, "%s: Internal error!\n", argv0);
        if(strcmp(in_path, "-")) fclose(in);
        if(strcmp(out_path, "-")) fclose(out);

        exit(EXIT_FAILURE);
    }

    if(ph_conv_convert(&conv, in)){
        fprintf(stderr, "%s:%lu: Error: %s\n", in_path, conv.line,
                ph_conv_get_error(&conv));
    }

    fwrite(conv.buffer.data, 1, conv.buffer.size, out);
    if(strcmp(in_path, "-")) fclose(in);
    if(strcmp(out_path, "-")) fclose(out);

    ph_conv_free(&conv);
}

static void link_start(char *argv0) {
    if(ph_linker_init(&linker, &ph_commands)){
        fprintf(stderr, "%s: Internal error!\n", argv0);

        exit(EXIT_FAILURE);
    }
}

static void link_add_file(char *argv0, char *in_path) {
    if(strcmp(in_path, "-")){
        in = fopen(in_path, "rb");
        if(in == NULL){
            fprintf(stderr, "%s: Failed to open %s!\n", argv0, in_path);
            ph_linker_free(&linker);

            exit(EXIT_FAILURE);
        }
    }else{
        in = stdin;
    }

    if(ph_linker_add_file(&linker, in)){
        fprintf(stderr, "%s: Internal error!\n", argv0);
        ph_linker_free(&linker);
        if(strcmp(in_path, "-")) fclose(in);

        exit(EXIT_FAILURE);
    }

    if(strcmp(in_path, "-")) fclose(in);
}

static void link_end(char *argv0, char *out_path, char *start_label) {
    if(ph_linker_link(&linker, start_label)){
        fprintf(stderr, "%s: Error: %s\n", argv0,
                ph_linker_get_error(&linker));
    }

    if(strcmp(out_path, "-")){
        out = fopen(out_path, "wb");
        if(out == NULL){
            fprintf(stderr, "%s: Failed to open %s!\n", argv0, out_path);
            ph_linker_free(&linker);

            exit(EXIT_FAILURE);
        }
    }

    fwrite(linker.out_buffer.data, 1, linker.out_buffer.size, out);

    if(strcmp(out_path, "-")) fclose(out);

    ph_linker_free(&linker);
}

int main(int argc, char **argv) {
    int opt;

    unsigned char link = 0;
    unsigned char compile = 0;

    unsigned char compiled_file = 0;

    char *out_path = "-";
    char *start_label = "main";

    /* TODO: Allow multiple input files for a single output file. */

    while((opt = getopt(argc, argv, "hcls:o:")) != -1){
        switch(opt){
            case 'h':
                fprintf(stderr, help_str, argv[0]);
                return EXIT_SUCCESS;
            case 'c':
                /* Compile */
                compile = 1;
                break;
            case 'l':
                /* Link */
                link = 1;
                break;
            case 's':
                /* Set the start label (for linking) */
                break;
            case 'o':
                /* Specify the output file */
                out_path = optarg;
                break;
        }
    }

    if(compile || !(compile^link)){
        for(;argv[optind];optind++){
            /* Files to compile */

            char *in_path = argv[optind];

            compile_file(argv[0], in_path, out_path, compiled_file);

            compiled_file = 1;
        }
    }

    if(!compiled_file){
        fprintf(stderr, "%s: No input files!\n", argv[0]);
        return EXIT_FAILURE;
    }

    if(link || !(link^compile)){
        /* Link */

        link_start(argv[0]);

        if(!(link^compile)){
            /* The input file is the output file */
            link_add_file(argv[0], out_path);
        }else{
            for(;argv[optind];optind++){
                /* Files to link */

                char *in_path = argv[optind];

                link_add_file(argv[0], in_path);
            }
            /* Use the output files */
        }

        link_end(argv[0], out_path, start_label);
    }

    return EXIT_SUCCESS;
}
