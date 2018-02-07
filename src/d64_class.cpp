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
// Letzte Änderung am 07.02.2018                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "d64_class.h"

D64Class::D64Class()
{
    DateiAnzahl = 0;
    D64Size = 0;
}

D64Class::~D64Class()
{
}

bool D64Class::CreateDiskImage(char* filename,char* diskname,char* diskid)
{
    // Clear Foramtierter Dir Block
    static unsigned char DIR_BLOCK[256] ={0x12,0x01,0x41,0x00,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x15,0xFF,0xFF,0x1F,0x11,0xFC,0xFF,0x07,0x13,0xFF,0xFF,0x07,0x13,0xFF,0xFF,0x07,0x13,0xFF,0xFF,0x07,0x13,0xFF,0xFF,0x07,0x13,0xFF,0xFF,0x07,0x13,0xFF,0xFF,0x07,0x12,0xFF,0xFF,0x03,0x12,0xFF,0xFF,0x03,0x12,0xFF,0xFF,0x03,0x12,0xFF,0xFF,0x03,0x12,0xFF,0xFF,0x03,0x12,0xFF,0xFF,0x03,0x11,0xFF,0xFF,0x01,0x11,0xFF,0xFF,0x01,0x11,0xFF,0xFF,0x01,0x11,0xFF,0xFF,0x01,0x11,0xFF,0xFF,0x01,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0x32,0x41,0xA0,0xA0,0xA0,0xA0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    unsigned char BLOCK[256];
    FILE* file;

    for(int i=0;i<256;i++) BLOCK[i]=0;

    file = fopen(filename, "wb");
    if (file == NULL)
    {
        return false;
    }

    for(int i=0;i<357;i++) fwrite(BLOCK,1,256,file);

    for(int i=0;i<16;i++)
    {
        if(diskname[i]==0) break;
        DIR_BLOCK[0x90+i]=diskname[i];
    }
    DIR_BLOCK[0xA2]=diskid[0];
    DIR_BLOCK[0xA3]=diskid[1];
    fwrite(DIR_BLOCK,1,256,file);

    BLOCK[1]=0xFF;
    fwrite(BLOCK,1,256,file);

    BLOCK[1]=0x00;
    for(int i=0;i<324;i++) fwrite(BLOCK,1,256,file);

    fclose(file);

    return true;
}

int D64Class::LoadD64(char* Filename)
{
    int Track,Sektor;
    FILE* file;
    size_t reading_elements;

    file = fopen(Filename, "rb");
    if (file == NULL)
    {
        return 1;
    }

    reading_elements = fread (D64Image,1,174848,file);
    fclose(file);

    if(reading_elements != 174848)
        return 0;

    ReadBlock(18,0,Block);

    // OutputBlock(Block);

    Track = Block[0];
    Sektor = Block[1];

    // Wenn Spur oder Sektor außerhalb der D64 Spezifikation ist
    // wird der Standard Ort gesetzt (18/1)
    if(Track > 35) Track = 18;
    if(Sektor > 21) Sektor = 1;

    int z;
    for (z=0;z<23;z++)
    {
        D64Name[z]=Block[0x90+z];
    }
    D64Name[z]=0;

    DateiAnzahl=0;
L20:
    int SI=0;
    int BX=0;
    if(Track==0) goto L50;
    ReadBlock(Track,Sektor,Block);
    if((Track == Block[0]) && (Sektor == Block[1])) goto L50;
    Track = Block[0];
    Sektor = Block[1];
L30:
    D64Files[DateiAnzahl].Typ=Block[SI+2];
    if(Block[SI+3]==0xFF) goto L50;

    D64Files[DateiAnzahl].Track=Block[SI+3];
    D64Files[DateiAnzahl].Sektor=Block[SI+4];

    ReadBlock(Block[SI+3],Block[SI+4],BlockTmp);
    D64Files[DateiAnzahl].Adresse=BlockTmp[3]<<8;
    D64Files[DateiAnzahl].Adresse+=BlockTmp[2];

    unsigned short int TEMP;
    TEMP=Block[SI+31]<<8;
    TEMP+=Block[SI+30];
    D64Files[DateiAnzahl].Laenge=TEMP;

    D64Files[DateiAnzahl].Name[16]=0;

    for (z=0;z<16;z++)
    {
        D64Files[DateiAnzahl].Name[z]=Block[SI+5+z];
    }

    SI+=32;
    BX++;
    DateiAnzahl++;

    D64Size+=D64Files[DateiAnzahl].Laenge;
    if (BX<8) goto L30;
    else goto L20;
L50:
    return 0;
}

void D64Class::UnLoadD64(void)
{
    for(int i=0;i<174848;i++) D64Image[i] = 0;
    for(int i=0;i<25;i++) D64Name[i] = 0;
    for(int i=0;i<256;i++)
    {
        Block[i] = 0;
        BlockTmp[i] = 0;
    }
    DateiAnzahl=0;
}

bool D64Class::ExportPrg(int DateiNummer,char* Filename)
{
    FILE* file;

    int BLOCK_ANZAHL;

    if(DateiAnzahl==0) return false;

    BLOCK_ANZAHL=D64Files[DateiNummer].Laenge;
    if( BLOCK_ANZAHL==0) return false;

    file = fopen(Filename, "wb");
    if (file == NULL)
    {
        return false;
    }

    int aktTck = D64Files[DateiNummer].Track;
    int aktSkt = D64Files[DateiNummer].Sektor;

    while(aktTck != 0)
    {
        ReadBlock(aktTck,aktSkt,Block);
        aktTck = Block[0];
        aktSkt = Block[1];
        if(aktTck == 0) fwrite(Block+2,1,aktSkt-1,file);
        else fwrite(Block+2,1,254,file);
    }

    fclose(file);
    return true;
}

void D64Class::ReadBlock(unsigned char Track, unsigned char Sektor, unsigned char * PUFFER)
{
    // Zeiger auf Track Anfang //	     <------------------------- Track 1-17 ----------------------> <------ Track 18-24 ------> <---- Track 25-30 ----> <-- Track 31-35 -->
    static unsigned short TRACK_INDEX[36]={0,0,21,42,63,84,105,126,147,168,189,210,231,252,273,294,315,336,357,376,395,414,433,452,471,490,508,526,544,562,580,598,615,632,649,666};
    // Sektoren pro Track //			  <------------------ Track 1-17 ------------------> <--- Track 18-24 --> <- Track 25-30 -> <-  31-35   ->
    static unsigned short TRACK_MAX_SEKTOR[36]={0,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,19,19,19,19,19,19,19,18,18,18,18,18,18,17,17,17,17,17};

    if(Track > 35)
    {
        for (int z=0;z<256;z++) PUFFER[z]=0;
        return;
    }

    if(Sektor >= TRACK_MAX_SEKTOR[Track])
    {
        for (int z=0;z<256;z++) PUFFER[z]=0;
        return;
    }

    int TEMP;

    TEMP=TRACK_INDEX[Track]+Sektor;
    TEMP*=256;
    for (int z=0;z<256;z++) PUFFER[z]=D64Image[TEMP+z];
}

void D64Class::OutputBlock(unsigned char *buffer)
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
