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

#include <commands.h>

#include <string.h>
#include <stdlib.h>

/* TODO: Speed up string comparisons with hashes. */

static int startverbatim(void *_conv, size_t argc, char **argv) {
    PHConv *conv = _conv;
    if(argc > 1) return PH_CONV_E_TOO_MANY_ARGS;

    (void)argv;

    conv->verbatim = 1;

    ph_buffer_putc(&conv->buffer, PH_CMD_STARTVERBATIM);

    return PH_CONV_SUCCESS;
}

static int endverbatim(void *_conv, size_t argc, char **argv) {
    PHConv *conv = _conv;
    if(argc > 1) return PH_CONV_E_TOO_MANY_ARGS;

    (void)argv;

    ph_buffer_truncate(&conv->buffer, conv->cmd_start);

    conv->verbatim = 0;

    ph_buffer_putc(&conv->buffer, PH_CMD_ENDVERBATIM);

    return PH_CONV_SUCCESS;
}

static int clear(void *_conv, size_t argc, char **argv) {
    PHConv *conv = _conv;
    if(conv->verbatim) return PH_CONV_SUCCESS;
    if(argc > 1) return PH_CONV_E_TOO_MANY_ARGS;

    (void)argv;

    ph_buffer_putc(&conv->buffer, PH_CMD_CLEAR);

    return PH_CONV_SUCCESS;
}

static int halign(void *_conv, size_t argc, char **argv) {
    PHConv *conv = _conv;
    if(conv->verbatim) return PH_CONV_SUCCESS;
    if(argc < 2) return PH_CONV_E_TOO_FEW_ARGS;
    if(argc > 2) return PH_CONV_E_TOO_MANY_ARGS;

    ph_buffer_putc(&conv->buffer, PH_CMD_HALIGN);

    if(!strcmp(argv[1], "left")){
        ph_buffer_putc(&conv->buffer, PH_CMD_ALIGN_LEFT);
    }else if(!strcmp(argv[1], "center")){
        ph_buffer_putc(&conv->buffer, PH_CMD_ALIGN_CENTER);
    }else if(!strcmp(argv[1], "right")){
        ph_buffer_putc(&conv->buffer, PH_CMD_ALIGN_RIGHT);
    }else{
        return PH_CONV_E_INCORRECT_ARGS;
    }

    return PH_CONV_SUCCESS;
}

static int valign(void *_conv, size_t argc, char **argv) {
    PHConv *conv = _conv;
    if(conv->verbatim) return PH_CONV_SUCCESS;
    if(argc < 2) return PH_CONV_E_TOO_FEW_ARGS;
    if(argc > 2) return PH_CONV_E_TOO_MANY_ARGS;

    ph_buffer_putc(&conv->buffer, PH_CMD_VALIGN);

    if(!strcmp(argv[1], "top")){
        ph_buffer_putc(&conv->buffer, PH_CMD_ALIGN_TOP);
    }else if(!strcmp(argv[1], "center")){
        ph_buffer_putc(&conv->buffer, PH_CMD_ALIGN_CENTER);
    }else if(!strcmp(argv[1], "bottom")){
        ph_buffer_putc(&conv->buffer, PH_CMD_ALIGN_BOTTOM);
    }else{
        return PH_CONV_E_INCORRECT_ARGS;
    }


    return PH_CONV_SUCCESS;
}

static int setx(void *_conv, size_t argc, char **argv) {
    PHConv *conv = _conv;
    unsigned short int x;

    if(conv->verbatim) return PH_CONV_SUCCESS;
    if(argc < 2) return PH_CONV_E_TOO_FEW_ARGS;
    if(argc > 2) return PH_CONV_E_TOO_MANY_ARGS;

    x = atoi(argv[1]);

    ph_buffer_putc(&conv->buffer, PH_CMD_SETX);
    ph_buffer_putc(&conv->buffer, x&0xFF);
    ph_buffer_putc(&conv->buffer, (x>>8)&0xFF);

    return PH_CONV_SUCCESS;
}

