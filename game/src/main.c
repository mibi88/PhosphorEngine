/* A small text adventure.
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

#include <stddef.h>

extern char _start_bss, _end_bss;
extern char _start_data, _end_data;
extern char _romdata_start;

/* The main function. */
extern int main(void);

__attribute__((section(".pretext")))

int _start(void) {
#if 1
    volatile char *bss_ptr, *data_ptr, *romdataptr;
    /* Clearing the bss. */
    bss_ptr = &_start_bss;
    for(;bss_ptr<&_end_bss;bss_ptr++){
        *bss_ptr = 0;
    }
    /* Load the ROM data into the RAM. */
    data_ptr = &_start_data;
    romdataptr = &_romdata_start;
    for(;data_ptr<&_end_data;data_ptr++,romdataptr++){
        *data_ptr++ = *romdataptr++;
    }
#endif
    return main();
}

void puts(char *str) {
    volatile char *out = (void*)(1024*1024);
    while(*str){
        *out = *str;
        str++;
    }
}

void gets(char *str, size_t max) {
    volatile char *in = (void*)(1024*1024+1);
    volatile char *out = (void*)(1024*1024);
    size_t i = 0;

    while(1){
        char c;

        while(!(c = *in));

        if(c == '\n'){
            str[i] = 0;
            *out = '\n';
            return;
        }else if(c == 0x7F){
            if(i){
                *out = 0x7F;
                *out = ' ';
                *out = 0x7F;
                i--;
            }
        }else if(i < max-1){
            str[i++] = c;
            *out = c;
        }
    }
    str[i] = 0;
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

int main(void) {
    char *a = ": Hello world!\n";
    char buffer[20];
    size_t i;
    volatile char *out = (void*)(1024*1024);
    for(i=0;i<80;i++){
        itoa(i, buffer, 20);
        puts(buffer);
        puts(a);
    }
    for(i=0x20;i<0x7F;i++){
        *out = i;
    }
    puts("\n> ");
    gets(buffer, 20);
    puts("You entered \"");
    puts(buffer);
    puts("\"\n");

    while(1);
    return 0;
}
