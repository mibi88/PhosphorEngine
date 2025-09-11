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

#ifndef PHOSPHOR_CONV_H
#define PHOSPHOR_CONV_H

#include <stdio.h>
#include <arena.h>

typedef struct {
    char *name;
    size_t pos;
} PHLabel;

typedef struct {
    /* TODO: Speed up searching with hashes. */
    int (**fncs)(void *_conv, size_t argc, char **argv);
    char **names;
    size_t count;
} PHCommands;

typedef struct {
    FILE *out;
    PHLabel *labels;
    PHArena names;
    unsigned char *data;

    unsigned char verbatim;

    size_t line;
    int error;

    PHCommands *commands;
    void *extra;
} PHConv;

enum {
    PH_CONV_SUCCESS,

    PH_CONV_E_UNSUPPORTED_CHAR,
    PH_CONV_E_TOKEN_TOO_LONG,
    PH_CONV_E_CMD_TOO_LONG,
    PH_CONV_E_CMD_MISSING,

    PH_CONV_E_AMOUNT
};

int ph_conv_init(PHConv *conv, FILE *out, PHCommands *commands,
                 void *extra);
int ph_conv_convert(PHConv *conv, FILE *in);
char *ph_conv_get_error(PHConv *conv);
void ph_conv_free(PHConv *conv);

#endif
