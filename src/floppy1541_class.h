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

//#include "./mos6502_class.h"
//#include "./mos6522_class.h"
#include "./structs.h"

class Floppy1541
{
public:
    Floppy1541(bool *reset, int samplerate = 0, int buffersize = 0, bool *floppy_found_breakpoint = nullptr);
    ~Floppy1541();

    void SetEnableFloppy(bool status);
    bool GetEnableFloppy(void);

    void SetDeviceNumber(uint8_t number);
    void SetWriteProtect(bool status);

    bool LoadDosRom(const char* filename);

    bool LoadDiskImage(FILE *file, int typ);		// 0=D64 , 1=G64
    void UnLoadDiskImage();

    void SetResetReady(bool* ResetReady, uint16_t ResetReadyAdr);
    void SetC64IEC(uint8_t* port);

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

    // Debugging
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

    uint8_t FloppyIECLocal = 0; // Local IEC Port;

    uint16_t History[256];
    uint8_t HistoryPointer = 0; // History Pointer

private:

};

#endif // FLOPPY1541_CLASS_H

