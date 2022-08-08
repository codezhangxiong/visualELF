#include <stdio.h>
#include "elfmap.h"

const char* showhead = "RVA        0  1  2  3  4  5  6  7   8  9  A  B  C  D  E  F  Value   \n";

elfmap::elfmap()
{
}

elfmap::~elfmap()
{
    if(datap!=NULL)
    {
        free(datap);
    }
}

bool elfmap::parser_ELFhead()
{
    char buff[64] = {0};
    sprintf(buff,"%02X-%c%c%c\n",
            ehdr->e_ident[0],ehdr->e_ident[1],ehdr->e_ident[2],ehdr->e_ident[3]);
    ehdrstr.Magic_number += buff;
    if(ehdr->e_ident[EI_CLASS] == ELFCLASSNONE)
    {
        sprintf(buff,"Invalid class\n");
    }
    else if(ehdr->e_ident[EI_CLASS] == ELFCLASS32)
    {
        sprintf(buff,"32-bit objects\n");
    }
    else if(ehdr->e_ident[EI_CLASS] == ELFCLASS64)
    {
        sprintf(buff,"64-bit objects\n");
    }
    ehdrstr.File_class += buff;
    if(ehdr->e_ident[EI_DATA] == ELFDATANONE)
    {
        sprintf(buff,"Invalid class\n");
    }
    else if(ehdr->e_ident[EI_DATA] == ELFDATA2LSB)
    {
        sprintf(buff,"little endian\n");
    }
    else if(ehdr->e_ident[EI_DATA] == ELFDATA2MSB)
    {
        sprintf(buff,"big endian\n");
    }
    ehdrstr.Data_encoding += buff;
    sprintf(buff,"%d",ehdr->e_ident[EI_VERSION]);
    ehdrstr.File_version += buff;
    return true;
}

bool elfmap::parser(const char *filename)
{
    string tempstr(filename);
    elffilename = tempstr.substr(tempstr.find_last_of('/') + 1);

    FILE* f;
    f = fopen(filename, "rb");
    fseek(f, 0, SEEK_END);
    datalen = ftell(f);
    fseek(f, 0, SEEK_SET);
    datap = (char*)malloc(datalen + 1);
    fread(datap, 1, datalen, f);
    fclose(f);

    ehdr = (Elf64_Ehdr*)datap;
    for(int i=0; i< ehdr->e_phnum; i++)
    {
        Elf64_Phdr *phdr = (Elf64_Phdr*)(datap + ehdr->e_phoff) + i;
        vphdr.push_back(phdr);
    }
    for(int i=0; i< ehdr->e_shnum; i++)
    {
        Elf64_Shdr *shdr = (Elf64_Shdr*)(datap + ehdr->e_shoff) + i;
        vshdr.push_back(shdr);
        if(ehdr->e_shstrndx == i)
        {
            shname = (char*)(datap + shdr->sh_offset);
        }
    }

    parser_ELFhead();

    return true;
}

string elfmap::get_content(const char* section)
{
    string retstr;
    string ss(section);

    if(ss == "EFL header")
    {
        retstr += showhead;
        retstr += get_hex_base(0,EI_NIDENT);
        retstr += ehdrstr.Magic_number;
        retstr += ehdrstr.File_class;
        retstr += ehdrstr.Data_encoding;
        retstr += ehdrstr.File_version;
    }
    else if(ss == "section header")
    {
        int i = 0;
        for(auto es : vshdr)
        {
            char sectionname[64] = {0};
            sprintf(sectionname,"[%02d]%s\n", i, shname+es->sh_name);
            retstr += sectionname;
            retstr += get_hex_base(ehdr->e_shoff+i*sizeof(Elf64_Shdr),sizeof(Elf64_Shdr));
            i++;
        }
    }
    else if(ss == "program header")
    {
        int i = 0;
        for(auto ep : vphdr)
        {
            char sectionname[64] = {0};
            sprintf(sectionname,"[%02d]\n", i);
            retstr += sectionname;
            retstr += get_hex_base(ehdr->e_phoff+i*sizeof(Elf64_Phdr),sizeof(Elf64_Phdr));
            i++;
        }
    }
    else
    {
        int i = 0;
        for(auto es : vshdr)
        {
            char sectionname[64] = {0};
            sprintf(sectionname,"[%02d]%s", i, shname+es->sh_name);
            if(ss == sectionname)
            {
                retstr += get_hex_base(es->sh_offset,es->sh_size);
            }
            i++;
        }
    }
    return retstr;
}

string elfmap::get_hex(const char* section)
{
    string retstr(showhead);
    string ss(section);

    if(ss == "all")
    {
        retstr += get_hex_base(0,datalen);
    }
    else if(ss == "EFL header")
    {
        retstr += get_hex_base(0,sizeof(Elf64_Ehdr));
    }
    else if(ss == "section header")
    {
        int i = 0;
        for(auto es : vshdr)
        {
            char sectionname[64] = {0};
            sprintf(sectionname,"[%02d]%s\n", i, shname+es->sh_name);
            retstr += sectionname;
            retstr += get_hex_base(ehdr->e_shoff+i*sizeof(Elf64_Shdr),sizeof(Elf64_Shdr));
            i++;
        }
    }
    else if(ss == "program header")
    {
        int i = 0;
        for(auto ep : vphdr)
        {
            char sectionname[64] = {0};
            sprintf(sectionname,"[%02d]\n", i);
            retstr += sectionname;
            retstr += get_hex_base(ehdr->e_phoff+i*sizeof(Elf64_Phdr),sizeof(Elf64_Phdr));
            i++;
        }
    }
    else
    {
        int i = 0;
        for(auto es : vshdr)
        {
            char sectionname[64] = {0};
            sprintf(sectionname,"[%02d]%s", i, shname+es->sh_name);
            if(ss == sectionname)
            {
                retstr += get_hex_base(es->sh_offset,es->sh_size);
            }
            i++;
        }
    }
    return retstr;
}

string elfmap::get_hex_base(int off,int len)
{
    string hexstr;
    for(int i = off; i< off+len; i+=0x10)
    {
        char p[16] = {0};
        sprintf(p,"%08X  ",i);
        string oneline(p);
        for(int j=0;j<0x10;j++)
        {
            if(i+j>=off+len) break;
            sprintf(p,"%02X ",*(unsigned char*)(datap + i + j));
            oneline += p;
            if(j == 7) oneline += " ";
        }
        while(oneline.size() < 60)
        {
            oneline += " ";
        }
        for(int j=0;j<0x10;j++)
        {
            if(i+j>=off+len) break;
            unsigned char temp = *(unsigned char*)(datap + i + j);
            if(temp>=0x20 && temp<=0x7f)
            {
                oneline += temp;
            }
            else
            {
                oneline += ".";
            }
        }
        oneline += "\n";
        hexstr += oneline;
    }
    return hexstr;
}
