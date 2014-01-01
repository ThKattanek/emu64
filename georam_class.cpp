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
// Letzte Änderung am 01.01.2014                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "georam_class.h"

GEORAMClass::GEORAMClass()
{
}

GEORAMClass::~GEORAMClass()
{
}

void GEORAMClass::Insert(void)
{
    GEORAMInsert = true;
    MemFrame = 0;
}

void GEORAMClass::Remove(void)
{
    GEORAMInsert = false;
}

int GEORAMClass::LoadRAM(char *filename)
{
    /// GEORAM Inhalt laden ...
    FILE* file;
    file = fopen(filename,"rb");
    if (file == NULL)
    {
        return 1;
    }
    fread(RAM,1,2097152,file);
    fclose(file);
    return 0;
}

int GEORAMClass::SaveRAM(char *filename)
{
    /// GEORAM Inhalt speichern ...
    FILE* file;
    file = fopen(filename,"wb");
    if (file == NULL)
    {
        return 1;
    }
    fwrite(RAM,1,2097152,file);
    fclose(file);
    return 0;
}

void GEORAMClass::ClearRAM(void)
{
    for(int i=0;i<2097152;i++) RAM[i] = 0;
}

void GEORAMClass::WriteIO1(unsigned short adresse,unsigned char wert)
{
    RAM[MemFrame + (adresse-0xde00)] = wert;
}

void GEORAMClass::WriteIO2(unsigned short adresse,unsigned char wert)
{
    switch(adresse)
    {
        case 0xdffe:	// WRITE ONLY
            _DFFE = wert & 0x3F;
            MemFrame = (_DFFF<<14)|(_DFFE<<8);
            break;
        case 0xdfff:	// WRITE ONLY
            _DFFF = wert & 0x7F;
            MemFrame = (_DFFF<<14)|(_DFFE<<8);
            break;
    }
}

unsigned char GEORAMClass::ReadIO1(unsigned short adresse)
{
    return RAM[MemFrame + (adresse-0xde00)];
}

unsigned char GEORAMClass::ReadIO2(unsigned short adresse)
{
    return 0;
}
