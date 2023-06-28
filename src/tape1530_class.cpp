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
// Letzte Änderung am 22.06.2023                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "./tape1530_class.h"
#include "./c64_file_types.h"

TAPE1530::TAPE1530(int samplerate, int puffersize, float cycles_per_second)
{
    CPU_PORT = NULL;

    this->cycles_per_second = cycles_per_second;

    Samplerate = (float)samplerate;
    FreqConvAddWert = 1.0/(cycles_per_second/Samplerate);
    FreqConvCounter = 0.0;

    PlayFrequenzFaktor = 4294967296.0 / cycles_per_second;     // 2^32 / Samplerate;

    PlayCounter = 0;
    PlayAddWert = 0;

    // WaveTableSinus füllen
    for(int i=0; i<SAMPLE_TBL_LENGTH; i++)
    {
        WaveTableSinus[i] = sin(((2*M_PI)/SAMPLE_TBL_LENGTH)*i+1.0*M_PI) * 0x1FFF;
    }

    // Konvertierungstabelle berechnen
    CalcTime2CounterTbl();

    TapeLenTime = 0;
    TapeLenCount = 0;

    ZyklenCounter=0;

    TapeBuffer = NULL;
    TapeBufferSize = 0;
    TapeBufferPos = 0;

    SoundBufferPos = 0;
    SoundBufferSize = puffersize;
    SoundBuffer = new signed short[SoundBufferSize*2];
    for(int i=0; i<(puffersize*2); i++)
        SoundBuffer[i] = 0;

    image_file = nullptr;

    WaitCounter = 0;

    PressedKeys = 0;
    TapeStatus = TAPE_IS_STOP;
    IsTapeInsert = false;
    IsRecTapeInsert = false;
    WritePotected = true;

    Volume = 0.0f;
    Counter = 0;
}

TAPE1530::~TAPE1530()
{
    StopRecordImage();
    if(SoundBuffer != NULL) delete[] SoundBuffer;
}

void TAPE1530::SetC64Zyklen(float cycles_per_second)
{
    this->cycles_per_second = cycles_per_second;
    FreqConvAddWert = 1.0/(cycles_per_second/Samplerate);
    PlayFrequenzFaktor = 4294967296.0 / cycles_per_second;     // 2^32 / Samplerate;
}

bool TAPE1530::LoadTapeImage(FILE *file, int typ)
{
	if(file == nullptr)
		return false;

	if(image_file != nullptr)
    {
		fclose(image_file);
        IsTapeInsert = false;
        WritePotected = true;
    }

	image_file = file;

    WAVEFormatTag = 0;
    WAVEChannels = 0;
    WAVESampleRate = 0;
    WAVEBytePerSek = 0;
    WAVEBlockAlign = 0;
    WAVEBitPerSample = 0;
    WAVEDataSize = 0;

    TapeLenTime = 0;
    TapeLenCount = 0;

	char Kennung[13];
	unsigned long reading_bytes;

	switch(typ)
	{
	case TAP:
		fread(Kennung,1,12,file);

		Kennung[12]=0;
		if(0!=strcmp("C64-TAPE-RAW",Kennung))
		{
			fclose(image_file);
			image_file = nullptr;
			return false;
		}

		fread(&TapeVersion,1,1,file);
		fseek(file,0x10,SEEK_SET);
		fread(&TapeBufferSize,1,4,file);

		// Speicher für TapeBuffer reservieren vorher evtl. alten wieder freigeben
		if(TapeBuffer != NULL)
			delete[] TapeBuffer;

		TapeBuffer = new unsigned char[TapeBufferSize];
		if(TapeBuffer == NULL) return false;

		// Tape Daten in Buffer laden
		reading_bytes = fread (TapeBuffer,1,TapeBufferSize,file);
		if(reading_bytes != TapeBufferSize)
		{
			std::cout << "Tapeimage ist defekt !" << std::endl << "Anzahl der Daten stimmt nicht mit der Anzahl im Tape Header überein." << std::endl;
			return false;
		}

		WaitCounter = 0;

		TapeType = TAP;
		IsTapeInsert = true;
		TapeBufferPos = 0;
		TapePosCycles = 0;

		TapePosIsStart = true;
		TapePosIsEnd = false;

		CalcTapeLenTime();

		fclose(image_file);
		image_file = nullptr;

		return true;
		break;

	case WAV:
		fread(Kennung,1,4,file);
		Kennung[4]=0;
		if(0!=strcmp("RIFF",Kennung))
		{
			fclose(image_file);
			image_file = nullptr;
			return false;
		}

		fseek(file,4,SEEK_CUR);

		fread(Kennung,1,4,file);
		Kennung[4]=0;
		if(0!=strcmp("WAVE",Kennung))
		{
			fclose(image_file);
			image_file = nullptr;
			return false;
		}

		fread(Kennung,1,4,file);
		Kennung[4]=0;
		if(0!=strcmp("fmt ",Kennung))
		{
			fclose(image_file);
			image_file = nullptr;
			return false;
		}

		fseek(file,4,SEEK_CUR);

		fread(&WAVEFormatTag,1,2,file);
		if(WAVEFormatTag != 1) return false;

		fread(&WAVEChannels,1,2,file);

		fread(&WAVESampleRate,1,4,file);
		fread(&WAVEBytePerSek,1,4,file);
		fread(&WAVEBlockAlign,1,2,file);
		fread(&WAVEBitPerSample,1,2,file);

		fread(Kennung,1,4,file);
		Kennung[4]=0;
		if(0!=strcmp("data",Kennung))
		{
			fclose(image_file);
			image_file = nullptr;
			return false;
		}

		fread(&WAVEDataSize,1,4,file);

		TapeType = WAV;
		IsTapeInsert = true;

		TapePosIsStart = true;
		TapePosIsEnd = false;

		WaveCounter = 0.0f;
        AddWaveWert = WAVESampleRate / cycles_per_second;

		return true;
		break;
	}
    return false;
}

