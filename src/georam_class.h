//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: georam_class.h                        //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef GEORAM_CLASS_H
#define GEORAM_CLASS_H

#include <inttypes.h>
#include <fstream>

// 512KiB, 1MiB, 2MiB, 4MiB

enum GEO_RAM_MODES {_512KiB, _1024KiB, _2048KiB, _4096KiB};
enum GEO_RAM_TYPE {_GEORAM, _NEORAM};

#define MAX_GEORAM_SIZE 4194304

const uint32_t geo_ram_size_tbl[]={524288, 1048576, 2097152, 4194304};

class GEORAMClass
{
    public:
    GEORAMClass();
    ~GEORAMClass();

    // Funktionen
    void WriteIO1(uint16_t address, uint8_t value);
    void WriteIO2(uint16_t address, uint8_t value);
    unsigned char ReadIO1(uint16_t address);
    unsigned char ReadIO2(uint16_t address);

    void Remove(void);
    void Insert(void);
    int LoadRamImage(const char *filename);
    int SaveRamImage(const char *filename);
    void ClearRAM(void);
    void SetGeoRamType(uint8_t type);   // _GEORAM or _NEORAM
    uint8_t GetGeoRamMode();            // return _512, _1024, _2048, 4096
    void SetGeoRamMode(uint8_t mode);   // _512, _1024, _2048, 4096

    private:
    // Variablen
    bool		geo_ram_insert;
                                        // $DFFE   $DFFF
    uint8_t     ram[MAX_GEORAM_SIZE];   // 64   x  256  x  256 = 4MiB // 4194304

    uint32_t    mem_frame;
    uint8_t     _dffe, _dfff;
    uint16_t    reg_start_address;
    uint8_t     geo_ram_mode;
};

#endif // GEORAM_CLASS_H
