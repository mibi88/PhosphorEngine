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

#include <phosphor/utils.h>

static volatile char *const out_reg = (void*)(1024*1024);
static volatile char *const in_reg = (void*)(1024*1024+1);
static volatile unsigned char *const audio_reg = (void*)(1024*1024+2);
static volatile unsigned int *const time_reg = (void*)(1024*1024+4);
static volatile unsigned short int *const xpos_reg = (void*)(1024*1024+8);
static volatile unsigned short int *const ypos_reg = (void*)(1024*1024+10);

void puts(char *str) {
    while(*str){
        *out_reg = *str;
        str++;
    }
}

void putc(char c) {
    *out_reg = c;
}

void gets(char *str, size_t max) {
    size_t i = 0;

    while(*in_reg);

    while(1){
        char c;

        while(!(c = *in_reg));

        if(c == '\n'){
            str[i] = 0;
            *out_reg = '\n';
            return;
        }else if(c == 0x7F){
            if(i){
                *out_reg = 0x7F;
                *out_reg = ' ';
                *out_reg = 0x7F;
                i--;
            }
        }else if(i < max-1){
            str[i++] = c;
            *out_reg = c;
        }
    }
    str[i] = 0;
}

void beep(unsigned char note, size_t duration) {
    unsigned int time = *time_reg;

    *audio_reg = note;
    while(*time_reg-time < duration);
    *audio_reg = 0x80;
}

void set_cur_x(unsigned short int x) {
    *xpos_reg = x;
}

void set_cur_y(unsigned short int y) {
    *ypos_reg = y;
}

unsigned short int get_cur_x(void) {
    return *xpos_reg;
}

unsigned short int get_cur_y(void) {
    return *ypos_reg;
}

void term_size(unsigned short int *w, unsigned short int *h) {
    unsigned short int x, y;

    x = get_cur_x();
    y = get_cur_y();

    set_cur_x(0xFFFF);
    set_cur_y(0xFFFF);

    *w = get_cur_x();
    *h = get_cur_y();

    set_cur_x(x);
    set_cur_y(y);
}

unsigned long int mstime(void) {
    return *time_reg;
}

void itoa(int i, char *buffer, size_t size) {
    char *max = buffer+size;
    char *start;
    int n = i;
    if(i == 0){
        buffer[0] = '0';
        buffer[1] = 0;
        return;
    }
    if(i < 0){
        i = -i;
        if(buffer+2 < max){
            *buffer++ = '-';
        }
    }
    start = buffer;
    while(n){
        if(buffer+2 < max){
            n /= 10;
            buffer++;
        }else{
            break;
        }
    }
    *buffer = 0;

    while(buffer-- != start){
        *buffer = '0'+(i%10);
        i /= 10;
    }
}

int strcmp(char *a, char *b) {
    for(;*a == *b && *b;a++,b++);
    return *a-*b;
}