bool TAPE1530::RecordTapeImage(FILE *file)
{
	if(file == nullptr)
		return false;

	StopRecordImage();

	image_file = file;

    char Kennung[] = "C64-TAPE-RAW";
    unsigned int WriteDWord = 0;

    // 0x00: Kennung
	fwrite(Kennung, 1, 12, image_file);
    // 0x0C: Version + 0x0D: Future expansion
	fwrite(&WriteDWord, 1, 4, image_file);
    // 0x10: Platzhalter Datasize
	fwrite(&WriteDWord, 1, 4, image_file);
    // 0x14: Daten

	TapePosCycles = RecCyclesCounter = RecTapeSize = 0;
	IsRecTapeInsert = true;

	return true;
}

void TAPE1530::StopRecordImage()
{
	if(image_file != nullptr && IsRecTapeInsert == true)
    {
        // Datasize eintragen
		fseek(image_file, 0x10, SEEK_SET);
		fwrite(&RecTapeSize, 1, 4, image_file);

		fclose(image_file);
		image_file = nullptr;
    }
    IsRecTapeInsert = false;
}

unsigned char TAPE1530::SetTapeKeys(unsigned char pressed_key)
{
    switch (pressed_key)
    {
    case TAPE_KEY_STOP:
        StopRecordImage();
        CPU_PORT->ConfigChanged(0, 0, 0x17);
        PressedKeys = 0;
        TapeStatus = TAPE_IS_STOP;
        break;
    case TAPE_KEY_PLAY:
        if(IsTapeInsert)
        {
            CPU_PORT->ConfigChanged(1, 0, 0x17);
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
        StopRecordImage();
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
        StopRecordImage();
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
        if(IsRecTapeInsert)
        {
            CPU_PORT->ConfigChanged(1, 0, 0x17);
            PressedKeys = TAPE_KEY_REC;
            TapeStatus = TAPE_IS_REC;
        }
        else
        {
            CPU_PORT->ConfigChanged(0, 0, 0x17);
            PressedKeys = 0;
        }
        //PressedKeys = 0;
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

void *TAPE1530::GetSoundBuffer()
{
    return SoundBuffer;
}

void TAPE1530::ZeroSoundBufferPos()
{
    SoundBufferPos = 0;
}

void TAPE1530::OneCycle()
{
    if(CPU_PORT == nullptr) return;
    if(!IsTapeInsert && !IsRecTapeInsert) return;

    static signed short WaveOut;
    static bool MotorStatusTmp = false;
    static unsigned char ReadByte;
    static unsigned short ReadWord;
    static unsigned char WriteByte;
    static unsigned short WriteWord;

    static unsigned char CPU_PORT_OLD;

    enum
    {
        Low,
        High,
        Middle
    };

    size_t redingbytes __attribute__ ((unused));
    int CycleCounter;
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
							redingbytes = fread(&ReadByte, 1, 1, image_file);
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
							 redingbytes = fread(&ReadWord, 1, 2, image_file);
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

                    if(TapeBuffer[TapeBufferPos] == 0)
                    {
                        // Wenn 0 dann die nächsten 3Bytes in unsigned int (32Bit) wandeln
                        // Mit Hilfe eines Zeigers auf cycles
                        unsigned int cycles = 0;
                        unsigned char *tmp = (unsigned char*)&cycles;
                        tmp[0] = TapeBuffer[TapeBufferPos+1];
                        tmp[1] = TapeBuffer[TapeBufferPos+2];
                        tmp[2] = TapeBuffer[TapeBufferPos+3];
                        WaitCounter = cycles;
                        TapeBufferPos += 4;
                    }
                    else WaitCounter = (TapeBuffer[TapeBufferPos++]<<3);

                    float Frequenz = 1.0 / (WaitCounter / cycles_per_second);
                    PlayCounter = 0;
                    PlayAddWert = (unsigned long)(Frequenz * PlayFrequenzFaktor);

                    //WaveOut = 1;
                    //WaitCounterHalf = WaitCounter >> 1;
                }
                else
                {
                    WaitCounter--;

                    WaveOut = (WaveTableSinus[(PlayCounter >> 16)&0xffff]);
                    PlayCounter += PlayAddWert;

                    //if(WaitCounter == WaitCounterHalf) WaveOut = 0x1FFF;
                }

                TapePosIsStart = false;

                if(TapeBufferPos >= TapeBufferSize)
                {
                    TapeBufferPos = TapeBufferSize-1;
                    TapePosIsEnd = true;
                }
                TapePosCycles++;
            }
            else MotorStatusTmp = false;
            break;

        case TAPE_IS_REC:
            if(CPU_PORT->DATASETTE_MOTOR && !TapePosIsEnd)
            {
                if(!MotorStatusTmp) WaitCounter = 0;
                MotorStatusTmp = true;

                if(((CPU_PORT->DATA & 8) == 8) && ((CPU_PORT_OLD & 8) == 0))	// PIN Wechsel
                {
                    if(RecCyclesCounter > 2040)
                    {
                        WriteByte = 0;
                        WriteWord = (unsigned short)(RecCyclesCounter << 8);
						fwrite(&WriteByte, 1, 1, image_file);
						fwrite(&WriteWord, 1, 3, image_file);
                        RecTapeSize += 4;
                    }
                    else
                    {
                        WriteByte = (unsigned char)(RecCyclesCounter >> 3);
                        WriteByte++;
						fwrite(&WriteByte, 1, 1, image_file);
                        RecTapeSize ++;
                    }
                    RecCyclesCounter = 0;
                }
                CPU_PORT_OLD = (CPU_PORT->DATA & 8);
                RecCyclesCounter++;
                TapePosCycles++;
            }
            else MotorStatusTmp = false;
            break;

        case TAPE_IS_FFW:
            CycleCounter = FFW_SPEED;
            while(CycleCounter != 0)
            {
                if(CPU_PORT->DATASETTE_MOTOR && !TapePosIsEnd)
                {
                    if(!MotorStatusTmp) WaitCounter = 0;
                    MotorStatusTmp = true;

                    if(WaitCounter == 0)
                    {
                        if(TapeBuffer[TapeBufferPos] == 0)
                        {
                            // Wenn 0 dann die nächsten 3Bytes in unsigned int (32Bit) wandeln
                            // Mit Hilfe eines Zeigers auf cycles
                            unsigned int cycles = 0;
                            unsigned char *tmp = (unsigned char*)&cycles;
                            tmp[0] = TapeBuffer[TapeBufferPos+1];
                            tmp[1] = TapeBuffer[TapeBufferPos+2];
                            tmp[2] = TapeBuffer[TapeBufferPos+3];
                            WaitCounter = cycles;
                            TapeBufferPos += 4;
                        }
                        else WaitCounter = (TapeBuffer[TapeBufferPos++]<<3);
                    }
                    else WaitCounter--;

                    TapePosIsStart = false;

                    if(TapeBufferPos >= TapeBufferSize)
                    {
                        TapeBufferPos = TapeBufferSize-1;
                        TapePosIsEnd = true;
                    }
                    TapePosCycles++;
                }
                else MotorStatusTmp = false;
                CycleCounter--;
            }
            break;

        case TAPE_IS_REW:
            CycleCounter = REW_SPEED;
            while(CycleCounter != 0)
            {
                if(CPU_PORT->DATASETTE_MOTOR && !TapePosIsStart)
                {
                    if(!MotorStatusTmp) WaitCounter = 0;
                    MotorStatusTmp = true;

                    if(WaitCounter == 0)
                    {
                        if(TapeBuffer[TapeBufferPos] == 0)
                        {
                            // Wenn 0 dann die nächsten 3Bytes in unsigned int (32Bit) wandeln
                            // Mit Hilfe eines Zeigers auf cycles
                            unsigned int cycles = 0;
                            unsigned char *tmp = (unsigned char*)&cycles;
                            tmp[0] = TapeBuffer[TapeBufferPos+1];
                            tmp[1] = TapeBuffer[TapeBufferPos+2];
                            tmp[2] = TapeBuffer[TapeBufferPos+3];
                            WaitCounter = cycles;
                            TapeBufferPos--;
                        }
                        else WaitCounter = (TapeBuffer[TapeBufferPos--]<<3);

                        if(TapeBufferPos > 3)
                        {
                            if((TapeBuffer[TapeBufferPos-3] == 0) && (TapeBuffer[TapeBufferPos-4] != 0))
                                TapeBufferPos -= 3;
                        }
                        else if(TapeBufferPos == 3)
                        {
                            if(TapeBuffer[0] == 0)
                            {
                                TapeBufferPos = 0;
                            }
                        }

                    }
                    else WaitCounter--;

                    TapePosIsEnd = false;

                    if(TapeBufferPos <= 0)
                    {
                        TapeBufferPos = 0;
                        TapePosIsStart = true;
                        TapePosCycles = 0;
                    }
                    else TapePosCycles--;
                }
                else MotorStatusTmp = false;

                CycleCounter--;
            }
            break;
        }
        LastTapeStatus = TapeStatus;
    }

    ZyklenCounter++;
    FreqConvCounter+=FreqConvAddWert;
    if(FreqConvCounter>=1.0f)
    {
        FreqConvCounter-=1.0f;
        ZyklenCounter=0;

        if(IsTapeInsert || IsRecTapeInsert)
        {
            SoundBuffer[SoundBufferPos] = SoundBuffer[SoundBufferPos+1] = short (WaveOut * Volume);
            SoundBufferPos += 2;
        }
        else
        {
            SoundBuffer[SoundBufferPos++]=0;
            SoundBuffer[SoundBufferPos++]=0;
        }
        if(SoundBufferPos >= SoundBufferSize*2) SoundBufferPos = 0;
    }
}

