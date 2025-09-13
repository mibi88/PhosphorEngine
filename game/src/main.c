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

#include <stddef.h>
#include <phosphor/utils.h>
#include <phosphor/adventure.h>

extern unsigned char ph_data[];
extern unsigned int ph_data_len;

int main(void) {
    PHAdventure adv;

#if 1

    char *a = ": Hello world!\n";
    char buffer[20];
    size_t i;
    volatile char *out = (void*)(1024*1024);

    unsigned short int x, y;
    unsigned short int w, h;

    for(i=0;i<80;i++){
        itoa(i, buffer, 20);
        puts(buffer);
        puts(a);
    }
    for(i=0x20;i<0x7F;i++){
        *out = i;
    }
    beep(3|(9<<3), 1000); /* Play A-3 (220Hz) for 1 second */
    puts("\n> ");
    gets(buffer, 20);
    puts("You entered \"");
    puts(buffer);
    puts("\"\n");

    /* Move to the bottom-right corner */
    x = get_cur_x();
    y = get_cur_y();

    set_cur_x(0xFFFF);
    set_cur_y(0xFFFF);

    w = get_cur_x()+1;
    h = get_cur_y()+1;

    set_cur_x(x);
    set_cur_y(y);

    puts("Terminal size: ");
    itoa(w, buffer, 20);
    puts(buffer);
    puts("x");
    itoa(h, buffer, 20);
    puts(buffer);
    puts("\n");

    puts("ph_data_len: ");
    itoa(ph_data_len, buffer, 20);
    puts(buffer);
    puts("\n");

    for(i=0;i<ph_data_len;i++){
        putc(ph_data[i]);
    }

    while(1);

#endif

    ph_adventure_init(&adv, ph_data);
    ph_adventure_run(&adv);
    return 0;
}
