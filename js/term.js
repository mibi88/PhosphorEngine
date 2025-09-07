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

function termInit(term, div) {
    term.div = div;
    term.x = 0;
    term.y = 0;
    for(var i=0;i<24;i++){
        var pre = document.createElement("pre");
        pre.id = "terminal-row-" + i;
        pre.style.margin = 0;
        pre.innerHTML += " ".repeat(80);
        term.div.appendChild(pre);
    }
}

function termScroll(term) {
    // TODO
    var pre1;
    for(i=1;i<24;i++){
        pre1 = document.getElementById("terminal-row-" + i);
        var pre2 = document.getElementById("terminal-row-" + (i-1));
        pre2.innerText = pre1.innerText
    }
    pre1.innerText = " ".repeat(80);

    term.y--;
}

function termPutC(term, char) {
    const down = (term) => {
        term.y++;
        if(term.y >= 24){
            termScroll(term);
        }
    };
    const newLine = (term) => {
        down(term);
        term.x = 0;
    };

    if(char == '\n'){
        newLine(term);
    }else{
        var idx = term.x;
        var pre = document.getElementById("terminal-row-" + term.y);
        pre.innerText = pre.innerText.substring(0, idx) + char +
                        pre.innerText.substring(idx+1);
        term.x++;
    }
    if(term.x >= 80){
        newLine(term);
    }
}

function termUpdate(term, timestamp) {
    /* Let the cursor blink. */
}
