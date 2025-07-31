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

Floppy1541::Floppy1541(bool *reset_line, int samplerate, int buffersize, bool *found_breakpoint)
{
    this->reset_line = reset_line;
    this->found_breakpoint = found_breakpoint;

    // initialize cpu 6502
    cpu = new MOS6502();
    cpu->RESET = reset_line;
    cpu->ReadProcTbl = ReadProcTbl;
    cpu->WriteProcTbl = WriteProcTbl;
    cpu->History = cpu_pc_history;
    cpu->HistoryPointer = &cpu_pc_history_position;
    cpu->Breakpoints = breakpoints;
    cpu->BreakStatus = &breakstate;
    cpu->BreakWerte = breakvalues;

    // initialize Memory Management

    memset(ram, 0, sizeof(ram));       // 2K RAM
    memset(rom, 0, sizeof(rom));       // 16K ROM

    for(int i=0;i<256;i++)
    {
        ReadProcTbl[i] = std::bind(&Floppy1541::ReadNoMem, this, std::placeholders::_1);
        WriteProcTbl[i] = std::bind(&Floppy1541::WriteNoMem, this, std::placeholders::_1, std::placeholders::_2);
    }

    for(int i=0;i<8;i++)
    {
        ReadProcTbl[i] = std::bind(&Floppy1541::ReadRam, this, std::placeholders::_1);
        WriteProcTbl[i] = std::bind(&Floppy1541::WriteRam, this,std::placeholders::_1, std::placeholders::_2);
    }

    for(int i=0;i<64;i++)
    {
        ReadProcTbl[i+0xC0] = std::bind(&Floppy1541::ReadRom, this, std::placeholders::_1);
    }

    // initialize MOS6522
    via1 = new MOS6522(reset_line, &via1_irq_line);
    via2 = new MOS6522(reset_line, &via2_irq_line);

    ReadProcTbl[0x18] = std::bind(&Floppy1541::ReadVia1, this, std::placeholders::_1);
    WriteProcTbl[0x18] = std::bind(&Floppy1541::WriteVia1, this, std::placeholders::_1, std::placeholders::_2);

    ReadProcTbl[0x1C] = std::bind(&Floppy1541::ReadVia2, this,std::placeholders::_1);
    WriteProcTbl[0x1C] = std::bind(&Floppy1541::WriteVia2, this,std::placeholders::_1, std::placeholders::_2);

    memset(cpu_pc_history, 0, sizeof(cpu_pc_history));
    memset(breakpoints, 0, sizeof(breakpoints));
    memset(breakvalues, 0, sizeof(breakvalues));
}

Floppy1541::~Floppy1541()
{
    if(cpu != nullptr) delete cpu;
    if(via1 != nullptr) delete via1;
    if(via2 != nullptr) delete via2;
}

void Floppy1541::SetEnableFloppy(bool state)
{
    if((!enable_floppy) && (state))
    {
        //MotorStatusOld = false;
        //DiskMotorOn = false;

        cycle_counter = 0;

        cpu->Reset();
        via1->Reset();
        via2->Reset();

        //StepperIncWait = true;

        //AktHalbSpur = -1; //1
        //GCR_PTR = GCRSpurStart = GCRImage + ((AktHalbSpur)) * GCR_TRACK_SIZE;
        //GCRSpurEnde = GCRSpurStart + TrackSize[AktHalbSpur];
    }
    else
    {
        //MotorLoopStatus = 0;
        //StepperLoopStatus = 0;
    }

    enable_floppy = state;
    if(!state)
        floppy_iec = 0xff;
}

bool Floppy1541::GetEnableFloppy()
{
    return enable_floppy;
}

void Floppy1541::SetDeviceNumber(uint8_t number)
{

}

void Floppy1541::SetWriteProtect(bool status)
{

}

bool Floppy1541::LoadDosRom(const char *filename)
{
    FILE *file;
    file = fopen(filename, "rb");
    if (file == nullptr)
        return false;

    if(0x4000 != fread (rom, 1, 0x4000, file))
    {
        fclose(file);
        return false;
    }

    fclose(file);
    return true;
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
    cpu->ResetReady = ResetReady;
    cpu->ResetReadyAdr = ResetReadyAdr;
}

