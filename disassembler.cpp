#include "disassembler.h"

command_t get32_bit(command_t command, int start) {
    return (command >> start) % 2;
}

command_t get32_bits(command_t command, int start, int end) {
    return (command >> start) % (1 << (end - start + 1));
}

command_t get32_opcode(command_t command) {
    return command % (1 << 7); /// opcode - 6 - 0
}

command_t get32_rd(command_t command) {
    return (command >> 7) % (1 << 5);  /// rd - 11 - 7
}

command_t get32_funct3(command_t command) {
    return (command >> 12) % (1 << 3);  /// funct3 - 14 - 12
}

command_t get32_rs1(command_t command) {
    return (command >> 15) % (1 << 5);  /// rs1 - 19 - 15
}

command_t get32_rs2(command_t command) {
    return (command >> 20) % (1 << 5);  /// rs1 - 24 - 20
}

command_t get32_funct7(command_t command) {
    return (command >> 25) % (1 << 7);  /// funct3 - 31 - 25
}

std::map <Elf32_Word, command32_type > command32_types_by_opcode = {{0b11, I},
                                                          {0b1111, I},
                                                          {0b10011, I},
                                                          {0b10111, U},
                                                          {0b11011, I},
                                                          {0b100011, S},
                                                          {0b110011, R},
                                                          {0b110111, U},
                                                          {0b111011, R},
                                                          {0b1100011, B},
                                                          {0b1100111, I},
                                                          {0b1101111, J},
                                                          {0b1110011, I}};

command32_type get_command32_type(command_t command) {
    return command32_types_by_opcode[get32_opcode(command)];
}

