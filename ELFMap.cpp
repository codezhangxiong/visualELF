#include <stdio.h>
#include "ELFMap.h"

const char* showHead = "RVA        0  1  2  3  4  5  6  7   8  9  A  B  C  D  E  F  Value   \n";

ELFMap::ELFMap()
{
}

ELFMap::~ELFMap()
{
    if(m_dataBuff!=NULL)
    {
        free(m_dataBuff);
    }
}

bool ELFMap::Parser(const char *fileNameAndPath)
{
    if(NULL == fileNameAndPath)
    {
        return false;
    }

    string sFileNameAndPath(fileNameAndPath);
    m_fileName = sFileNameAndPath.substr(sFileNameAndPath.find_last_of('/') + 1);

    FILE* f;
    f = fopen(fileNameAndPath, "rb");
    fseek(f, 0, SEEK_END);
    m_dataSize = ftell(f);
    fseek(f, 0, SEEK_SET);
    m_dataBuff = (char*)malloc(m_dataSize + 1);
    fread(m_dataBuff, 1, m_dataSize, f);
    fclose(f);

    //解析
    m_ehdr = (Elf64_Ehdr*)m_dataBuff;

    char* shNameSectionHead = m_dataBuff + m_ehdr->e_shoff + sizeof(Elf64_Shdr) * m_ehdr->e_shstrndx;
    m_shNameSection = m_dataBuff+((Elf64_Shdr*)shNameSectionHead)->sh_offset;
    for(int i=0; i< m_ehdr->e_shnum; i++)
    {
        Elf64_Shdr *shdr = (Elf64_Shdr*)(m_dataBuff + m_ehdr->e_shoff) + i;
        m_vshdr.push_back(shdr);
        if(string(".strtab") == string(m_shNameSection + shdr->sh_name))
        {
            m_symNameSection = m_dataBuff+ shdr->sh_offset;
        }
        //
        else if(string(".symtab") == string(m_shNameSection + shdr->sh_name))
        {
            char* sectionBegin = m_dataBuff+shdr->sh_offset;
            int symCount = shdr ->sh_size/sizeof(Elf64_Sym);
            for(int i =0;i<symCount;i++)
            {
                Elf64_Sym* sym = (Elf64_Sym*)sectionBegin+i;
                m_vsym.push_back(sym);
            }
        }
        /*
        if(SHT_SYMTAB == shdr->sh_type)
        {
            m_symNameSection = m_dataBuff + shdr->sh_offset;
            int n = (shdr->sh_size)/sizeof(Elf64_Sym);
            for(int i = 0;i<n;i++)
            {
                Elf64_Sym *sym = (Elf64_Sym *)(shdr->sh_offset)+i;
                vsym.push_back(sym);
            }
        }*/
    }
    for(int i=0; i< m_ehdr->e_phnum; i++)
    {
        Elf64_Phdr *phdr = (Elf64_Phdr*)(m_dataBuff + m_ehdr->e_phoff) + i;
        m_vphdr.push_back(phdr);
    }

    return true;
}

bool ELFMap::ParserELFHead()
{/*
    char dataBuff[64] = {0};
    char valueBuff[64] = {0};

    //Magic number
    sprintf(dataBuff,"%02X%02X%02X%02X    ",m_dataBuff[0],m_dataBuff[1],m_dataBuff[2],m_dataBuff[3]);
    sprintf(valueBuff,"%02X-%c%c%c            ",m_dataBuff[0],m_dataBuff[1],m_dataBuff[2],m_dataBuff[3]);
    ehdrstr.magicNumber.pFile = "00000000    ";
    ehdrstr.magicNumber.Data = dataBuff;
    ehdrstr.magicNumber.Value = valueBuff;
    ehdrstr.magicNumber.Description = "magicNumber\n";

    //File class
    sprintf(dataBuff,"%02X          ",m_dataBuff[4]);
    if(m_ehdr->e_ident[EI_CLASS] == ELFCLASSNONE)
    {
        sprintf(valueBuff,"Invalid class     ");
    }
    else if(m_ehdr->e_ident[EI_CLASS] == ELFCLASS32)
    {
        sprintf(valueBuff,"32-bit objects    ");
    }
    else if(m_ehdr->e_ident[EI_CLASS] == ELFCLASS64)
    {
        sprintf(valueBuff,"64-bit objects    ");
    }
    ehdrstr.File_class.pFile = "00000004    ";
    ehdrstr.File_class.Data = dataBuff;
    ehdrstr.File_class.Value = valueBuff;
    ehdrstr.File_class.Description = "File_class\n";

    //Data encoding
    sprintf(dataBuff,"%02X          ",m_dataBuff[5]);
    if(m_ehdr->e_ident[EI_DATA] == ELFDATANONE)
    {
        sprintf(valueBuff,"Invalid data      ");
    }
    else if(m_ehdr->e_ident[EI_DATA] == ELFDATA2LSB)
    {
        sprintf(valueBuff,"little endian     ");
    }
    else if(m_ehdr->e_ident[EI_DATA] == ELFDATA2MSB)
    {
        sprintf(valueBuff,"big endian        ");
    }
    ehdrstr.Data_encoding.pFile = "00000005    ";
    ehdrstr.Data_encoding.Data = dataBuff;
    ehdrstr.Data_encoding.Value = valueBuff;
    ehdrstr.Data_encoding.Description = "Data_encoding\n";

    //File version
    sprintf(dataBuff,"%02X          ",m_dataBuff[6]);
    sprintf(valueBuff,"%-2d                ",m_dataBuff[6]);
    ehdrstr.File_version.pFile = "00000006    ";
    ehdrstr.File_version.Data = dataBuff;
    ehdrstr.File_version.Value = valueBuff;
    ehdrstr.File_version.Description = "File_version\n";
*/
    return true;
}

