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
// Letzte Änderung am 31.12.2013                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "reu_class.h"

REUClass::REUClass()
{
    REUInsert = false;

    ClearRAM();

    DMAStatus = 0;

    Reset();
}

REUClass::~REUClass()
{
}

void REUClass::Insert(void)
{
    REUInsert = true;
    REUWait_FF00 = false;
}

void REUClass::Remove(void)
{
    REUInsert = false;
}

int REUClass::LoadRAM(char *filename)
{
    /// REU Inhalt laden ...
    FILE* file;
    file = fopen (filename,"rb");
    if (file == NULL)
    {
        return 1;
    }
    for(int i=0;i<256;i++) fread(RamBaenke[i],1,0x10000,file);
    fclose(file);
    return 0;
}

int REUClass::SaveRAM(char *filename)
{
    /// REU Inhalt speichern ...
    FILE* file;
    file = fopen (filename,"wb");
    if (file == NULL)
    {
        return 1;
    }
    for(int i=0;i<256;i++) fwrite(RamBaenke[i],1,0x10000,file);
    fclose(file);
    return 0;
}

void REUClass::ClearRAM(void)
{
    for(int i=0;i<256;i++)
        for(int j=0;j<0x10000;j++)
            RamBaenke[i][j] = 0;
}

void REUClass::OneZyklus(void)
{
    static unsigned char tmp1;

    if(!REUInsert) return;
    if(!*RESET) Reset();
    if(!BA_STATUS) *BA = false;

    if(*WRITE_FF00)
    {
        *WRITE_FF00 = false;

        if(REUWait_FF00)
        {
            BA_STATUS = false;		// CPU anhalten
            CPUWaitCounter = 3;		// 3 Zyklen Warten bis CPU anhält
            TransferStart = true;
            switch(TransferTyp)
            {
            case 0:
                DMAStatus = 2;
                break;
            case 1:
                DMAStatus = 1;
                break;
            case 2:
                DMAStatus = 3;
                break;
            case 3:
                DMAStatus = 4;
                break;
            }
        }
    }

    if(TransferStart)
    {
        if(CPUWaitCounter > 0)
        {
            CPUWaitCounter--;
        }
        else
        {
            switch(TransferTyp)
            {
            case 0:		// STASH  C64 ---> REU
                RamBaenke[Bank][AdresseREU] = Read(AdresseC64);
                if((IO[10] & 128) != 128) AdresseC64++;
                if((IO[10] & 64) != 64)
                {
                    AdresseREU++;
                    if(AdresseREU == 0) Bank++;
                }
                Counter--;
                if(Counter == 0)
                {
                    TransferStart = false;
                    IO[0] |= 64;
                    BA_STATUS = true;
                    if((IO[1] & 32) != 32)
                    {
                        IO[2] = (unsigned char) AdresseC64;
                        IO[3] = (unsigned char) (AdresseC64>>8);
                        IO[4] = (unsigned char) AdresseREU;
                        IO[5] = (unsigned char) (AdresseREU>>8);
                        IO[6] = Bank;
                        IO[7] = (unsigned char) Counter;
                        IO[8] = (unsigned char) (Counter>>8);
                    }
                    if((IO[9] & 128) == 128)	// Interrup ist zugelasssen ?
                    {
                        // Ja
                        if((IO[9] & 64) == 64) IO[0] |= 128;
                    }
                }
                break;
            case 1:		// FETCH  REU ---> C64
                Write(AdresseC64,RamBaenke[Bank][AdresseREU]);
                if((IO[10] & 128) != 128) AdresseC64++;
                if((IO[10] & 64) != 64)
                {
                    AdresseREU++;
                    if(AdresseREU == 0) Bank++;
                }
                Counter--;
                if(Counter == 0)
                {
                    TransferStart = false;
                    IO[0] |= 64;
                    BA_STATUS = true;
                    if((IO[1] & 32) != 32)
                    {
                        IO[2] = (unsigned char) AdresseC64;
                        IO[3] = (unsigned char) (AdresseC64>>8);
                        IO[4] = (unsigned char) AdresseREU;
                        IO[5] = (unsigned char) (AdresseREU>>8);
                        IO[6] = Bank;
                        IO[7] = (unsigned char) Counter;
                        IO[8] = (unsigned char) (Counter>>8);
                    }
                    if((IO[9] & 128) == 128)	// Interrup ist zugelasssen ?
                    {
                        // Ja
                        if((IO[9] & 64) == 64) IO[0] |= 128;
                    }
                }
                break;
            case 2:		// SWAP   C64 <--> REU
                tmp1 = Read(AdresseC64);
                Write(AdresseC64,RamBaenke[Bank][AdresseREU]);
                RamBaenke[Bank][AdresseREU] = tmp1;
                if((IO[10] & 128) != 128) AdresseC64++;
                if((IO[10] & 64) != 64)
                {
                    AdresseREU++;
                    if(AdresseREU == 0) Bank++;
                }
                Counter--;
                if(Counter == 0)
                {
                    TransferStart = false;
                    IO[0] |= 64;
                    BA_STATUS = true;
                    if((IO[1] & 32) != 32)
                    {
                        IO[2] = (unsigned char) AdresseC64;
                        IO[3] = (unsigned char) (AdresseC64>>8);
                        IO[4] = (unsigned char) AdresseREU;
                        IO[5] = (unsigned char) (AdresseREU>>8);
                        IO[6] = Bank;
                        IO[7] = (unsigned char) Counter;
                        IO[8] = (unsigned char) (Counter>>8);
                    }
                    if((IO[9] & 128) == 128)	// Interrup ist zugelasssen ?
                    {
                        // Ja
                        if((IO[9] & 64) == 64) IO[0] |= 128;
                    }
                }
                break;
            case 3:		// VERIFY C64 ---- REU
                if(RamBaenke[Bank][AdresseREU] != Read(AdresseC64))
                {
                    TransferStart = false;
                    IO[0] |= 32;
                    IO[0] |= 64;
                    BA_STATUS = true;
                    if((IO[9] & 128) == 128)	// Interrup ist zugelasssen ?
                    {
                        // Ja
                        if((IO[9] & 32) == 32) IO[0] |= 128;
                    }
                }
                if((IO[10] & 128) != 128) AdresseC64++;
                if((IO[10] & 64) != 64)
                {
                    AdresseREU++;
                    if(AdresseREU == 0) Bank++;
                }
                Counter--;
                if(Counter == 0)
                {
                    TransferStart = false;
                    IO[0] |= 64;
                    BA_STATUS = true;
                    if((IO[1] & 32) != 32)
                    {
                        IO[2] = (unsigned char) AdresseC64;
                        IO[3] = (unsigned char) (AdresseC64>>8);
                        IO[4] = (unsigned char) AdresseREU;
                        IO[5] = (unsigned char) (AdresseREU>>8);
                        IO[6] = Bank;
                        IO[7] = (unsigned char) Counter;
                        IO[8] = (unsigned char) (Counter>>8);
                    }
                    if((IO[9] & 128) == 128)	// Interrup ist zugelasssen ?
                    {
                        // Ja
                        if((IO[9] & 64) == 64) IO[0] |= 128;
                    }
                }
                break;
            }
        }
    }
    if((IO[0] & 128) == 128) CpuTriggerInterrupt(REU_IRQ);
    else CpuClearInterrupt(REU_IRQ);
}

