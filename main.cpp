#include <set>
#include "elf.h"
#include "disassembler.h"

int main(int argc, char *argv[]) {

    if (argc != 3) {
        std::cout << "ERROR: command line arguments error" << std::endl;
        return 0;
    }
    char* in_name = argv[1]; //"strong16";
    char* out_name = argv[2]; //"disassembled16.txt";
    FILE * elf_file;
    FILE * out_file;
    elf_file = fopen (in_name,"rb");

    if (elf_file == NULL) {
        std::cout << "ERROR: cannot open ELF file" << std::endl;
        return 0;
    }

    // Get FileHeader
    Elf32_Ehdr file_header;
    try {
        load_file_header(elf_file, file_header);
    } catch (...) {
        std::cout << "ERROR: cannot read file header" << std::endl;
        fclose(elf_file);
        return 0;
    }

    // Get SectionHeaders
    Elf32_Shdr section_headers[file_header.e_shnum];
    try {
        load_section_headers(elf_file, file_header, section_headers);
    } catch (...) {
        std::cout << "ERROR: cannot read section headers" << std::endl;
        fclose(elf_file);
        return 0;
    }

    // Get .shstrtab
    std::string shstrtab;
    try {
        shstrtab = load_shstrtab(elf_file, file_header, section_headers);
    } catch (...) {
        std::cout << "ERROR: cannot read section .shstrtab" << std::endl;
        fclose(elf_file);
        return 0;
    }

    // Get header indexes
    auto text_header = get_section_header(file_header, section_headers, shstrtab, ".text");
    auto symtab_header = get_section_header(file_header, section_headers, shstrtab, ".symtab");
    auto strtab_header = get_section_header(file_header, section_headers, shstrtab, ".strtab");

    // Get .strtab
    std::string strtab;
    try {
        strtab = load_strtab(elf_file, file_header, strtab_header);
    } catch (...) {
        std::cout << "ERROR: cannot read section .strtab" << std::endl;
        fclose(elf_file);
        return 0;
    }

    // Get .symtab
    Elf32_Half sym_length = text_header.sh_size / sizeof(Elf32_Sym);
    Elf32_Sym symtab[sym_length];
    try {
        //load_text(elf_file, text_header, text);
        fseek(elf_file, symtab_header.sh_offset, SEEK_SET);
        fread((char *) &symtab, symtab_header.sh_size, 1, elf_file);
    }  catch (...) {
        std::cout << "ERROR: cannot read section .symtab" << std::endl;
        fclose(elf_file);
        return 0;
    }
    auto st_names = get_st_names(symtab_header, symtab, strtab);
    auto text_labels = get_text_labels(symtab, st_names);

    // Get .text
    Elf32_Half text_length = text_header.sh_size / sizeof(RV16_command);
    RV16_command text[text_length];
    try {
        fseek(elf_file, text_header.sh_offset, SEEK_SET);
        fread((char *) &text, text_header.sh_size, 1, elf_file);
    } catch (...) {
        std::cout << "ERROR: cannot read section .text" << std::endl;
        fclose(elf_file);
        return 0;
    }

    // Close IN
    fclose(elf_file);

    out_file = fopen (out_name,"w");
    if (out_file == NULL) {
        std::cout << "ERROR: cannot open output file" << std::endl;
        return 0;
    }

    std::set<std::string> jump_commands = {"jal", "beq", "bne", "btl", "bge", "btlu", "bgeu", "c.j", "c.jal", "c.bnez", "c.beqz"};

    try {
        // Get unnamed labels
        std::vector<command_t> jumps;
        for (int text_index = 0; text_index < text_length; text_index++) {
            Elf32_Off com_off = text_header.sh_addr + text_index * sizeof(RV16_command);
            command_t command = text[text_index];
            std::string command_name = "unknown_command!";

            if (command % 4 == 0b11) {
                command = (((command_t) text[++text_index]) << 16) + (command);
                command_name = get_command32_name(command);
                if (jump_commands.count(command_name)) {
                    jumps.push_back(com_off + get_command32_imma(command));
                }
            } else {
                command_name = get_command16_name(command);
                if (jump_commands.count(command_name)) {
                    jumps.push_back(com_off + get_command16_imma(command));
                }
            }
        }

        int k = 0;
        for (int jump_index = 0; jump_index < jumps.size(); jump_index++) {
            if (!text_labels.count(jumps[jump_index])) {
                char buff[20];
                snprintf(buff, sizeof(buff), "LOC_%05x", k++);
                std::string buffAsStdStr = buff;
                text_labels[jumps[jump_index]] = buffAsStdStr;
            }
        }


        fprintf(out_file, ".text\n");
        for (int text_index = 0; text_index < text_length; text_index++) {
            Elf32_Off com_off = text_header.sh_addr + text_index * sizeof(RV16_command);
            command_t command = text[text_index];

            std::string label;
            if (text_labels.count(com_off)) label = text_labels[com_off];
            std::string command_name = "unknown_command!";
            std::string s1 = "lol";
            std::string s2 = "kek";
            std::string s3 = "shlyopa";

            if (command % 4 == 0b11) {
                command = (((command_t) text[++text_index]) << 16) + (command);
                command_name = get_command32_name(command);
                s1 = get_command32_s1(command);
                s2 = get_command32_s2(command);
                s3 = get_command32_s3(command);

                if (jump_commands.count(command_name)) {
                    if (get_command32_type(command) == U || get_command32_type(command) == J)
                        s2 = text_labels[com_off + get_command32_imma(command)];
                    else
                        s3 = text_labels[com_off + get_command32_imma(command)];
                }

                std::set<std::string> read_store = {"lb", "lh", "lw", "lbu", "lhu", "sb", "sh", "sw"};
                if (command_name == "unknown_command") {
                    fprintf(out_file, "%08x %10s: %s\n", com_off, label.c_str(), command_name.c_str());
                } else if (read_store.count(command_name)) {
                    fprintf(out_file, "%08x %10s: %s %s, %s(%s)\n", com_off, label.c_str(), command_name.c_str(),
                            s1.c_str(), s3.c_str(), s2.c_str());
                } else if (get_command32_type(command) == U || get_command32_type(command) == J) {
                    fprintf(out_file, "%08x %10s: %s %s, %s\n", com_off, label.c_str(), command_name.c_str(),
                            s1.c_str(), s2.c_str());
                } else {
                    fprintf(out_file, "%08x %10s: %s %s, %s, %s\n", com_off, label.c_str(), command_name.c_str(),
                            s1.c_str(),
                            s2.c_str(), s3.c_str());
                }
            } else {
                command_name = get_command16_name(command);
                //std::string type = get_command16_type(command);
                s1 = get_command16_s1(command);
                s2 = get_command16_s2(command);
                s3 = get_command16_s3(command);

                std::set<std::string> read_store = {"c.lw", "c.sw"};
                std::set<std::string> no_param = {"c.ebreak", "c.nop"};
                std::set<std::string> one_param = {"c.j", "c.jal", "c.jr", "c.jalr"};
                std::set<std::string> two_param = {"c.li", "c.lui", "c.bnez", "c.beqz", "c.lwsp", "c.swsp", "c.mv"};


                if (jump_commands.count(command_name)) {
                    if (one_param.count(command_name))
                        s1 = text_labels[com_off + get_command16_imma(command)];
                    else if (two_param.count(command_name))
                        s2 = text_labels[com_off + get_command16_imma(command)];
                    else
                        s3 = text_labels[com_off + get_command16_imma(command)];
                }

                if (command_name == "unknown_command") {
                    fprintf(out_file, "%08x %10s: %s\n", com_off, label.c_str(), command_name.c_str());
                } else if (read_store.count(command_name)) {
                    fprintf(out_file, "%08x %10s: %s %s, %s(%s)\n", com_off, label.c_str(), command_name.c_str(),
                            s1.c_str(), s3.c_str(), s2.c_str());
                } else if (no_param.count(command_name)) {
                    fprintf(out_file, "%08x %10s: %s\n", com_off, label.c_str(), command_name.c_str());
                } else if (one_param.count(command_name)) {
                    fprintf(out_file, "%08x %10s: %s %s\n", com_off, label.c_str(), command_name.c_str(), s1.c_str());
                } else if (two_param.count(command_name)) {
                    fprintf(out_file, "%08x %10s: %s %s, %s\n", com_off, label.c_str(), command_name.c_str(),
                            s1.c_str(), s2.c_str());
                } else {
                    fprintf(out_file, "%08x %10s: %s %s, %s, %s\n", com_off, label.c_str(), command_name.c_str(),
                            s1.c_str(), s2.c_str(), s3.c_str());
                }
            }
        }
        fprintf(out_file, "\n");
        fprintf(out_file, ".symtab\n");
        fprintf(out_file, "%s %-15s %7s %-8s %-8s %-8s %6s %s\n", "Symbol", "Value", "Size", "Type", "Bind", "Vis",
                "Index", "Name");
        for (Elf32_Half st_index = 0; st_index < st_names.size(); st_index++) {
            char *st_type_s = get_sym_type(symtab[st_index]);
            char *st_bind_s = get_sym_bind(symtab[st_index]);
            char *st_vis_s = get_sym_vis(symtab[st_index]);
            std::string st_ind_s = get_sym_ind(symtab[st_index], file_header.e_shnum);

            fprintf(out_file, "[%4i] 0x%-15X %5i %-8s %-8s %-8s %6s %s\n", st_index, symtab[st_index].st_value,
                    symtab[st_index].st_size, st_type_s, st_bind_s, st_vis_s, st_ind_s.c_str(),
                    st_names[st_index].c_str());

        }
        fclose(out_file);
    } catch (...) {
        std::cout << "ERROR: cannot write output" << std::endl;
        fclose(out_file);
        return 0;
    }




    return 0;
}
