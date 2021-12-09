#ifndef UNELFER_DISASSEMBLER_H
#define UNELFER_DISASSEMBLER_H

#include "elf.h"

char get_command32_type(RV32_command);

std::string get_command32_name(RV32_command command);

RV32_command get_opcode(RV32_command command);

RV32_command get_rd(RV32_command command) ;

RV32_command get_funct3(RV32_command command);

RV32_command get_rs1(RV32_command command);

RV32_command get_rs2(RV32_command command) ;

RV32_command get_funct7(RV32_command command);

std::string get_command32_rd(RV32_command command);

std::string get_command32_rs1(RV32_command command);

std::string get_command32_rs2(RV32_command command);

std::string get_command32_immaI(RV32_command command);
std::string get_command32_immaS(RV32_command command);
std::string get_command32_immaU(RV32_command command);
std::string get_command32_immaJ(RV32_command command);
std::string get_command32_immaB(RV32_command command);


#endif //UNELFER_DISASSEMBLER_H
