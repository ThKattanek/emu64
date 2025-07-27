//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: floppy1541_class.cpp                  //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "./floppy1541_class.h"
#include "./c64_file_types.h"

#include <iostream>

Floppy1541::Floppy1541(bool *reset, int samplerate, int buffersize, bool *floppy_found_breakpoint)
{
    // Constructor implementation
}

Floppy1541::~Floppy1541()
{
    // Destructor implementation
}

void Floppy1541::SetEnableFloppy(bool status)
{

}

bool Floppy1541::GetEnableFloppy()
{
    return false;
}

void Floppy1541::SetDeviceNumber(uint8_t number)
{

}

void Floppy1541::SetWriteProtect(bool status)
{

}

bool Floppy1541::LoadDosRom(const char *filename)
{
    return false;
}

bool Floppy1541::LoadDiskImage(FILE *file, int typ)
{
    return false;
}

void Floppy1541::UnLoadDiskImage()
{

}

void Floppy1541::SetResetReady(bool *ResetReady, uint16_t ResetReadyAdr)
{

}

void Floppy1541::SetC64IEC(uint8_t *port)
{

}

bool Floppy1541::CheckImageDirectoryWrite()
{
    return false;
}

uint8_t *Floppy1541::GetCurrentD64ImageBuffer()
{
    return nullptr;
}

bool Floppy1541::OneCycle()
{
    return false;
}

void Floppy1541::SetEnableFloppySound(bool status)
{

}

bool Floppy1541::GetEnableFloppySound()
{
    return false;
}

int Floppy1541::LoadFloppySounds(const char *motor_sound_filename, const char *motor_on_sound_filename, const char *motor_off_sound_filename, const char *anschlag_sound_filename, const char *stepper_dec_sound_filename, const char *Stepper_inc_sound_filename)
{
    return 0x01;
}

void Floppy1541::SetFloppySoundVolume(float_t volume)
{

}

void *Floppy1541::GetSoundBuffer()
{
    return nullptr;
}

void Floppy1541::ZeroSoundBufferPos()
{

}

void Floppy1541::GetFloppyInfo(FLOPPY_INFO *fi)
{

}

void Floppy1541::UpdateBreakGroup()
{

}

int Floppy1541::GetBreakGroupCount()
{
    return 0;
}

BREAK_GROUP *Floppy1541::GetBreakGroup(int index)
{
    return nullptr;
}

int16_t Floppy1541::AddBreakGroup()
{
    return 0;
}

void Floppy1541::DelBreakGroup(int index)
{

}

void Floppy1541::DeleteAllBreakGroups()
{

}

int Floppy1541::LoadBreakGroups(const char *filename)
{
    return -1;
}

bool Floppy1541::SaveBreakGroups(const char *filename)
{
    return false;
}

bool Floppy1541::CheckBreakpoints()
{
    return false;
}

uint8_t *Floppy1541::GetRamPointer()
{

}

uint8_t Floppy1541::ReadByte(uint16_t address)
{
    return 0;
}

void Floppy1541::WriteByte(uint16_t address, uint8_t value)
{

}

void Floppy1541::SetCpuReg(REG_STRUCT *reg)
{

}

void Floppy1541::GetCpuReg(REG_STRUCT *reg, IREG_STRUCT *ireg)
{

}

void Floppy1541::ResetCycleCounter()
{

}
