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

#ifndef PHOSPHOR_FORMAT_H
#define PHOSPHOR_FORMAT_H

#define PH_CMD_VERSION 1

#define PH_CMD_START 0x80
#define PH_CMD_AMOUNT (PH_CMD_END-PH_CMD_START-1)

#define PH_CONV_TOKEN_MAX 32
#define PH_CONV_CMD_MAX_TOKENS 8

enum {
    PH_CMD_STARTVERBATIM = PH_CMD_START,
    PH_CMD_ENDVERBATIM,
    PH_CMD_CLEAR,
    PH_CMD_HALIGN,
    PH_CMD_VALIGN,
    PH_CMD_SETX,
    PH_CMD_SETY,
    PH_CMD_PAGEBREAK,
    PH_CMD_LABEL,
    PH_CMD_GOTO,
    PH_CMD_CASE,
    PH_CMD_DCASE,
    PH_CMD_CLEARCASES,
    PH_CMD_ASK,
    PH_CMD_ASKC,
    PH_CMD_DELAY,
    PH_CMD_NOTE,
    PH_CMD_STARTBGM,
    PH_CMD_ENDBGM,

    PH_CMD_EXTENDED,

    PH_CMD_END
};

enum {
    PH_CMD_ALIGN_LEFT   = 0,
    PH_CMD_ALIGN_TOP    = 0,
    PH_CMD_ALIGN_CENTER = 1,
    PH_CMD_ALIGN_RIGHT  = 2,
    PH_CMD_ALIGN_BOTTOM = 2
};

#endif
