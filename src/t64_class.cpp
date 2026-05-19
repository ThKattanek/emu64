//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: t64_class.cpp                         //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "t64_class.h"

T64Class::T64Class(FILE *file)
{
    if(file == nullptr)
    {
        error_code = 0xff;
        return;
    }

    this->file = file;
    file_entries = nullptr;

    error_code = LoadT64(file);
}

T64Class::~T64Class()
{
    if(file_entries != nullptr)
    {
        delete[] file_entries;
        file_entries = nullptr;
    }

    if(header != nullptr)
    {
        delete header;
        header = nullptr;
    }
}

uint16_t T64Class::GetTotalEntries()
{
    if(header == nullptr)
    {
        return 0;   // Kein gültiger Header
    }

    return header->total_file_entries;
}

char *T64Class::GetFileName(int index)
{
    if(index < 0 || index >= header->total_file_entries)
    {
        return nullptr;    // Ungültiger Index
    }

    return file_entries[index].filename;
}

uint8_t T64Class::GetC64sFileType(int index)
{
    if(index < 0 || index >= header->total_file_entries)
    {
        return 0xff;   // Ungültiger Index
    }

    return file_entries[index].c64s_file_type;
}

uint8_t T64Class::GetFloppy1541FileType(int index)
{
    if(index < 0 || index >= header->total_file_entries)
    {
        return 0xff;   // Ungültiger Index
    }

    return file_entries[index].floppy1541_file_type;
}

int T64Class::LoadFileToRAM(int index, uint8_t *c64_ram)
{
    if (file == nullptr)
        return -1;  // Kein gültiges Dateihandle

    if(index < 0 || index >= header->total_file_entries)
    {
        return -2;  // Ungültiger Index
    }

    uint16_t start_address = file_entries[index].start_address;
    uint16_t end_address = file_entries[index].end_address;
    uint32_t data_offset = file_entries[index].data_offset;
    uint32_t file_size = end_address - start_address + 1;

    if(file_size > 65536)  // Maximale Dateigröße von 64KB überschritten
    {
        return -3;  // Puffer zu klein
    }

    fseek(file, data_offset, SEEK_SET);
    size_t read_bytes = fread(c64_ram + start_address, 1, file_size, file);

    if(read_bytes != file_size)
    {
        return -4;  // Fehler beim Lesen der Dateidaten
    }

    c64_ram[0x2B] = start_address & 0xff;
    c64_ram[0x2C] = start_address >> 8;

    c64_ram[0x2D] = static_cast<uint8_t>(end_address);
    c64_ram[0x2E] = static_cast<uint8_t>(end_address>>8);
    c64_ram[0xAE] = static_cast<uint8_t>(end_address);
    c64_ram[0xAF] = static_cast<uint8_t>(end_address>>8);

    return 0;   // Erfolg
}

int T64Class::GetFileOffset(int index)
{
    if(index < 0 || index >= header->total_file_entries)
    {
        return -1;    // Ungültiger Index
    }

    return static_cast<int>(file_entries[index].data_offset);
}

uint16_t T64Class::GetFileStartAddress(int index)
{
    if(index < 0 || index >= header->total_file_entries)
    {
        return 0;   // Ungültiger Index
    }

    return file_entries[index].start_address;
}

uint16_t T64Class::GetFileEndAddress(int index)
{
    if(index < 0 || index >= header->total_file_entries)
    {
        return 0;   // Ungültiger Index
    }

    return file_entries[index].end_address;
}

int T64Class::LoadT64(FILE *file)
{
    bool t64_found = false;
    size_t reading_bytes;

    header = new T64_HEADER;

    reading_bytes = fread(header, 1, sizeof(T64_HEADER)-1, file);
    if(reading_bytes != sizeof(T64_HEADER)-1) // -1 wegen dem letzten Byte des Image Namens, da dieser 25 Bytes lang ist, aber nur 24 Bytes gelesen werden
    {
        return 0x01;    // Fehler beim Lesen des Headers
    }
    header->image_name[24] = 0; // Nullterminiere den Image Namen

    // Prüfe ob die ersten 3 BYTES 'C64' sind und ob in den ersten 15 Bytes irgendwo 'tape' steht
    if((header->signature[0] != 'C') || (header->signature[1] != '6') || (header->signature[2] != '4'))
    {
        return 0x02;    // Fehlerhafte Signatur (erste 3 Bytes müssen 'C64' sein)
    }

    for(int i=3; i<15; i++)
    {
        if((header->signature[i] == 't') && (header->signature[i+1] == 'a') && (header->signature[i+2] == 'p') && (header->signature[i+3] == 'e'))
        {
            t64_found = true;
            break;
        }
    }

    if(!t64_found)
    {
        return 0x02;    // Fehlerhafte Signatur ('tape' nicht in der Signatur gefunden)
    }

    if(header->total_file_entries == 0)
    {
        return 0x03;    // Fehler: Keine Dateien in der T64 Datei gefunden
    }

    file_entries = new T64_FILE_ENTRY[header->total_file_entries];

    for(int i=0; i < header->total_file_entries; i++)
    {
        reading_bytes = fread(&file_entries[i], 1, sizeof(T64_FILE_ENTRY)-1, file);
        if(reading_bytes != sizeof(T64_FILE_ENTRY)-1) // -1 wegen dem letzten Byte des Dateinamens, da dieser 17 Bytes lang ist, aber nur 16 Bytes gelesen werden
        {
            return 0x04;    // Fehler beim Lesen eines Dateieintrags
        }
        file_entries[i].filename[16] = 0; // Nullterminiere den Dateinamen
    }

    return 0x00;
}