command32_name get_command32_name_e(command_t command) {
    command32_type command_type = get_command32_type(command);
    auto opcode = get32_opcode(command);
    auto funct3 = get32_funct3(command);
    auto funct7 = get32_funct7(command);

    switch (command_type) {
        case I: {
            switch (opcode) {
                case 0b11: {
                    switch (funct3) {
                        case 0: return i_lb;        //RV32I
                        case 0b1: return i_lh;      //RV32I
                        case 0b10: return i_lw;     //RV32I
                        case 0b11: return i_128;
                        case 0b100: return i_lbu;      //RV32I
                        case 0b101: return i_lhu;      //RV32I
                        case 0b110: return i_128;
                    }
                } return i_unknown;
                case 0b1111: {
                    switch (funct3) {
                        case 0: return i_fence;       //RV32I
                        case 0b1: return i_fence_i;
                    }

                } return i_unknown;
                case 0b10011: {
                    switch (funct3) {
                        case 0: return i_addi;       //RV32I
                        case 0b1: {
                            switch (funct7) {
                                case 0: return i_slli;      //RV32I
                            }
                        } break;
                        case 0b10: return i_slti;       //RV32I
                        case 0b11: return i_sltiu;      //RV32I
                        case 0b100: return i_ori;      //RV32I
                        case 0b101: {
                            switch (funct7) {
                                case 0:  return i_srli;      //RV32I
                                case 0b0100000: return i_srai;      //RV32I
                            }
                        } return i_unknown;
                        case 0b110: return i_ori;      //RV32I
                        case 0b111: return i_andi;      //RV32I
                    }
                }  return i_unknown;
                case 0b11011: {
                    switch (funct3) {
                        case 0: return i_128;
                        case 0b1:  {
                            switch (funct7) {
                                case 0: return i_128;
                            }
                        } return i_unknown;
                        case 0b101: {
                            switch (funct7) {
                                case 0:  return i_128;
                                case 0b0100000: return i_128;
                            }
                        } return i_unknown;
                    }
                } return i_unknown;
                case 0b1100111: {
                    switch (funct3) {
                        case 0: return i_jalr;  //RV32I
                    }
                } return i_unknown;
                case 0b1110011: {
                    switch (funct3) {
                        case 0: {
                            switch (funct7) {
                                case 0:   return i_ecall;       //RV32I
                                case 0b1: return i_ebreak;      //RV32I
                            }
                        } return i_unknown;
                        case 0b1: return i_csrrw;
                        case 0b10: return i_csrrs;
                        case 0b11: return i_csrrc;
                        case 0b101: return i_csrrwi;
                        case 0b110: return i_csrrsi;
                        case 0b111: return i_csrrci;
                    }
                }  return i_unknown;
            }
        } return i_unknown;
        case U: {
            switch (opcode) {
                case 0b10111: return i_auipc; //RV32I
                case 0b110111: return i_lui;  //RV32I
            }
        } return i_unknown;
        case S: {
            switch (opcode) {
                case 0b100011: {
                    switch (funct3) {
                        case 0: return i_sb;      //RV32I
                        case 0b1: return i_sh;      //RV32I
                        case 0b10: return i_sw;      //RV32I
                        case 0b11: return i_128;
                    }
                }
            }
        } return i_unknown;
        case R: {
            switch (opcode) {
                case 0b110011: {
                    switch (funct3) {
                        case 0: {
                            switch (funct7) {
                                case 0:  return i_add;      //RV32I
                                case 0b1:  return m_mul;    //RV32M
                                case 0b0100000: return i_sub;      //RV32I
                            }
                        } return i_unknown;
                        case 0b1: {
                            switch (funct7) {
                                case 0:  return i_sll;      //RV32I
                                case 0b1:  return m_mulh;    //RV32M
                            }
                        } return i_unknown;
                        case 0b10: {
                            switch (funct7) {
                                case 0:  return i_slt;      //RV32I
                                case 0b1:  return m_mulhsu;    //RV32M
                            }
                        } return i_unknown;
                        case 0b11: {
                            switch (funct7) {
                                case 0:  return i_sltu;      //RV32I
                                case 0b1:  return m_mulhu;    //RV32M
                            }
                        } return i_unknown;
                        case 0b100: {
                            switch (funct7) {
                                case 0:  return i_xor;      //RV32I
                                case 0b1:  return m_div;    //RV32M
                            }
                        } return i_unknown;
                        case 0b101: {
                            switch (funct7) {
                                case 0:  return i_srl;      //RV32I
                                case 0b1:  return m_divu;    //RV32M
                                case 0b0100000: return i_sra;      //RV32I
                            }
                        } return i_unknown;
                        case 0b110: {
                            switch (funct7) {
                                case 0:  return i_or;      //RV32I
                                case 0b1:  return m_rem;    //RV32M
                            }
                        } return i_unknown;
                        case 0b111: {
                            switch (funct7) {
                                case 0:  return i_and;      //RV32I
                                case 0b1:  return m_remu;    //RV32M
                            }
                        } return i_unknown;
                    } return i_unknown;
                } return i_unknown;
                case 0b111011: {
                    switch (funct3) {
                        case 0: {
                            switch (funct7) {
                                case 0:  return i_128;
                                case 0b0100000: return i_128;
                            }
                        } return i_unknown;
                        case 0b1: {
                            switch (funct7) {
                                case 0:  return i_128;
                            }
                        } return i_unknown;
                        case 0b101: {
                            switch (funct7) {
                                case 0:  return i_128;
                                case 0b0100000: return i_128;
                            }
                        } return i_unknown;
                    }
                } return i_unknown;

            } return i_unknown;
        } return i_unknown;
        case B: {
            switch (opcode) {
                case 0b1100011: {
                    switch (funct3) {
                        case 0: return i_beq;      //RV32I
                        case 0b1: return i_bne;    //RV32I
                        case 0b100: return i_blt;  //RV32I
                        case 0b101: return i_bge;  //RV32I
                        case 0b110: return i_bltu; //RV32I
                        case 0b111: return i_bgeu; //RV32I
                    }
                } return i_unknown;
            } return i_unknown;
        } return i_unknown;
        case J: {
            switch (opcode) {
                case 0b1101111: return i_jal;  //RV32I
            }
        } return i_unknown;
    }
    return i_unknown;
}