static int sety(void *_conv, size_t argc, char **argv) {
    PHConv *conv = _conv;
    unsigned short int y;

    if(conv->verbatim) return PH_CONV_SUCCESS;
    if(argc < 2) return PH_CONV_E_TOO_FEW_ARGS;
    if(argc > 2) return PH_CONV_E_TOO_MANY_ARGS;

    y = atoi(argv[1]);

    ph_buffer_putc(&conv->buffer, PH_CMD_SETY);
    ph_buffer_putc(&conv->buffer, y&0xFF);
    ph_buffer_putc(&conv->buffer, (y>>8)&0xFF);

    return PH_CONV_SUCCESS;
}

static int pagebreak(void *_conv, size_t argc, char **argv) {
    PHConv *conv = _conv;
    if(conv->verbatim) return PH_CONV_SUCCESS;
    if(argc > 1) return PH_CONV_E_TOO_MANY_ARGS;

    (void)argv;

    ph_buffer_putc(&conv->buffer, PH_CMD_PAGEBREAK);

    return PH_CONV_SUCCESS;
}

static int label(void *_conv, size_t argc, char **argv) {
    PHConv *conv = _conv;
    if(conv->verbatim) return PH_CONV_SUCCESS;
    if(argc < 2) return PH_CONV_E_TOO_FEW_ARGS;
    if(argc > 2) return PH_CONV_E_TOO_MANY_ARGS;

    ph_buffer_putc(&conv->buffer, PH_CMD_LABEL);
    ph_buffer_puts(&conv->buffer, (unsigned char*)argv[1]);
    ph_buffer_putc(&conv->buffer, 0);

    return PH_CONV_SUCCESS;
}

static int goto_cmd(void *_conv, size_t argc, char **argv) {
    PHConv *conv = _conv;
    if(conv->verbatim) return PH_CONV_SUCCESS;
    if(argc < 2) return PH_CONV_E_TOO_FEW_ARGS;
    if(argc > 2) return PH_CONV_E_TOO_MANY_ARGS;

    ph_buffer_putc(&conv->buffer, PH_CMD_GOTO);
    ph_buffer_write(&conv->buffer, (unsigned char*)argv[1], strlen(argv[1])+1);

    return PH_CONV_SUCCESS;
}

static int case_cmd(void *_conv, size_t argc, char **argv) {
    PHConv *conv = _conv;
    if(conv->verbatim) return PH_CONV_SUCCESS;
    if(argc < 3) return PH_CONV_E_TOO_FEW_ARGS;
    if(argc > 3) return PH_CONV_E_TOO_MANY_ARGS;

    ph_buffer_putc(&conv->buffer, PH_CMD_CASE);
    ph_buffer_write(&conv->buffer, (unsigned char*)argv[1], strlen(argv[1])+1);
    ph_buffer_write(&conv->buffer, (unsigned char*)argv[2], strlen(argv[2])+1);

    return PH_CONV_SUCCESS;
}

static int dcase(void *_conv, size_t argc, char **argv) {
    PHConv *conv = _conv;
    if(conv->verbatim) return PH_CONV_SUCCESS;
    if(argc < 3) return PH_CONV_E_TOO_FEW_ARGS;
    if(argc > 3) return PH_CONV_E_TOO_MANY_ARGS;

    ph_buffer_putc(&conv->buffer, PH_CMD_DCASE);
    ph_buffer_write(&conv->buffer, (unsigned char*)argv[1], strlen(argv[1])+1);
    ph_buffer_write(&conv->buffer, (unsigned char*)argv[2], strlen(argv[2])+1);

    return PH_CONV_SUCCESS;
}

static int clearcases(void *_conv, size_t argc, char **argv) {
    PHConv *conv = _conv;
    if(conv->verbatim) return PH_CONV_SUCCESS;
    if(argc > 1) return PH_CONV_E_TOO_MANY_ARGS;

    (void)argv;

    ph_buffer_putc(&conv->buffer, PH_CMD_CLEARCASES);

    return PH_CONV_SUCCESS;
}

static int ask(void *_conv, size_t argc, char **argv) {
    PHConv *conv = _conv;
    if(conv->verbatim) return PH_CONV_SUCCESS;
    if(argc > 1) return PH_CONV_E_TOO_MANY_ARGS;

    (void)argv;

    ph_buffer_putc(&conv->buffer, PH_CMD_ASK);

    return PH_CONV_SUCCESS;
}

static int askc(void *_conv, size_t argc, char **argv) {
    PHConv *conv = _conv;
    if(conv->verbatim) return PH_CONV_SUCCESS;
    if(argc > 1) return PH_CONV_E_TOO_MANY_ARGS;

    (void)argv;

    ph_buffer_putc(&conv->buffer, PH_CMD_ASKC);

    return PH_CONV_SUCCESS;
}

