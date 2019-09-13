//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: mmu_class.h                           //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 13.09.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef MMU_CLASS_H
#define MMU_CLASS_H

#include <functional>

#include "./structs.h"
#include "./mos6510_class.h"

class MMU
{
public:
    /// Funktionen ///
    MMU(void);
    ~MMU(void);
    void Reset();

    void ChangeMemMap(void);
    unsigned char* GetFarbramPointer(void);
    unsigned char* GetRAMPointer(void);
    bool LoadKernalRom(const char *filename);
    bool LoadBasicRom(const char *filename);
    bool LoadCharRom(const char *filename);
    unsigned char GetReadSource(unsigned char page);
    unsigned char GetWriteDestination(unsigned char page);
    //bool SaveFreez(FILE* File);
    //bool LoadFreez(FILE *File,unsigned short Version);
    /// Variablen ///

    MOS6510_PORT *CPU_PORT;

    // Zeiger für Read / Write Funktionen ///
    // Diese werden Teilweise Intern und Extern gesetzt ///
    // Ein Aufruf erfolg immer ohne Überprüfung auf gültigen Zeiger !!! //

    std::function<unsigned char(unsigned short)> CPUReadProcTbl[0x100];
    std::function<void(unsigned short,unsigned char)> CPUWriteProcTbl[0x100];
    std::function<unsigned char(unsigned short)> VICReadProcTbl[0x100];

    std::function<unsigned char(unsigned short)>* GetCPUReadProcTable;
    std::function<unsigned char(unsigned short)>* GetVICReadProcTable;
    std::function<void(unsigned short, unsigned char)>* GetCPUWriteProcTable();
    std::function<void(unsigned short,unsigned char)> VicIOWriteProc;
    std::function<void(unsigned short,unsigned char)> SidIOWriteProc;
    std::function<void(unsigned short,unsigned char)> Cia1IOWriteProc;
    std::function<void(unsigned short,unsigned char)> Cia2IOWriteProc;
    std::function<void(unsigned short,unsigned char)> IO1WriteProc;
    std::function<void(unsigned short,unsigned char)> IO2WriteProc;
    std::function<void(unsigned short,unsigned char)> CRTRom1WriteProc;
    std::function<void(unsigned short,unsigned char)> CRTRom2WriteProc;
    std::function<void(unsigned short,unsigned char)> CRTRom3WriteProc;
    std::function<unsigned char(unsigned short)> CRTRom1ReadProc;
    std::function<unsigned char(unsigned short)> CRTRom2ReadProc;
    std::function<unsigned char(unsigned short)> CRTRom3ReadProc;
    std::function<unsigned char(unsigned short)> VicIOReadProc;
    std::function<unsigned char(unsigned short)> SidIOReadProc;
    std::function<unsigned char(unsigned short)> Cia1IOReadProc;
    std::function<unsigned char(unsigned short)> Cia2IOReadProc;
    std::function<unsigned char(unsigned short)> IO1ReadProc;
    std::function<unsigned char(unsigned short)> IO2ReadProc;

    bool *GAME;
    bool *EXROM;
    bool *RAM_H;
    bool *RAM_L;

    unsigned char MEMORY_MAP;
    unsigned char MEMORY_MAP_OLD;

    bool	*EasyFlashDirty1;
    unsigned char *EasyFlashByte1;
    bool	*EasyFlashDirty2;
    unsigned char *EasyFlashByte2;

private:
    /// Funktionen ///
    void InitProcTables(void);
    unsigned char ReadZeroPage(unsigned short adresse);
    void WriteZeroPage(unsigned short adresse, unsigned char wert);
    unsigned char ReadRam(unsigned short adresse);
    void WriteRam(unsigned short adresse, unsigned char wert);
    unsigned char ReadFarbRam(unsigned short adresse);
    void WriteFarbRam(unsigned short adresse, unsigned char wert);
    unsigned char ReadKernalRom(unsigned short adresse);
    unsigned char ReadBasicRom(unsigned short adresse);
    unsigned char ReadCharRom(unsigned short adresse);
    unsigned char ReadCRT1(unsigned short adresse);
    void WriteCRT1(unsigned short adresse, unsigned char wert);
    unsigned char ReadCRT2(unsigned short adresse);
    void WriteCRT2(unsigned short adresse, unsigned char wert);
    unsigned char ReadCRT3(unsigned short adresse);
    void WriteCRT3(unsigned short adresse, unsigned char wert);
    unsigned char ReadVicCharRomBank0(unsigned short adresse);
    unsigned char ReadVicCharRomBank2(unsigned short adresse);
    unsigned char ReadVicRam(unsigned short adresse);
    unsigned char ReadOpenAdress(unsigned short adresse);
    void WriteOpenAdress(unsigned short adresse, unsigned char wert);

    /// Variablen ///
    unsigned char RAM[0x10000];			// 64KB
    unsigned char BASIC_ROM[0x2000];		// 8KB ab 0xA000
    unsigned char CHAR_ROM[0x1000];		// 4KB ab 0xD000
    unsigned char FARB_RAM[0x0400];		// 1KB ab 0xD800
    unsigned char KERNAL_ROM[0x2000];		// 8KB ab 0xE000

    /// AktMemoryMap Visuall ///
    unsigned char MapReadSource[0x100];
    unsigned char MapWriteDestination[0x100];
};
#endif // MMU_CLASS_H