std::string get_command32_name(command_t command) {
    switch (get_command32_name_e(command)) {
        case i_unknown: return "unknown_command";
  case              i_128: return "unknown_command";
        case             i_hint: return "unknown_command";
        case           i_hse: return "unknown_command";
        case          i_float: return "unknown_command";
        case            i_illegal: return "unknown_command";
        case          i_res: return "unknown_command";
        case         i_lui: return "lui";
        case         i_auipc: return "auipc";
        case        i_jal: return "jal";
        case        i_jalr: return "jalr";
        case        i_beq: return "beq";
        case        i_bne: return "bne";
        case       i_blt: return "blt";
        case       i_bge: return "bge";
        case       i_bltu: return "bltu";
        case       i_bgeu: return "bgeu";
        case      i_lb: return "lb";
        case      i_lw: return "lw";
        case      i_lh: return "lh";
        case      i_lbu: return "lbu";
        case      i_lhu: return "lhu";
        case      i_sb: return "sb";
        case      i_sw: return "sw";
        case      i_sh: return "sh";
        case      i_addi: return "addi";
        case      i_slti: return "slti";
        case      i_sltiu: return "sltiu";
        case      i_xori: return "xori";
        case      i_ori: return "ori";
        case      i_andi: return "andi";
        case     i_slli: return "slli";
        case     i_srli: return "srli";
        case      i_srai: return "srai";
        case      i_add: return "add";
        case      i_sub: return "sub";
        case      i_sll: return "sll";
        case      i_slt: return "slt";
        case      i_sltu: return "sltu";
        case     i_xor: return "xor";
        case      i_srl: return "srl";
        case      i_sra: return "sra";
        case      i_or: return "or";
        case      i_and: return "and";
        case      i_fence: return "fence";
        case      i_fence_i: return "fence.i";
        case     i_ecall: return "ecall";
        case      i_ebreak: return "ebreak";
        case       i_csrrw: return "csrrw";
        case      i_csrrs: return "csrrs";
        case       i_csrrc: return "csrrc";
        case      i_csrrwi: return "csrrwi";
        case      i_csrrsi: return "csrrsi";
        case       i_csrrci: return "csrrci";
        case       m_mul: return "mul";
        case      m_mulh: return "mulh";
        case      m_mulhsu: return "mulhsu";
        case      m_mulhu: return "mulhu";
        case      m_div: return "div";
        case      m_divu: return "divu";
        case      m_rem: return "rem";
        case      m_remu: return "remu";
    }
}

std::string get_command32_s1(command_t command) {
    switch (get_command32_type(command)) {
        case R:
            return get_command32_rd(command);
        case I:
            return get_command32_rd(command);
        case S:
            return get_command32_rs2(command);
        case B:
            return get_command32_rs1(command);
        case U:
            return get_command32_rd(command);
        case J:
            return get_command32_rd(command);
        case unknown_type32:
            break;
    }
    return "ERROR";
}

std::string get_command32_s2(command_t command) {
    switch (get_command32_type(command)) {
        case R:
            return get_command32_rs1(command);
        case I:
            return get_command32_rs1(command);
        case S:
            return get_command32_rs1(command);
        case B:
            return get_command32_rs2(command);
        case U:
            return std::to_string(get_command32_imma(command));
        case J:
            return std::to_string(get_command32_imma(command));
        case unknown_type32:
            break;
    }
    return "ERROR";
}

std::string get_command32_s3(command_t command) {
    switch (get_command32_type(command)) {
        case R:
            return get_command32_rs2(command);
        case I:
            return std::to_string(get_command32_imma(command));
        case S:
            return std::to_string(get_command32_imma(command));
        case B:
            return std::to_string(get_command32_imma(command));
        case U:
            return "cheburek";
        case J:
            return "cheburek";
        case unknown_type32:
            break;
    }
    return "ERROR";
}