inline unsigned char REUClass::Read(unsigned short adresse)
{
    unsigned char wert = ReadProcTbl[(adresse)>>8](adresse);
    return wert;
}

inline void REUClass::Write(unsigned short adresse, unsigned char wert)
{
    WriteProcTbl[(adresse)>>8](adresse,wert);
}

void REUClass::WriteIO1(unsigned short /*adresse*/,unsigned char /*wert*/)
{
}

void REUClass::WriteIO2(unsigned short adresse,unsigned char wert)
{
    if(!REUInsert) return;
    switch(adresse)
    {
    case 0xdf00:	// READ ONLY
        break;
    case 0xdf01:
        IO[1] = wert;
        Bank = IO[6];
        AdresseC64 = IO[2] | (IO[3]<<8);
        AdresseREU = IO[4] | (IO[5]<<8);
        Counter = IO[7] | (IO[8]<<8);
        TransferTyp = wert & 3;
        if((wert & 128) == 128)	// Befehl ausführen
        {
            if((IO[1] & 0x10) == 0x10)
            {
                BA_STATUS = false;		// CPU anhalten
                CPUWaitCounter = 3;		// 3 Zyklen Warten bis CPU anhält
                TransferStart = true;
                switch(TransferTyp)
                {
                case 0:
                    DMAStatus = 2;
                    break;
                case 1:
                    DMAStatus = 1;
                    break;
                case 2:
                    DMAStatus = 3;
                    break;
                case 3:
                    DMAStatus = 4;
                    break;
                }
            }
            else
            {
                REUWait_FF00 = true;
            }
        }
        break;
    case 0xdf02:
        IO[2] = wert;
        break;
    case 0xdf03:
        IO[3] = wert;
        break;
    case 0xdf04:
        IO[4] = wert;
        break;
    case 0xdf05:
        IO[5] = wert;
        break;
    case 0xdf06:
        IO[6] = wert;
        break;
    case 0xdf07:
        IO[7] = wert;
        break;
    case 0xdf08:
        IO[8] = wert;
        break;
    case 0xdf09:
        IO[9] = wert;
        break;
    case 0xdf0a:
        IO[10] = wert;
        break;
    default:
        break;
    }
}

