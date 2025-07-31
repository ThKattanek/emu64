//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: floppy1541_class.h                    //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef FLOPPY1541_CLASS_H
#define FLOPPY1541_CLASS_H

#include <math.h>
#include <cstring>
#include <cstdio>
#include <functional>

#include "./mos6502_class.h"
#include "./mos6522_class.h"
#include "./structs.h"

#define MAX_BREAK_GROUP_NUM 255

class Floppy1541
{
public:
    Floppy1541(bool *reset_line, int samplerate = 0, int buffersize = 0, bool *found_breakpoint = nullptr);
    ~Floppy1541();

    void SetEnableFloppy(bool state);
    bool GetEnableFloppy(void);

    void SetDeviceNumber(uint8_t number);
    void SetWriteProtect(bool status);

    bool LoadDosRom(const char* filename);

    bool LoadDiskImage(FILE *file, int typ);		// 0=D64 , 1=G64
    void UnLoadDiskImage();

    void SetResetReady(bool* ResetReady, uint16_t ResetReadyAdr);
    void SetC64IEC(uint8_t* port);
    uint8_t GetFloppyIEC();

    bool CheckImageDirectoryWrite();
    uint8_t *GetCurrentD64ImageBuffer();

    // Emulation
    bool OneCycle();

    // Floppy Sound
    void SetEnableFloppySound(bool status);
    bool GetEnableFloppySound();
    int LoadFloppySounds(const char* motor_sound_filename, const char* motor_on_sound_filename, const char* motor_off_sound_filename, const char* anschlag_sound_filename, const char* stepper_dec_sound_filename, const char* Stepper_inc_sound_filename);
    void SetFloppySoundVolume(float_t volume);
    void* GetSoundBuffer();
    void ZeroSoundBufferPos();

    // Floppy 1541 Debugging
    void GetFloppyInfo(FLOPPY_INFO *fi);
    void UpdateBreakGroup();
    int GetBreakGroupCount();
    BREAK_GROUP* GetBreakGroup(int index);
    int16_t AddBreakGroup();
    void DelBreakGroup(int index);
    void DeleteAllBreakGroups();
    int LoadBreakGroups(const char *filename);
    bool SaveBreakGroups(const char *filename);
    bool CheckBreakpoints();
    uint8_t* GetRamPointer();
    uint8_t ReadByte(uint16_t address);
    void WriteByte(uint16_t address, uint8_t value);
    void SetCpuReg(REG_STRUCT *reg);
    void GetCpuReg(REG_STRUCT *reg, IREG_STRUCT *ireg);
    void ResetCycleCounter();
    uint16_t GetCpuPCHistory(uint8_t pos);
    uint8_t GetCpuPCHistoryPosition();

private:

    bool *reset_line = nullptr;
    bool via1_irq_line = false;
    bool via2_irq_line = false;

    uint8_t floppy_iec = 0;

    bool enable_floppy = false;

    MOS6502 *cpu = nullptr;
    MOS6522 *via1 = nullptr;
    MOS6522 *via2 = nullptr;

    // Memory Handling
    std::function<uint8_t(uint16_t)>  ReadProcTbl[256];
    std::function<void(uint16_t, uint8_t)> WriteProcTbl[256];

    void WriteNoMem(uint16_t address, uint8_t value);
    uint8_t ReadNoMem(uint16_t address);
    void WriteRam(uint16_t address, uint8_t value);
    uint8_t ReadRam(uint16_t address);
    uint8_t ReadRom(uint16_t address);

    // VIA Read/Write I/O
    uint8_t ReadVia1(uint16_t address);
    void WriteVia1(uint16_t address, uint8_t value);
    uint8_t ReadVia2(uint16_t address);
    void WriteVia2(uint16_t address, uint8_t value);

    uint8_t ram[0x800];     // 2K RAM
    uint8_t rom[0x4000];    // 16K ROM

    // Floppy 1541 Debugging
    uint16_t cpu_pc_history[256];
    uint8_t cpu_pc_history_position = 0;
    int cycle_counter = 0;

    ///////////////////// BREAKPOINTS CPU //////////////////////

    // Bit 0 = PC Adresse
    // Bit 1 = AC
    // Bit 2 = XR
    // Bit 3 = YR
    // Bit 4 = Lesen von einer Adresse
    // Bit 5 = Schreiben in einer Adresse
    // Bit 6 = Lesen eines Wertes
    // Bit 7 = Schreiben eines Wertes
    // Bit 8 = Beim erreichen einer bestommten Raster Zeile
    // Bit 9 = Beim erreichen eines Zyklus in einer Rasterzeile

    uint16_t        breakpoints[0x10000];
    uint16_t        breakvalues[16];
    uint16_t        breakstate = 0;
    bool            *found_breakpoint;

    uint8_t         breakgroup_count = 0;
    BREAK_GROUP     *breakgroup[MAX_BREAK_GROUP_NUM];

    ////////////////////////////////////////////////////////////
};

#endif // FLOPPY1541_CLASS_H

