//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: reu_class.cpp                         //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 13.09.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef REU_CLASS_H
#define REU_CLASS_H

#include <fstream>
#include <functional>

#include "./structs.h"

class REUClass
{
    public:
    REUClass();
    ~REUClass();

    // Funktionen
    void WriteIO1(unsigned short adresse,unsigned char wert);
    void WriteIO2(unsigned short adresse,unsigned char wert);
    unsigned char ReadIO1(unsigned short adresse);
    unsigned char ReadIO2(unsigned short adresse);

    void Insert(void);
    void Remove(void);
    int LoadRAM(const char *filename);
    int SaveRAM(const char *filename);
    void ClearRAM(void);
    void Reset(void);
    void OneZyklus(void);

    bool SaveFreez(FILE *File);
    bool LoadFreez(FILE *File,unsigned short Version);

    std::function<unsigned char(unsigned short)> *ReadProcTbl;
    std::function<void(unsigned short,unsigned char)> *WriteProcTbl;
    std::function<void(int)> CpuTriggerInterrupt;
    std::function<void(int)> CpuClearInterrupt;

    // Variablen
    bool *BA;			// Extern
    bool *RESET;		// Extern
    bool *WRITE_FF00;	// Extern

    unsigned char DMAStatus;	// 0=kein Transfer / 1=Lesen / 2=Schreiben / 3=Swap / 4=Verify

    private:
    // Funktionen
    inline unsigned char Read(unsigned short adresse);
    inline void Write(unsigned short adresse, unsigned char wert);

    // Variablen

    bool			BA_STATUS;

    //unsigned char	RamBaenke[32][0x10000];	// 32 x 64KB = 2MB
    unsigned char	RamBaenke[256][0x10000]; // 256 x 64KB = 16MB

    bool			REUInsert;
    bool			REUWait_FF00;

    // IO
    unsigned char	IO[0x0B];
    unsigned char	CPUWaitCounter;
    bool			TransferStart;
    unsigned short	AdresseC64;
    unsigned short	AdresseREU;
    unsigned short	Counter;
    unsigned char	Bank;
    unsigned char	TransferTyp;
};

#endif // REU_CLASS_H
