/* A small RV32I emulator written in JS.
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

debug = 0;

output = document.getElementById("output");

function loadBinary(url, onLoad, error) {
    var xmlhttp = new XMLHttpRequest();
    xmlhttp.onreadystatechange = () => {
        if(xmlhttp.status == 200 && xmlhttp.readyState == 4){
            var data = Array.from(new Uint8Array(xmlhttp.response));
            onLoad(data);
        }else if(xmlhttp.readyState == 4){
            error();
        }
    };
    xmlhttp.open("GET", url, true);
    xmlhttp.send();
    xmlhttp.responseType = "arraybuffer";
}

function start() {
    var keyqueue = [];
    const out = {};

    termInit(out, document.getElementById("terminal"));

    const msg = "Press any key to start...";
    for(var i=0;i<msg.length;i++){
        termPutC(out, msg[i]);
    }

    window.onkeydown = (event) => {
        loadBinary("main", (rom) => {
            const debug = 0;
            const rtDebug = 0;
            const runOnce = 0;
            const stepInstrs = 2000;

            console.log(rom);
            const cpu = {};

            var ram = [];

            var time = Math.floor(Date.now());

            var writeTmp;

            // Audio output
            const audioCtx = new AudioContext();
            const oscillator = audioCtx.createOscillator();

            oscillator.type = "square";
            oscillator.connect(audioCtx.destination);
            oscillator.start();
            oscillator.frequency.setValueAtTime(0, audioCtx.currentTime);

            // Add an event listener to handle keypresses
            window.onkeydown = (event) => {
                var id = event.key.charCodeAt(0);
                if(event.key == "Enter") id = 0x0A;
                else if(event.key == "Backspace") id = 0x7F;
                else if(id < 0x20 || id > 0x7F || event.key.length != 1) return;

                if(keyqueue.length >= 255){
                    /* Keep the queue from filling up too much. */
                    keyqueue.shift();
                }
                keyqueue.push(id);
            }

            function r(rv, addr) {
                if(addr < 1024*1024){
                    return ram[addr];
                }else if(addr < 1024*1024+16){
                    switch(addr){
                        case 1024*1024+1:
                            /* stdin */
                            if(keyqueue.length > 0){
                                id = keyqueue.shift();
                                console.log("in", id);
                                return id&0xFF;
                            }
                            break;

                        case 1024*1024+8:
                            /* Cursor X */
                            return out.x;

                        case 1024*1024+9:
                            /* Cursor X high byte */
                            return out.x>>8;

                        case 1024*1024+10:
                            /* Cursor Y */
                            return out.y;

                        case 1024*1024+11:
                            /* Cursor Y high byte */
                            return out.y>>8;

                        case 1024*1024+4:
                            /* Lowest timestamp byte (also used to update the
                             * timestamp) */
                            time = Math.floor(Date.now());
                            return time&0xFF;
                        case 1024*1024+5:
                            /* Timestamp byte 1 */
                            return (time>>8)&0xFF;
                        case 1024*1024+6:
                            /* Timestamp byte 2 */
                            return (time>>16)&0xFF;
                        case 1024*1024+7:
                            /* Timestamp byte 3 */
                            return (time>>24)&0xFF;
                    }
                    return 0; /* TODO */
                }
                return rom[(addr-(1024*1024+16))%(1024*1024)];
            }

            function w(rv, addr, byte) {
                if(addr < 1024*1024){
                    ram[addr] = byte;
                }else if(addr < 1024*1024+16){
                    /* I/O registers */
                    switch(addr){
                        case 1024*1024:
                            /* Console output register */
                            termPutC(out, String.fromCharCode(byte));
                            break;

                        case 1024*1024+8:
                            /* Cursor X low byte */
                            writeTmp = byte;
                            break;

                        case 1024*1024+9:
                            /* Cursor X high byte */
                            termSetX(out, writeTmp|(byte<<8));
                            break;

                        case 1024*1024+10:
                            /* Cursor Y */
                            writeTmp = byte;
                            break;

                        case 1024*1024+11:
                            /* Cursor Y high byte */
                            termSetY(out, writeTmp|(byte<<8));
                            break;

                        case 1024*1024+2:
                            /* Audio out */
                            /* Semitones:
                             *
                             * 0:  C
                             * 1:  C#
                             * 2:  D
                             * 3:  D#
                             * 4:  E
                             * 5:  F
                             * 6:  F#
                             * 7:  G
                             * 8:  G#
                             * 9:  A
                             * 10: A#
                             * 11: B
                             */
                            var octave = byte&7;
                            var semitone = byte>>3;
                            var ref_pitch = 440;
                            var frequency = (16.35*(1<<octave))*2**(semitone/12);
                            if(byte&0x80){
                                /* Stop audio by setting the 7th bit */
                                frequency = 0;
                            }
                            oscillator.frequency.setValueAtTime(frequency,
                                                                audioCtx
                                                                .currentTime);
                            break;
                    }
                }
            }

            if(debug){
                console.log("--- Disassembly start ---");

                RVInit(cpu, 1024*1024+16, r, w);

                while(cpu.pc <= 1024*1024+16+rom.length+4){
                    try{
                        RVLoadInstr(cpu);
                        console.log((cpu.pc-4).toString(16) + ": " +
                                    RVDisAs(cpu, 0));
                    }catch(e){
                        console.log((cpu.pc-4).toString(16) + ": <unknown>");
                    }
                }

                console.log("--- Disassembly end   ---")
            }

            RVInit(cpu, 1024*1024+16, r, w);

            function run(timestamp) {
                for(i=0;i<stepInstrs || cpu.jam;i++){
                    RVLoadInstr(cpu);
                    if(rtDebug){
                        console.log(RVGetEmuState(cpu, false));
                    }
                    RVRunInstr(cpu);
                }

                termUpdate(out, timestamp);

                if(cpu.jam) console.log("Jammed!");
                else if(!runOnce) requestAnimationFrame(run);
            }
            requestAnimationFrame(run);
        }, () => {
            alert("Failed to load code");
        });
    }
}

start();