std::string get_command32_registry(Elf32_Half reg) {
    switch (reg) {
        case 0: return "zero";
        case 1: return "ra";
        case 2: return "sp";
        case 3: return "gt";
        case 4: return "tp";
        case 5: return "t0";
        case 6: return "t1";
        case 7: return "t2";
        case 8: return "s0";
        case 9: return "s1";
    }

    if (10 <= reg && reg <= 17)
        return "a" + std::to_string(reg-10);
    if (18 <= reg && reg <= 27)
        return "s" + std::to_string(reg-16);
    if (27 <= reg && reg <= 31)
        return "t" + std::to_string(reg-25);
}

std::string get_command32_rd(command_t command) {
    return get_command32_registry(get32_rd(command));
}

std::string get_command32_rs1(command_t command) {
    return get_command32_registry(get32_rs1(command));
}

std::string get_command32_rs2(command_t command) {
    return get_command32_registry(get32_rs2(command));
}

imma_t get_command32_imma(command_t command) {
    switch (get_command32_type(command)) {
        case I: return (
                    -(get32_bit(command, 31)<<11)+
                    get32_bits(command, 20, 30)
                );
        case S: return (
                    -(get32_bit(command, 31)<<11) +
                    (get32_bits(command, 25, 30)<<5) +
                    (get32_bits(command, 7, 11))
            );
        case U: return (
                    -(get32_bit(command, 31)<<31)+
                    (get32_bits(command, 12, 30)<<12)
                );
            // if (imma & (1 << 31)) imma = -((imma - 1) ^ ((1 << 31) - 1 + ((1 << 31))));

        case J: return (
                    -(get32_bit(command, 31)<<20) +
                    (get32_bits(command, 21, 30)<<1) +
                    (get32_bit(command, 20)<<11) +
                    (get32_bits(command, 12, 19)<<12)
                );

        case B: return (
                    -(get32_bit(command, 31)<<12) +
                    (get32_bits(command, 25, 30)<<5) +
                    (get32_bits(command, 8, 11)<<1) +
                    (get32_bit(command, 7)<<11)
                );
    }
}

///////////////////////////////////////////////////////////////

command_t get16_opcode(command_t command) {
    return command % (1 << 2); /// opcode - 1 - 0
}

command_t get16_rs2(command_t command) {
    return (command >> 2) % (1 << 5); /// rs2 - 6 - 2
}

command_t get16_rs1(command_t command) {
    return (command >> 7) % (1 << 5); /// rs1 - 11 - 7
}

command_t get16_funct4(command_t command) {
    return (command >> 12) % (1 << 4); /// funct4 - 15 - 12
}

command_t get16_funct3(command_t command) {
    return (command >> 13) % (1 << 3); /// funct4 - 15 - 13
}

command_t get16_funct6(command_t command) {
    return (command >> 10) % (1 << 6); /// funct4 - 15 - 10
}

command_t get16_rs2_1(command_t command) {
    return (command >> 2) % (1 << 3); /// rs2_1 - 4 - 2
}

command_t get16_rs1_1(command_t command) {
    return (command >> 7) % (1 << 3); /// rs1_1 - 9 - 7
}


command_t get16_bit(command_t command, int start) {
    return (command >> start) % 2;
}

command_t get16_bits(command_t command, int start, int end) {
    return (command >> start) % (1 << (end - start + 1));
}


bool is16_nz(command_t command) {
    return get16_bit(command, 12) > 0 || get16_bits(command,2, 6) > 0;
}

std::string get_command16_registry(Elf32_Half reg) {
    return get_command32_registry(reg+8);
}

std::string get16_rs2_name(command_t command) {
    return get_command32_registry(get16_rs2(command));
}

std::string get16_rs1_name(command_t command) {
    return get_command32_registry(get16_rs1(command));
}

std::string get16_rs2_1_name(command_t command) {
    return get_command16_registry(get16_rs2_1(command));
}

