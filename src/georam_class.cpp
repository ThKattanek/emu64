//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: georam_class.cpp                      //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "georam_class.h"

GEORAMClass::GEORAMClass() :
    geo_ram_insert(false),
    mem_frame(0),
    reg_start_address(0xdf80),  // original GeoRAM
    geo_ram_mode(_512KiB)
{
}

GEORAMClass::~GEORAMClass()
{
}

void GEORAMClass::Insert(void)
{
    geo_ram_insert = true;
    mem_frame = 0;
}

void GEORAMClass::Remove(void)
{
    geo_ram_insert = false;
}

int GEORAMClass::LoadImage(const char *filename)
{
    /// GEORAM Inhalt laden ...
    FILE* file;
    file = fopen(filename, "rb");
    if (file == NULL)
    {
        return 1;
    }

    size_t reading_bytes = fread(ram, 1, MAX_GEORAM_SIZE, file);

    if(reading_bytes != geo_ram_size_tbl[geo_ram_mode & 0x03])
    {
        // GeoRam Image entspricht nicht der aktuell eingestellten GeoRAM Größe.
        return 2;
    }

    fclose(file);
    return 0;
}

int GEORAMClass::SaveImage(const char *filename)
{
    /// GEORAM Inhalt speichern ...
    FILE* file;
    file = fopen(filename, "wb");
    if (file == NULL)
    {
        return 1;
    }
    fwrite(ram, 1, geo_ram_size_tbl[geo_ram_mode & 0x03], file);
    fclose(file);
    return 0;
}

void GEORAMClass::ClearRAM(void)
{
    for(int i=0; i < MAX_GEORAM_SIZE; i++) ram[i] = 0;
}

void GEORAMClass::SetGeoRamType(uint8_t type)
{
    switch(type)
    {
        case _GEORAM:
        reg_start_address = 0xdf80;
        break;

        case _NEORAM:
        reg_start_address = 0xdfc0;
        break;

        default:
        reg_start_address = 0xdf80;
        break;
        }
}

void GEORAMClass::SetGeoRamMode(uint8_t mode)
{
        geo_ram_mode = mode;
}

void GEORAMClass::WriteIO1(uint16_t adresse, uint8_t value)
{
    ram[mem_frame + (adresse - 0xde00)] = value;
}

void GEORAMClass::WriteIO2(uint16_t address, uint8_t value)
{
    // georam_type 0 = georam / 1 = neoram
    // georam address from $df80 - $dfff        // source: schematic Berkley Softworhs geoRAM 512 (U13A -> 74LS139) //
    // neoram address from $dfc0 - $dfff        // source: schematic Neoram-level2 (V3 -> 74138N) // (12.01.2007)

    if(((address & 0x01) == 0x00) && (address >= reg_start_address))
    {
        // "Track"
        _dffe = value & 0x3f;

        mem_frame = (_dfff << 14) | (_dffe << 8);
    }

    if(((address & 0x01) == 0x01) && (address >= reg_start_address))
    {
        // "Sector"
        switch(geo_ram_mode)
        {
        case _512KiB:
            _dfff = value & 0x1f;
            break;
        case _1024KiB:
            _dfff = value & 0x3f;
            break;
        case _2048KiB:
            _dfff = value & 0x7f;
            break;
        case _4096KiB:
            _dfff = value;
            break;
        default:
            _dfff = value & 0x1f;   // default geoRAM
            break;
        }

        mem_frame = (_dfff << 14) | (_dffe << 8);
    }
}

unsigned char GEORAMClass::ReadIO1(uint16_t address)
{
    return ram[mem_frame + (address - 0xde00)];
}

unsigned char GEORAMClass::ReadIO2(uint16_t address)
{
    (void)address;

    return 0;
}
