#ifndef ELFMAP_H
#define ELFMAP_H
#include "myelf.h"
#include <string>
#include <vector>
using namespace std;

typedef struct {
    string Magic_number;
    string File_class;
    string Data_encoding;
    string File_version;
} Elf64_Ehdr_str;

class elfmap
{
public:
    elfmap();
    ~elfmap();
    bool parser(const char* filename);
    bool parser_ELFhead();
    string get_content(const char* section);
    string get_hex(const char* section);
    string get_hex_base(int off,int len);

    string elffilename;
    char* datap;
    long datalen;
    Elf64_Ehdr *ehdr;
    char* shname;
    vector<Elf64_Shdr *> vshdr;
    vector<Elf64_Phdr *> vphdr;
    Elf64_Ehdr_str ehdrstr{
        "Magic_number   ",
        "File_class     ",
        "Data_encoding  ",
        "File_version   "
    };
};

#endif // ELFMAP_H
