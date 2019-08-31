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
// Letzte Änderung am 31.08.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "./d64_class.h"

D64Class::D64Class()
{
    file_count = 0;
    d64_size = 0;

    for(int i=0; i<D64_NAME_LENGHT+1; i++) d64_name[i] = 0;
}

D64Class::~D64Class()
{
}

bool D64Class::CreateDiskImage(const char *filename, const char *diskname, const char *diskid)
{
    // Clear Foramtierter Dir Block
    static uint8_t directory_block[256] ={0x12,0x01,0x41,0x00,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x11,0xFC,0xFF,0x07,0x13,0xFF,0xFF,0x07,0x13,0xFF,0xFF,0x07,0x13,0xFF,0xFF,0x07,0x13,0xFF,0xFF,0x07,0x13,0xFF,0xFF,0x07,0x13,0xFF,0xFF,0x07,0x12,0xFF,0xFF,0x03,0x12,0xFF,0xFF,0x03,0x12,0xFF,0xFF,0x03,0x12,0xFF,0xFF,0x03,0x12,0xFF,0xFF,0x03,0x12,0xFF,0xFF,0x03,0x11,0xFF,0xFF,0x01,0x11,0xFF,0xFF,0x01,0x11,0xFF,0xFF,0x01,0x11,0xFF,0xFF,0x01,0x11,0xFF,0xFF,0x01,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0x32,0x41,0xA0,0xA0,0xA0,0xA0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    uint8_t block[256];
    FILE* file;

    for(int i=0; i<256; i++) block[i]=0;

    file = fopen(filename, "wb");
    if (file == nullptr)
    {
        return false;
    }

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

int D64Class::LoadD64(const char *filename)
{
    uint8_t track, sector;
    FILE* file;
    size_t reading_elements;

    file = fopen(filename, "rb");
    if (file == nullptr)
    {
        return 1;
    }

    reading_elements = fread (d64_image,1,174848,file);
    fclose(file);

    if(reading_elements != 174848)
        return 0;

    ReadBlock(18, 0, block);

    // OutputBlock(Block);

    track = block[0];
    sector = block[1];

    // Wenn Spur oder Sektor außerhalb der D64 Spezifikation ist
    // wird der Standard Ort gesetzt (18/1)
    if(track > 35) track = 18;
    if(sector > 21) sector = 1;

    for (int z=0; z<23; z++)
    {
        d64_name[z] = static_cast<char>(block[0x90+z]);
    }

    d64_name[D64_NAME_LENGHT]=0;

    file_count=0;

    if(track != 0)
    {
        L20:
        int si = 0;
        int bx = 0;

        ReadBlock(track, sector,block);

        if(!((track == block[0]) && (sector == block[1])))
        {
            track = block[0];
            sector = block[1];
            L30:
            d64_files[file_count].Typ = block[si+2];

            if(block[si+3] != 0xFF)
            {
                d64_files[file_count].Track = block[si+3];
                d64_files[file_count].Sektor = block[si+4];

                ReadBlock(block[si+3],block[si+4], block_tmp);
                d64_files[file_count].Adresse = static_cast<uint16_t>(block_tmp[3] << 8);
                d64_files[file_count].Adresse += block_tmp[2];

                uint16_t tmp;
                tmp = static_cast<uint16_t>(block[si+31] << 8);
                tmp += block[si+30];
                d64_files[file_count].Laenge = tmp;

                d64_files[file_count].Name[16] = 0;

                for (int z=0; z<16; z++)
                {
                    d64_files[file_count].Name[z] = static_cast<char>(block[si+5+z]);
                }

                si += 32;
                bx++;
                file_count++;

                d64_size += d64_files[file_count].Laenge;


                if (bx<8) goto L30;
                else goto L20;
            }
        }
    }
    return 0;
}

void D64Class::UnLoadD64()
{
    for(int i=0; i<174848; i++) d64_image[i] = 0;
    for(int i=0; i<25; i++) d64_name[i] = 0;
    for(int i=0; i<256; i++)
    {
        block[i] = 0;
        block_tmp[i] = 0;
    }
    file_count = 0;
}

bool D64Class::ExportPrg(int file_number, const char *filename)
{
    FILE* file;

    uint16_t block_count;

    if(file_count==0) return false;

    block_count = d64_files[file_number].Laenge;
    if( block_count == 0) return false;

    file = fopen(filename, "wb");
    if (file == nullptr)
    {
        return false;
    }

    uint8_t current_track = d64_files[file_number].Track;
    uint8_t current_sector = d64_files[file_number].Sektor;

    while(current_track != 0)
    {
        ReadBlock(current_track, current_sector, block);
        current_track = block[0];
        current_sector = block[1];
        if(current_track == 0) fwrite(block+2, 1, current_sector - 1, file);
        else fwrite(block+2, 1, 254, file);
    }

    fclose(file);
    return true;
}

void D64Class::ReadBlock(uint8_t track, uint8_t sector, uint8_t *buffer)
{
    // Zeiger auf Track Anfang //	     <------------------------- Track 1-17 ----------------------> <------ Track 18-24 ------> <---- Track 25-30 ----> <-- Track 31-35 -->
    static uint16_t track_index[36]={0,0,21,42,63,84,105,126,147,168,189,210,231,252,273,294,315,336,357,376,395,414,433,452,471,490,508,526,544,562,580,598,615,632,649,666};
    // Sektoren pro Track //			  <------------------ Track 1-17 ------------------> <--- Track 18-24 --> <- Track 25-30 -> <-  31-35   ->
    static uint16_t track_max_sector[36]={0,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,19,19,19,19,19,19,19,18,18,18,18,18,18,17,17,17,17,17};

    if(track > 35)
    {
        for (int z=0;z<256;z++) buffer[z]=0;
        return;
    }

    if(sector >= track_max_sector[track])
    {
        for (int z=0;z<256;z++) buffer[z]=0;
        return;
    }

    int tmp;

    tmp = track_index[track] + sector;
    tmp *= 256;
    for (int z=0; z<256; z++) buffer[z] = d64_image[tmp + z];
}

void D64Class::OutputBlock(uint8_t *buffer)
{
    for(int i=0; i<16; i++)
    {
        int adr = i*16;
        for(int j=0; j<16; j++)
        {
            printf("%2.2X ",buffer[adr+j]);
        }
        printf("\n");
    }
    printf("\n");
}
