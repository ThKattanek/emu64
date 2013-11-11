//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: siddump.h                             //
//						//
// Dieser Sourcecode ist Copyright gesch�tzt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte �nderung am 12.04.2011		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#ifndef SIDDUMP_H
#define SIDDUMP_H

#include <cstring>
#include <fstream>

using namespace std;

class SIDDumpClass
{
///// Funktionen /////

public:
    SIDDumpClass(unsigned char* sidio);
    ~SIDDumpClass(void);
    bool StartCapture(char* filename);
    void StopCapture(void);
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
    unsigned char* Dump;
    int	DumpPos;

public:
    unsigned char RegOut;
    unsigned char RegWertOut;

    unsigned char *WriteReg;
    unsigned char *SidIO;
};

#endif //SIDDUMP_H
