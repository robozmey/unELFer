#ifndef UNELFER_DISASSEMBLER_H
#define UNELFER_DISASSEMBLER_H

#include "elf.h"

enum command32_type {
    R,
    I,
    S,
    U,
    J,
    B,
    unknown_type32
};

enum command32_name {
    i_unknown,
    i_128,
    i_hint,
    i_hse,
    i_float,
    i_illegal,
    i_res,
    i_lui,
    i_auipc,
    i_jal,
    i_jalr,
    i_beq,
    i_bne,
    i_blt,
    i_bge,
    i_bltu,
    i_bgeu,
    i_lb,
    i_lw,
    i_lh,
    i_lbu,
    i_lhu,
    i_sb,
    i_sw,
    i_sh,
    i_addi,
    i_slti,
    i_sltiu,
    i_xori,
    i_ori,
    i_andi,
    i_slli,
    i_srli,
    i_srai,
    i_add,
    i_sub,
    i_sll,
    i_slt,
    i_sltu,
    i_xor,
    i_srl,
    i_sra,
    i_or,
    i_and,
    i_fence,
    i_fence_i,
    i_ecall,
    i_ebreak,
    i_csrrw,
    i_csrrs,
    i_csrrc,
    i_csrrwi,
    i_csrrsi,
    i_csrrci,
    m_mul,
    m_mulh,
    m_mulhsu,
    m_mulhu,
    m_div,
    m_divu,
    m_rem,
    m_remu
};


command32_type get_command32_type(command_t);

std::string get_command32_name(command_t command);
std::string get_command32_s1(command_t command);
std::string get_command32_s2(command_t command);
std::string get_command32_s3(command_t command);

command_t get32_opcode(command_t command);
command_t get32_rd(command_t command) ;
command_t get32_funct3(command_t command);
command_t get32_rs1(command_t command);
command_t get32_rs2(command_t command) ;
command_t get32_funct7(command_t command);

std::string get_command32_rd(command_t command);
std::string get_command32_rs1(command_t command);
std::string get_command32_rs2(command_t command);

imma_t get_command32_imma(command_t command);

enum command16_type {
    CR,
    CI,
    CSS,
    CA,
    CIW,
    CL,
    CS,
    CB,
    CJ,
    illegal_type16,
//    b128_type16,
//    float_type16,
//    hint_type16,
//    hse_type16,
//    res_type16,
    unknown_type16
};

enum command16_name {
    c_unknown,
    c_128,
    c_hint,
    c_hse,
    c_float,
    c_illegal,
    c_res,
    c_addi4spn,
    c_lw,
    c_sw,
    c_nop,
    c_addi,
    c_jal,
    c_li,
    c_addi16sp,
    c_lui,
    c_srli,
    c_srai,
    c_andi,
    c_sub,
    c_xor,
    c_or,
    c_and,
    c_j,
    c_beqz,
    c_bnez,
    c_slli,
    c_lwsp,
    c_jr,
    c_mv,
    c_ebreak,
    c_jalr,
    c_add,
    c_swsp
};

std::string get_command16_name(command_t command);
//command16_type get_command16_type(command_t command);
imma_t get_command16_imma(command_t command);

std::string get_command16_s1(command_t command);
std::string get_command16_s2(command_t command);
std::string get_command16_s3(command_t command);

#endif //UNELFER_DISASSEMBLER_H