unsigned char REUClass::ReadIO1(unsigned short /*adresse*/)
{
    return 0;
}

unsigned char REUClass::ReadIO2(unsigned short adresse)
{
    static unsigned char ret;

    if(!REUInsert) return 0x00;	// Eigl. Zufallszahlen
    switch(adresse)
    {
    case 0xdf00:
        ret = IO[0];
        IO[0] &= 31; // BIT 5/6/7 löschen
        return ret;
        break;
    case 0xdf01:
        return IO[1];
        break;
    case 0xdf02:
        return IO[2];
        break;
    case 0xdf03:
        return IO[3];
        break;
    case 0xdf04:
        return IO[4];
        break;
    case 0xdf05:
        return IO[5];
        break;
    case 0xdf06:
        return IO[6];
        break;
    case 0xdf07:
        return IO[7];
        break;
    case 0xdf08:
        return IO[8];
        break;
    case 0xdf09:
        return IO[9];
        break;
    case 0xdf0a:
        return IO[10];
        break;
    default:
        return 0;	// Eigtl. Zufallszahlen
        break;
    }
}

inline void REUClass::Reset(void)
{
    BA_STATUS = true;
    TransferStart = false;
    IO[0] = 16;
    IO[1] = 16;
    IO[2] = 0;
    IO[3] = 0;
    IO[4] = 0;
    IO[5] = 0;
    IO[6] = 0;
    IO[7] = 255;
    IO[8] = 255;
    IO[9] = 31;
    IO[10] = 63;
}

bool REUClass::SaveFreez(FILE *File)
{
    /// REU Inhalt speichern ...
    for(int i=0;i<32;i++) fwrite(RamBaenke[i],1,0x10000,File);
    return true;
}

bool REUClass::LoadFreez(FILE *File,unsigned short Version)
{
    switch(Version)
    {
    case 0x0101:
        /// REU Inhalt laden ...
        for(int i=0;i<32;i++) fread(RamBaenke[i],1,0x10000,File);
        break;
    }
    return true;
}
