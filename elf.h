#ifndef UNELFER_ELF_H
#define UNELFER_ELF_H

#include <iostream>
#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include <map>

/* 32-bit ELF base types. */
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Word;
typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Off;

#define EI_NIDENT	16

typedef struct {
    unsigned char	e_ident[EI_NIDENT];	/* ELF "magic number" */
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;		/* Entry point virtual address */
    Elf32_Off e_phoff;		/* Program header table file offset */
    Elf32_Off e_shoff;		/* Section header table file offset */
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
} Elf32_Ehdr;

typedef struct {
    Elf32_Word sh_name;		/* Section name, index in string tbl */
    Elf32_Word sh_type;		/* Type of section */
    Elf32_Word sh_flags;		/* Miscellaneous section attributes */
    Elf32_Addr sh_addr;		/* Section virtual addr at execution */
    Elf32_Off  sh_offset;		/* Section file offset */
    Elf32_Word sh_size;		/* Size of section in bytes */
    Elf32_Word sh_link;		/* Index of another section */
    Elf32_Word sh_info;		/* Additional section information */
    Elf32_Word sh_addralign;	/* Section alignment */
    Elf32_Word sh_entsize;	/* Entry size if section holds table */
} Elf32_Shdr;

typedef struct {
    Elf32_Word      st_name;
    Elf32_Addr      st_value;
    Elf32_Word      st_size;
    unsigned char   st_info;
    unsigned char   st_other;
    Elf32_Half      st_shndx;
} Elf32_Sym;

typedef uint64_t command_t;
typedef uint16_t RV16_command;
typedef int32_t imma_t;

std::string get_name(char* c);

void load_file_header(FILE *, Elf32_Ehdr&);

void load_section_headers(FILE *, Elf32_Ehdr&, Elf32_Shdr*);

std::string load_shstrtab(FILE *, Elf32_Ehdr&, Elf32_Shdr*);

Elf32_Shdr& get_section_header(Elf32_Ehdr&, Elf32_Shdr*, std::string, std::string);

std::string load_strtab(FILE *, Elf32_Ehdr&, Elf32_Shdr&);

std::vector < Elf32_Sym > load_symtab(std::ifstream& elf_file, Elf32_Shdr& symtab_header);

std :: vector < std :: string > get_st_names(Elf32_Shdr& symtab_header, Elf32_Sym* symtab, std::string strtab);

std::map <Elf32_Off, std::string> get_text_labels(Elf32_Sym* symtab, std :: vector < std :: string >& st_names);

void load_text(std::ifstream& elf_file, Elf32_Shdr& text_header, command_t* text);

char* get_sym_type(Elf32_Sym& sym);

char* get_sym_bind(Elf32_Sym& sym);

char* get_sym_vis(Elf32_Sym& sym);

std::string get_sym_ind(Elf32_Sym& sym, Elf32_Half e_shnum);

#endif //UNELFER_ELF_H
