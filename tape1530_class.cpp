//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: tape1530_class.cpp                    //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 08.09.2016                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "tape1530_class.h"

TAPE1530::TAPE1530(int samplerate, int puffersize)
{
    CPU_PORT = NULL;

    Samplerate=(double)samplerate;
    FreqConvAddWert=((double)1.0)/((double)985248.0/Samplerate);
    FreqConvCounter=0.0;

    ZyklenCounter=0;

    SoundBufferPos = 0;
    SoundBufferSize = puffersize;
    SoundBuffer = new unsigned short[SoundBufferSize];

    file = NULL;
    WaitCounter = 0;

    PressedKeys = 0;
    TapeStatus = TAPE_IS_STOP;
    IsTapeInsert = false;
    IsRecTapeInsert = false;
    WritePotected = true;

    Volume = 0.4f;
    Counter = 0;

    TapePos = 0;
    TapeSize = 0;
}

bool TAPE1530::LoadTapeImage(char *filename)
{
    char EXT[4];

    if(file != NULL)
    {
        fclose(file);
        IsTapeInsert = false;
        WritePotected = true;
    }

    int len = (int)strlen(filename);
    strcpy(EXT,filename+len-3);

    EXT[0]=toupper(EXT[0]);
    EXT[1]=toupper(EXT[1]);
    EXT[2]=toupper(EXT[2]);

    if(0==strcmp("TAP",EXT))
    {
        char Kennung[13];

        file = fopen(filename, "rb");
        if (file == NULL)
        {
            return false;
        }

        fread(Kennung,1,12,file);
        Kennung[12]=0;
        if(0!=strcmp("C64-TAPE-RAW",Kennung))
        {
            fclose(file);
            return false;
        }

        fread(&TapeVersion,1,1,file);
        fseek(file,0x10,SEEK_SET);
        fread(&TapeSize,1,4,file);

        TapeType = 0;
        IsTapeInsert = true;
        TapePos = 0;

        TapePosIsStart = true;
        TapePosIsEnd = false;

        return true;
    }

    if(0==strcmp("WAV",EXT))
    {
        char Kennung[5];

        file = fopen(filename, "rb");
        if (file == NULL)
        {
            return false;
        }

        fread(Kennung,1,sizeof(Kennung),file);
        Kennung[4]=0;
        if(0!=strcmp("RIFF",Kennung))
        {
            fclose(file);
            return false;
        }

        fseek(file,4,SEEK_CUR);

        fread(Kennung,1,sizeof(Kennung),file);
        Kennung[4]=0;
        if(0!=strcmp("WAVE",Kennung))
        {
            fclose(file);
            return false;
        }

        fread(Kennung,1,sizeof(Kennung),file);
        Kennung[4]=0;
        if(0!=strcmp("fmt ",Kennung))
        {
            fclose(file);
            return false;
        }

        fseek(file,4,SEEK_CUR);

        fread(&WAVEFormatTag,1,sizeof(WAVEFormatTag),file);
        if(WAVEFormatTag != 1) return false;

        fread(&WAVEChannels,1,sizeof(WAVEChannels),file);

        fread(&WAVESampleRate,1,sizeof(WAVESampleRate),file);
        fread(&WAVEBytePerSek,1,sizeof(WAVEBytePerSek),file);
        fread(&WAVEBlockAlign,1,sizeof(WAVEBlockAlign),file);
        fread(&WAVEBitPerSample,1,sizeof(WAVEBitPerSample),file);

        fread(Kennung,1,sizeof(Kennung),file);
        Kennung[4]=0;
        if(0!=strcmp("data",Kennung))
        {
            fclose(file);
            return false;
        }

        fread(&WAVEDataSize,1,sizeof(WAVEDataSize),file);

        TapeType = 1;
        IsTapeInsert = true;
        TapePos = 0;

        TapePosIsStart = true;
        TapePosIsEnd = false;

        WaveCounter = 0.0f;
        AddWaveWert = (double)WAVESampleRate / 985248.0f;

        return true;
    }

    return false;
}

unsigned char TAPE1530::SetTapeKeys(unsigned char pressed_key)
{
    switch (pressed_key)
    {
    case TAPE_KEY_STOP:
        CPU_PORT->ConfigChanged(0, 0, 0x17);
        PressedKeys = 0;
        TapeStatus = TAPE_IS_STOP;
        break;
    case TAPE_KEY_PLAY:
        if(IsTapeInsert)
        {
            CPU_PORT->ConfigChanged(1, 0, 0x17);
            WaitCounter = 0;
            PressedKeys = TAPE_KEY_PLAY;
            TapeStatus = TAPE_IS_PLAY;
        }
        else
        {
            CPU_PORT->ConfigChanged(0, 0, 0x17);
            PressedKeys = 0;
        }
        break;
    case TAPE_KEY_REW:
        if(IsTapeInsert)
        {
            CPU_PORT->ConfigChanged(1, 0, 0x17);
            PressedKeys = TAPE_KEY_REW;
            TapeStatus = TAPE_IS_REW;
        }
        else
        {
            CPU_PORT->ConfigChanged(0, 0, 0x17);
            PressedKeys = 0;
        }
        break;
    case TAPE_KEY_FFW:
        if(IsTapeInsert)
        {
            CPU_PORT->ConfigChanged(1, 0, 0x17);
            PressedKeys = TAPE_KEY_FFW;
            TapeStatus = TAPE_IS_FFW;
        }
        else
        {
            CPU_PORT->ConfigChanged(0, 0, 0x17);
            PressedKeys = 0;
        }
        break;
    case TAPE_KEY_REC:
        PressedKeys = 0;
        break;
    case TAPE_KEY_PAUSE:
        PressedKeys = 0;
        break;
    default:
        PressedKeys = 0;
        break;
    }
    return PressedKeys;
}