std::string get16_rs1_1_name(command_t command) {
    return get_command16_registry(get16_rs1_1(command));
}

command16_name get_command16_name_e(command_t command) {
    auto opcode = get16_opcode(command);
    auto rs2 = get16_rs2(command);
    auto rs1 = get16_rs1(command);
    auto funct3 = get16_funct3(command);
    auto funct4 = get16_funct4(command);
    auto funct6 = get16_funct6(command);
    auto rs1_1 = get16_rs1_1(command);
    auto rs2_1 = get16_rs2_1(command);
    if (command == 0) return c_illegal;
    switch (opcode) {
        case 0b0:
            switch (funct3) {
                case 0b0:
                    if (is16_nz(command)) return c_addi4spn;
                    else return c_res;
                case 0b1:
                    return c_float;
                case 0b10:
                    return c_lw;
                case 0b11:
                    return c_float;
                case 0b101:
                    return c_float;
                case 0b110:
                    return c_sw;
                case 0b111:
                    return c_float;
            } return c_unknown;
        case 0b1:
            switch (funct3) {
                case 0b0:
                    if (rs1 == 0)
                        if (is16_nz(command)) return c_hint;
                        else return c_nop;
                    else
                    if (is16_nz(command)) return c_addi;
                    else return c_hint;
                case 0b1:
                    return c_jal;
                case 0b10:
                    return c_li;
                case 0b11:
                    if (is16_nz(command))
                        if (rs1 == 2) return c_addi16sp;
                        else if (rs1 != 0) return c_lui;
                        else return c_128;
                    else return c_128;
                case 0b100:
                    switch (get16_bits(command, 10, 11)) {
                        case 0b0:
                            if (is16_nz(command))
                                if (get16_bit(command, 12)) return c_srli;
                                else return c_res;
                            else return c_128;
                        case 0b1:
                            if (is16_nz(command))
                                if (get16_bit(command, 12) == 1) return c_srai;
                                else return c_res;
                            else return c_128;
                        case 0b10:
                            return c_andi;
                        case 0b11:
                            if (get16_bit(command, 12) == 0) {
                                switch (get16_bits(command, 5, 6)) {
                                    case 0b0:
                                        return c_sub;
                                    case 0b1:
                                        return c_xor;
                                    case 0b10:
                                        return c_or;
                                    case 0b11:
                                        return c_and;
                                }
                                return c_128;
                            }
                            else return c_128;
                    }
                case 0b101:
                    return c_j;
                case 0b110:
                    return c_beqz;
                case 0b111:
                    return c_bnez;
            } return c_unknown;
        case 0b10: {
            switch (funct3) {
                case 0b0:
                    if (is16_nz(command))
                        if (get16_bit(command, 12) == 0) return c_slli;
                        else return c_hse;
                    else return c_128;
                case 0b1:
                    if (rs2 != 0) return c_float;
                    else return c_128;
                case 0b10:
                    return c_lwsp;
                case 0b11:
                    if (rs2 != 0) return c_float;
                    else return c_128;
                case 0b100:
                    if (get16_bit(command, 12) == 0) {
                        if (rs1 == 0) return c_res;
                        else if (rs2 == 0) return c_jr;
                        else return c_mv;
                    } else {
                        if (rs1 == 0) return c_ebreak;
                        else if (rs2 == 0) return c_jalr;
                        else return c_add;
                    }
                case 0b101:
                    return c_float;
                case 0b110:
                    return c_swsp;
                case 0b111:
                    return c_float;
            }
        }
    }

}




