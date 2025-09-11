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
#include <string.h>

#define PH_CONV_TOKEN_MAX 32
#define PH_CONV_CMD_MAX_TOKENS 8

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

    unsigned char line_start = 1;
    unsigned char command = 0;

    unsigned char has_space = 1;

    unsigned char escaped = 0;

    char *ifs = " \t\n#";

    char token[PH_CONV_TOKEN_MAX];
    char cmd[PH_CONV_CMD_MAX_TOKENS][PH_CONV_TOKEN_MAX+1];
    size_t token_len = 0;
    size_t command_tok = 0;

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
            }else if((r&0xF0) == 0xE0){
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

        if(!((c >= 0x20 && c <= 0x7E) || (c >= 0xA0 && c <= 0xFF) ||
             c == '\r' || c == '\n' || c == '\t')){
            conv->error = PH_CONV_E_UNSUPPORTED_CHAR;
            break;
        }

        if(c == '\\'){
            escaped = 1;
            continue;
        }else if(c == '\r'){
            continue;
        }

        if(strchr(ifs, c) == NULL){
            if(line_start){
                line_start = 0;
            }
            if(!command){
                fputc(c, conv->out);
            }

            /* Add char to token */

            if(token_len < PH_CONV_TOKEN_MAX-1){
                token[token_len++] = c;
            }else if(command){
                conv->error = PH_CONV_E_TOKEN_TOO_LONG;
                break;
            }

            has_space = 0;
        }else{
            if(c == '#' && line_start && !escaped){
                /* Command */
                command = 1;
                command_tok = 0;
            }

            if(command && !has_space){
                /* Token done */

                if(command_tok < PH_CONV_CMD_MAX_TOKENS-1){
                    memcpy(cmd[command_tok], token, token_len);
                    cmd[command_tok][token_len] = 0;
                    command_tok++;
                }else{
                    conv->error = PH_CONV_E_CMD_TOO_LONG;
                    break;
                }
            }else if(!has_space && !line_start){
                /* Add space to output */

                fputc(' ', conv->out);
            }

            has_space = 1;

            token_len = 0;
        }

        if(c == '\n'){
            if(command){
                size_t i;
                for(i=0;i<command_tok;i++){
                    printf("%s, ", cmd[i]);
                }
                puts("");
            }

            conv->line++;
            line_start = 1;
        }

        escaped = 0;
    }

    return conv->error;
}

char *ph_conv_get_error(PHConv *conv) {
    static char *errors[PH_CONV_E_AMOUNT] = {
        "Success",
        "Unsupported char",
        "Command token too long",
        "Command too long"
    };

    return errors[conv->error];
}

void ph_conv_free(PHConv *conv) {
    free(conv->labels);
    conv->labels = NULL;
    ph_arena_free(&conv->names);
}