void TAPE1530::OneCycle()
{
    if(file == NULL) return;
    if(CPU_PORT == NULL) return;

    static unsigned short WaveOut;
    static bool MotorStatusTmp = false;
    static unsigned char ReadByte;
    static unsigned short ReadWord;

    enum
    {
        Low,
        High,
        Middle
    };

    static int CycleCounter;
    static int WaveStatus;
    static int OldWaveStatus;


    switch (TapeType)
    {
    case WAV:
    {
        switch (TapeStatus)
        {
            case TAPE_IS_PLAY:
            if(CPU_PORT->DATASETTE_MOTOR)
            {
                WaveCounter += AddWaveWert;
                if(WaveCounter >= 1.0f)
                {
                    WaveCounter -= 1.0f;
                    for(int i=0;i<(WAVEChannels);i++)
                    {
                        switch(WAVEBitPerSample)
                        {
                            case 8:
                            fread(&ReadByte,1,sizeof(ReadByte),file);
                            if((i == WAVEDataChannel) || (WAVEChannels == 1))
                            {
                                if(ReadByte < WAVELowPeek8Bit) WaveStatus = Low;
                                    else if(ReadByte > WAVEHighPeek8Bit) WaveStatus = High;


                                if((OldWaveStatus == Low) && (WaveStatus == High)) CassRead = !CassRead;
                                OldWaveStatus = WaveStatus;

                                if(CassRead) WaveOut = 0x1FFF;
                                else WaveOut = 1;
                            }
                            break;

                            case 16:
                            fread(&ReadWord,1,sizeof(ReadWord),file);
                            if((i == WAVEDataChannel) || (WAVEChannels == 1))
                            {
                                ReadWord += 0x8000;
                                if(ReadWord < WAVELowPeek16Bit) WaveStatus = Low;
                                    else if(ReadWord > WAVEHighPeek16Bit) WaveStatus = High;


                                if((OldWaveStatus == Low) && (WaveStatus == High)) CassRead = !CassRead;
                                OldWaveStatus = WaveStatus;

                                if(CassRead) WaveOut = 0x1FFF;
                                else WaveOut = 1;
                            }
                            break;
                        }
                    }
                }
                Counter += 0.00000330823863;
            }
            break;
        }
        break;
    }
    break;

    case TAP:
        switch (TapeStatus)
        {
        case TAPE_IS_STOP:
            break;
        case TAPE_IS_PLAY:
            if(CPU_PORT->DATASETTE_MOTOR && !TapePosIsEnd)
            {
                if(!MotorStatusTmp) WaitCounter = 0;
                MotorStatusTmp = true;

                if(WaitCounter == 0)
                {
                    CassRead = !CassRead;
                    if(WaveOut == 1)
                    {
                        WaveOut = 0x1FFF;

                    }
                    else
                    {
                        WaveOut = 1;

                    }

                    fread (&ReadByte,1,1,file);
                    TapePos++;

                    if(ReadByte == 0)
                    {
                        unsigned long tmp;
                        fread (&tmp,1,3,file);
                        TapePos+=3;
                        WaitCounter = tmp>>8;
                    }
                    else WaitCounter = (ReadByte*8);
                }
                WaitCounter--;
                TapePosIsStart = false;
                Counter += 0.00000330823863;

                if(TapePos >= TapeSize) TapePosIsEnd = true;

            }
            else MotorStatusTmp = false;

            break;
        case TAPE_IS_FFW:
            CycleCounter = 15;
            while(CycleCounter != 0)
            {
                if(CPU_PORT->DATASETTE_MOTOR && !TapePosIsEnd)
                {
                    if(!MotorStatusTmp) WaitCounter = 0;
                    MotorStatusTmp = true;

                    if(WaitCounter == 0)
                    {
                        fread (&ReadByte,1,1,file);
                        TapePos++;

                        if(ReadByte == 0)
                        {
                            unsigned long tmp;
                            fread (&tmp,1,3,file);
                            TapePos+=3;
                            WaitCounter = tmp>>8;
                        }
                        else WaitCounter = (ReadByte*8);
                    }
                    WaitCounter--;
                    TapePosIsStart = false;
                    Counter += 0.00000330823863;

                    if(TapePos >= TapeSize) TapePosIsEnd = true;

                }
                else MotorStatusTmp = false;
                CycleCounter--;
            }
            break;

        case TAPE_IS_REW:
            CycleCounter = 15;
            while(CycleCounter != 0)
            {
                if(CPU_PORT->DATASETTE_MOTOR && !TapePosIsStart)
                {
                    TapePosIsEnd = false;
                    Counter -= 0.00000330823863;
                }
                CycleCounter--;
            }
            break;
        }

    }
    ZyklenCounter++;
    FreqConvCounter+=FreqConvAddWert;
    if(FreqConvCounter>=(double)1.0)
    {
        FreqConvCounter-=(double)1.0;
        ZyklenCounter=0;

        if(IsTapeInsert || IsRecTapeInsert)SoundBuffer[SoundBufferPos++] = short (WaveOut * Volume);
        else SoundBuffer[SoundBufferPos++]=0;
        if(SoundBufferPos >= SoundBufferSize) SoundBufferPos = 0;
    }
}
