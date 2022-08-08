#ifndef ELFMAP_H
#define ELFMAP_H
#include "myelf.h"
#include <string>
#include <vector>
using namespace std;

class elfmap
{
public:
    elfmap();
    ~elfmap();
    bool parser(const char* filename);
    string get_hex(const char* section);
    string get_hex_base(int off,int len);

    string elffilename;
    char* datap;
    long datalen;
    Elf64_Ehdr *ehdr;
    char* shname;
    vector<Elf64_Shdr *> vshdr;
    vector<Elf64_Phdr *> vphdr;
};

#endif // ELFMAP_H