void Floppy1541::SetC64IEC(uint8_t *port)
{

}

uint8_t Floppy1541::GetFloppyIEC()
{
    return floppy_iec;
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
    /*
    if(FloppySoundEnabled)
    {
        if(DiskMotorOn != MotorStatusOld)
        {
            if(DiskMotorOn)
            {
                MotorLoopStatus = 1;
                FloppySound00Pos = 0;
            }
            else
            {
                MotorLoopStatus = 3;
                FloppySound02Pos = 0;
            }
        }
        MotorStatusOld = DiskMotorOn;

        if(StepperInc)
        {
            StepperInc = false;
            StepperLoopStatus = 1;
            FloppySound03Pos = 0;
        }

        if(StepperDec)
        {
            StepperDec = false;
            StepperLoopStatus = 2;
            FloppySound04Pos = 0;
        }

        if(StepperAnschlag)
        {
            StepperAnschlag = false;
            StepperLoopStatus = 3;
            FloppySound05Pos = 0;
        }

        FreqConvCounter+=FreqConvAddWert;
        if(FreqConvCounter>=1.0f)
        {
            FreqConvCounter-=1.0f;

            if(FloppySoundsLoaded && FloppyEnabled)
                RenderFloppySound();
            else {
                SoundBuffer[SoundBufferPos] = SoundBuffer[SoundBufferPos+1] = 0;
                SoundBufferPos += 2;
                if(SoundBufferPos >= SoundBufferSize*2) SoundBufferPos = 0;
            }
        }
    }
    */

    if(!enable_floppy) return true;

    // Disk Wechsel Simulieren
    /*
    if(DiskChangeSimState != 0)
    {
        DiskChangeSimCycleCounter--;
        if(DiskChangeSimCycleCounter == 0)
        {
            DiskChangeSimState--;
            if(DiskChangeSimState != 0)
            {
                WriteProtect = !WriteProtect;
                DiskChangeSimCycleCounter = DISK_CHANGE_STATE_CYCLES;
            }
            else WriteProtect = WriteProtectAkt;
        }
    }
    */

    cycle_counter++;

    // PHI1
    via1->OneZyklus();
    via2->OneZyklus();

    if(via2->GetIOZero() & 0x04) cpu->SET_SR_BIT6();

    if(via1_irq_line || via2_irq_line)
    {
        // If IRQ Line is active, set the IRQ line of the CPU
        cpu->SetInterrupt(true);
    }
    else
    {
        // If IRQ Line is inactive, clear the IRQ line of the CPU
        cpu->SetInterrupt(false);
    }

    cpu->Phi1();

    if(!*reset_line)
    {
        //StepperIncWait = true;
        //AktHalbSpur = -1; //1
        //GCR_PTR = GCRSpurStart = GCRImage + ((AktHalbSpur)) * GCR_TRACK_SIZE;
        //GCRSpurEnde = GCRSpurStart + TrackSize[AktHalbSpur];
    }

    // PHI2
    bool ret = cpu->OneZyklus();
    if(breakstate != 0) *found_breakpoint = true;
    return ret;
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
    for(int i=0; i<= 0xffff;i++) breakpoints[i] = 0;
    for(int i=0;i<breakgroup_count;i++)
    {
        BREAK_GROUP* bg = breakgroup[i];
        if(bg->Enable)
        {
            if(bg->bPC) breakpoints[bg->iPC] |= 1;
            if(bg->bAC) breakpoints[bg->iAC] |= 2;
            if(bg->bXR) breakpoints[bg->iXR] |= 4;
            if(bg->bYR) breakpoints[bg->iYR] |= 8;

            if(bg->bRAddress)
            {
                uint16_t address = bg->iRAddress;
                for(int j=0; j<bg->iRAddressCount; j++)
                    breakpoints[address++] |= 16;
            }

            if(bg->bWAddress)
            {
                uint16_t address = bg->iWAddress;
                for(int j=0; j<bg->iWAddressCount; j++)
                    breakpoints[address++] |= 32;
            }

            if(bg->bRWert) breakpoints[bg->iRWert] |= 64;
            if(bg->bWWert) breakpoints[bg->iWWert] |= 128;
            if(bg->bRZ) breakpoints[bg->iRZ] |= 256;
            if(bg->bRZZyklus) breakpoints[bg->iRZZyklus] |= 512;
        }
    }
}

