#include <set>
#include "elf.h"
#include "disassembler.h"

int main(int argc, char *argv[]) {
    char* in_name = "strong";
    char* out_file = "disassembled.txt";
    if (argc != 3) return 1;
    in_name = argv[1];
    out_file = argv[2];

    std::ifstream elf_file(in_name); //HelloWorld-AMDx86

    // Get FileHeader
    Elf32_Ehdr file_header;
    load_file_header(elf_file, file_header);

    // Get SectionHeaders
    Elf32_Shdr section_headers[file_header.e_shnum];
    load_section_headers(elf_file, file_header, section_headers);

    // Get .shstrtab
    auto shstrtab = load_shstrtab(elf_file, file_header, section_headers);

    // Get header indexes
    auto text_header = get_section_header(file_header, section_headers, shstrtab, ".text");
    auto symtab_header = get_section_header(file_header, section_headers, shstrtab, ".symtab");
    auto strtab_header = get_section_header(file_header, section_headers, shstrtab, ".strtab");

    // Get .strtab
    auto strtab = load_strtab(elf_file, file_header, strtab_header);

    // Get .symtab
//    auto symtab = std::vector<Elf32_Sym>(); //load_symtab(elf_file, symtab_header);
//
    Elf32_Half sym_length = text_header.sh_size / sizeof(Elf32_Sym);
    Elf32_Sym symtab[sym_length];
    //load_text(elf_file, text_header, text);
    elf_file.seekg(symtab_header.sh_offset, std::ios::beg);
    elf_file.read((char*)&symtab, symtab_header.sh_size);
    auto st_names = get_st_names(symtab_header, symtab, strtab);
    auto text_labels = get_text_labels(symtab, st_names);


    // Get .text
   // RV32_command
    Elf32_Half text_length = text_header.sh_size / sizeof(RV32_command);
    RV32_command text[text_length];
    //load_text(elf_file, text_header, text);
    elf_file.seekg(text_header.sh_offset, std::ios::beg);
    elf_file.read((char*)&text, text_header.sh_size);


    freopen (out_file,"w",stdout);

    printf(".text\n");
    for (int text_index = 0; text_index < text_length; text_index++) {
        Elf32_Off com_off = text_header.sh_addr + text_index *  sizeof(RV32_command);
        auto command = text[text_index];

        std::string label;  if (text_labels.count(com_off)) label = text_labels[com_off];
        std::string command_name = get_command32_name(command);
        std::string s1 = "lol";
        std::string s2 = "kek";
        std::string s3 = "shlyopa";

        if (get_command32_type(command) == 'R') {
            s1 = get_command32_rd(command);
            s2 = get_command32_rs1(command);
            s3 = get_command32_rs2(command);
        }
        if (get_command32_type(command) == 'I') {
            s1 = get_command32_rd(command);
            s2 = get_command32_rs1(command);
            s3 = get_command32_immaI(command);
        }
        if (get_command32_type(command) == 'S') {
            s1 = get_command32_rs1(command);
            s2 = get_command32_rs2(command);
            s3 = get_command32_immaS(command);
        }
        if (get_command32_type(command) == 'B') {
            s1 = get_command32_rs1(command);
            s2 = get_command32_rs2(command);
            s3 = get_command32_immaB(command);
        }
        if (get_command32_type(command) == 'U') {
            s1 = get_command32_rd(command);
            s2 = get_command32_immaU(command);
        }
        if (get_command32_type(command) == 'J') {
            s1 = get_command32_rd(command);
            s2 = get_command32_immaJ(command);
        }

        std::set <std::string> read_store = {"lb", "lh", "lw", "lbu", "lhu", "sb", "sh", "sw"};

        if (read_store.count(command_name)) {
            printf("%08x %10s: %s %s, %s(%s)\n", com_off, label.c_str(), command_name.c_str(), s1.c_str(), s3.c_str(), s2.c_str());
        } else if (get_command32_type(command) == 'U' || get_command32_type(command) == 'J') {
            printf("%08x %10s: %s %s, %s\n", com_off, label.c_str(), command_name.c_str(), s1.c_str(), s2.c_str());
        } else {
            printf("%08x %10s: %s %s, %s, %s\n", com_off, label.c_str(), command_name.c_str(), s1.c_str(), s2.c_str(), s3.c_str());
        }
     //   printf("%08x\n", command);



    }
    printf("\n");
    printf(".symtab\n");
    printf("%s %-15s %7s %-8s %-8s %-8s %6s %s\n", "Symbol", "Value", "Size", "Type", "Bind", "Vis", "Index", "Name");
    for (Elf32_Half st_index = 0; st_index < st_names.size(); st_index++) {
        char* st_type_s = get_sym_type(symtab[st_index]);
        char* st_bind_s = get_sym_bind(symtab[st_index]);
        char* st_vis_s = get_sym_vis(symtab[st_index]);
        std::string st_ind_s = get_sym_ind(symtab[st_index], file_header.e_shnum);

        printf("[%4i] 0x%-15X %5i %-8s %-8s %-8s %6s %s\n", st_index, symtab[st_index].st_value, symtab[st_index].st_size, st_type_s, st_bind_s, st_vis_s, st_ind_s.c_str(), st_names[st_index].c_str());

    }

    fclose(stdout);
    elf_file.close();




    return 0;
}
