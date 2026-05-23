//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: crt_class.h                           //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "./crt_class.h"
#include <cstring>

CRTClass::CRTClass(FILE *file)
{
    this->file = file;

    if(file == nullptr)
    {
        error_code = 0xff;
        return;
    }

    error_code = LoadCRT(file);
}

CRTClass::~CRTClass()
{
    for(int i=0; i<chip_count; ++i)
    {
        if(chip_header[i]->chip_rom != nullptr)
        {
            delete[] chip_header[i]->chip_rom;
            chip_header[i]->chip_rom = nullptr;
        }
    }

    if(header != nullptr)
    {
        delete header;
        header = nullptr;
    }
}

uint16_t CRTClass::GetImageVersion()
{
    return header->version << 8 | header->version >> 8;   // Die Version ist im oberen Byte gespeichert
}

uint16_t CRTClass::GetCartridgeType()
{
    return header->cartridge_type << 8 | header->cartridge_type >> 8;   // Die Version ist im oberen Byte gespeichert
}

int CRTClass::GetChipType(int index)
{
    if(index < 0 || index >= chip_count)
        return -1;  // Ungültiger Index

    return chip_header[index]->chip_type << 8 | chip_header[index]->chip_type >> 8;
}

uint16_t CRTClass::GetChipBankNumber(int index)
{
    if(index < 0 || index >= chip_count)
        return 0;  // Ungültiger Index

    return chip_header[index]->bank_number << 8 | chip_header[index]->bank_number >> 8;
}

uint16_t CRTClass::GetChipLoadAddress(int index)
{
    if(index < 0 || index >= chip_count)
        return 0;  // Ungültiger Index

    return chip_header[index]->load_address << 8 | chip_header[index]->load_address >> 8;
}

uint16_t CRTClass::GetChipRomSize(int index)
{
    if(index < 0 || index >= chip_count)
        return 0;  // Ungültiger Index

    return chip_header[index]->rom_size << 8 | chip_header[index]->rom_size >> 8;
}

void CRTClass::CopyChipRomData(int index, uint8_t *destination)
{
    if(index < 0 || index >= chip_count)
        return;  // Ungültiger Index

    if(destination == nullptr)
        return;  // Ungültiger Zielpuffer

    memcpy(destination, chip_header[index]->chip_rom, GetChipRomSize(index));
}

uint32_t CRTClass::GetHeaderLength()
{
    return SwapEndianness(header->header_length);
}

int CRTClass::LoadCRT(FILE *file)
{
    size_t reading_bytes;

    header = new CRT_HEADER;

    reading_bytes = fread(header, 1, sizeof(CRT_HEADER)-1, file);
    if(reading_bytes != sizeof(CRT_HEADER)-1) // -1 wegen dem letzten Byte des Image Namens, da dieser 33 Bytes lang ist, aber nur 32 Bytes gelesen werden
    {
        return 0x01;    // Fehler beim Lesen des Headers
    }
    header->cartridge_name[sizeof(header->cartridge_name)-1] = 0; // Nullterminiere den Image Namen

    if(0 != strncmp("C64 CARTRIDGE   ", header->signature, 16))
    {
        fclose(file);
        return 0x02;
    }

    chip_count = 0;
    chip_header[chip_count] = new CHIP_HEADER;

    while((fread(chip_header[chip_count], 1, (sizeof(CHIP_HEADER) - sizeof(CHIP_HEADER::chip_rom)), file) == (sizeof(CHIP_HEADER) - sizeof(CHIP_HEADER::chip_rom))))
    {
        if(0 != strncmp("CHIP", chip_header[chip_count]->signature, 4))
        {
            break;  // Kein gültiger CHIP Header mehr, wir haben alle Chips gelesen
        }

        // Jetzt können wir die ROM Daten für diesen Chip lesen
        chip_count++;
        uint16_t chip_rom_size = GetChipRomSize(chip_count-1);

        chip_header[chip_count-1]->chip_rom = new uint8_t[chip_rom_size]; // Bereite den Speicher für die ROM Daten vor
        reading_bytes = fread(chip_header[chip_count-1]->chip_rom, 1, chip_rom_size, file);
        if(reading_bytes != chip_rom_size)
        {
            return 0x03;    // Fehler beim Lesen der Chip Daten
        }

        if(chip_count >= MAX_CHIPS)
        {
            break;  // Wir haben die maximale Anzahl an Chips erreicht
        }
        else
            chip_header[chip_count] = new CHIP_HEADER; // Bereite den nächsten Chip Header vor
    }
    return 0x00;
}

int CRTClass::SaveCRT(FILE *file)
{
    if(file == nullptr)
        return 0x01;    // Ungültige Datei

    return 0x00;    // Noch nicht implementiert
}

// Convertiert die Endianness eines 32 Bit Wertes (Little Endian <-> Big Endian)
inline uint32_t CRTClass::SwapEndianness(uint32_t value)
{
    uint32_t h,l;

    l = (value >> 16) & 0xffff;
    h = value & 0xffff;

    l = ((l << 8) | (l >> 8)) & 0xffff;
    h = ((h << 8) | (h >> 8)) & 0xffff;

    value = h<<16 | l;

    return value;
}