int Floppy1541::GetBreakGroupCount()
{
    return breakgroup_count;
}

BREAK_GROUP *Floppy1541::GetBreakGroup(int index)
{
    if(index >= breakgroup_count) return nullptr;
    return breakgroup[index];
}

int16_t Floppy1541::AddBreakGroup()
{
    if(breakgroup_count == MAX_BREAK_GROUP_NUM) return -1;

    breakgroup[breakgroup_count] = new BREAK_GROUP;
    memset(breakgroup[breakgroup_count],0,sizeof(BREAK_GROUP));
    breakgroup[breakgroup_count]->iRAddressCount = 1;
    breakgroup[breakgroup_count]->iWAddressCount = 1;
    breakgroup_count ++;
    return breakgroup_count - 1;
}

void Floppy1541::DelBreakGroup(int index)
{
    if(index >= breakgroup_count) return;
    delete breakgroup[index];
    breakgroup_count--;
    for(int i = index; i < breakgroup_count; i++) breakgroup[i] = breakgroup[i+1];
    UpdateBreakGroup();
}

void Floppy1541::DeleteAllBreakGroups()
{
    for(int i=0;i<breakgroup_count;i++)
    {
        delete breakgroup[i];
    }
    breakgroup_count = 0;
    UpdateBreakGroup();
}