void TAPE1530::SetTapeSoundVolume(float volume)
{
    this->Volume = volume;
}

unsigned int TAPE1530::GetCounter()
{
    if((signed int)TapePosCycles < 0)
        TapePosCycles = 0;
    float time = TapePosCycles / cycles_per_second;

    int time_sek = roundf(time);
    float conv_wert;

    if(time_sek < 1800)
        conv_wert = Time2CounterTbl[time_sek];
    else
        conv_wert = Time2CounterTbl[1799];
    return time * conv_wert * 100;
}

float TAPE1530::GetTapeLenTime()
{
    return TapeLenTime;
}

unsigned int TAPE1530::GetTapeLenCount()
{
    return TapeLenCount;
}

bool TAPE1530::IsPressedRecord()
{
    if(TapeStatus == TAPE_IS_REC) return true;
    else return false;
}

void TAPE1530::CalcTime2CounterTbl()
{
    static float tbl1[9] = {0.357,0.3425,0.3175,0.2964,0.2786,0.2623,0.2479,0.2353,0.2220};
    static float tbl2[9] = {0,146,315,506,718,953,1210,1487,1800};

    int pos1 = 0;
    for(int i=0; i<8; i++)
    {
        float mul = (tbl1[i+1] - tbl1[i]) / (tbl2[i+1] - tbl2[i]);

        int j=0;
        while(pos1 < tbl2[i+1])
        {
            Time2CounterTbl[pos1] = tbl1[i] + mul*j;
            j++;
            pos1++;
        }
    }
}

void TAPE1530::CalcTapeLenTime()
{
    unsigned int sum_cycles = 0;
    unsigned int i = 0;

    while(i < TapeBufferSize)
    {
        if(TapeBuffer[i] == 0)
        {
            // Wenn 0 dann die nächsten 3Bytes in unsigned int (32Bit) wandeln
            // Mit Hilfe eines Zeigers auf cycles
            unsigned int cycles = 0;
            unsigned char *tmp = (unsigned char*)&cycles;
            tmp[0] = TapeBuffer[i+1];
            tmp[1] = TapeBuffer[i+2];
            tmp[2] = TapeBuffer[i+3];
            sum_cycles += cycles;
            i += 4;
        }
        else sum_cycles += (TapeBuffer[i++]<<3);
    }

    TapeLenTime = sum_cycles / cycles_per_second;

    unsigned int TapeLenTimeInt = roundf(TapeLenTime);

    // Zu Counter Konvertieren
    float conv_wert;
    if(TapeLenTimeInt < 1800)
        conv_wert = Time2CounterTbl[TapeLenTimeInt];
    else
        conv_wert = Time2CounterTbl[1799];

    TapeLenCount = TapeLenTime * conv_wert;
}