std::string get_command16_name(command_t command) {
    switch (get_command16_name_e(command)) {
        case c_unknown: return "unknown_command";
        case c_128: return  "unknown_command";
        case c_hint: return "unknown_command";
        case c_hse: return "unknown_command";
        case c_float: return "unknown_command";
        case c_illegal: return "unknown_command";
        case c_res: return "unknown_command";
        case c_addi4spn: return "c.addi4spn";
        case c_lw: return "c.lw";
        case c_sw: return "c.sw";
        case c_nop: return "c.nop";
        case c_addi: return "c.addi";
        case c_jal: return "c.jal";
        case c_li: return "c.li";
        case c_addi16sp: return "c.addi16sp";
        case c_lui: return "c.lui";
        case c_srli: return "c.srli";
        case c_srai: return "c.srai";
        case c_andi: return "c.andi";
        case c_sub: return "c.sub";
        case c_xor: return "c.xor";
        case c_or: return "c.or";
        case c_and: return "c.and";
        case c_j: return "c.j";
        case c_beqz: return "c.beqz";
        case c_bnez: return "c.bnez";
        case c_slli: return "c.slli";
        case c_lwsp: return "c.lwsp";
        case c_jr: return "c.jr";
        case c_mv: return "c.mv";
        case c_ebreak: return "c.ebreak";
        case c_jalr: return "c.jalr";
        case c_add: return "c.add";
        case c_swsp: return "c.swsp";
    }
}

command16_type get_command16_type(command_t command) {
    switch (get_command16_name_e(command)) {
        case c_unknown: return unknown_type16;
        case c_128: return unknown_type16;
        case c_hint: return unknown_type16;
        case c_hse: return unknown_type16;
        case c_float: return unknown_type16;
        case c_illegal: return unknown_type16;
        case c_res: return unknown_type16;
        case c_addi4spn: return CIW;
        case c_lw: return CL;
        case c_sw: return CS;
        case c_nop: return CI;
        case c_addi: return CI;
        case c_jal: return CJ;
        case c_li: return CI;
        case c_addi16sp: return CI;
        case c_lui: return CI;
        case c_srli: return CI;
        case c_srai: return CI;
        case c_andi: return CI;
        case c_sub: return CA;
        case c_xor: return CA;
        case c_or: return CA;
        case c_and: return CA;
        case c_j: return CJ;
        case c_beqz: return CB;
        case c_bnez: return CB;
        case c_slli: return CI;
        case c_lwsp: return CI;
        case c_jr: return CR;
        case c_mv: return CR;
        case c_ebreak: return CR;
        case c_jalr: return CR;
        case c_add: return CR;
        case c_swsp: return CSS;
    }
}

