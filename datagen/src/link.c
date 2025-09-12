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

#include <link.h>

#include <format.h>

#include <stdlib.h>
#include <string.h>

int ph_linker_init(PHLinker *linker) {
    if(ph_buffer_init(&linker->in_buffer, 64)) return 1;
    if(ph_buffer_init(&linker->out_buffer, 64)){
        ph_buffer_free(&linker->in_buffer);
        return 1;
    }
    if(ph_arena_init(&linker->names, 64)){
        ph_buffer_free(&linker->in_buffer);
        ph_buffer_free(&linker->out_buffer);
        return 1;
    }
    linker->labels = NULL;

    linker->error = 0;

    linker->label_count = 0;
    return 0;
}

int ph_linker_add_file(PHLinker *linker, FILE *in) {
    size_t size;

    fseek(in, 0, SEEK_END);
    size = ftell(in);
    rewind(in);

    if(ph_buffer_alloc(&linker->in_buffer, size)) return 1;

    if(fread(linker->in_buffer.data+linker->in_buffer.cur, 1, size,
             in) != size) return 1;

    linker->in_buffer.cur += size;
    linker->in_buffer.size += size;

    return 0;
}

int ph_linker_link(PHLinker *linker, char *start) {
    size_t i;
    unsigned char label[PH_CONV_TOKEN_MAX+1];
    size_t label_cur = 0;
    unsigned char in_label = 0;

    size_t removed = 0;

    /* Search all labels */
    for(i=0;i<linker->in_buffer.size;i++){
        unsigned char c = linker->in_buffer.data[i];
        if(in_label){
            if(c == 0){
                PHLabel *new = NULL;

                in_label = 0;
                label[label_cur] = 0;

                new = realloc(linker->labels, (linker->label_count+1)*
                              sizeof(PHLabel));
                if(new == NULL){
                    linker->error = PH_LINK_E_INTERNAL;
                    return 1;
                }
                linker->labels = new;

                linker->labels[linker->label_count]
                              .name = ph_arena_alloc(&linker->names,
                                                     1, label_cur+1);

                if(linker->labels[linker->label_count].name == NULL){
                    linker->error = PH_LINK_E_INTERNAL;
                    return 1;
                }

                memcpy(linker->labels[linker->label_count].name, label,
                       label_cur+1);
                linker->labels[linker->label_count].pos = i;

                linker->label_count++;
            }else{
                if(label_cur < PH_CONV_TOKEN_MAX-1){
                    label[label_cur] = c;
                    label_cur++;
                }
            }
        }else if(c == PH_CMD_LABEL){
            in_label = 1;
            label_cur = 0;
        }
    }

    /* Replace labels with offsets */
    /* TODO */
    for(i=0;i<linker->label_count;i++){
        PHLabel *label = linker->labels+i;

        printf("\"%s\": %lu\n", label->name, label->pos);
    }

    return 0;
}

char *ph_linker_get_error(PHLinker *linker) {
    static char *const errors[PH_LINK_E_AMOUNT] = {
        "Success!",
        "Internal error!",
        "Unknown label!"
    };

    return errors[linker->error];
}

void ph_linker_free(PHLinker *linker) {
    ph_buffer_free(&linker->in_buffer);
    ph_buffer_free(&linker->out_buffer);
}
