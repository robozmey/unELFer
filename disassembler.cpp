#include "disassembler.h"

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

std::map <Elf32_Word, char > command32_types_by_opcode = {{0b11, 'I'},
                                                          {0b1111, 'I'},
                                                          {0b10011, 'I'},
                                                          {0b10111, 'U'},
                                                          {0b11011, 'I'},
                                                          {0b100011, 'S'},
                                                          {0b110011, 'R'},
                                                          {0b110111, 'U'},
                                                          {0b111011, 'R'},
                                                          {0b1100011, 'B'},
                                                          {0b1100111, 'I'},
                                                          {0b1101111, 'J'},
                                                          {0b1110011, 'I'}};

char get_command32_type(command_t command) {
    return command32_types_by_opcode[get32_opcode(command)];
}

std::string get_command32_name(command_t command) {
    char command_type = get_command32_type(command);
    auto opcode = get32_opcode(command);
    auto funct3 = get32_funct3(command);
    auto funct7 = get32_funct7(command);

    auto error_128 = "128_command32";
    auto error_f = "float_comand32";
    auto error_r = "reserved_comand32";
    auto error_u = "unknown_comand32";

    switch (command_type) {
        case 'I': {
            switch (opcode) {
                case 0b11: {
                    switch (funct3) {
                        case 0: return "lb";        //RV32I
                        case 0b1: return "lh";      //RV32I
                        case 0b10: return "lw";     //RV32I
                        case 0b11: return "ld";
                        case 0b100: return "lbu";      //RV32I
                        case 0b101: return "lhu";      //RV32I
                        case 0b110: return "lwu";
                    }
                } return error_u;
                case 0b1111: {
                    switch (funct3) {
                        case 0: return "fence";       //RV32I
                        case 0b1: return "fence.i";
                    }

                } return error_u;
                case 0b10011: {
                    switch (funct3) {
                        case 0: return "addi";       //RV32I
                        case 0b1: {
                            switch (funct7) {
                                case 0: return "slli";      //RV32I
                            }
                        } break;
                        case 0b10: return "slti";       //RV32I
                        case 0b11: return "sltiu";      //RV32I
                        case 0b100: return "xori";      //RV32I
                        case 0b101: {
                            switch (funct7) {
                                case 0:  return "srli";      //RV32I
                                case 0b0100000: return "srai";      //RV32I
                            }
                        } return error_u;
                        case 0b110: return "ori";      //RV32I
                        case 0b111: return "andi";      //RV32I
                    }
                }  return error_u;
                case 0b11011: {
                    switch (funct3) {
                        case 0: return "addiw";
                        case 0b1:  {
                            switch (funct7) {
                                case 0: return "slliw";
                            }
                        } return error_u;
                        case 0b101: {
                            switch (funct7) {
                                case 0:  return "srliw";
                                case 0b0100000: return "sraiw";
                            }
                        } return error_u;
                    }
                } return error_u;
                case 0b1100111: {
                    switch (funct3) {
                        case 0: return "jalr";  //RV32I
                    }
                } return error_u;
                case 0b1110011: {
                    switch (funct3) {
                        case 0: {
                            switch (funct7) {
                                case 0:   return "ecall";       //RV32I
                                case 0b1: return "ebreak";      //RV32I
                            }
                        } return error_u;
                        case 0b1: return "CSRRW";
                        case 0b10: return "CSRRS";
                        case 0b11: return "CSRRC";
                        case 0b101: return "CSRRWI";
                        case 0b110: return "CSRRSI";
                        case 0b111: return "CSRRCI";
                    }
                }  return error_u;
            }
        } return error_u;
        case 'U': {
            switch (opcode) {
                case 0b10111: return "auipc"; //RV32I
                case 0b110111: return "lui";  //RV32I
            }
        } return error_u;
        case 'S': {
            switch (opcode) {
                case 0b100011: {
                    switch (funct3) {
                        case 0: return "sb";      //RV32I
                        case 0b1: return "sh";      //RV32I
                        case 0b10: return "sw";      //RV32I
                        case 0b11: return "sd";
                    }
                }
            }
        } return error_u;
        case 'R': {
            switch (opcode) {
                case 0b110011: {
                    switch (funct3) {
                        case 0: {
                            switch (funct7) {
                                case 0:  return "add";      //RV32I
                                case 0b1:  return "mul";    //RV32M
                                case 0b0100000: return "sub";      //RV32I
                            }
                        } return error_u;
                        case 0b1: {
                            switch (funct7) {
                                case 0:  return "sll";      //RV32I
                                case 0b1:  return "mulh";    //RV32M
                            }
                        } return error_u;
                        case 0b10: {
                            switch (funct7) {
                                case 0:  return "slt";      //RV32I
                                case 0b1:  return "mulshu";    //RV32M
                            }
                        } return error_u;
                        case 0b11: {
                            switch (funct7) {
                                case 0:  return "sltu";      //RV32I
                                case 0b1:  return "mulhu";    //RV32M
                            }
                        } return error_u;
                        case 0b100: {
                            switch (funct7) {
                                case 0:  return "xor";      //RV32I
                                case 0b1:  return "div";    //RV32M
                            }
                        } return error_u;
                        case 0b101: {
                            switch (funct7) {
                                case 0:  return "srl";      //RV32I
                                case 0b1:  return "divu";    //RV32M
                                case 0b0100000: return "sra";      //RV32I
                            }
                        } return error_u;
                        case 0b110: {
                            switch (funct7) {
                                case 0:  return "or";      //RV32I
                                case 0b1:  return "rem";    //RV32M
                            }
                        } return error_u;
                        case 0b111: {
                            switch (funct7) {
                                case 0:  return "and";      //RV32I
                                case 0b1:  return "remu";    //RV32M
                            }
                        } return error_u;
                    } return error_u;
                } return error_u;
                case 0b111011: {
                    switch (funct3) {
                        case 0: {
                            switch (funct7) {
                                case 0:  return "addw";
                                case 0b0100000: return "subw";
                            }
                        } return error_u;
                        case 0b1: {
                            switch (funct7) {
                                case 0:  return "sllw";
                            }
                        } return error_u;
                        case 0b101: {
                            switch (funct7) {
                                case 0:  return "srlw";
                                case 0b0100000: return "sraw";
                            }
                        } return error_u;
                    }
                } return error_u;

            } return error_u;
        } return error_u;
        case 'B': {
            switch (opcode) {
                case 0b1100011: {
                    switch (funct3) {
                        case 0: return "beq";      //RV32I
                        case 0b1: return "bne";    //RV32I
                        case 0b100: return "blt";  //RV32I
                        case 0b101: return "bge";  //RV32I
                        case 0b110: return "bltu"; //RV32I
                        case 0b111: return "bgeu"; //RV32I
                    }
                } return error_u;
            } return error_u;
        } return error_u;
        case 'J': {
            switch (opcode) {
                case 0b1101111: return "jal";  //RV32I
            }
        } return error_u;
    }
    return error_u;
}

