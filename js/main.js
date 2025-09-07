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
    keyqueue = []

    // Add an event listener to handle keypresses
    window.onkeydown = (event) => {
        var id = event.key.charCodeAt(0);
        console.log(event.key);
        if(event.key == "Enter") id = 0x0A;
        else if(event.key == "Backspace") id = 0x7F;
        else if(id < 0x20 || id > 0x7F || event.key.length != 1) return;

        keyqueue.push(id);
    }

    loadBinary("main", (rom) => {
        const debug = 0;
        const rtDebug = 0;
        const runOnce = 0;
        const stepInstrs = 2000;

        console.log(rom);
        const cpu = {};
        const out = {};

        var ram = [];

        function r(rv, addr) {
            if(addr < 1024*1024){
                return ram[addr];
            }else if(addr < 1024*1024+16){
                if(addr == 1024*1024+1){
                    /* stdin */
                    if(keyqueue.length > 0){
                        id = keyqueue.shift();
                        console.log("in", id);
                        return id&0xFF;
                    }
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
                if(addr == 1024*1024){
                    /* Console output register */
                    termPutC(out, String.fromCharCode(byte));
                }else if(addr == 1024*1024+1){
                    /* Console output register */
                    console.log(byte);
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
        termInit(out, document.getElementById("terminal"));

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

start();
