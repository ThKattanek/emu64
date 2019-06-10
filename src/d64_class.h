//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: d64_class.h                           //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 10.06.2019        		//
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef D64_CLASS_H
#define D64_CLASS_H

#include <cstring>
#include <fstream>
#include <iostream>

using namespace std;

#include "./structs.h"

#define D64_NAME_LENGHT 24

class D64Class
{
    public:
    D64Class();
    ~D64Class();
    bool CreateDiskImage(const char* filename, const char* diskname, const char* diskid);
    int LoadD64(const char* filename);
    void UnLoadD64();
    bool ExportPrg(int file_number, const char* filename);

    char        d64_name[D64_NAME_LENGHT+1];
    uint16_t    file_count;
    uint32_t    d64_size;
    D64_FILES   d64_files[256];

    private:
    void ReadBlock(uint8_t track, uint8_t sector, uint8_t* buffer);
    void OutputBlock(uint8_t* buffer);
    unsigned char d64_image[174848];
    unsigned char block[256];
    unsigned char block_tmp[256];
};

#endif // D64_CLASS_H