imma_t get_command16_imma(command_t command) {
    switch (get_command16_name_e(command)) {
        case c_addi4spn: return (
                    -(get16_bits(command, 11, 12)<<4) +
                    (get16_bits(command, 7, 10)<<6) +
                    (get16_bit(command, 6)<<2) +
                    (get16_bit(command, 5)<<3));
        case c_addi: return (
                    -(get16_bit(command, 12)<<5) +
                    (get16_bits(command, 2, 6)));
        case c_jal: return (
                    -(get16_bit(command, 12)<<11) +
                    (get16_bit(command, 11)<<4) +
                    (get16_bits(command, 9, 10)<<8) +
                    (get16_bit(command, 8)<<10) +
                    (get16_bit(command, 7)<<6) +
                    (get16_bit(command, 6)<<7) +
                    (get16_bits(command, 3, 5)<<1) +
                    (get16_bit(command, 2)<<5));
        case c_li: return (
                    -(get16_bit(command, 12)<<5) +
                    (get16_bits(command, 2, 6)));
        case c_addi16sp: return (
                    -(get16_bit(command, 12)<<9) +
                    (get16_bit(command, 6)<<4) +
                    (get16_bit(command, 5)<<6) +
                    (get16_bits(command, 3, 4)<<7) +
                    (get16_bit(command, 2)<<5)); // 2
        case c_lui: return (
                    -(get16_bit(command, 12)<<17) +
                    (get16_bits(command, 2, 6)<<12));
        case c_srli: return (
                    (get16_bit(command, 12)<<5) +
                    (get16_bits(command, 2, 6)));
        case c_srai: return (
                    (get16_bit(command, 12)<<5) +
                    (get16_bits(command, 2, 6)));
        case c_andi: return (
                    -(get16_bit(command, 12)<<5) +
                    (get16_bits(command, 2, 6)));
        case c_j: return (
                    -(get16_bit(command, 12)<<11) +
                    (get16_bit(command, 11)<<4) +
                    (get16_bits(command, 9, 10)<<8) +
                    (get16_bit(command, 8)<<10) +
                    (get16_bit(command, 7)<<6) +
                    (get16_bit(command, 6)<<7) +
                    (get16_bits(command, 3, 5)<<1) +
                    (get16_bit(command, 2)<<5));
        case c_beqz: return (
                    -(get16_bit(command, 12)<<8) +
                    (get16_bits(command, 10, 11)<<3) +
                    (get16_bits(command, 5, 6)<<6) +
                    (get16_bits(command, 3, 4)<<1) +
                    (get16_bit(command, 2)<<5));
        case c_bnez: return (
                    -(get16_bit(command, 12)<<8) +
                    (get16_bits(command, 10, 11)<<3) +
                    (get16_bits(command, 5, 6)<<6) +
                    (get16_bits(command, 3, 4)<<1) +
                    (get16_bit(command, 2)<<5));
        case c_slli: return (
                    (get16_bit(command, 12)<<5) +
                    (get16_bits(command, 2, 6)));
        case c_lwsp: return (
                    (get16_bit(command, 12)<<5) +
                    (get16_bits(command, 4, 6)<<2) +
                    (get16_bits(command, 2, 3)<<6));
        case c_swsp: return (
                    (get16_bits(command, 9, 12)<<2) +
                    (get16_bits(command, 7, 8)<<6));
        case c_unknown:
            break;
        case c_128:
            break;
        case c_hint:
            break;
        case c_hse:
            break;
        case c_float:
            break;
        case c_illegal:
            break;
        case c_res:
            break;
        case c_lw:
            break;
        case c_sw:
            break;
        case c_nop:
            break;
        case c_sub:
            break;
        case c_xor:
            break;
        case c_or:
            break;
        case c_and:
            break;
        case c_jr:
            break;
        case c_mv:
            break;
        case c_ebreak:
            break;
        case c_jalr:
            break;
        case c_add:
            break;
    }
    return 111111;
}

std::string get_command16_s1(command_t command) {
    switch (get_command16_name_e(command)) {
        case c_unknown: return "unknown_param";
        case c_128: return  "unknown_param";
        case c_hint: return "unknown_param";
        case c_hse: return "unknown_param";
        case c_float: return "unknown_param";
        case c_illegal: return "unknown_param";
        case c_res: return "unknown_param";
        case c_addi4spn: return get16_rs2_1_name(command);
        case c_lw: return get16_rs2_1_name(command);
        case c_sw: return get16_rs1_1_name(command);
        case c_nop: return "unknown_param"; // nop == addi
        case c_addi: return get16_rs1_name(command);
        case c_jal: return std::to_string(get_command16_imma(command));
        case c_li: return get16_rs1_name(command);
        case c_addi16sp: return get16_rs1_name(command); // 2
        case c_lui: return get16_rs1_name(command);
        case c_srli: return get16_rs1_1_name(command);
        case c_srai: return get16_rs1_1_name(command);
        case c_andi: return get16_rs1_1_name(command);
        case c_sub: return get16_rs1_1_name(command);
        case c_xor: return get16_rs1_1_name(command);
        case c_or: return get16_rs1_1_name(command);
        case c_and: return get16_rs1_1_name(command);
        case c_j: return std::to_string(get_command16_imma(command));
        case c_beqz: return get16_rs1_1_name(command);;
        case c_bnez: return get16_rs1_1_name(command);;
        case c_slli: return get16_rs1_name(command);
        case c_lwsp: return get16_rs1_name(command);
        case c_jr: return get16_rs1_name(command);
        case c_mv: return get16_rs1_name(command);
        case c_ebreak: return "unknown_param"; //"c.ebreak";
        case c_jalr: return get16_rs1_name(command);
        case c_add: return get16_rs1_name(command);
        case c_swsp: return get16_rs2_name(command);
    }
}

