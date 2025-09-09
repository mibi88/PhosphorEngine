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

function termInit(term, div, w, h) {
    term.div = div;

    term.x = 0;
    term.y = 0;

    term.w = w;
    term.h = h;

    for(var i=0;i<term.h;i++){
        var pre = document.createElement("pre");
        pre.id = "terminal-row-" + i;
        pre.style.margin = 0;
        pre.innerHTML += " ".repeat(term.w);
        term.div.appendChild(pre);
    }
}

function termScroll(term) {
    var pre1;
    for(i=1;i<term.h;i++){
        pre1 = document.getElementById("terminal-row-" + i);
        var pre2 = document.getElementById("terminal-row-" + (i-1));
        pre2.textContent = pre1.textContent
    }
    pre1.textContent = " ".repeat(term.w);

    term.y--;
}

function __termRemoveCur(term) {
    var pre = document.getElementById("terminal-row-" + term.y);
    try{
        var char = document.getElementById("terminal-cursor").textContent;
        var before = pre.textContent.substring(0, term.x);
        var after = pre.textContent.substring(term.x+1);

        pre.textContent = before + char + after;
    }catch(e){
        console.log("Failed to remove cursor");
    }
}

function __termAddCur(term) {
    const escape = (str) => {
        return str.replace(/&/g, "&amp;")
                  .replace(/</g, "&lt;")
                  .replace(/>/g, "&gt;")
                  .replace(/"/g, "&quot;")
                  .replace(/'/g, "&#039;");
    }

    const curStart = "<span id=\"terminal-cursor\">";
    const curEnd = "</span>";

    var pre = document.getElementById("terminal-row-" + term.y);
    var before = escape(pre.textContent.substring(0, term.x));
    var after = escape(pre.textContent.substring(term.x+1));
    var char = escape(pre.textContent.charAt(term.x));

    pre.innerHTML = before + curStart + char + curEnd +
                    after;
}

function termSetX(term, x) {
    if(x < 0) x = 0;
    else if(x >= term.w-1) x = term.w-1;

    __termRemoveCur(term);
    term.x = x;
    __termAddCur(term);
}

function termSetY(term, y) {
    if(y < 0) y = 0;
    else if(y >= term.h-1) y = term.h-1;

    __termRemoveCur(term);
    term.y = y;
    __termAddCur(term);
}

function termPutC(term, char) {
    __termRemoveCur(term);

    const down = (term) => {
        term.y++;
        if(term.y >= term.h){
            termScroll(term);
        }
    };
    const newLine = (term) => {
        down(term);
        term.x = 0;
    };

    if(char == '\n'){
        newLine(term);
    }else if(char.charCodeAt(0) == 0x7F){
        /* Backspace: Moves the cursor back one char (the program handles
         * erasing) */
        term.x--;
        if(term.x < 0){
            term.x = term.w-1;
            if(term.y > 0) term.y--;
            else term.x = 0;
        }
    }else{
        var pre = document.getElementById("terminal-row-" + term.y);
        if(char.charCodeAt(0) != 0x11){
            /* Using 0x12 (ASCII DC2) to advance without writing any text. */
            pre.textContent = pre.textContent.substring(0, term.x) + char +
                              pre.textContent.substring(term.x+1);
        }
        term.x++;
    }
    if(term.x >= term.w){
        newLine(term);
    }

    __termAddCur(term);
}

function termUpdate(term, timestamp) {
    /* Let the cursor blink. */
    const delta = 600;
    const fgColor = "#FFBF00";
    const bgColor = "black";

    var s = (timestamp/delta)&1;
    var cur = document.getElementById("terminal-cursor");
    if(s){
        cur.style.backgroundColor = fgColor;
        cur.style.color = bgColor;
    }else{
        cur.style.backgroundColor = bgColor;
        cur.style.color = fgColor;
    }
}
