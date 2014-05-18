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
// Letzte Änderung am 18.05.2014                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef GEORAM_CLASS_H
#define GEORAM_CLASS_H

#include <fstream>

class GEORAMClass
{
    public:
    GEORAMClass();
    ~GEORAMClass();

    // Funktionen
    void WriteIO1(unsigned short adresse,unsigned char wert);
    void WriteIO2(unsigned short adresse,unsigned char wert);
    unsigned char ReadIO1(unsigned short adresse);
    unsigned char ReadIO2(unsigned short adresse);

    void Remove(void);
    void Insert(void);
    int LoadRAM(char *filename);
    int SaveRAM(char *filename);
    void ClearRAM(void);

    private:

    // Variablen
    bool			GEORAMInsert;
    unsigned char	RAM[2097152]; // 128 x 64 x 256 = 2MB
    int				MemFrame;
    unsigned char	_DFFE,_DFFF;
};

#endif // GEORAM_CLASS_H
