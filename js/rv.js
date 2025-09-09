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

function RVInit(rv, start, read, write) {
    rv.regs = [];
    rv.regs.length = 32;
    rv.regs.fill(1024*1024-1, 0, 32);

    rv.read = read;
    rv.write = write;

    rv.pc = start;

    rv.jam = 0;
}

function RVLoadInstr(rv) {
    const R = 0;
    const I = 1;
    const S = 2;
    const B = 3;
    const U = 4;
    const J = 5;
    const F = 6; /* For FENCE */
    const E = 7; /* For ECALL and EBREAK */

    const instrTypes = {
        0x37: U, /* 0110111 LUI */
        0x17: U, /* 0010111 AUIPC */
        0x6f: J, /* 1101111 JAL */
        0x63: B, /* 1100011 Branch instructions */
        0x67: I, /* 1100111 JALR */
        0x03: I, /* 0000011 Load instructions */
        0x13: I, /* 0010011 Register-immediate and constant shift
                  * instructions */
        0x23: S, /* 0100011 Store instructions */
        0x33: R, /* 0110011 Register-register instructions */
        0x0f: F, /* 0001111 FENCE */
        0x73: E  /* 1110011 ECALL and EBREAK */
    };

    instr = rv.read(rv, rv.pc);
    instr |= rv.read(rv, rv.pc+1)<<8;
    instr |= rv.read(rv, rv.pc+2)<<16;
    instr |= rv.read(rv, rv.pc+3)<<24;
    rv.pc += 4;

    rv.opcode = instr&0x7F;

    rv.type = instrTypes[rv.opcode];
    switch(rv.type){
        case R:
            rv.rd = (instr>>7)&31;
            rv.funct3 = (instr>>12)&7;
            rv.rs1 = (instr>>15)&31;
            rv.rs2 = (instr>>20)&31;
            rv.funct7 = (instr>>25)&127;
            break;
        case I:
            rv.rd = (instr>>7)&31;
            rv.funct3 = (instr>>12)&7;
            rv.rs1 = (instr>>15)&31;
            rv.imm = instr>>20;
            rv.funct7 = (instr>>25)&127;
            break;
        case S:
            rv.imm = (instr>>7)&31;
            rv.funct3 = (instr>>12)&3;
            rv.rs1 = (instr>>15)&31;
            rv.rs2 = (instr>>20)&31;
            rv.imm |= (instr>>25)<<5;
            break;
        case B:
            rv.imm = ((instr>>7)&1)<<11;
            rv.imm |= ((instr>>8)&15)<<1;
            rv.funct3 = (instr>>12)&7;
            rv.rs1 = (instr>>15)&31;
            rv.rs2 = (instr>>20)&31;
            rv.imm |= ((instr>>25)&63)<<5;
            rv.imm |= (instr>>31)<<12;
            break;
        case U:
            rv.rd = (instr>>7)&31;
            rv.imm = instr&0xfffff000;
            rv.funct3 = 0;
            break;
        case J:
            rv.rd = (instr>>7)&31;
            rv.imm = instr&0xff000;
            rv.imm |= ((instr>>20)&1)<<11;
            rv.imm |= ((instr>>21)&0x3FF)<<1;
            rv.imm |= (instr>>31)<<20;
            rv.funct3 = 0;
            break;
    }
}