std::string get_command16_s2(command_t command) {
    switch (get_command16_name_e(command)) {
        case c_unknown: return "unknown_param";
        case c_128: return  "unknown_param";
        case c_hint: return "unknown_param";
        case c_hse: return "unknown_param";
        case c_float: return "unknown_param";
        case c_illegal: return "unknown_param";
        case c_res: return "unknown_param";
        case c_addi4spn: return "sp";
        case c_lw: return get16_rs1_1_name(command);
        case c_sw: return get16_rs2_1_name(command);
        case c_nop: return "unknown_param"; // nop == addi
        case c_addi: return get16_rs1_name(command);
        case c_jal: return "unknown_param";
        case c_li: return std::to_string(get_command16_imma(command));
        case c_lui: return std::to_string(get_command16_imma(command));
        case c_srli: return get16_rs1_1_name(command);
        case c_srai: return get16_rs1_1_name(command);
        case c_andi: return get16_rs1_1_name(command);
        case c_sub: return get16_rs1_1_name(command);
        case c_xor: return get16_rs1_1_name(command);
        case c_or: return get16_rs1_1_name(command);
        case c_and: return get16_rs1_1_name(command);
        case c_j: return "unknown_param";
        case c_beqz: return std::to_string(get_command16_imma(command));
        case c_bnez: return std::to_string(get_command16_imma(command));
        case c_slli: return get16_rs1_name(command);
        case c_lwsp: return std::to_string(get_command16_imma(command));
        case c_jr: return "unknown_param";
        case c_mv: return get16_rs2_name(command);
        case c_ebreak: return "unknown_param"; //"c.ebreak";
        case c_jalr: return "unknown_param";
        case c_add: return get16_rs1_name(command);
        case c_swsp: return std::to_string(get_command16_imma(command));
    }
}

std::string get_command16_s3(command_t command) {
    switch (get_command16_name_e(command)) {
        case c_unknown: return "unknown_param";
        case c_128: return  "unknown_param";
        case c_hint: return "unknown_param";
        case c_hse: return "unknown_param";
        case c_float: return "unknown_param";
        case c_illegal: return "unknown_param";
        case c_res: return "unknown_param";
        case c_addi4spn: return std::to_string(get_command16_imma(command));
        case c_lw: return get16_rs1_1_name(command);
        case c_sw: return get16_rs2_1_name(command);
        case c_nop: return "unknown_param"; // nop == addi
        case c_addi: return std::to_string(get_command16_imma(command));
        case c_jal: return "unknown_param";
        case c_li: return "unknown_param";
        case c_addi16sp: return std::to_string(get_command16_imma(command));
        case c_lui: return "unknown_param";
        case c_srli: return std::to_string(get_command16_imma(command));
        case c_srai: return std::to_string(get_command16_imma(command));
        case c_andi: return std::to_string(get_command16_imma(command));
        case c_sub: return get16_rs2_1_name(command);
        case c_xor: return get16_rs2_1_name(command);
        case c_or: return get16_rs2_1_name(command);
        case c_and: return get16_rs2_1_name(command);
        case c_j: return "unknown_param";
        case c_beqz: return "unknown_param";
        case c_bnez: return "unknown_param";
        case c_slli: return std::to_string(get_command16_imma(command));
        case c_lwsp: return "unknown_param";
        case c_jr: return "unknown_param";
        case c_mv: return "unknown_param";
        case c_ebreak: return "unknown_param"; //"c.ebreak";
        case c_jalr: return "unknown_param";
        case c_add: return get16_rs2_name(command);
        case c_swsp: return "unknown_param";
    }
}