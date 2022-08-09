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
    Elf64_Ehdr_str tempstr;
    char dataBuff[64] = {0};
    char valueBuff[64] = {0};

    //Magic_number
    sprintf(dataBuff,"%02X%02X%02X%02X    ",datap[0],datap[1],datap[2],datap[3]);
    sprintf(valueBuff,"%02X-%c%c%c            ",datap[0],datap[1],datap[2],datap[3]);
    ehdrstr.Magic_number.pFile = "00000000    ";
    ehdrstr.Magic_number.Data = dataBuff;
    ehdrstr.Magic_number.Value = valueBuff;
    ehdrstr.Magic_number.Description = "Magic_number\n";

    //File_class
    sprintf(dataBuff,"%02X          ",datap[4]);
    if(ehdr->e_ident[EI_CLASS] == ELFCLASSNONE)
    {
        sprintf(valueBuff,"Invalid class     ");
    }
    else if(ehdr->e_ident[EI_CLASS] == ELFCLASS32)
    {
        sprintf(valueBuff,"32-bit objects    ");
    }
    else if(ehdr->e_ident[EI_CLASS] == ELFCLASS64)
    {
        sprintf(valueBuff,"64-bit objects    ");
    }
    ehdrstr.File_class.pFile = "00000004    ";
    ehdrstr.File_class.Data = dataBuff;
    ehdrstr.File_class.Value = valueBuff;
    ehdrstr.File_class.Description = "File_class\n";

    //Data_encoding
    sprintf(dataBuff,"%02X          ",datap[5]);
    if(ehdr->e_ident[EI_DATA] == ELFDATANONE)
    {
        sprintf(valueBuff,"Invalid data      ");
    }
    else if(ehdr->e_ident[EI_DATA] == ELFDATA2LSB)
    {
        sprintf(valueBuff,"little endian     ");
    }
    else if(ehdr->e_ident[EI_DATA] == ELFDATA2MSB)
    {
        sprintf(valueBuff,"big endian        ");
    }
    ehdrstr.Data_encoding.pFile = "00000005    ";
    ehdrstr.Data_encoding.Data = dataBuff;
    ehdrstr.Data_encoding.Value = valueBuff;
    ehdrstr.Data_encoding.Description = "Data_encoding\n";

    //File_version
    sprintf(dataBuff,"%02X          ",datap[6]);
    sprintf(valueBuff,"%-2d                ",datap[6]);
    ehdrstr.File_version.pFile = "00000006    ";
    ehdrstr.File_version.Data = dataBuff;
    ehdrstr.File_version.Value = valueBuff;
    ehdrstr.File_version.Description = "File_version\n";

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
        retstr += ehdrstr.Magic_number.pFile;
        retstr += ehdrstr.Magic_number.Data;
        retstr += ehdrstr.Magic_number.Value;
        retstr += ehdrstr.Magic_number.Description;
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
        retstr += ehdrstr.File_version.Description;
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
