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
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef REU_CLASS_H
#define REU_CLASS_H

#include <cstdint>
#include <cstdio>
#include <functional>

#define MAX_BANK_COUNT 256

class REUClass
{
public:
    REUClass();
    ~REUClass();

    // Funktionen
    void WriteIO1(uint16_t adresse, uint8_t wert);
    void WriteIO2(uint16_t adresse, uint8_t wert);
    uint8_t ReadIO1(uint16_t adresse);
    uint8_t ReadIO2(uint16_t adresse);

    void Insert(void);
    void Remove(void);
    void SetRamBankCount(uint16_t count) { RamBankCount = count; }
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

    uint16_t        RamBankCount;    // Anzahl der 64KB Bänke (1-256)
    uint8_t     	RamBaenke[MAX_BANK_COUNT][0x10000]; // 256 x 64KB = 16MB

    bool			REUInsert;
    bool			REUWait_FF00;

    // IO
    uint8_t     	IO[0x0B];
    uint8_t     	CPUWaitCounter;
    bool			TransferStart;
    uint16_t    	AdresseC64;
    uint16_t    	AdresseREU;
    uint16_t    	Counter;
    uint8_t         Bank;
    uint8_t     	TransferTyp;
};

#endif // REU_CLASS_H
