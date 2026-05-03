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

// 128KiB, 256KiB, 512KiB, 1MiB, 2MiB, 4MiB, 8MiB, 16MiB

enum REU_RAM_MODES {REU_128KiB, REU_256KiB, REU_512KiB, REU_1MiB, REU_2MiB, REU_4MiB, REU_8MiB, REU_16MiB};

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
    void SetReuRamMode(uint8_t mode);   // _128, _256, _512, _1024, _2048, _4096, _8192, _16384
    uint8_t GetReuRamMode();            // return _128, _256, _512, _1024, _2048, _4096, _8192, _16384

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

    uint8_t         reu_ram_mode;       // 128kB, 256kB, 512kB, 1MB, 2MB, 4MB, 8MB, 16MB
    uint16_t        ram_bank_count;     // Anzahl der 64KB Bänke (1-256)
    uint8_t     	ram_baenke[MAX_BANK_COUNT][0x10000]; // 256 x 64KB = 16MB
    uint8_t     	ram_bank_mask;      // Maske für die Banknummer (0-255)

    bool			is_reu_insert;
    bool			reu_wait_ff00;

    // IO
    uint8_t     	io[0x0B];
    uint8_t     	cpu_wait_counter;
    bool			tarnsfer_start;
    uint16_t    	c64_address;
    uint16_t    	reu_address;
    uint16_t    	counter;
    uint8_t         current_bank;
    uint8_t     	transfer_typ;
};

#endif // REU_CLASS_H
