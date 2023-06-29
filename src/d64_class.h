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
// Letzte Änderung am 26.06.2021         		//
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef D64_CLASS_H
#define D64_CLASS_H

#include <cstring>
#include <fstream>
#include <iostream>

#define D64_IMAGE_SIZE 174848

#include "./structs.h"

#define D64_NAME_LENGHT 24

class D64Class
{
    public:
    D64Class();
    ~D64Class();
	bool CreateDiskImage(FILE *file, const char* diskname, const char* diskid);
	int LoadD64(FILE *file);
    void ReLoad(uint8_t* d64_image_buffer);
    void UnLoadD64();
    bool ExportPrg(int file_number, const char* filename);

    char        d64_name[D64_NAME_LENGHT+1];
    uint16_t    file_count;
    uint32_t    d64_size;
    D64_FILES   d64_files[256];

    private:
    void ReadBlock(uint8_t track, uint8_t sector, uint8_t* buffer);
    void OutputBlock(uint8_t* buffer);
    void UpdateImageData();
    uint8_t d64_image[D64_IMAGE_SIZE];
    uint8_t block[256];
    uint8_t block_tmp[256];
};

#endif // D64_CLASS_H
