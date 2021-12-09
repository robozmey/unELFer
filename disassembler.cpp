#include "disassembler.h"

RV32_command get_opcode(RV32_command command) {
    return command % (1 << 7); /// opcode - 6 - 0
}

RV32_command get_rd(RV32_command command) {
    return (command >> 7) % (1 << 5);  /// rd - 11 - 7
}

RV32_command get_funct3(RV32_command command) {
    return (command >> 12) % (1 << 3);  /// funct3 - 14 - 12
}

RV32_command get_rs1(RV32_command command) {
    return (command >> 15) % (1 << 5);  /// rs1 - 19 - 15
}

RV32_command get_rs2(RV32_command command) {
    return (command >> 20) % (1 << 5);  /// rs1 - 24 - 20
}

RV32_command get_funct7(RV32_command command) {
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

char get_command32_type(RV32_command command) {
    return command32_types_by_opcode[get_opcode(command)];
}

std::string get_command32_name(RV32_command command) {
    char command_type = get_command32_type(command);
    auto opcode = get_opcode(command);
    auto funct3 = get_funct3(command);
    auto funct7 = get_funct7(command);

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
                } break;
                case 0b1111: {
                    switch (funct3) {
                        case 0: return "fence";       //RV32I
                        case 0b1: return "fence.i";
                    }

                } break;
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
                        } break;
                        case 0b110: return "ori";      //RV32I
                        case 0b111: return "andi";      //RV32I
                    }
                }  break;
                case 0b11011: {
                    switch (funct3) {
                        case 0: return "addiw";
                        case 0b1:  {
                            switch (funct7) {
                                case 0: return "slliw";
                            }
                        } break;
                        case 0b101: {
                            switch (funct7) {
                                case 0:  return "srliw";
                                case 0b0100000: return "sraiw";
                            }
                        } break;
                    }
                } break;
                case 0b1100111: {
                    switch (funct3) {
                        case 0: return "jalr";  //RV32I
                    }
                } break;
                case 0b1110011: {
                    switch (funct3) {
                        case 0: {
                            switch (funct7) {
                                case 0:   return "ecall";       //RV32I
                                case 0b1: return "ebreak";      //RV32I
                            }
                        } break;
                        case 0b1: return "CSRRW";
                        case 0b10: return "CSRRS";
                        case 0b11: return "CSRRC";
                        case 0b101: return "CSRRWI";
                        case 0b110: return "CSRRSI";
                        case 0b111: return "CSRRCI";
                    }
                }  break;
            }
        } break;
        case 'U': {
            switch (opcode) {
                case 0b10111: return "auipc"; //RV32I
                case 0b110111: return "lui";  //RV32I
            }
        } break;
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
        } break;
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
                        } break;
                        case 0b1: {
                            switch (funct7) {
                                case 0:  return "sll";      //RV32I
                                case 0b1:  return "mulh";    //RV32M
                            }
                        } break;
                        case 0b10: {
                            switch (funct7) {
                                case 0:  return "slt";      //RV32I
                                case 0b1:  return "mulshu";    //RV32M
                            }
                        } break;
                        case 0b11: {
                            switch (funct7) {
                                case 0:  return "sltu";      //RV32I
                                case 0b1:  return "mulhu";    //RV32M
                            }
                        } break;
                        case 0b100: {
                            switch (funct7) {
                                case 0:  return "xor";      //RV32I
                                case 0b1:  return "div";    //RV32M
                            }
                        } break;
                        case 0b101: {
                            switch (funct7) {
                                case 0:  return "srl";      //RV32I
                                case 0b1:  return "divu";    //RV32M
                                case 0b0100000: return "sra";      //RV32I
                            }
                        } break;
                        case 0b110: {
                            switch (funct7) {
                                case 0:  return "or";      //RV32I
                                case 0b1:  return "rem";    //RV32M
                            }
                        } break;
                        case 0b111: {
                            switch (funct7) {
                                case 0:  return "and";      //RV32I
                                case 0b1:  return "remu";    //RV32M
                            }
                        } break;
                    }
                } break;
                case 0b111011: {
                    switch (funct3) {
                        case 0: {
                            switch (funct7) {
                                case 0:  return "addw";
                                case 0b0100000: return "subw";
                            }
                        } break;
                        case 0b1: {
                            switch (funct7) {
                                case 0:  return "sllw";
                            }
                        } break;
                        case 0b101: {
                            switch (funct7) {
                                case 0:  return "srlw";
                                case 0b0100000: return "sraw";
                            }
                        } break;
                    }
                } break;

            }
        } break;
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
                }
            }
        } break;
        case 'J': {
            switch (opcode) {
                case 0b1101111: return "jal";  //RV32I
            }
        } break;
    }
    return "unknown_command";
}

std::string get_command_registry(Elf32_Half reg) {
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

std::string get_command32_rd(RV32_command command) {
    return get_command_registry(get_rd(command));
}

std::string get_command32_rs1(RV32_command command) {
    return get_command_registry(get_rs1(command));
}

std::string get_command32_rs2(RV32_command command) {
    return get_command_registry(get_rs2(command));
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

std::string get_command32_immaI(RV32_command command) { // done
    int32_t imma = (get_funct7(command) << 5) + get_rs2(command);
    if (imma & (1 << 11)) imma = -((imma - 1) ^ ((1 << 12) - 1));
    return std::to_string(imma);
}

std::string get_command32_immaS(RV32_command command) { // done?
    int32_t imma = (get_funct7(command) << 5) + get_rd(command);
    if (imma & (1 << 11)) imma = -((imma - 1) ^ ((1 << 12) - 1));
    return std::to_string(imma);
}

std::string get_command32_immaU(RV32_command command) { // done
    int32_t imma = command >> 12 << 12;
   // if (imma & (1 << 31)) imma = -((imma - 1) ^ ((1 << 31) - 1 + ((1 << 31))));
    return std::to_string(imma);
}

std::string get_command32_immaJ(RV32_command command) { // done
    auto x = command >> 12;
    int32_t imma = ((x >> 19) << 20) + (((x >> 9) % (1 << 10)) << 1) + (((x >> 8) % 2) << 11) + ((x % (1 << 8)) << 12) ;
   // if (imma & (1 << 20)) imma = -((imma - 1) ^ ((1 << 13) - 1));
    return std::to_string(imma);
}

std::string get_command32_immaB(RV32_command command) { // done
    auto f7 = get_funct7(command);
    auto rd = get_rd(command);
    int32_t imma = ((f7 >> 6) << 12) + ((f7 % (1 << 6))<< 5) + (((rd >> 1) % (1 << 4)) << 1) + ((rd % 2) << 11);
    if (imma & (1 << 12)) imma = -((imma - 1) ^ ((1 << 13) - 1));
    return std::to_string(imma);
}