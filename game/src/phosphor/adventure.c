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

#include <phosphor/adventure.h>
#include <phosphor/utils.h>

#include <format.h>

#define _C (adv->data[adv->cur])

void ph_adventure_init(PHAdventure *adv, unsigned char *data) {
    adv->case_count = 0;
    adv->data = data;
}

#if 1 /* Debugging stuff */

static void iprint(char *name, int i) {
    static char buffer[20];

    puts(name);
    puts(": ");
    itoa(i, buffer, 20);
    puts(buffer);
    putc('\n');
}

#define _IP(v) iprint(#v, v)

#endif

static int streq(unsigned char *a, unsigned char *b) {
    for(;*a == *b && *b;a++,b++);
    if(*a != *b) return 0;
    return 1;
}

static volatile char *const in_reg = (void*)(1024*1024+1);

void ph_adventure_run(PHAdventure *adv) {
    size_t target;
    unsigned char c;

    static unsigned char buffer[PH_ADV_CASE_LEN_MAX];

    unsigned char lines = 0;

    unsigned short int before;

    while(1){
        switch((c = _C)){
            case PH_CMD_GOTO:
                adv->cur++;
                target = _C;
                adv->cur++;
                target |= _C<<8;
                adv->cur++;
                target |= _C<<16;
                adv->cur++;
                target |= _C<<24;
                adv->cur++;
                /* FIXME: Why do I have such a weird behaviour depending on the
                 * sign. */
                adv->cur += target+(target&0x80000000 ? 1 : 0);
                break;
            case PH_CMD_DCASE:
            case PH_CMD_CASE:
                if(adv->case_count < PH_ADV_CASE_MAX){
                    size_t n;

                    adv->cur++;
                    for(n=0;_C;n++){
                        adv->case_buffer[adv->case_count].name[n] = _C;
                        adv->cur++;
                    }
                    adv->case_buffer[adv->case_count].name[n] = 0;
                    adv->cur++;

                    target = _C;
                    adv->cur++;
                    target |= _C<<8;
                    adv->cur++;
                    target |= _C<<16;
                    adv->cur++;
                    target |= _C<<24;
                    adv->cur++;

                    target = adv->cur+target+1;

                    adv->case_buffer[adv->case_count].offset = target;
                    if(c == PH_CMD_DCASE){
                        puts("command: ");
                        puts((char*)adv->case_buffer[adv->case_count].name);
                        putc('\n');
                    }

                    adv->case_count++;
                }
                break;
            case PH_CMD_ASK:
            case PH_CMD_ASKC:
                puts("\n> ");
                gets((char*)buffer, PH_ADV_CASE_LEN_MAX);

                {
                    size_t n;
                    for(n=0;n<adv->case_count;n++){
                        if(streq(adv->case_buffer[n].name,
                                 buffer)){
                            target = adv->case_buffer[n].offset;
                            adv->cur = target;
                            break;
                        }
                    }
                    if(n == adv->case_count){
                        puts("Invalid input\n");
                        break;
                    }
                }
                if(c == PH_CMD_ASKC) adv->case_count = 0;
                break;
            case PH_CMD_STARTVERBATIM:
                puts("Verbatim start\n");
                adv->cur++;
                break;
            case PH_CMD_ENDVERBATIM:
                puts("Verbatim end\n");
                adv->cur++;
                break;

            default:
                /* TODO: Add word wrap etc. */
                before = get_cur_x();
                putc(_C);
                if(get_cur_x() < before || _C == '\n') lines++;
                if(lines >= 23){
                    puts("Continue...");
                    while(!(*in_reg));
                    lines = 0;
                    putc('\n');
                }
                adv->cur++;
        }
    }
}
