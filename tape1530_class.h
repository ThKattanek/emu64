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
// Letzte Änderung am 06.09.2016                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef TAPE1530_H
#define TAPE1530_H

#include <cstring>
#include <fstream>

#include "mos6510_class.h"

enum TAPE_IMAGE_TYP
{
    TAP,
    WAV
};

/*
enum TAPE_KEY_STATE
{
    TAPE_IS_STOP,
    TAPE_IS_PLAY,
    TAPE_IS_REW,
    TAPE_IS_FFW,
    TAPE_IS_REC,
    TAPE_IS_PAUSE
};
*/

class TAPE1530
{
public:
    TAPE1530(int samplerate, int puffersize);
    bool LoadTapeImage(char* filename);
    void OneCycle(void);

private:

    MOS6510_PORT    *CpuPort;

    FILE            *file;

    bool            IsTapeInsert;
    bool            IsRecTapeInsert;
    bool            WritePotected;
    bool            CassRead;

    unsigned int    TapeType;
    unsigned char   TapeVersion;

    unsigned int    TapeSize;
    unsigned int    TapePos;

    bool            TapePosIsStart;
    bool            TapePosIsEnd;

    unsigned char   PressedKeys;
    int             TapeStatus;

    unsigned int	ZyklenCounter;
    unsigned int	WaitCounter;

    float           Counter;

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
    unsigned short	*SoundBuffer;
    unsigned int	SoundBufferPos;
    unsigned int	SoundBufferSize;
    bool			SoundOutputEnable;
    float           Volume;

};

#endif // TAPE1530_H