string ELFMap::GetContentELFHead()
{
    return "test";
}

string ELFMap::GetContentSectionHead()
{
    return "test";
}

string ELFMap::GetContentProgramHead()
{
    return "test";
}

string ELFMap::GetContent(const char* part)
{
    string retstr;
    string ss(part);

    if(ss == "EFL header")
    {/*
        retstr += m_ehdrstr.magicNumber.pFile;
        retstr += ehdrstr.magicNumber.Data;
        retstr += ehdrstr.magicNumber.Value;
        retstr += ehdrstr.magicNumber.Description;
        retstr += ehdrstr.File_class.pFile;
        retstr += ehdrstr.File_class.Data;
        retstr += ehdrstr.File_class.Value;
        retstr += ehdrstr.File_class.Description;
        retstr += ehdrstr.Data_encoding.pFile;
        retstr += ehdrstr.Data_encoding.Data;
        retstr += ehdrstr.Data_encoding.Value;
        retstr += ehdrstr.Data_encoding.Description;
        retstr += ehdrstr.File_version.pFile;
        retstr += ehdrstr.File_version.Data;
        retstr += ehdrstr.File_version.Value;
        retstr += ehdrstr.File_version.Description;*/
    }
    else if(ss == "section header")
    {/*
        int i = 0;
        for(auto it : m_vshdr)
        {
            char sectionName[64] = {0};
            sprintf(sectionName,"[%02d]%s\n", i, m_shNameSection + it -> sh_name);
            retstr += sectionName;
            retstr += GetHexBase(m_ehdr->e_shoff+i*sizeof(Elf64_Shdr),sizeof(Elf64_Shdr));
            i++;
        }*/
    }
    else if(ss == "program header")
    {
        int i = 0;
        for(auto ep : m_vphdr)
        {/*
            char sectionname[64] = {0};
            sprintf(sectionname,"[%02d]\n", i);
            retstr += sectionname;
            retstr += GetHexBase(ehdr->e_phoff+i*sizeof(Elf64_Phdr),sizeof(Elf64_Phdr));
            i++;*/
        }
    }
    else
    {
        int i = 0;
        for(auto es : m_vshdr)
        {
            char sectionname[64] = {0};
            sprintf(sectionname,"[%02d]%s", i, m_shNameSection+es->sh_name);
            if(ss == sectionname)
            {
                retstr += GetHexBase(m_dataBuff+es->sh_offset,es->sh_size);
            }
            i++;
        }
    }
    return retstr;
}

string ELFMap::GetHex(const char* part)
{
    string retStr(showHead);
    string ss(part);

    if(ss == "all") //所有内容
    {
        retStr += GetHexBase(m_dataBuff,m_dataSize);
    }
    else if(ss == "EFL header")
    {
        retStr += GetHexBase(m_dataBuff,sizeof(Elf64_Ehdr));
    }
    else if(ss == "section header")
    {
        int i = 0;
        for(auto it : m_vshdr)
        {
            char sectionName[64] = {0};
            sprintf(sectionName,"[%02d]%s\n", i, m_shNameSection + it ->sh_name);
            retStr += sectionName;
            retStr += GetHexBase((char*)it,sizeof(Elf64_Shdr));
            i++;
        }
    }
    else if(ss == "program header")
    {
        int i = 0;
        for(auto it : m_vphdr)
        {
            char sectionName[64] = {0};
            sprintf(sectionName,"[%02d]\n", i);
            retStr += sectionName;
            retStr += GetHexBase((char*)it,sizeof(Elf64_Phdr));
            i++;
        }
    }
    else if(ss.substr(4,ss.length()-4) == ".symtab")
    {
        int i = 0;
        for(auto it : m_vsym)
        {
            char title[64] = {0};
            sprintf(title,"[%02d]%s\n", i,m_symNameSection+it->st_name);
            retStr += title;
            retStr += GetHexBase((char*)(it+i),sizeof(Elf64_Sym));
            i++;
        }
    }
    else
    {
        int i = 0;
        for(auto it : m_vshdr)
        {
            char sectionName[64] = {0};
            sprintf(sectionName,"[%02d]%s", i, m_shNameSection+it->sh_name);
            if(ss == sectionName)
            {
                retStr += GetHexBase(m_dataBuff+it->sh_offset,it->sh_size);
            }
            i++;
        }
    }
    return retStr;
}

string ELFMap::GetHexBase(char* begin,int len)
{
    string hexStr;
    int off = begin - m_dataBuff;

    for(int i = off; i< off+len ; i+=0x10)
    {
        char p[16] = {0};
        sprintf(p,"%08X  ",i);
        string hexOneLine(p);
        for(int j=0;j<0x10;j++)
        {
            if(i+j>=off+len) break;
            sprintf(p,"%02X ",*(unsigned char*)(m_dataBuff + i + j));
            hexOneLine += p;
            if(j == 7) hexOneLine += " ";
        }
        while(hexOneLine.size() < 60)
        {
            hexOneLine += " ";
        }
        for(int j=0;j<0x10;j++)
        {
            if(i+j>=off+len) break;
            unsigned char temp = *(unsigned char*)(m_dataBuff + i + j);
            if(temp>=0x20 && temp<=0x7f)
            {
                hexOneLine += temp;
            }
            else
            {
                hexOneLine += ".";
            }
        }
        hexOneLine += "\n";
        hexStr += hexOneLine;
    }
    return hexStr;
}