function RVGetInstr(rv) {
    const instrLUT = {
        /* Opcodes */
        0x37: {
            /* funct3 */
            0: (rv) => {
                /* LUI */
                return "LUI";
            }
        },
        0x17: {
            /* funct3 */
            0: (rv) => {
                /* AUIPC */
                return "AUIPC";
            }
        },
        0x6f: {
            /* funct3 */
            0: (rv) => {
                /* JAL */
                return "JAL";
            }
        },
        0x63: {
            /* funct3 */
            0: (rv) => {
                /* BEQ */
                return "BEQ";
            },
            1: (rv) => {
                /* BNE */
                return "BNE";
            },
            4: (rv) => {
                /* BLT */
                return "BLT";
            },
            5: (rv) => {
                /* BGE */
                return "BGE";
            },
            6: (rv) => {
                /* BLTU */
                return "BLTU";
            },
            7: (rv) => {
                /* BGEU */
                return "BGEU";
            }
        },
        0x67: {
            /* funct3 */
            0: (rv) => {
                /* JALR */
                return "JALR";
            }
        },
        0x03: {
            /* funct3 */
            0: (rv) => {
                /* LB */
                return "LB";
            },
            1: (rv) => {
                /* LH */
                return "LH";
            },
            2: (rv) => {
                /* LW */
                return "LW";
            },
            4: (rv) => {
                /* LBU */
                return "LBU";
            },
            5: (rv) => {
                /* LHU */
                return "LHU";
            }
        },
        0x13: {
            /* funct3 */
            0: (rv) => {
                /* ADDI */
                return "ADDI";
            },
            2: (rv) => {
                /* SLTI */
                return "SLTI";
            },
            3: (rv) => {
                /* SLTIU */
                return "SLTIU";
            },
            4: (rv) => {
                /* XORI */
                return "XORI";
            },
            6: (rv) => {
                /* ORI */
                return "ORI";
            },
            7: (rv) => {
                /* ANDI */
                return "ANDI";
            },

            1: (rv) => {
                /* SLLI */
                return "SLLI";
            },
            5: (rv) => {
                if(rv.funct7){
                    /* SRAI */
                    return "SRAI";
                }else{
                    /* SRLI */
                    return "SRLI";
                }
            }
        },
        0x23: {
            /* funct3 */
            0: (rv) => {
                /* SB */
                return "SB";
            },
            1: (rv) => {
                /* SH */
                return "SH";
            },
            2: (rv) => {
                /* SW */
                return "SW";
            }
        },
        0x33: {
            /* funct3 */
            0: (rv) => {
                if(rv.funct7){
                    /* SUB */
                    return "SUB";
                }else{
                    /* ADD */
                    return "ADD";
                }
            },
            1: (rv) => {
                /* SLL */
                return "SLL";
            },
            2: (rv) => {
                /* SLT */
                return "SLT";
            },
            3: (rv) => {
                /* SLTU */
                return "SLTU";
            },
            4: (rv) => {
                /* XOR */
                return "XOR";
            },
            5: (rv) => {
                if(rv.funct7){
                    /* SRA */
                    return "SRA";
                }else{
                    /* SRL */
                    return "SRL";
                }
            },
            6: (rv) => {
                /* OR */
                return "OR";
            },
            7: (rv) => {
                /* AND */
                return "AND";
            }
        },
        0x0f: {
            /* funct3 */
            0: (rv) => {
                /* FENCE */
                return "FENCE";
                /* Implemented as a NOP, as we only simulate a single hart and
                 * run every instruction one after another. */
            }
        },
        0x73: {
            /* funct3 */
            0: (rv) => {
                /* ECALL/EBREAK */
                return "ECALL/EBREAK";
            }
        }
    };

    return instrLUT[rv.opcode][rv.funct3](rv);
}