static int delay(void *_conv, size_t argc, char **argv) {
    PHConv *conv = _conv;
    unsigned short int d;

    if(conv->verbatim) return PH_CONV_SUCCESS;
    if(argc < 2) return PH_CONV_E_TOO_FEW_ARGS;
    if(argc > 2) return PH_CONV_E_TOO_MANY_ARGS;

    d = atoi(argv[1]);

    ph_buffer_putc(&conv->buffer, PH_CMD_DELAY);
    ph_buffer_putc(&conv->buffer, d&0xFF);
    ph_buffer_putc(&conv->buffer, (d>>8)&0xFF);

    return PH_CONV_SUCCESS;
}

#define PH_CMD_SEMITONES 12

static int note(void *_conv, size_t argc, char **argv) {
    PHConv *conv = _conv;
    static const char *const semitones[PH_CMD_SEMITONES] = {
        "C-",
        "C#",
        "D-",
        "D#",
        "E-",
        "F-",
        "F#",
        "G-",
        "G#",
        "A-",
        "A#",
        "B-"
    };

    unsigned char semitone;
    unsigned char octave;
    unsigned char i;

    unsigned short int duration;

    if(conv->verbatim) return PH_CONV_SUCCESS;
    if(argc < 3) return PH_CONV_E_TOO_FEW_ARGS;
    if(argc > 3) return PH_CONV_E_TOO_MANY_ARGS;

    if(strlen(argv[1]) != 3) return PH_CONV_E_INCORRECT_ARGS;
    if(argv[1][2] < '0' || argv[1][2] > '7'){
        return PH_CONV_E_INCORRECT_ARGS;
    }
    octave = argv[1][2]-'0';
    for(i=0;i<PH_CMD_SEMITONES;i++){
        if(!memcmp(argv[1], semitones[i], 2)){
            semitone = i;
            break;
        }
    }
    if(i >= PH_CMD_SEMITONES) return PH_CONV_E_INCORRECT_ARGS;

    duration = atoi(argv[2]);

    ph_buffer_putc(&conv->buffer, PH_CMD_NOTE);
    ph_buffer_putc(&conv->buffer, octave|(semitone<<3));
    ph_buffer_putc(&conv->buffer, duration&0xFF);
    ph_buffer_putc(&conv->buffer, (duration>>8)&0xFF);

    return PH_CONV_SUCCESS;
}

static int startbgm(void *_conv, size_t argc, char **argv) {
    PHConv *conv = _conv;
    if(conv->verbatim) return PH_CONV_SUCCESS;
    if(argc > 1) return PH_CONV_E_TOO_MANY_ARGS;

    (void)argc;
    (void)argv;

    ph_buffer_putc(&conv->buffer, PH_CMD_STARTBGM);

    return PH_CONV_SUCCESS;
}

static int endbgm(void *_conv, size_t argc, char **argv) {
    PHConv *conv = _conv;
    if(conv->verbatim) return PH_CONV_SUCCESS;
    if(argc > 1) return PH_CONV_E_TOO_MANY_ARGS;

    (void)argc;
    (void)argv;

    ph_buffer_putc(&conv->buffer, PH_CMD_ENDBGM);

    return PH_CONV_SUCCESS;
}

static int (*fncs[])(void *_conv, size_t argc, char **argv) = {
    startverbatim,
    endverbatim,
    clear,
    halign,
    valign,
    setx,
    sety,
    pagebreak,
    label,
    goto_cmd,
    case_cmd,
    dcase,
    clearcases,
    ask,
    askc,
    delay,
    note,
    startbgm,
    endbgm
};

static char *names[] = {
    "startverbatim",
    "endverbatim",
    "clear",
    "halign",
    "valign",
    "setx",
    "sety",
    "pagebreak",
    "label",
    "goto",
    "case",
    "dcase", /* Displayed case */
    "clearcases", /* Clear the case list */
    "ask",
    "askc", /* Ask and clear case list */
    "delay",
    "note",
    "startbgm", /* Start background music */
    "endbgm" /* End background music */
};

PHCommands ph_commands = {
    fncs,
    names,
    PH_CMD_AMOUNT
};
