cmake_minimum_required(VERSION 3.20)
project(unELFer)

set(CMAKE_CXX_STANDARD 14)

add_executable(unELFer main.cpp elf.cpp elf.h disassembler.cpp disassembler.h)
