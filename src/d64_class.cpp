//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: d64_class.cpp                         //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include <cstring>
#include "./d64_class.h"

D64Class::D64Class()
{
    file_count = 0;
    d64_size = 0;
    d64_tracks = 35;
    d64_has_error_map = false;
    memset(d64_image, 0, sizeof(d64_image));
    memset(block, 0, sizeof(block));
    memset(block_tmp, 0, sizeof(block_tmp));

    for(int i=0; i<D64_NAME_LENGHT+1; i++) d64_name[i] = 0;
}

D64Class::~D64Class()
{
}

bool D64Class::CreateDiskImage(FILE *file, const char *diskname, const char *diskid)
{
    // Clear Foramtierter Dir Block
    static uint8_t directory_block[256] ={0x12,0x01,0x41,0x00,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x11,0xFC,0xFF,0x07,0x13,0xFF,0xFF,0x07,0x13,0xFF,0xFF,0x07,0x13,0xFF,0xFF,0x07,0x13,0xFF,0xFF,0x07,0x13,0xFF,0xFF,0x07,0x13,0xFF,0xFF,0x07,0x12,0xFF,0xFF,0x03,0x12,0xFF,0xFF,0x03,0x12,0xFF,0xFF,0x03,0x12,0xFF,0xFF,0x03,0x12,0xFF,0xFF,0x03,0x12,0xFF,0xFF,0x03,0x11,0xFF,0xFF,0x01,0x11,0xFF,0xFF,0x01,0x11,0xFF,0xFF,0x01,0x11,0xFF,0xFF,0x01,0x11,0xFF,0xFF,0x01,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0x32,0x41,0xA0,0xA0,0xA0,0xA0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    uint8_t block[256];

    for(int i=0; i<256; i++) block[i]=0;

    if (file == nullptr)
        return false;

    for(int i=0; i<357; i++) fwrite(block, 1, 256, file);

    for(int i=0; i<16; i++)
    {
        if(diskname[i] == 0) break;
        directory_block[0x90+i] = static_cast<uint8_t>(diskname[i]);
    }
    directory_block[0xA2] = static_cast<uint8_t>(diskid[0]);
    directory_block[0xA3] = static_cast<uint8_t>(diskid[1]);
    fwrite(directory_block, 1, 256, file);

    block[1] = 0xFF;
    fwrite(block, 1, 256, file);

    block[1] = 0x00;
    for(int i=0 ;i<324; i++) fwrite(block, 1, 256, file);

    fclose(file);

    return true;
}

int D64Class::LoadD64(FILE *file)
{
    if (file == nullptr)
    {
        UnLoadD64();
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    int tracks;
    bool has_error_map;
    if(!D64DetectLayout(file_size, &tracks, &has_error_map))
    {
        fclose(file);
        UnLoadD64();
        return 1;
    }

    size_t image_size = static_cast<size_t>(D64_TRACK_INDEX[tracks + 1]) * 256;
    memset(d64_image, 0, sizeof(d64_image));
    size_t reading_elements = fread(d64_image, 1, image_size, file);
    fclose(file);

    if(reading_elements != image_size)
    {
        UnLoadD64();
        return 1;
    }

    d64_tracks = tracks;
    d64_has_error_map = has_error_map;

    UpdateImageData();

    return 0;
}

void D64Class::ReLoad(const uint8_t *d64_image_buffer, int tracks)
{
    if(tracks < 35 || tracks > D64_MAX_TRACKS) tracks = 35;

    memset(d64_image, 0, sizeof(d64_image));
    memcpy(d64_image, d64_image_buffer, static_cast<size_t>(D64_TRACK_INDEX[tracks + 1]) * 256);
    d64_tracks = tracks;
    UpdateImageData();
}

void D64Class::UnLoadD64()
{
    memset(d64_image, 0, sizeof(d64_image));
    memset(d64_name, 0, sizeof(d64_name));
    memset(block, 0, sizeof(block));
    memset(block_tmp, 0, sizeof(block_tmp));
    memset(d64_files, 0, sizeof(d64_files));
    file_count = 0;
    d64_size = 0;
    d64_tracks = 35;
    d64_has_error_map = false;
}

bool D64Class::ExportPrg(int file_number, const char *filename)
{
    if(file_number < 0 || file_number >= file_count) return false;

    FILE* file = fopen(filename, "wb");
    if (file == nullptr)
        return false;

    uint8_t current_track = d64_files[file_number].Track;
    uint8_t current_sector = d64_files[file_number].Sektor;
    uint16_t max_blocks = D64_MAX_SECTORS; // Mehr Blöcke kann das Image nicht enthalten

    while(current_track != 0 && max_blocks > 0)
    {
        max_blocks--;
        if(!ReadBlock(current_track, current_sector, block))
            break;
        current_track = block[0];
        current_sector = block[1];
        if(current_track != 0)
            fwrite(block+2, 1, 254, file);
    }

    // Letzter Block: block[1] ist der Index des letzten genutzten Bytes
    if(current_track == 0 && current_sector > 1)
        fwrite(block+2, 1, current_sector - 1u, file);
    // Sonst ist die maximale Blockanzahl erreicht oder ein Block ungültig: Image vermutlich beschädigt

    fclose(file);
    return true;
}

bool D64Class::ReadBlock(uint8_t track, uint8_t sector, uint8_t *buffer)
{
    if(track < 1 || track > d64_tracks || sector >= D64_NUM_SECTORS[track-1])
    {
        memset(buffer, 0, 256);
        return false;
    }

    memcpy(buffer, d64_image + (D64_TRACK_INDEX[track] + sector) * 256, 256);
    return true;
}

void D64Class::UpdateImageData()
{
    file_count = 0;
    d64_size = 0;
    memset(d64_name, 0, sizeof(d64_name));

    if(!ReadBlock(18, 0, block))
        return;

    uint8_t track = block[0];
    uint8_t sector = block[1];

    for (int z=0; z<23; z++)
        d64_name[z] = static_cast<char>(block[0x90+z]);

    // Wenn Spur oder Sektor außerhalb der D64 Spezifikation ist
    // wird der Standard Ort gesetzt (18/1)
    if(track < 1 || track > d64_tracks) track = 18;
    if(sector >= D64_NUM_SECTORS[track-1]) sector = 1;

    // Directory Kette durchlaufen. Jeder Sektor wird nur einmal besucht (Schutz vor Schleifen)
    static bool visited[D64_MAX_SECTORS];
    memset(visited, 0, sizeof(visited));

    while(track != 0 && file_count < D64_MAX_FILES)
    {
        if(!ReadBlock(track, sector, block))
            break;

        int sector_index = D64_TRACK_INDEX[track] + sector;
        if(visited[sector_index])
            break;
        visited[sector_index] = true;

        uint8_t next_track = block[0];
        uint8_t next_sector = block[1];

        for(int si=0; si<256 && file_count < D64_MAX_FILES; si+=32)
        {
            // Leerer Eintrag (Typ und Spur 0) überspringen. Gelöschte Dateien (Typ ohne
            // Bit 7, "Splat-Files") bleiben erhalten, die GUI blendet sie optional aus.
            if(block[si+2] == 0 && block[si+3] == 0)
                continue;

            D64_FILES &entry = d64_files[file_count];
            entry.Typ = block[si+2];
            entry.Track = block[si+3];
            entry.Sektor = block[si+4];

            ReadBlock(entry.Track, entry.Sektor, block_tmp);
            entry.Adresse = static_cast<uint16_t>((block_tmp[3] << 8) | block_tmp[2]);
            entry.Laenge = static_cast<uint16_t>((block[si+31] << 8) | block[si+30]);

            for (int z=0; z<16; z++)
                entry.Name[z] = static_cast<char>(block[si+5+z]);
            entry.Name[16] = 0;

            d64_size += entry.Laenge;
            file_count++;
        }

        track = next_track;
        sector = next_sector;
    }
}