int Floppy1541::LoadBreakGroups(const char *filename)
{
    FILE *file;
    char id[10];
    char version;
    unsigned char group_count;
    size_t reading_elements;

    DeleteAllBreakGroups();

    file = fopen (filename, "rb");
    if (file == NULL)
    {
        /// Datei konnte nicht geöffnet werden ///
        return -1;
    }

    /// Kennung ///
    fread(id, sizeof(id), 1, file);
    if(0 != strcmp("EMU64_BPT", id))
    {
        /// Kein Emu64 Format ///
        fclose(file);
        return -2;
    }

    /// Version ///
    fread(&version, sizeof(version), 1, file);

    switch(version)
    {
    case 1:
        /// Groupanzahl ///
        reading_elements = fread(&group_count, sizeof(group_count), 1, file);
        if(reading_elements != 1)
        {
            fclose(file);
            return -5;
        }

        if(group_count == 0)
        {
            fclose(file);
            return -4;
        }

        /// Groups ///
        for(int ii=0; ii<group_count; ii++)
        {
            int i = AddBreakGroup();
            fread(breakgroup[i]->Name, sizeof(breakgroup[i]->Name), 1, file);
            fread(&breakgroup[i]->Enable, sizeof(breakgroup[i]->Enable), 1, file);
            fread(&breakgroup[i]->bPC, sizeof(breakgroup[i]->bPC), 1, file);
            fread(&breakgroup[i]->iPC, sizeof(breakgroup[i]->iPC), 1, file);
            fread(&breakgroup[i]->bAC, sizeof(breakgroup[i]->bAC), 1, file);
            fread(&breakgroup[i]->iAC, sizeof(breakgroup[i]->iAC), 1, file);
            fread(&breakgroup[i]->bXR, sizeof(breakgroup[i]->bXR), 1, file);
            fread(&breakgroup[i]->iXR, sizeof(breakgroup[i]->iXR), 1, file);
            fread(&breakgroup[i]->bYR, sizeof(breakgroup[i]->bYR), 1, file);
            fread(&breakgroup[i]->iYR, sizeof(breakgroup[i]->iYR), 1, file);
            fread(&breakgroup[i]->bRAddress, sizeof(breakgroup[i]->bRAddress), 1, file);
            fread(&breakgroup[i]->iRAddress, sizeof(breakgroup[i]->iRAddress), 1, file);
            fread(&breakgroup[i]->bWAddress, sizeof(breakgroup[i]->bWAddress), 1, file);
            fread(&breakgroup[i]->iWAddress, sizeof(breakgroup[i]->iWAddress), 1, file);
            fread(&breakgroup[i]->bRWert, sizeof(breakgroup[i]->bRWert), 1, file);
            fread(&breakgroup[i]->iRWert, sizeof(breakgroup[i]->iRWert), 1, file);
            fread(&breakgroup[i]->bWWert, sizeof(breakgroup[i]->bWWert), 1, file);
            fread(&breakgroup[i]->iWWert, sizeof(breakgroup[i]->iWWert), 1, file);
            fread(&breakgroup[i]->bRZ, sizeof(breakgroup[i]->bRZ), 1, file);
            fread(&breakgroup[i]->iRZ, sizeof(breakgroup[i]->iRZ), 1, file);
            fread(&breakgroup[i]->bRZZyklus, sizeof(breakgroup[i]->bRZZyklus), 1, file);
            fread(&breakgroup[i]->iRZZyklus, sizeof(breakgroup[i]->iRZZyklus), 1, file);

            // version 2 compatiblity
            breakgroup[i]->iRAddressCount = 1;
            breakgroup[i]->iWAddressCount = 1;
        }
        break;

    case 2:
        /// ChangeLog
        /// added iRAddressCount
        /// added iWAddressCount

        /// Groupanzahl ///
        reading_elements = fread(&group_count, sizeof(group_count), 1, file);
        if(reading_elements != 1)
        {
            fclose(file);
            return -5;
        }

        if(group_count == 0)
        {
            fclose(file);
            return -4;
        }

        /// Groups ///
        for(int ii=0;ii<group_count;ii++)
        {
            int i = AddBreakGroup();
            fread(breakgroup[i]->Name, sizeof(breakgroup[i]->Name), 1, file);
            fread(&breakgroup[i]->Enable, sizeof(breakgroup[i]->Enable), 1, file);
            fread(&breakgroup[i]->bPC, sizeof(breakgroup[i]->bPC), 1, file);
            fread(&breakgroup[i]->iPC, sizeof(breakgroup[i]->iPC), 1, file);
            fread(&breakgroup[i]->bAC, sizeof(breakgroup[i]->bAC), 1, file);
            fread(&breakgroup[i]->iAC, sizeof(breakgroup[i]->iAC), 1, file);
            fread(&breakgroup[i]->bXR, sizeof(breakgroup[i]->bXR), 1, file);
            fread(&breakgroup[i]->iXR, sizeof(breakgroup[i]->iXR), 1, file);
            fread(&breakgroup[i]->bYR, sizeof(breakgroup[i]->bYR), 1, file);
            fread(&breakgroup[i]->iYR, sizeof(breakgroup[i]->iYR), 1, file);
            fread(&breakgroup[i]->bRAddress, sizeof(breakgroup[i]->bRAddress), 1, file);
            fread(&breakgroup[i]->iRAddress, sizeof(breakgroup[i]->iRAddress), 1, file);
            fread(&breakgroup[i]->iRAddressCount, sizeof(breakgroup[i]->iRAddressCount), 1, file);
            fread(&breakgroup[i]->bWAddress, sizeof(breakgroup[i]->bWAddress), 1, file);
            fread(&breakgroup[i]->iWAddress, sizeof(breakgroup[i]->iWAddress), 1, file);
            fread(&breakgroup[i]->iWAddressCount, sizeof(breakgroup[i]->iWAddressCount), 1, file);
            fread(&breakgroup[i]->bRWert, sizeof(breakgroup[i]->bRWert), 1, file);
            fread(&breakgroup[i]->iRWert, sizeof(breakgroup[i]->iRWert), 1, file);
            fread(&breakgroup[i]->bWWert, sizeof(breakgroup[i]->bWWert), 1, file);
            fread(&breakgroup[i]->iWWert, sizeof(breakgroup[i]->iWWert), 1, file);
            fread(&breakgroup[i]->bRZ, sizeof(breakgroup[i]->bRZ), 1, file);
            fread(&breakgroup[i]->iRZ, sizeof(breakgroup[i]->iRZ), 1, file);
            fread(&breakgroup[i]->bRZZyklus, sizeof(breakgroup[i]->bRZZyklus), 1, file);
            fread(&breakgroup[i]->iRZZyklus, sizeof(breakgroup[i]->iRZZyklus), 1, file);
        }
        break;

    default:
        /// Unbekannte Version ///
        fclose(file);
        return -3;
    }

    fclose(file);
    return 0;
}

