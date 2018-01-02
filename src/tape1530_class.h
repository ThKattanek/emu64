//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: tape1530_class.h                      //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 02.01.2018                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef TAPE1530_H
#define TAPE1530_H

#include <cstring>
#include <fstream>
#include <iostream>
#include <math.h>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#include "mos6510_class.h"

enum TAPE_IMAGE_TYP
{
    TAP,
    WAV
};

#define REW_SPEED 12 // 15-Fache Geschwindigkeit
#define FFW_SPEED 12 // 15-Fache Geschwindigkeit

#define SAMPLE_TBL_LENGTH 0x10000

class TAPE1530
{
public:
    TAPE1530(int samplerate, int puffersize);
    ~TAPE1530();
    bool LoadTapeImage(char* filename);
    bool RecordTapeImage(char* filename);
    void StopRecordImage();
    unsigned char SetTapeKeys(unsigned char pressed_key);
    void *GetSoundBuffer(void);
    void ZeroSoundBufferPos();
    void OneCycle(void);

    void SetTapeSoundVolume(float volume);
    unsigned int GetCounter();

    float GetTapeLenTime();
    unsigned int GetTapeLenCount();
    bool IsPressedRecord();

    MOS6510_PORT    *CPU_PORT;
    bool            CassRead;

private:
    void CalcTime2CounterTbl(void);
    void CalcTapeLenTime();

    FILE            *file;
    FILE            *recfile;

    unsigned char   *TapeBuffer;
    unsigned int    TapeBufferSize;
    unsigned int    TapeBufferPos;
    unsigned int    TapePosCycles;
    unsigned int    RecCyclesCounter;
    unsigned int    RecTapeSize;

    float           TapeLenTime;
    unsigned int    TapeLenCount;

    bool            IsTapeInsert;
    bool            IsRecTapeInsert;
    bool            WritePotected;

    unsigned int    TapeType;
    unsigned char   TapeVersion;

    bool            TapePosIsStart;
    bool            TapePosIsEnd;

    unsigned char   PressedKeys;
    int             TapeStatus;
    int             LastTapeStatus;

    unsigned int	ZyklenCounter;
    unsigned int	WaitCounter;
    unsigned int    WaitCounterHalf;

    float           Counter;
    float Time2CounterTbl[1800];    // Platz für 1800 sek (30min)


    float           AddWaveWert;
    float           WaveCounter;
    unsigned short  WAVEFormatTag;
    unsigned short  WAVEChannels;
    unsigned long   WAVESampleRate;
    unsigned long	WAVEBytePerSek;
    unsigned short	WAVEBlockAlign;
    unsigned short	WAVEBitPerSample;
    unsigned long	WAVEDataSize;
    unsigned short	WAVEDataChannel;
    unsigned short	WAVEHighPeek8Bit;
    unsigned short	WAVELowPeek8Bit;
    unsigned short	WAVEHighPeek16Bit;
    unsigned short	WAVELowPeek16Bit;

    // Für Soundausgabe
    float           Samplerate;
    float           FreqConvCounter;
    float           FreqConvAddWert;
    signed short	*SoundBuffer;
    unsigned int	SoundBufferPos;
    unsigned int	SoundBufferSize;
    bool			SoundOutputEnable;
    float           Volume;

    float PlayFrequenzFaktor;
    unsigned long PlayCounter;
    unsigned long PlayAddWert;

    unsigned short WaveTableSinus[SAMPLE_TBL_LENGTH];
};

#endif // TAPE1530_H
