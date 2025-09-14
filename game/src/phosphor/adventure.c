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

static volatile char *const in_reg = (void*)(1024*1024+1);

#define _VALID(c) ((c) < PH_CMD_START || (c) >= PH_CMD_END)

void ph_adventure_run(PHAdventure *adv) {
    size_t target;
    size_t start;
    unsigned char c;

    static unsigned char buffer[PH_ADV_CASE_LEN_MAX];

    unsigned char lines = 0;

    unsigned short int before;

    unsigned char halign = PH_CMD_ALIGN_LEFT;
    unsigned char valign = PH_CMD_ALIGN_TOP;

    unsigned char note;

    unsigned char loading_bgm = 0;

    unsigned short int w, h;

    unsigned char verbatim = 0;

    unsigned short int x;

    size_t i;

    term_size(&w, &h);

    while(1){
        switch((c = _C)){
            case PH_CMD_STARTVERBATIM:
                verbatim = 1;
                adv->cur++;
                break;

            case PH_CMD_ENDVERBATIM:
                verbatim = 0;
                adv->cur++;
                break;

            case PH_CMD_CLEAR:
                for(i=0;i<h;i++){
                    putc('\n');
                }
                set_cur_x(0);
                set_cur_y(0);
                adv->cur++;
                break;

            case PH_CMD_HALIGN:
                adv->cur++;
                halign = _C&3;
                adv->cur++;
                break;

            case PH_CMD_VALIGN:
                adv->cur++;
                valign = _C&3;
                adv->cur++;
                break;

            case PH_CMD_SETX:
                adv->cur++;
                target = _C;
                adv->cur++;
                target |= _C<<8;
                adv->cur++;
                set_cur_x(target);
                break;

            case PH_CMD_SETY:
                adv->cur++;
                target = _C;
                adv->cur++;
                target |= _C<<8;
                adv->cur++;
                set_cur_x(target);
                break;

            case PH_CMD_PAGEBREAK:
                if(get_cur_y()){
                    set_cur_x(0);
                    set_cur_y(h-1);
                    puts("Continue...");
                    while(!(*in_reg));

                    for(i=0;i<h;i++){
                        putc('\n');
                    }
                    set_cur_x(0);
                    set_cur_y(0);
                }
                adv->cur++;
                break;

            case PH_CMD_LABEL:
                /* NOTE: This command shouldn't occur in normal conditions */
                break;

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

            case PH_CMD_CLEARCASES:
                adv->case_count = 0;
                break;

            case PH_CMD_ASK:
            case PH_CMD_ASKC:
                puts("\n> ");
                gets((char*)buffer, PH_ADV_CASE_LEN_MAX);

                {
                    size_t n;
                    for(n=0;n<adv->case_count;n++){
                        if(!strcmp((char*)adv->case_buffer[n].name,
                                   (char*)buffer)){
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

            case PH_CMD_DELAY:
                adv->cur++;
                target = _C;
                adv->cur++;
                target |= _C<<8;
                adv->cur++;
                start = mstime();
                while(mstime()-start < target){
                    /* Run the sound engine */
                }
                break;

            case PH_CMD_NOTE:
                adv->cur++;
                note = _C;
                adv->cur++;
                target = _C;
                adv->cur++;
                target |= _C<<8;
                adv->cur++;
                if(loading_bgm){
                    /* TODO */
                }else{
                    beep(note, target);
                }
                break;

            case PH_CMD_STARTBGM:
                loading_bgm = 1;
                break;

            case PH_CMD_ENDBGM:
                loading_bgm = 0;
                break;

            default:
                /* TODO: Add word wrap etc. */
                if(_VALID(_C)){
                    if(verbatim){
                        putc(_C);
                        adv->cur++;
                    }else{
                        /* Wrap */
                        start = adv->cur;
                        for(i=0;i<w && _VALID(_C) && _C>=0x20;){
                            size_t n;

                            /* Check if text up to the next boundary can fit
                             * on this line. */

                            target = adv->cur;
                            for(n=i;;n++,adv->cur++){
                                if(n >= w){
                                    adv->cur = target;
                                    break;
                                }
                                if(_C == ' ' || _C == '\t' || _C == '\n' ||
                                   !_VALID(_C)) break;
                            }
                            if(!_VALID(_C)) break;
                            if(n < w){
                                if(_C == '\n'){
                                    adv->cur++;
                                    break;
                                }
                                adv->cur++;
                                n++;
                            }
                            i=n;
                        }

                        /* NOTE: target contains the width here */
                        target = adv->cur-start;

                        if(halign == PH_CMD_ALIGN_LEFT){
                            x = 0;
                        }else if(halign == PH_CMD_ALIGN_CENTER){
                            x = (w-1-target)/2;
                        }else if(halign == PH_CMD_ALIGN_RIGHT){
                            x = w-1-target;
                        }

                        adv->cur = start;

                        set_cur_x(x);
                        for(i=0;i<target;i++){
                            putc(_C);
                            adv->cur++;
                        }
                        putc('\n');
                        if(get_cur_y() >= h-1){
                            set_cur_x(0);
                            puts("Continue...");
                            while(!(*in_reg));

                            for(i=0;i<h;i++){
                                putc('\n');
                            }
                            set_cur_x(0);
                            set_cur_y(0);
                        }
                    }
                }
        }
    }
}
