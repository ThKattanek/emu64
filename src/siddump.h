//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: siddump.h                             //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 18.12.2018                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef SIDDUMP_H
#define SIDDUMP_H

#include <cstring>
#include <stdio.h>

class SIDDumpClass
{
///// Funktionen /////

public:
    SIDDumpClass(unsigned char* sidio);
    ~SIDDumpClass(void);
    bool StartCapture(const char *filename);
    void StopCapture(void);
    int  GetCycleCounts(void);
    void CycleTickCapture(void);
    bool LoadDump(char* filename);
    void ClearDump(void);
    void PlayDump(void);
    void StopDump(void);
    bool CycleTickPlay(void);
private:

///// Variable /////
private:

    FILE* CaptureFile;
    FILE* PlayFile;

    bool  CaptureEnable;
    bool  PlayEnable;
    bool  DumpIsLoaded;
    int   DumpSize;
    int	  CycleCounter;
    int   CycleCounts;
    unsigned char* Dump;
    int	DumpPos;
    unsigned char Reg;
    unsigned char RegWert;

public:
    unsigned char RegOut;
    unsigned char RegWertOut;

    unsigned char *WriteReg;
    unsigned char *SidIO;
};

#endif //SIDDUMP_H