function RVDisAs(rv, showRegNums) {
    const R = 0;
    const I = 1;
    const S = 2;
    const B = 3;
    const U = 4;
    const J = 5;
    const F = 6; /* For FENCE */
    const E = 7; /* For ECALL and EBREAK */

    const ABINames = [
        "zero",
        "ra",
        "sp",
        "gp",
        "tp",
        "t0",
        "t1",
        "t2",
        "s0", /* or fp */
        "s1",
        "a0",
        "a1",
        "a2",
        "a3",
        "a4",
        "a5",
        "a6",
        "a7",
        "s2",
        "s3",
        "s4",
        "s5",
        "s6",
        "s7",
        "s8",
        "s9",
        "s10",
        "s11",
        "t3",
        "t4",
        "t5",
        "t6"
    ];

    if(showRegNums){
        var rd = "x" + rv.rd;
        var rs1 = "x" + rv.rs1;
        var rs2 = "x" + rv.rs2;
    }else{
        var rd = ABINames[rv.rd];
        var rs1 = ABINames[rv.rs1];
        var rs2 = ABINames[rv.rs2];
    }

    const disAsLUT = [
        (rv) => {
            /* R */
            return RVGetInstr(rv) + " " + rd + ", " + rs1 + ", " + rs2;
        },
        (rv) => {
            /* I */
            if(rv.opcode == 0x03 || rv.opcode == 0x67){
                return RVGetInstr(rv) + " " + rd + ", " + rv.imm + "(" +
                       rs1 + ")";
            }else{
                return RVGetInstr(rv) + " " + rd + ", " + rs1 + ", " +
                       rv.imm;
            }
        },
        (rv) => {
            /* S */
            return RVGetInstr(rv) + " " + rs2 + ", " + rv.imm + "(" +
                   rs1 + ")";
        },
        (rv) => {
            /* B */
            return RVGetInstr(rv) + " " + rs1 + ", " + rs2 + ", " +
                   rv.imm;
        },
        (rv) => {
            /* U */
            return RVGetInstr(rv) + " " + rd + ", " + rv.imm;
        },
        (rv) => {
            /* J */
            return RVGetInstr(rv) + " " + rd + ", " + rv.imm;
        },
        (rv) => {
            /* F */
            return RVGetInstr(rv);
        },
        (rv) => {
            /* E */
            return RVGetInstr(rv);
        }
    ];

    return disAsLUT[rv.type](rv);
}

function RVGetEmuState(rv, showRegNums) {
    const ABINames = [
        "zero",
        "ra",
        "sp",
        "gp",
        "tp",
        "t0",
        "t1",
        "t2",
        "s0", /* or fp */
        "s1",
        "a0",
        "a1",
        "a2",
        "a3",
        "a4",
        "a5",
        "a6",
        "a7",
        "s2",
        "s3",
        "s4",
        "s5",
        "s6",
        "s7",
        "s8",
        "s9",
        "s10",
        "s11",
        "t3",
        "t4",
        "t5",
        "t6"
    ];

    var str = (rv.pc-4).toString(16) + ": " + RVDisAs(rv, 0);

    for(var i=0;i<32;i++){
        var l=str.length;
        for(var n=l;(n&7) || n == l;n++){
            str += " ";
        }
        str += (showRegNums ? "x" + i : ABINames[i]) + ": " + rv.regs[i];
    }

    return str;
}

