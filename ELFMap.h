#ifndef ELFMAP_H
#define ELFMAP_H
#include "myelf.h"
#include <string>
#include <vector>
using namespace std;

class ELFMap
{
public:
    ELFMap();
    ~ELFMap();
    //解析文件、ELF头、节表、程序段表
    bool Parser(const char *fileNameAndPath);
    bool ParserELFHead();
    bool ParserSectionHead();
    bool ParserProgramHead();
    bool ParserSymbolTable();
    //获取部分名对应的内容、ELF头、节表、程序段表
    string GetContent(const char *part);
    string GetContentELFHead();
    string GetContentSectionHead();
    string GetContentProgramHead();
    //获取部分名对应的二进制字符串
    string GetHex(const char *part);
    string GetHexBase(int off, int len);

    //文件名、缓存地址和缓存大小
    string m_fileName;
    char *m_dataBuff;
    int m_dataSize;
    // ELF头、节表、程序段表、符号表和重定位表
    Elf64_Ehdr *m_ehdr;
    vector<Elf64_Shdr *> m_vshdr;
    vector<Elf64_Phdr *> m_vphdr;
    vector<Elf64_Sym *> m_vsym;
    //节表名节地址、符号表名节地址
    char *m_shNameSection;
    char *m_symNameSection;
};

//单行显示内容
typedef struct
{
    string pFile;
    string data;
    string value;
    string desc;
} OneLine;
/*
typedef struct {
    OneLine fileClass;
    OneLine dataEncoding;
    OneLine fileVersion;
} ELFHeadString;
*/

#endif // ELFMAP_H
