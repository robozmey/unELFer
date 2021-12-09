#include "elf.h"

std::string get_name(char* c) {
    std::string name = "";
    while (*c > 0) {
        name += *c;
        c++;
    }
    return name;
}

void load_file_header(std::ifstream &elf_file, Elf32_Ehdr &file_header) {
    elf_file.seekg(0, std::ios::beg);
    elf_file.read((char *) &file_header, 64);
}

void load_section_headers(std::ifstream& elf_file, Elf32_Ehdr& file_header, Elf32_Shdr* section_headers) {
    elf_file.seekg(file_header.e_shoff, std::ios::beg);
    elf_file.read((char*)section_headers, file_header.e_shentsize * file_header.e_shnum);
}

std::string load_shstrtab(std::ifstream& elf_file, Elf32_Ehdr& file_header, Elf32_Shdr* section_headers) {
    auto shstrtab_header = section_headers[file_header.e_shstrndx];
    elf_file.seekg(shstrtab_header.sh_offset, std::ios::beg);
    char shstrtab[shstrtab_header.sh_size];
    elf_file.read((char*)&shstrtab, shstrtab_header.sh_size);
    return std::string(shstrtab, shstrtab_header.sh_size);
}

Elf32_Shdr& get_section_header(Elf32_Ehdr& file_header, Elf32_Shdr* section_headers, std::string shstrtab, std::string name1){
    for (Elf32_Half sh_index = 0; sh_index < file_header.e_shnum; sh_index++) {

        auto sh_name = section_headers[sh_index].sh_name;

        std::string name = get_name(&shstrtab[sh_name]);

        if (name == name1)
            return section_headers[sh_index];
    }
};

std::string load_strtab(std::ifstream& elf_file, Elf32_Ehdr &file_header, Elf32_Shdr& strtab_header) {
    elf_file.seekg(strtab_header.sh_offset, std::ios::beg);
    char strtab[strtab_header.sh_size];
    elf_file.read((char*)&strtab, strtab_header.sh_size);
    return std::string(strtab, strtab_header.sh_size);
}

std::vector < Elf32_Sym > load_symtab(std::ifstream& elf_file, Elf32_Shdr& symtab_header) {
    elf_file.close();
    Elf32_Half st_length = symtab_header.sh_size / sizeof(Elf32_Sym);
    Elf32_Sym symtab[st_length];
    elf_file.seekg(symtab_header.sh_offset, std::ios::beg);
    elf_file.read((char*)symtab, sizeof (Elf32_Sym));
    elf_file.open("strong");
    return std::vector < Elf32_Sym > (symtab, symtab+st_length);;
}

std :: vector < std :: string > get_st_names(Elf32_Shdr& symtab_header, Elf32_Sym* symtab, std::string strtab) {
    Elf32_Half st_length = symtab_header.sh_size / sizeof(Elf32_Sym);
    std :: vector < std :: string > st_names(st_length);
    for (Elf32_Half st_index = 0; st_index < st_length; st_index++) {

        auto st_name = symtab[st_index].st_name;

        std::string name = get_name(&strtab[st_name]);

        st_names[st_index] = name;
    }
    return st_names;
}

std::map <Elf32_Off, std::string> get_text_labels(Elf32_Sym* symtab, std :: vector < std :: string >& st_names) {
    std::map <Elf32_Off, std::string> text_labels;
    for (Elf32_Half st_index = 0; st_index < st_names.size(); st_index++) {
        if ((symtab[st_index].st_info & 0xf) == 2)
            text_labels[symtab[st_index].st_value] = st_names[st_index];
    }
    return text_labels;
}

void load_text(std::ifstream& elf_file, Elf32_Shdr& text_header, RV32_command* text) {
    elf_file.seekg(text_header.sh_offset, std::ios::beg);
    elf_file.read((char*)&text, text_header.sh_size);
}


char* get_sym_type(Elf32_Sym& sym) {
    auto type = sym.st_info & 0xf;
    switch (type) {
        case 0: return "NOTYPE";
        case 1: return "OBJECT";
        case 2: return "FUNC";
        case 3: return "SECTION";
        case 4: return "FILE";
        case 5: return "COMMON";
        case 6: return "TLS";
        case 10: return "LOOS";
        case 12: return "HIOS";
        case 13: return "LOPROC";
        case 14: return "SPARC_REGISTER";
        case 15: return "HIPROC";
    }
}

char* get_sym_bind(Elf32_Sym& sym) {
    auto bind = sym.st_info >> 4;
    switch (bind) {
        case 0: return "LOCAL";
        case 1: return "GLOBAL";
        case 2: return "WEAK";
        case 3: return "SECTION";
        case 10: return "LOOS";
        case 12: return "HIOS";
        case 13: return "LOPROC";
        case 15: return "HIPROC";
    }
}

char* get_sym_vis(Elf32_Sym& sym) {
    auto vis = sym.st_other & 0x3;
    switch (vis) {
        case 0: return "DEFAULT";
        case 1: return "INTERNAL";
        case 2: return "HIDDEN";
        case 3: return "PROTECTED";
        case 4: return "EXPORTED";
        case 5: return "SINGLETON";
        case 6: return "ELIMINATE";
    }
}

std::string get_sym_ind(Elf32_Sym& sym, Elf32_Half e_shnum) {
    auto ind = sym.st_shndx;
    switch (ind) {
        case 0: return "UNDEF";
        case 0xff00: return "LORESERVE";
//        case 0xff00: return "LOPROC";
//        case 0xff00: return "BEFORE";
        case 0xff01: return "AFTER";
        case 0xff02: return "AMD64_LCOMMON";
        case 0xff1f: return "HIPROC";
        case 0xff20: return "LOOS";
        case 0xff3f: return "LOSUNW";
//        case 0xff3f: return "SUNW_IGNORE";
//        case 0xff3f: return "HISUNW";
//        case 0xff3f: return "HIOS";
        case 0xfff1: return "ABS";
        case 0xfff2: return "COMMON";
        case 0xffff: return "XINDEX";
//        case 0xffff: return "HIRESERVE";
    }
    return std::to_string(sym.st_shndx);
}


