//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: siddump.cpp                           //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 30.03.2011		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#include "siddump.h"

SIDDumpClass::SIDDumpClass(unsigned char* sidio)
{
    DumpIsLoaded = false;
    SidIO = sidio;
    CaptureEnable = false;
    PlayEnable = false;
}

SIDDumpClass::~SIDDumpClass(void)
{
    StopCapture();
}

bool SIDDumpClass::StartCapture(char* filename)
{
    if(PlayEnable) return false;
    StopCapture();

    CaptureEnable = true;
    CycleCounter = 0;

    CaptureFile = fopen (filename,"wb");
    if (CaptureFile == NULL)
    {
        return false;
    }

    CycleCounter = 0;

    char Kennung[]={"SID_DUMP"};
    fwrite (&Kennung,1,8,CaptureFile);

    return true;
}

void SIDDumpClass::StopCapture(void)
{
    if(!CaptureEnable) return;

    CaptureEnable = false;
    fclose(CaptureFile);
}

void SIDDumpClass::CycleTickCapture(void)
{
    static unsigned char Reg;
    static unsigned char RegWert;

    static unsigned short Cycles;
    static unsigned char CyclesB;
    static int CycleLong;

    if(!CaptureEnable) return;

    if(*WriteReg == 0xFF)
    {
        CycleCounter++;
        return;
    }

    Cycles = (unsigned short)CycleCounter;

    if(CycleCounter<256) CycleLong = 0;
    else if(CycleCounter<65536) CycleLong = 1;
    else
    {
        CycleLong = 1;
        CycleCounter = 0xFFFF;
    }

    Reg = *WriteReg|(CycleLong<<5);
    RegWert = SidIO[*WriteReg];

    fwrite (&Reg,1,1,CaptureFile);
    fwrite (&RegWert,1,1,CaptureFile);
	
    if(CycleLong == 0)
    {
        CyclesB = (unsigned char)Cycles;
        fwrite (&CyclesB,1,1,CaptureFile);
    }
    else fwrite (&Cycles,1,2,CaptureFile);

    CycleCounter = 0;
}

bool SIDDumpClass::LoadDump(char* filename)
{
    if(DumpIsLoaded)
    {
        DumpIsLoaded = false;
        delete Dump;
    }

    PlayFile = fopen(filename, "rb");
    if (PlayFile == NULL)
    {
        return false;
    }

    fseek(PlayFile,0,SEEK_END);
    DumpSize = ftell(PlayFile)-8;
    Dump = new unsigned char[DumpSize];

    fseek(PlayFile,0,SEEK_SET);

    char Kennung[9];
    Kennung[8]=0;
    int size_read = fread (Kennung,1,8,PlayFile);
    if(0!=strcmp("SID_DUMP",Kennung))
    {
        return false;
    }

    size_read = fread (Dump,1,DumpSize,PlayFile);

    fclose(PlayFile);

    DumpIsLoaded = true;

    return true;
}

void SIDDumpClass::ClearDump(void)
{
    if(PlayEnable)
    {
        PlayEnable = false;
    }

    if(DumpIsLoaded)
    {
        DumpIsLoaded = false;
        delete Dump;
    }
}

void SIDDumpClass::PlayDump(void)
{
    if(DumpIsLoaded)
    {
        DumpPos = 0;
        CycleCounter = 0;
        PlayEnable = true;
    }
}

void SIDDumpClass::StopDump(void)
{
    PlayEnable = false;
}

bool SIDDumpClass::CycleTickPlay(void)
{
    static unsigned char Reg;
    static unsigned char RegWert;

    if(PlayEnable)
    {
        if(DumpPos == 0)
        {
            RegOut = Reg = Dump[DumpPos++];
            RegWertOut = RegWert = Dump[DumpPos++];
            DumpPos++;

            Reg = Dump[DumpPos++];
            RegWert = Dump[DumpPos++];
            if((Reg>>5) == 0)
            {
                CycleCounter = Dump[DumpPos++];
            }
            else
            {
                CycleCounter = Dump[DumpPos++];
                CycleCounter |= Dump[DumpPos++]<<8;
            }

            if(DumpPos>=DumpSize)
            {
                /// Neu Anfang ///
                DumpPos = 0;
                CycleCounter = 0;
                PlayEnable = true;

                ///  STOP ///
                /*
                RegOut = 24;
                RegWertOut = 0;
                PlayEnable = false;
                */
            }
            Reg &= 0x1F;
            return true;
        }


        CycleCounter--;
        if(CycleCounter <= 0)
        {
            RegOut = Reg;
            RegWertOut = RegWert;

            Reg = Dump[DumpPos++];
            RegWert = Dump[DumpPos++];
            if((Reg>>5) == 0)
            {
                CycleCounter = Dump[DumpPos++];
            }
            else
            {
                CycleCounter = Dump[DumpPos++];
                CycleCounter |= Dump[DumpPos++]<<8;
            }

            if(DumpPos>=DumpSize)
            {
                /// Neu Anfang ///
                DumpPos = 0;
                CycleCounter = 0;
                PlayEnable = true;

                ///  STOP ///
                /*
                RegOut = 24;
                RegWertOut = 0;
                PlayEnable = false;
                */
            }
            Reg &= 0x1F;
            return true;
        }
    }
    return false;
}