bool Floppy1541::SaveBreakGroups(const char *filename)
{
    FILE *file;
    char id[] = "EMU64_BPT";
    char version = 2;

    file = fopen (filename, "wb");
    if (file == NULL)
        return false;

    /// Kennung ///
    fwrite(id, sizeof(id), 1, file);

    /// Version ///
    fwrite(&version, sizeof(version), 1, file);

    /// Groupanzahl ///
    fwrite(&breakgroup_count, sizeof(breakgroup_count), 1, file);

    /// Groups ///
    for(int i = 0; i<breakgroup_count; i++)
    {
        fwrite(breakgroup[i]->Name, sizeof(breakgroup[i]->Name), 1, file);
        fwrite(&breakgroup[i]->Enable, sizeof(breakgroup[i]->Enable), 1, file);
        fwrite(&breakgroup[i]->bPC, sizeof(breakgroup[i]->bPC), 1, file);
        fwrite(&breakgroup[i]->iPC, sizeof(breakgroup[i]->iPC), 1, file);
        fwrite(&breakgroup[i]->bAC, sizeof(breakgroup[i]->bAC), 1, file);
        fwrite(&breakgroup[i]->iAC, sizeof(breakgroup[i]->iAC), 1, file);
        fwrite(&breakgroup[i]->bXR, sizeof(breakgroup[i]->bXR), 1, file);
        fwrite(&breakgroup[i]->iXR, sizeof(breakgroup[i]->iXR), 1, file);
        fwrite(&breakgroup[i]->bYR, sizeof(breakgroup[i]->bYR), 1, file);
        fwrite(&breakgroup[i]->iYR, sizeof(breakgroup[i]->iYR), 1, file);
        fwrite(&breakgroup[i]->bRAddress, sizeof(breakgroup[i]->bRAddress), 1, file);
        fwrite(&breakgroup[i]->iRAddress, sizeof(breakgroup[i]->iRAddress), 1, file);
        fwrite(&breakgroup[i]->iRAddressCount, sizeof(breakgroup[i]->iRAddressCount), 1, file);
        fwrite(&breakgroup[i]->bWAddress, sizeof(breakgroup[i]->bWAddress), 1, file);
        fwrite(&breakgroup[i]->iWAddress, sizeof(breakgroup[i]->iWAddress), 1, file);
        fwrite(&breakgroup[i]->iWAddressCount, sizeof(breakgroup[i]->iWAddressCount), 1, file);
        fwrite(&breakgroup[i]->bRWert, sizeof(breakgroup[i]->bRWert), 1, file);
        fwrite(&breakgroup[i]->iRWert, sizeof(breakgroup[i]->iRWert), 1, file);
        fwrite(&breakgroup[i]->bWWert, sizeof(breakgroup[i]->bWWert), 1, file);
        fwrite(&breakgroup[i]->iWWert, sizeof(breakgroup[i]->iWWert), 1, file);
        fwrite(&breakgroup[i]->bRZ, sizeof(breakgroup[i]->bRZ), 1, file);
        fwrite(&breakgroup[i]->iRZ, sizeof(breakgroup[i]->iRZ), 1, file);
        fwrite(&breakgroup[i]->bRZZyklus, sizeof(breakgroup[i]->bRZZyklus), 1, file);
        fwrite(&breakgroup[i]->iRZZyklus, sizeof(breakgroup[i]->iRZZyklus), 1, file);
    }

    fclose(file);
    return true;
}

