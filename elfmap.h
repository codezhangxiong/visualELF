#ifndef ELFMAP_H
#define ELFMAP_H
#include "myelf.h"
#include <string>
#include <vector>
using namespace std;

typedef struct {
    string pFile;
    string Data;
    string Value;
    string Description;
} OneLine;

typedef struct {
    OneLine Magic_number;
    OneLine File_class;
    OneLine Data_encoding;
    OneLine File_version;
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
    long symoff;
    long symnameoff;
    Elf64_Ehdr *ehdr;
    char* shname;
    vector<Elf64_Shdr *> vshdr;
    vector<Elf64_Phdr *> vphdr;
    vector<Elf64_Sym *> vsym;
    Elf64_Ehdr_str ehdrstr;
};

#endif // ELFMAP_H
