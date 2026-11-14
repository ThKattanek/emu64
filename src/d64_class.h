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
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef D64_CLASS_H
#define D64_CLASS_H

#include <cstdio>
#include <cstdint>

#include "./structs.h"
#include "./d64_layout.h"

#define D64_NAME_LENGHT 24
#define D64_MAX_FILES 256

class D64Class
{
    public:
    D64Class();
    ~D64Class();
    bool CreateDiskImage(FILE *file, const char* diskname, const char* diskid);
    int LoadD64(FILE *file);                                    // 0 = ok, 1 = Fehler (Datei fehlt/zu klein)
    void ReLoad(const uint8_t* d64_image_buffer, int tracks);   // Image vom Emulator (35, 40 oder 42 Spuren)
    void UnLoadD64();
    bool ExportPrg(int file_number, FILE *file);

    char        d64_name[D64_NAME_LENGHT+1];
    uint16_t    file_count;
    uint32_t    d64_size;                   // Summe der Blöcke aller Dateien
    int         d64_tracks;                 // Spurzahl des geladenen Images (35, 40, 42)
    bool        d64_has_error_map;          // Image enthält Fehlerinfo
    D64_FILES   d64_files[D64_MAX_FILES];

    private:
    bool ReadBlock(uint8_t track, uint8_t sector, uint8_t* buffer);
    void UpdateImageData();
    uint8_t d64_image[D64_IMAGE_SIZE_MAX];
    uint8_t block[256];
    uint8_t block_tmp[256];
};

#endif // D64_CLASS_H