std::string get_command32_s1(command_t command) {
    switch (get_command32_type(command)) {
        case 'R':
            return get_command32_rd(command);
        case 'I':
            return get_command32_rd(command);
        case 'S':
            return get_command32_rs2(command);
        case 'B':
            return get_command32_rs1(command);
        case 'U':
            return get_command32_rd(command);
        case 'J':
            return get_command32_rd(command);
    }
    return "ERROR";
}

std::string get_command32_s2(command_t command) {
    switch (get_command32_type(command)) {
        case 'R':
            return get_command32_rs1(command);
        case 'I':
            return get_command32_rs1(command);
        case 'S':
            return get_command32_rs1(command);
        case 'B':
            return get_command32_rs2(command);
        case 'U':
            return get_command32_immaU(command);
        case 'J':
            return get_command32_immaJ(command);
    }
    return "ERROR";
}

std::string get_command32_s3(command_t command) {
    switch (get_command32_type(command)) {
        case 'R':
            return get_command32_rs2(command);
        case 'I':
            return get_command32_immaI(command);
        case 'S':
            return get_command32_immaS(command);
        case 'B':
            return get_command32_immaB(command);
        case 'U':
            return "cheburek";
        case 'J':
            return "cheburek";
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

std::string to_hex_string(Elf32_Word w){
    static const char* digits = "0123456789abcdef";
    size_t hex_len = sizeof (Elf32_Word);
    std::string rc(hex_len,'0');
    for (size_t i=0, j=(hex_len-1)*4 ; i<hex_len; ++i,j-=4)
        rc[i] = digits[(w>>j) & 0x0f];
    while (rc.size() > 1 && rc[0] == '0')
        rc.erase(0, 1);
    return std::to_string(w);//return rc;
}

std::string get_command32_immaI(command_t command) { // done
    int32_t imma = (get32_funct7(command) << 5) + get32_rs2(command);
    if (imma & (1 << 11)) imma = -((imma - 1) ^ ((1 << 12) - 1));
    return std::to_string(imma);
}

std::string get_command32_immaS(command_t command) { // done?
    int32_t imma = (get32_funct7(command) << 5) + get32_rd(command);
    if (imma & (1 << 11)) imma = -((imma - 1) ^ ((1 << 12) - 1));
    return std::to_string(imma);
}

std::string get_command32_immaU(command_t command) { // done
    int32_t imma = command >> 12 << 12;
   // if (imma & (1 << 31)) imma = -((imma - 1) ^ ((1 << 31) - 1 + ((1 << 31))));
    return std::to_string(imma);
}

std::string get_command32_immaJ(command_t command) { // done
    auto x = command >> 12;
    int32_t imma = ((x >> 19) << 20) + (((x >> 9) % (1 << 10)) << 1) + (((x >> 8) % 2) << 11) + ((x % (1 << 8)) << 12) ;
   // if (imma & (1 << 20)) imma = -((imma - 1) ^ ((1 << 13) - 1));
    return std::to_string(imma);
}

std::string get_command32_immaB(command_t command) { // done
    auto f7 = get32_funct7(command);
    auto rd = get32_rd(command);
    int32_t imma = ((f7 >> 6) << 12) + ((f7 % (1 << 6))<< 5) + (((rd >> 1) % (1 << 4)) << 1) + ((rd % 2) << 11);
    if (imma & (1 << 12)) imma = -((imma - 1) ^ ((1 << 13) - 1));
    return std::to_string(imma);
}

imma_t get_command32_imma(command_t command) {
    switch (get_command32_type(command)) {
        case 'I': { // done
            int32_t imma = (get32_funct7(command) << 5) + get32_rs2(command);
            if (imma & (1 << 11)) imma = -((imma - 1) ^ ((1 << 12) - 1));
            return imma;
        }

        case 'S': { // done
            int32_t imma = (get32_funct7(command) << 5) + get32_rd(command);
            if (imma & (1 << 11)) imma = -((imma - 1) ^ ((1 << 12) - 1));
            return (imma);
        }

        case 'U': { // done
            int32_t imma = command >> 12 << 12;
            // if (imma & (1 << 31)) imma = -((imma - 1) ^ ((1 << 31) - 1 + ((1 << 31))));
            return (imma);
        }

        case 'J': { // done
            auto x = command >> 12;
            int32_t imma =
                    ((x >> 19) << 20) + (((x >> 9) % (1 << 10)) << 1) + (((x >> 8) % 2) << 11) + ((x % (1 << 8)) << 12);
            // if (imma & (1 << 20)) imma = -((imma - 1) ^ ((1 << 13) - 1));
            return (imma);
        }

        case 'B': { // done
            auto f7 = get32_funct7(command);
            auto rd = get32_rd(command);
            int32_t imma =
                    ((f7 >> 6) << 12) + ((f7 % (1 << 6)) << 5) + (((rd >> 1) % (1 << 4)) << 1) + ((rd % 2) << 11);
            if (imma & (1 << 12)) imma = -((imma - 1) ^ ((1 << 13) - 1));
            return (imma);
        }
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
                    if (get16_bit(command, 12)) {
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
                    (get16_bit(command, 12)<<17) +
                    (get16_bits(command, 2, 6)<<12));
        case c_srli: return (
                    (get16_bit(command, 12)<<5) +
                    (get16_bits(command, 2, 6)));
        case c_srai: return (
                    (get16_bit(command, 12)<<5) +
                    (get16_bits(command, 2, 6)));
        case c_andi: return (
                    (get16_bit(command, 12)<<5) +
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