bool Floppy1541::CheckBreakpoints()
{
    int breaks_io = 0;

    for (int i=0;i<breakgroup_count;i++)
    {
        BREAK_GROUP* bg = breakgroup[i];
        int count1 = 0;
        int count2 = 0;

        if(bg->Enable)
        {
            if(bg->bPC)
            {
                count1++;
                if((breakstate & 1) && (breakvalues[0] == bg->iPC)) count2++;
            }
            if(bg->bAC)
            {
                count1++;
                if((breakstate & 2) && (breakvalues[1] == bg->iAC)) count2++;
            }
            if(bg->bXR)
            {
                count1++;
                if((breakstate & 4) && (breakvalues[2] == bg->iXR)) count2++;
            }
            if(bg->bYR)
            {
                count1++;
                if((breakstate & 8) && (breakvalues[3] == bg->iYR)) count2++;
            }
            if(bg->bRAddress)
            {
                count1++;
                if((breakstate & 16) && (breakvalues[4] == bg->iRAddress)) count2++;
            }
            if(bg->bWAddress)
            {
                count1++;
                if((breakstate & 32) && (breakvalues[5] == bg->iWAddress)) count2++;
            }
            if(bg->bRWert)
            {
                count1++;
                if((breakstate & 64) && (breakvalues[6] == bg->iRWert)) count2++;
            }
            if(bg->bWWert)
            {
                count1++;
                if((breakstate & 128) && (breakvalues[7] == bg->iWWert)) count2++;
            }
        }
        if((count1 == count2) && (count1 > 0))
        {
            breakgroup[i]->bTrue = true;
            breaks_io++;
        }
        else breakgroup[i]->bTrue = false;
    }
    breakstate = 0;

    if(breaks_io > 0)
    {
        return true;
    }
    else return false;
}

uint8_t *Floppy1541::GetRamPointer()
{
    return ram;
}

uint8_t Floppy1541::ReadByte(uint16_t address)
{
    return ReadProcTbl[(address >> 8)](address);
}

void Floppy1541::WriteByte(uint16_t address, uint8_t value)
{
    WriteProcTbl[(address >> 8)](address, value);
}

void Floppy1541::SetCpuReg(REG_STRUCT *reg)
{
    cpu->SetRegister(reg);
}

void Floppy1541::GetCpuReg(REG_STRUCT *reg, IREG_STRUCT *ireg)
{
    if(!enable_floppy) return;

    if(ireg != nullptr)
    {
        cpu->GetInterneRegister(ireg);
        ireg->cycle_counter = cycle_counter;
    }

    if(reg != nullptr)
        cpu->GetRegister(reg);
}

void Floppy1541::ResetCycleCounter()
{
    cycle_counter = 0;
}

uint16_t Floppy1541::GetCpuPCHistory(uint8_t pos)
{
    return cpu_pc_history[pos];
}

uint8_t Floppy1541::GetCpuPCHistoryPosition()
{
    return cpu_pc_history_position;
}

void Floppy1541::WriteNoMem(uint16_t, uint8_t)
{
}

uint8_t Floppy1541::ReadNoMem(uint16_t)
{
    return 0x00; // Return 0 for unmapped memory
}

void Floppy1541::WriteRam(uint16_t address, uint8_t value)
{
    ram[address & 0x7ff] = value;
}

uint8_t Floppy1541::ReadRam(uint16_t address)
{
    return ram[address & 0x7ff];
}

uint8_t Floppy1541::ReadRom(uint16_t address)
{
    return rom[(address - 0xc000) & 0x3fff]; // Read from ROM, address must be in range 0xc000-0xffff];
}

uint8_t Floppy1541::ReadVia1(uint16_t address)
{
    return via1->ReadIO(address);
}

void Floppy1541::WriteVia1(uint16_t address, uint8_t value)
{
    via1->WriteIO(address, value);
}

uint8_t Floppy1541::ReadVia2(uint16_t address)
{
    return via2->ReadIO(address);
}

void Floppy1541::WriteVia2(uint16_t address, uint8_t value)
{
    via2->WriteIO(address, value);
}