function RVRunInstr(rv) {
    if(rv.jam) return;

    const toUint = (n) => {
        return n < 0 ? 0x100000000+n : n;
    };
    const toInt = (n) => {
        return n&0xFFFFFFFF;
    };
    const signExtend = (b, tb, n) => {
        return n<<(tb-b)>>(tb-b);
    };
    const signExtendU = (b, tb, n) => {
        return toUint(signExtend(b, tb, n));
    };

    const instrLUT = {
        /* Opcodes */
        0x37: {
            /* funct3 */
            0: (rv) => {
                /* LUI */
                rv.regs[rv.rd] = rv.imm;
            }
        },
        0x17: {
            /* funct3 */
            0: (rv) => {
                /* AUIPC */
                var i = toUint(rv.imm);
                rv.regs[rv.rd] = toInt(rv.pc+i-4);
            }
        },
        0x6f: {
            /* funct3 */
            0: (rv) => {
                /* JAL */
                rv.regs[rv.rd] = toInt(rv.pc);
                rv.pc += rv.imm-4;
            }
        },
        0x63: {
            /* funct3 */
            0: (rv) => {
                /* BEQ */
                if(rv.regs[rv.rs1] == rv.regs[rv.rs2]){
                    rv.pc += rv.imm-4;
                }
            },
            1: (rv) => {
                /* BNE */
                if(rv.regs[rv.rs1] != rv.regs[rv.rs2]){
                    rv.pc += rv.imm-4;
                }
            },
            4: (rv) => {
                /* BLT */
                if(rv.regs[rv.rs1] < rv.regs[rv.rs2]){
                    rv.pc += rv.imm-4;
                }
            },
            5: (rv) => {
                /* BGE */
                if(rv.regs[rv.rs1] >= rv.regs[rv.rs2]){
                    rv.pc += rv.imm-4;
                }
            },
            6: (rv) => {
                /* BLTU */
                var a = toUint(rv.regs[rv.rs1]);
                var b = toUint(rv.regs[rv.rs2]);
                if(a < b){
                    rv.pc += rv.imm-4;
                }
            },
            7: (rv) => {
                /* BGEU */
                var a = toUint(rv.regs[rv.rs1]);
                var b = toUint(rv.regs[rv.rs2]);
                if(a >= b){
                    rv.pc += rv.imm-4;
                }
            }
        },
        0x67: {
            /* funct3 */
            0: (rv) => {
                /* JALR */
                var v = rv.regs[rv.rs1];
                rv.regs[rv.rd] = toInt(rv.pc);
                rv.pc = toUint((rv.imm+v)&~1);
            }
        },
        0x03: {
            /* funct3 */
            0: (rv) => {
                /* LB */
                var addr = toUint(toInt(rv.imm+rv.regs[rv.rs1]));
                rv.regs[rv.rd] = signExtend(8, 32, rv.read(rv, addr));
            },
            1: (rv) => {
                /* LH */
                var addr = toUint(toInt(rv.imm+rv.regs[rv.rs1]));
                rv.regs[rv.rd] = rv.read(rv, addr);
                rv.regs[rv.rd] |= rv.read(rv, addr+1)<<8;
                rv.regs[rv.rd] = signExtend(16, 32, rv.regs[rv.rd]);
            },
            2: (rv) => {
                /* LW */
                var addr = toUint(toInt(rv.imm+rv.regs[rv.rs1]));
                rv.regs[rv.rd] = rv.read(rv, addr);
                rv.regs[rv.rd] |= rv.read(rv, addr+1)<<8;
                rv.regs[rv.rd] |= rv.read(rv, addr+2)<<16;
                rv.regs[rv.rd] |= rv.read(rv, addr+3)<<24;
            },
            4: (rv) => {
                /* LBU */
                var addr = toUint(toInt(rv.imm+rv.regs[rv.rs1]));
                rv.regs[rv.rd] = rv.read(rv, addr);
            },
            5: (rv) => {
                /* LHU */
                var addr = toUint(toInt(rv.imm+rv.regs[rv.rs1]));
                rv.regs[rv.rd] = rv.read(rv, addr);
                rv.regs[rv.rd] |= rv.read(rv, addr+1)<<8;
            }
        },
        0x13: {
            /* funct3 */
            0: (rv) => {
                /* ADDI */
                rv.regs[rv.rd] = toInt(rv.regs[rv.rs1]+rv.imm);
            },
            2: (rv) => {
                /* SLTI */
                if(rv.regs[rv.rs1] < rv.imm){
                    rv.regs[rv.rd] = 1;
                }else{
                    rv.regs[rv.rd] = 0;
                }
            },
            3: (rv) => {
                /* SLTIU */
                var a = toUint(rv.regs[rv.rs1]);
                if(a < toUint(rv.imm)){
                    rv.regs[rv.rd] = 1;
                }else{
                    rv.regs[rv.rd] = 0;
                }
            },
            4: (rv) => {
                /* XORI */
                rv.regs[rv.rd] = rv.regs[rv.rs1]^rv.imm;
            },
            6: (rv) => {
                /* ORI */
                rv.regs[rv.rd] = rv.regs[rv.rs1]|rv.imm;
            },
            7: (rv) => {
                /* ANDI */
                rv.regs[rv.rd] = rv.regs[rv.rs1]&rv.imm;
            },

            1: (rv) => {
                /* SLLI */
                rv.regs[rv.rd] = rv.regs[rv.rs1]<<(rv.imm&31);
            },
            5: (rv) => {
                if(rv.funct7){
                    /* SRAI */
                    rv.regs[rv.rd] = rv.regs[rv.rs1]>>(rv.imm&31);
                }else{
                    /* SRLI */
                    rv.regs[rv.rd] = (rv.regs[rv.rs1]>>(rv.imm&31))&
                                     (2**(32-(rv.imm&31))-1);
                }
            }
        },
        0x23: {
            /* funct3 */
            0: (rv) => {
                /* SB */
                var addr = toUint(toInt(rv.imm+rv.regs[rv.rs1]));
                rv.write(rv, addr, rv.regs[rv.rs2]&0xFF);
            },
            1: (rv) => {
                /* SH */
                var addr = toUint(toInt(rv.imm+rv.regs[rv.rs1]));
                rv.write(rv, addr, rv.regs[rv.rs2]&0xFF);
                rv.write(rv, addr+1, (rv.regs[rv.rs2]>>8)&0xFF);
            },
            2: (rv) => {
                /* SW */
                var addr = toUint(toInt(rv.imm+rv.regs[rv.rs1]));
                rv.write(rv, addr, rv.regs[rv.rs2]&0xFF);
                rv.write(rv, addr+1, (rv.regs[rv.rs2]>>8)&0xFF);
                rv.write(rv, addr+2, (rv.regs[rv.rs2]>>16)&0xFF);
                rv.write(rv, addr+3, (rv.regs[rv.rs2]>>24)&0xFF);
            }
        },
        0x33: {
            /* funct3 */
            0: (rv) => {
                if(rv.funct7){
                    /* SUB */
                    rv.regs[rv.rd] = toInt(rv.regs[rv.rs1]-rv.regs[rv.rs2]);
                }else{
                    /* ADD */
                    rv.regs[rv.rd] = toInt(rv.regs[rv.rs1]+rv.regs[rv.rs2]);
                }
            },
            1: (rv) => {
                /* SLL */
                rv.regs[rv.rd] = rv.regs[rv.rs1]<<(rv.regs[rb.rs2]&31);
            },
            2: (rv) => {
                /* SLT */
                if(rv.regs[rv.rs1] < rv.regs[rv.rs2]){
                    rv.regs[rv.rd] = 1;
                }else{
                    rv.regs[rv.rd] = 0;
                }
            },
            3: (rv) => {
                /* SLTU */
                var a = toUint(rv.regs[rv.rs1]);
                var b = toUint(rv.regs[rv.rs2]);
                if(a < b){
                    rv.regs[rv.rd] = 1;
                }else{
                    rv.regs[rv.rd] = 0;
                }
            },
            4: (rv) => {
                /* XOR */
                rv.regs[rv.rd] = rv.regs[rv.rs1]^rv.regs[rv.rs2];
            },
            5: (rv) => {
                if(rv.funct7){
                    /* SRA */
                    var shift = rv.regs[rv.rs2]&31;
                    rv.regs[rv.rd] = rv.regs[rv.rs1]>>shift;
                }else{
                    /* SRL */
                    var shift = rv.regs[rv.rs2]&31;
                    rv.regs[rv.rd] = (rv.regs[rv.rs1]>>shift)&
                                     (2**(32-shift)-1);
                }
            },
            6: (rv) => {
                /* OR */
                rv.regs[rv.rd] = rv.regs[rv.rs1]|rv.regs[rv.rs2];
            },
            7: (rv) => {
                /* AND */
                rv.regs[rv.rd] = rv.regs[rv.rs1]&rv.regs[rv.rs2];
            }
        },
        0x0f: {
            /* funct3 */
            0: (rv) => {
                /* FENCE */
                console.log("FENCE");
                /* Implemented as a NOP, as we only simulate a single hart and
                 * run every instruction one after another. */
            }
        },
        0x73: {
            /* funct3 */
            0: (rv) => {
                /* ECALL/EBREAK */
                console.log("ECALL/EBREAK");
                /* Jam the CPU for now */
                rv.jam = 1;
            }
        }
    };

    /* x0 is hardwired to 0. */
    rv.regs[0] = 0;
    instrLUT[rv.opcode][rv.funct3](rv);
    rv.regs[0] = 0;
}

function RVInstr(rv) {
    RVLoadInstr(rv);
    RVRunInstr(rv);
}

function RVStop(rv) {
    //
}
