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

#include <conv.h>

#include <stdlib.h>

int ph_conv_init(PHConv *conv, FILE *out) {
    conv->out = out;
    conv->labels = NULL;
    conv->data = NULL;

    if(ph_arena_init(&conv->names, 64)) return 1;

    return 0;
}

int ph_conv_convert(PHConv *conv, FILE *in) {
    unsigned char r;
    unsigned long c;

    unsigned char byte_count = 0;
    unsigned char required_byte_count = 1;

    conv->line = 1;
    conv->error = PH_CONV_SUCCESS;

    while(fread(&r, 1, 1, in) == 1){
        /* Parse UTF-8 sequences */

        byte_count++;
        if(byte_count == required_byte_count){
            /* NOTE: I'm not refusing overlong form UTF-8 sequences */
            if(r == 0xF4){
                /* 4 byte UTF-8 char */
                c = r&0x07;
                required_byte_count = 4;
            }else if((r&0xFC) == 0xF0){
                /* 4 byte UTF-8 char */
                c = r&0x07;
                required_byte_count = 4;
            }else if((r&0xF0) == 0xF0){
                /* 3 byte UTF-8 char */
                c = r&0x0F;
                required_byte_count = 3;
            }else if((r&0xE0) == 0xC0){
                /* Two byte UTF-8 char */
                c = r&0x1F;
                required_byte_count = 2;
            }else{
                /* Single byte char */
                c = r;
                required_byte_count = 1;
            }
            byte_count = 0;
        }else{
            c <<= 6;
            c |= r&0x3F;
        }

        if(byte_count != required_byte_count-1) continue;

        /* Parse the data itself. */

        printf("%lu, ", c);
    }

    return conv->error;
}

char *ph_conv_get_error(PHConv *conv) {
    static char *errors[PH_CONV_E_AMOUNT] = {
        "Success!"
    };

    return errors[conv->error];
}

void ph_conv_free(PHConv *conv) {
    free(conv->labels);
    conv->labels = NULL;
    ph_arena_free(&conv->names);
}
