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

#include <buffer.h>

#include <stdlib.h>
#include <string.h>

int ph_buffer_init(PHBuffer *buffer, size_t step) {
    buffer->data = malloc(step);
    if(buffer->data == NULL){
        return 1;
    }

    buffer->cur = 0;
    buffer->size = 0;
    buffer->max = step;
    buffer->step = step;

    return 0;
}

int ph_buffer_write(PHBuffer *buffer, unsigned char *data, int size) {
    if(buffer->cur+size > buffer->max){
        size_t new_size = buffer->max+((buffer->cur+size)/buffer->step+1)*
                          buffer->step;
        unsigned char *new;

        new = realloc(buffer->data, new_size);
        if(new == NULL) return 1;

        buffer->data = new;
        buffer->max = new_size;
    }

    memcpy(buffer->data+buffer->cur, data, size);
    buffer->size += size;
    buffer->cur += size;

    return 0;
}

int ph_buffer_putc(PHBuffer *buffer, unsigned char c) {
    return ph_buffer_write(buffer, &c, 1);
}

int ph_buffer_puts(PHBuffer *buffer, unsigned char *str) {
    return ph_buffer_write(buffer, str, strlen((const char*)str));
}

void ph_buffer_seek(PHBuffer *buffer, size_t pos, int whence) {
    switch(whence){
        case PH_BUFFER_START:
            if(pos <= buffer->size) buffer->cur = pos;
            break;
        case PH_BUFFER_END:
            if(pos <= buffer->size) buffer->cur = buffer->size-pos;
            break;
        case PH_BUFFER_CUR_INC:
            if(buffer->cur+pos <= buffer->size) buffer->cur += pos;
            break;
        case PH_BUFFER_CUR_DEC:
            if(pos <= buffer->cur) buffer->cur -= pos;
            break;
    }
}

void ph_buffer_truncate(PHBuffer *buffer, size_t max) {
    if(max < buffer->size){
        buffer->size = max;
        if(buffer->cur > max) buffer->cur = max;
    }
}

void ph_buffer_free(PHBuffer *buffer) {
    free(buffer->data);
    buffer->data = NULL;
}
