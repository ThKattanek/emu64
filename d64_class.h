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
// Letzte Änderung am 18.05.2014        		//
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef D64_CLASS_H
#define D64_CLASS_H

#include "cstring"
#include <fstream>
using namespace std;

#include "structs.h"

class D64Class
{
    public:
    D64Class();
    ~D64Class();
    bool CreateDiskImage(char* filename,char* diskname,char* diskid);
    int LoadD64(char* Filename);
    char D64Name[25];
    void UnLoadD64(void);
    bool ExportPrg(int DateiNummer,char* Dateiname);
    int	  DateiAnzahl;
    int   D64Size;
    D64_FILES D64Files[256];

    private:
    void ReadBlock(unsigned char Track, unsigned char Sektor, unsigned char * PUFFER);
    unsigned char D64Image[174848];
    unsigned char Block[256];
    unsigned char BlockTmp[256];
};

#endif // D64_CLASS_H
