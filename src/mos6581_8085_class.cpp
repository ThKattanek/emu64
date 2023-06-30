//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: mos6581_8085class.cpp                 //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 22.06.2023                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "mos6581_8085_class.h"
#include "mos6581_8085_calc.h"
#include "mos6581_8085_wellenformen.h"

unsigned int RateCounterPeriod[]={9,32,63,95,149,220,267,313,392,977,1954,3126,3907,11720,19532,31251};
unsigned int SustainLevel[]={0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};
fc_point f0_points_6581[31]={{0,220},{0,220},{128,230},{256,250},{384,300},{512,420},{640,780},{768,1600},{832,2300},{896,3200},{960,4300},{992,5000},{1008,5400},{1016,5700},{1023,6000},{1023,6000},{1024,4600},{1024,4600},{1032,4800},{1056,5300},{1088,6000},{1120,6600},{1152,7200},{1280,9500},{1408,12000},{1536,14500},{1664,16000},{1792,17100},{1920,17700},{2047,18000},{2047,18000}};
fc_point f0_points_8580[19]={{0,0},{0,0},{128,800},{256,1600},{384,2500},{512,3300},{640,4100},{768,4800},{896,5600},{1024,6500},{1152,7500},{1280,8400},{1408,9200},{1536,9800},{1664,10500},{1792,11000},{1920,11700},{2047,12500},{2047,12500}};

MOS6581_8085::MOS6581_8085(int nummer,int samplerate,int puffersize,int *error)
{
    Voice[0] = new VOICEClass();
    Voice[1] = new VOICEClass();
    Voice[2] = new VOICEClass();
    
    SidNummer = nummer;
    this->FilterFrequenz=0;
    Samplerate=samplerate;
    C64ZyklenSek = 985248;
    FreqConvAddWert=1.0f/(C64ZyklenSek/Samplerate);
    FreqConvCounter=0.0;

    IODelayEnable = false;
    IODelayRPos = 0;
    IODelayWPos = 1000;

    LastWriteValue = 0;

    for(int i=0; i<1048576; i++)
    {
        IODelayPuffer[i][0] = 0x20;
        IODelayPuffer[i][1] = 0;
    }

    Voice[0]->OscEnable = true;
    Voice[1]->OscEnable = true;
    Voice[2]->OscEnable = true;

    this->FilterOn=true;
    Voice[0]->SyncSource=2;
    Voice[1]->SyncSource=0;
    Voice[2]->SyncSource=1;
    Voice[0]->SyncDestination=1;
    Voice[1]->SyncDestination=2;
    Voice[2]->SyncDestination=0;

    interpolate(f0_points_6581, f0_points_6581+ sizeof(f0_points_6581)/sizeof(*f0_points_6581) - 1,PointPlotter<int>(f0_6581), 1.0);
    interpolate(f0_points_8580, f0_points_8580+ sizeof(f0_points_8580)/sizeof(*f0_points_8580) - 1,PointPlotter<int>(f0_8580), 1.0);
	
    SoundOutputEnable=false;
    SetChipType(0);

    this->CycleExact=false;

    IoDump = new SIDDumpClass(IO);
    IoDump->WriteReg = &WriteReg;

    Zyklencounter = 0;
    SoundBufferPos = 0;
    SoundBufferSize = puffersize;
    SoundBuffer = new short[SoundBufferSize];
    SoundBufferV0 = new short[SoundBufferSize];
    SoundBufferV1 = new short[SoundBufferSize];
    SoundBufferV2 = new short[SoundBufferSize];

    PotX = PotY = 0xFF;

    Recording = false;
    RecSampleCounter = 0;

    Reset();
    *error = 0;
}

MOS6581_8085::~MOS6581_8085()
{
    delete[] SoundBuffer;
    delete[] SoundBufferV0;
    delete[] SoundBufferV1;
    delete[] SoundBufferV2;

    delete IoDump;

    delete Voice[2];
    delete Voice[1];
    delete Voice[0];
}

void MOS6581_8085::ChangeSampleRate(int samplerate,int puffersize)
{
    delete[] SoundBuffer;
    delete[] SoundBufferV0;
    delete[] SoundBufferV1;
    delete[] SoundBufferV2;

    Samplerate=samplerate;
    FreqConvAddWert=1.0f/(C64ZyklenSek/Samplerate);
    FreqConvCounter=0.0;

    SoundBufferPos = 0;
    SoundBufferSize = puffersize;
	
    SoundBuffer = new short[SoundBufferSize];
    SoundBufferV0 = new short[SoundBufferSize];
    SoundBufferV1 = new short[SoundBufferSize];
    SoundBufferV2 = new short[SoundBufferSize];
}

void MOS6581_8085::Reset(void)
{
    OscReset();
    EnvReset();
    FilterReset();
}

void MOS6581_8085::SetChipType(int type)
{
    SidModel = type;

    if(SidModel==MOS_6581)
    {
        Wave0=Wave6581_0;
        Wave1=Wave6581_1;
        Wave2=Wave6581_2;
        Wave3=Wave6581_3;

        WaveZero=0x380;
        VoiceDC=0x800*0xFF;

        MixerDC=-0xfff*0xff/18>>7;
        f0=f0_6581;
        f0_points=f0_points_6581;
        f0_count=sizeof(f0_points_6581)/sizeof(*f0_points_6581);
    }
    else
    {
        Wave0=Wave8580_0;
        Wave1=Wave8580_1;
        Wave2=Wave8580_2;
        Wave3=Wave8580_3;

        WaveZero=0x800;
        VoiceDC=0;

        MixerDC=0;
        f0=f0_8580;
        f0_points=f0_points_8580;
        f0_count=sizeof(f0_points_8580)/sizeof(*f0_points_8580);
    }
    SetW0();
    SetQ();
}

void MOS6581_8085::SetC64Zyklen(float C64ZyklenSek)
{
    this->C64ZyklenSek = C64ZyklenSek;
    FreqConvAddWert=1.0f/(C64ZyklenSek/Samplerate);
}

void MOS6581_8085::SetVoiceEnable(int nr, bool status)
{
    Voice[nr]->OscEnable = status;
}

void MOS6581_8085::SetPotXY(unsigned char pot_x, unsigned char pot_y)
{
    PotX = pot_x;
    PotY = pot_y;
}

void MOS6581_8085::ZeroSoundBufferPos()
{
    SoundBufferPos = 0;
}

bool MOS6581_8085::OneZyklus(void)
{
    ret = false;
    
    if(!*RESET) Reset();

    if(LastWriteCounter > 0)
    {
        LastWriteCounter--;
        if(LastWriteCounter == 0)
            LastWriteValue = 0;
    }

    if(CycleExact)
    {
    	OscZyklus();
    	EnvZyklus();
        FilterZyklus(VoiceOutput(0),VoiceOutput(1),VoiceOutput(2),0);
    }
    
    Zyklencounter++;
    FreqConvCounter+=FreqConvAddWert;
    if(FreqConvCounter>=1.0f)
    {
        FreqConvCounter-=1.0f;
    	if(!CycleExact)
    	{
            OscZyklus(Zyklencounter);
            EnvZyklus(Zyklencounter);
            FilterZyklus(Zyklencounter,VoiceOutput(0),VoiceOutput(1),VoiceOutput(2),0);
    	}
    	Zyklencounter=0;
		
        if(SoundOutputEnable)
        {
            if(SoundBufferPos < SoundBufferSize)
            {
                SoundBufferV0[SoundBufferPos] = VoiceOutput(0);
                SoundBufferV1[SoundBufferPos] = VoiceOutput(1);
                SoundBufferV2[SoundBufferPos] = VoiceOutput(2);
                SoundBuffer[SoundBufferPos++]=FilterOutput()>>4;
            }
        }
        else
        {
            if(SoundBufferPos < SoundBufferSize)
            {
                SoundBufferV0[SoundBufferPos] = VoiceOutput(0);
                SoundBufferV1[SoundBufferPos] = VoiceOutput(1);
                SoundBufferV2[SoundBufferPos] = VoiceOutput(2);
                SoundBuffer[SoundBufferPos++]=0;
            }
        }
        ret = true;
    }

    ///////// IO DUMP //////////
    ////////////////////////////

    // Capture
    IoDump->CycleTickCapture();
    // Playing
    if(IoDump->CycleTickPlay())WriteIO(IoDump->RegOut,IoDump->RegWertOut);

    //////// RECORD ////////
    if(Recording)
    {
        RecSampleBuffer[RecSampleCounter++] = FilterOutput()>>4;
        if(RecSampleCounter == 19656) RecSampleCounter = 0;
    }
    WriteReg = 0xFF;

    //////// IO Delay ///////
    if(IODelayEnable)
    {
        if(IODelayPuffer[IODelayRPos][0] < 0x20)
            WriteIO(IODelayPuffer[IODelayRPos][0],IODelayPuffer[IODelayRPos][1]);

        IODelayRPos++;
        IODelayRPos &= 0xFFFFF;
        IODelayWPos++;
        IODelayWPos &= 0xFFFFF;

        IODelayPuffer[IODelayWPos][0] = 0x20;
    }

    return ret;
}

bool MOS6581_8085::SaveFreez(FILE* File)
{
    fwrite(IO,1,32,File);
    return true;
}

bool MOS6581_8085::LoadFreez(FILE *File,unsigned short Version)
{
    switch(Version)
    {
    case 0x0100:
    case 0x0101:
        int size_read = fread(IO,1,32,File);
        if(size_read == 32) for(int i=0;i<32;i++) WriteIO(i,IO[i]);
        return false;
        break;
    }
    return true;
}

unsigned char MOS6581_8085::ReadIO(unsigned short adresse)
{
    switch(adresse&0x1F)
    {
    case 25: // AD Wandler 1
        if(SidNummer == 0)
            return PotX;
         break;

    case 26: // AD Wandler 2
        if(SidNummer == 0)
            return PotY;
        break;

    case 27:
        return OscOutput(2)>>4;

    case 28:
        return EnvOutput(2);

    default:
        return LastWriteValue;
    }
    return 0;
}

void MOS6581_8085::SetIODelayEnable(bool enable)
{
    IODelayEnable = enable;
}

void MOS6581_8085::WriteIO(unsigned short adresse,unsigned char wert)
{

    LastWriteValue = wert;
    LastWriteCounter = LAST_WRITE_COUNTER_START;

    static bool KeyNext;

    if(!IODelayEnable)
    {
        WriteReg = adresse & 0x1F;
        IO[adresse & 0x1F]=wert;
    }
    else
    {
        if(adresse > 0xD3FF)  // kommt von CPU
        {
            IODelayPuffer[IODelayWPos][0] = adresse & 0x1F;
            IODelayPuffer[IODelayWPos][1] = wert;
            return;
        }
        else                // kommt aus Delas
        {
            wert = IODelayPuffer[IODelayRPos][1];
            WriteReg = adresse & 0x1F;
            IO[adresse & 0x1F]=wert;
        }
    }

    switch(adresse&0x1F)
    {
    case 0: // FrequenzLO für Stimme 0
        Voice[0]->Frequenz=(Voice[0]->Frequenz & 0xFF00) | (wert & 0x00FF);
        break;

    case 1: // FrequenzHI für Stimme 0
        Voice[0]->Frequenz=((wert<<8) & 0xFF00) | (Voice[0]->Frequenz & 0x00FF);
        break;

    case 2: // PulsweiteLO für Stimme 0
        Voice[0]->Pulsweite=(Voice[0]->Pulsweite & 0xF00) | (wert & 0x0FF);
        break;

    case 3: // PulsweiteHI für Stimme 0
        Voice[0]->Pulsweite=((wert<<8) & 0xF00) | (Voice[0]->Pulsweite & 0x0FF);
        break;

    case 4: // Kontrol Register für Stimme 0
        Voice[0]->Waveform=(wert>>4) & 0x0F;
        Voice[0]->RingBit = !(~wert & 0x04);
        Voice[0]->SyncBit = !(~wert & 0x02);
        if(wert & 0x08)
        {
            Voice[0]->Accu=0;
            Voice[0]->ShiftReg=0;
        }
        else if(Voice[0]->TestBit) Voice[0]->ShiftReg=0x7FFFF8;
        Voice[0]->TestBit = !(~wert & 0x08);
        
        KeyNext = wert & 0x01;
        if (!Voice[0]->KeyBit&&KeyNext)
        {
            Voice[0]->State=ATTACK;
            Voice[0]->RatePeriod=RateCounterPeriod[Voice[0]->Attack];
            Voice[0]->HoldZero=false;
        }
        else if (Voice[0]->KeyBit&&!KeyNext)
        {
            Voice[0]->State=RELEASE;
            Voice[0]->RatePeriod=RateCounterPeriod[Voice[0]->Release];
        }
        Voice[0]->KeyBit=KeyNext;
        break;

    case 5: // Attack-Decay für Stimme 0
        Voice[0]->Attack=(wert>>4)&0x0F;
        Voice[0]->Decay=wert&0x0F;
        if(Voice[0]->State==ATTACK) Voice[0]->RatePeriod=RateCounterPeriod[Voice[0]->Attack];
        else if (Voice[0]->State==DECAY_SUSTAIN) Voice[0]->RatePeriod=RateCounterPeriod[Voice[0]->Decay];
        break;

    case 6:		// Sustain-Release für Stimme 0
        Voice[0]->Sustain=(wert>>4)&0x0F;
        Voice[0]->Release=wert&0x0F;
        if (Voice[0]->State==RELEASE) Voice[0]->RatePeriod=RateCounterPeriod[Voice[0]->Release];
        break;
        
        /////////////////////////////////////////////////////////////

    case 7: // FrequenzLO für Stimme 1
        Voice[1]->Frequenz=(Voice[1]->Frequenz & 0xFF00) | (wert & 0x00FF);
        break;

    case 8: // FrequenzHI für Stimme 1
        Voice[1]->Frequenz=((wert<<8) & 0xFF00) | (Voice[1]->Frequenz & 0x00FF);
        break;

    case 9: // PulsweiteLO für Stimme 1
        Voice[1]->Pulsweite=(Voice[1]->Pulsweite & 0xF00) | (wert & 0x0FF);
        break;

    case 10: // PulsweiteHI für Stimme 1
        Voice[1]->Pulsweite=((wert<<8) & 0xF00) | (Voice[1]->Pulsweite & 0x0FF);
        break;

    case 11: // Kontrol Register für Stimme 1
        Voice[1]->Waveform=(wert>>4)&0x0F;
        Voice[1]->RingBit = !(~wert&0x04);
        Voice[1]->SyncBit = !(~wert&0x02);
        if(wert&0x08)
        {
            Voice[1]->Accu=0;
            Voice[1]->ShiftReg=0;
        }
        else if(Voice[1]->TestBit) Voice[1]->ShiftReg=0x7FFFF8;
        Voice[1]->TestBit = !(~wert&0x08);
        
        KeyNext=wert&0x01;
        if (!Voice[1]->KeyBit&&KeyNext)
        {
            Voice[1]->State=ATTACK;
            Voice[1]->RatePeriod=RateCounterPeriod[Voice[1]->Attack];
            Voice[1]->HoldZero=false;
        }
        else if (Voice[1]->KeyBit&&!KeyNext)
        {
            Voice[1]->State=RELEASE;
            Voice[1]->RatePeriod=RateCounterPeriod[Voice[1]->Release];
        }
        Voice[1]->KeyBit=KeyNext;
        break;

    case 12: // Attack-Decay für Stimme 1
        Voice[1]->Attack=(wert>>4)&0x0F;
        Voice[1]->Decay=wert&0x0F;
        if(Voice[1]->State==ATTACK) Voice[1]->RatePeriod=RateCounterPeriod[Voice[1]->Attack];
        else if (Voice[1]->State==DECAY_SUSTAIN) Voice[1]->RatePeriod=RateCounterPeriod[Voice[1]->Decay];
        break;

    case 13: // Sustain-Release für Stimme 1
        Voice[1]->Sustain=(wert>>4)&0x0F;
        Voice[1]->Release=wert&0x0F;
        if (Voice[1]->State==RELEASE) Voice[1]->RatePeriod=RateCounterPeriod[Voice[1]->Release];
        break;
        
        /////////////////////////////////////////////////////////////

    case 14: // FrequenzLO für Stimme 2
        Voice[2]->Frequenz=(Voice[2]->Frequenz & 0xFF00) | (wert & 0x00FF);
        break;

    case 15: // FrequenzHI für Stimme 2
        Voice[2]->Frequenz=((wert<<8) & 0xFF00) | (Voice[2]->Frequenz & 0x00FF);
        break;

    case 16: // PulsweiteLO für Stimme 2
        Voice[2]->Pulsweite=(Voice[2]->Pulsweite & 0xF00) | (wert & 0x0FF);
        break;

    case 17: // PulsweiteHI für Stimme 2
        Voice[2]->Pulsweite=((wert<<8) & 0xF00) | (Voice[2]->Pulsweite & 0x0FF);
        break;

    case 18: // Kontrol Register für Stimme 2
        Voice[2]->Waveform=(wert>>4)&0x0F;
        Voice[2]->RingBit = !(~wert&0x04);
        Voice[2]->SyncBit = !(~wert&0x02);
        if(wert&0x08)
        {
            Voice[2]->Accu=0;
            Voice[2]->ShiftReg=0;
        }
        else if(Voice[2]->TestBit) Voice[2]->ShiftReg=0x7FFFF8;
        Voice[2]->TestBit = !(~wert&0x08);
        
        KeyNext=wert&0x01;
        if (!Voice[2]->KeyBit&&KeyNext)
        {
            Voice[2]->State=ATTACK;
            Voice[2]->RatePeriod=RateCounterPeriod[Voice[2]->Attack];
            Voice[2]->HoldZero=false;
        }
        else if (Voice[2]->KeyBit&&!KeyNext)
        {
            Voice[2]->State=RELEASE;
            Voice[2]->RatePeriod=RateCounterPeriod[Voice[2]->Release];
        }
        Voice[2]->KeyBit=KeyNext;
        break;

    case 19: // Attack-Decay für Stimme 2
        Voice[2]->Attack=(wert>>4)&0x0F;
        Voice[2]->Decay=wert&0x0F;
        if(Voice[2]->State==ATTACK) Voice[2]->RatePeriod=RateCounterPeriod[Voice[2]->Attack];
        else if (Voice[2]->State==DECAY_SUSTAIN) Voice[2]->RatePeriod=RateCounterPeriod[Voice[2]->Decay];
        break;

    case 20: // Sustain-Release für Stimme 2
        Voice[2]->Sustain=(wert>>4)&0x0F;
        Voice[2]->Release=wert&0x0F;
        if (Voice[2]->State==RELEASE) Voice[2]->RatePeriod=RateCounterPeriod[Voice[2]->Release];
        break;

    case 21: // FilterfrequenzLO
        FilterFrequenz=(FilterFrequenz & 0x7F8) | (wert & 0x007);
        SetW0();
        break;

    case 22: // FilterfrequenzHI
        FilterFrequenz=((wert<<3) & 0x7F8) | (FilterFrequenz & 0x007);
        SetW0();
        break;

    case 23: // Filterkontrol_1 und Resonanz
        FilterResonanz=(wert>>4)&0x0F;
        SetQ();
        FilterKey=wert&0x0F;
        break;

    case 24: // Filterkontrol_2 und Lautstärke
        Voice3Off=wert&0x80;
        HpBpLp=(wert>>4)&0x07;
        Volume=wert&0x0F;
        break;
    }
}

//////////////// ALLE SID MODULE ////////////////

inline int MOS6581_8085::VoiceOutput(int voice)
{
    return (OscOutput(voice)-WaveZero)*EnvOutput(voice) + VoiceDC;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Oscillator /////////////////////////////////////////////////////////////////////////////////////

inline void MOS6581_8085::OscZyklus(void)
{
    static unsigned int AccuPrev;
    static unsigned int Bit0;

    for(int i=0;i<3;i++)
    {
        v=Voice[i];
        if(!v->TestBit)
        {
            AccuPrev=v->Accu;
            v->Accu+=v->Frequenz;
            v->Accu&=0xFFFFFF;
            v->MsbRising=!(AccuPrev&0x800000)&&(v->Accu&0x800000);
            if(!(AccuPrev&0x080000)&&(v->Accu&0x080000))
            {
                Bit0=((v->ShiftReg>>22)^(v->ShiftReg>>17))&0x01;
                v->ShiftReg<<=1;
                v->ShiftReg&=0x7FFFFF;
                v->ShiftReg|=Bit0;
            }
        }
    }

    /// Sycronisieren ?? ///
    for(int i=0;i<3;i++)
    {
        v=Voice[i];
        vs=Voice[v->SyncSource];
        vd=Voice[v->SyncDestination];
        if (v->MsbRising && vd->SyncBit && !(v->SyncBit && vs->MsbRising))
        {
            vd->Accu=0;
        }
    }
}

inline void MOS6581_8085::OscZyklus(int zyklen)
{
    static unsigned int AccuPrev;
    static unsigned int AccuDelta;
    static unsigned int ShiftPeriod;
    static unsigned int Bit0;

    for(int i=0;i<3;i++)
    {
        v=Voice[i];
        if(!v->TestBit)
        {
            AccuPrev=v->Accu;
            AccuDelta=zyklen*v->Frequenz;
            v->Accu+=AccuDelta;
            v->Accu&=0xFFFFFF;
            v->MsbRising=!(AccuPrev&0x800000)&&(v->Accu&0x800000);
            ShiftPeriod=0x100000;
            while (AccuDelta)
            {
                if (AccuDelta < ShiftPeriod)
                {
                    ShiftPeriod=AccuDelta;
                    if(ShiftPeriod<=0x80000)
                    {
                        if (((v->Accu-ShiftPeriod)&0x080000)||!(v->Accu& 0x080000))
                        {
                            break;
                        }
                    }
                    else
                    {
                        if (((v->Accu-ShiftPeriod)&0x080000)&&!(v->Accu&0x080000))
                        {
                            break;
                        }
                    }
                }
                Bit0=((v->ShiftReg>>22)^(v->ShiftReg>>17))&0x01;
                v->ShiftReg<<=1;
                v->ShiftReg&=0x7FFFFF;
                v->ShiftReg|=Bit0;
                AccuDelta-=ShiftPeriod;
            }
        }
    }
}

inline unsigned int MOS6581_8085::OscOutput(int voice)
{
    if(!Voice[voice]->OscEnable) return 0;

    v = Voice[voice];
    vs = Voice[v->SyncSource];

    switch (v->Waveform)
    {
    case 0x01:
        return WaveDreieck(v,vs);

    case 0x02:
        return WaveSaegezahn(v);

    case 0x03:
        return Wave0[WaveSaegezahn(v)]<<4;

    case 0x04:
        return (v->TestBit||(v->Accu>>12)>=v->Pulsweite)?0xFFF:0x000;

    case 0x05:
        return (Wave1[WaveDreieck(v,vs)>>1]<<4)&WaveRechteck(v);

    case 0x06:
        return (Wave2[WaveSaegezahn(v)]<<4)&WaveRechteck(v);

    case 0x07:
        return (Wave3[WaveSaegezahn(v)]<<4)&WaveRechteck(v);

    case 0x08:
        return WaveRauschen(v);

    default:
        return 0;
    }
}

inline unsigned int MOS6581_8085::WaveDreieck(VOICEClass *v,VOICEClass *vs)
{
    static unsigned int tmp;

    tmp = (v->RingBit ? v->Accu ^ vs->Accu : v->Accu)& 0x800000;
    return ((tmp ? ~v->Accu : v->Accu) >> 11) & 0xfff;
}

inline unsigned int MOS6581_8085::WaveSaegezahn(VOICEClass *v)
{
    return v->Accu>>12;
}

inline unsigned int MOS6581_8085::WaveRechteck(VOICEClass *v)
{
    return (v->TestBit||(v->Accu>>12)>=v->Pulsweite)?0xFFF:0x000;
}

inline unsigned int MOS6581_8085::WaveRauschen(VOICEClass *v)
{
    return ((v->ShiftReg&0x400000)>>11)|((v->ShiftReg&0x100000)>>10)|((v->ShiftReg&0x010000)>>7)|((v->ShiftReg&0x002000)>>5)|((v->ShiftReg&0x000800)>>4)|((v->ShiftReg&0x000080)>>1)|((v->ShiftReg&0x000010)<<1)|((v->ShiftReg&0x000004)<< 2);
}

inline void MOS6581_8085::OscReset(void)
{
    for(int i=0;i<3;i++)
    {
        v=Voice[i];
        v->Accu=0;
        v->ShiftReg=0x7FFFF8;
        v->Frequenz=0;
        v->Pulsweite=0;
        v->TestBit=false;
        v->RingBit=false;
        v->SyncBit=false;
        v->MsbRising=false;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Envelope ///////////////////////////////////////////////////////////////////////////////////////

inline void MOS6581_8085::EnvZyklus(void)
{
    for(int i=0;i<3;i++)
    {
        v=Voice[i];
        if(++v->RateCounter & 0x8000) ++v->RateCounter &= 0x7FFF;
        if(v->RateCounter != v->RatePeriod) goto L10;
        v->RateCounter=0;
        if(v->State == ATTACK || ++v->ExponentialCounter == v->ExponentialCounterPeriod)
        {
            v->ExponentialCounter=0;
            if(v->HoldZero) goto L10;

            switch(v->State)
            {
            case ATTACK:
                ++v->EnvCounter &= 0xFF;
                if(v->EnvCounter==0xFF)
                {
                    v->State=DECAY_SUSTAIN;
                    v->RatePeriod=RateCounterPeriod[v->Decay];
                }
                break;

            case DECAY_SUSTAIN:
                if(v->EnvCounter != SustainLevel[v->Sustain]) --v->EnvCounter;
                break;

            case RELEASE:
                --v->EnvCounter &= 0xFF;
                break;
            }

            switch(v->EnvCounter)
            {
            case 0xFF:
                v->ExponentialCounterPeriod=1;
                break;

            case 0x5D:
                v->ExponentialCounterPeriod=2;
                break;

            case 0x36:
                v->ExponentialCounterPeriod=4;
                break;

            case 0x1A:
                v->ExponentialCounterPeriod=8;
                break;

            case 0x0E:
                v->ExponentialCounterPeriod=16;
                break;

            case 0x06:
                v->ExponentialCounterPeriod=30;
                break;

            case 0x00:
                v->ExponentialCounterPeriod=1;
                v->HoldZero=true;
                break;
            }
        }
L10:;
    }
}

inline void MOS6581_8085::EnvZyklus(int zyklen)
{
    for(int i=0;i<3;i++)
    {
        v=Voice[i];
        int rate_step = v->RatePeriod-v->RateCounter;
        if (rate_step <= 0)
        {
            rate_step += 0x7fff;
        }

        while (zyklen)
        {
            if (zyklen < rate_step)
            {
                v->RateCounter+=zyklen;
                if (v->RateCounter & 0x8000)
                {
                    ++v->RateCounter &= 0x7fff;
                }
                goto L10;
            }
            v->RateCounter = 0;
            zyklen -= rate_step;

            if(v->State == ATTACK || ++v->ExponentialCounter == v->ExponentialCounterPeriod)
            {
                v->ExponentialCounter = 0;
                if (v->HoldZero)
                {
                    rate_step = v->RatePeriod;
                    continue;
                }

                switch(v->State)
                {
                case ATTACK:
                    ++v->EnvCounter &= 0xFF;
                    if(v->EnvCounter==0xFF)
                    {
                        v->State=DECAY_SUSTAIN;
                        v->RatePeriod=RateCounterPeriod[v->Decay];
                    }
                    break;

                case DECAY_SUSTAIN:
                    if(v->EnvCounter != SustainLevel[v->Sustain]) --v->EnvCounter;
                    break;

                case RELEASE:
                    --v->EnvCounter &= 0xFF;
                    break;
                }
                switch(v->EnvCounter)
                {
                case 0xFF:
                    v->ExponentialCounterPeriod=1;
                    break;

                case 0x5D:
                    v->ExponentialCounterPeriod=2;
                    break;

                case 0x36:
                    v->ExponentialCounterPeriod=4;
                    break;

                case 0x1A:
                    v->ExponentialCounterPeriod=8;
                    break;

                case 0x0E:
                    v->ExponentialCounterPeriod=16;
                    break;

                case 0x06:
                    v->ExponentialCounterPeriod=30;
                    break;

                case 0x00:
                    v->ExponentialCounterPeriod=1;
                    v->HoldZero=true;
                    break;
                }
            }
            rate_step = v->RatePeriod;
        }
	L10:;
    }
}

inline void MOS6581_8085::EnvReset(void)
{
    for(int i=0;i<3;i++)
    {
        v=Voice[i];
        v->EnvCounter=0;
        v->Attack=0;
        v->Decay=0;
        v->Sustain=0;
        v->Release=0;
        v->KeyBit=false;
        v->RateCounter=0;
        v->State=RELEASE;
        v->RatePeriod=RateCounterPeriod[v->Release];
        v->HoldZero=true;
        v->ExponentialCounter=0;
        v->ExponentialCounterPeriod=1;
    }
}

inline unsigned int MOS6581_8085::EnvOutput(int voice)
{
    if(!Voice[voice]->OscEnable) return 0;
    return Voice[voice]->EnvCounter;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Filter /////////////////////////////////////////////////////////////////////////////////////////

inline void MOS6581_8085::FilterZyklus(int voice1,int voice2,int voice3,int ext_in)
{
    voice1 >>= 7;
    voice2 >>= 7;

    if(Voice3Off&&!(FilterKey&0x04))
    {
       voice3 = 0;
    }
    else
    {
        voice3 >>= 7;
    }

    ext_in >>= 7;

    if(!FilterOn)
    {
        Vnf = voice1 + voice2 + voice3 + ext_in;
        Vhp = Vbp = Vlp = 0;
        return;
    }

    int Vi;

    switch (FilterKey)
    {
    default:

    case 0x00:
        Vi = 0;
        Vnf = voice1 + voice2 + voice3 + ext_in;
        break;

    case 0x01:
        Vi = voice1;
        Vnf = voice2 + voice3 + ext_in;
        break;

    case 0x02:
        Vi = voice2;
        Vnf = voice1 + voice3 + ext_in;
        break;

    case 0x03:
        Vi = voice1 + voice2;
        Vnf = voice3 + ext_in;
        break;

    case 0x04:
        Vi = voice3;
        Vnf = voice1 + voice2 + ext_in;
        break;

    case 0x05:
        Vi = voice1 + voice3;
        Vnf = voice2 + ext_in;
        break;

    case 0x06:
        Vi = voice2 + voice3;
        Vnf = voice1 + ext_in;
        break;

    case 0x07:
        Vi = voice1 + voice2 + voice3;
        Vnf = ext_in;
        break;

    case 0x08:
        Vi = ext_in;
        Vnf = voice1 + voice2 + voice3;
        break;

    case 0x09:
        Vi = voice1 + ext_in;
        Vnf = voice2 + voice3;
        break;

    case 0x0A:
        Vi = voice2 + ext_in;
        Vnf = voice1 + voice3;
        break;

    case 0x0B:
        Vi = voice1 + voice2 + ext_in;
        Vnf = voice3;
        break;

    case 0x0C:
        Vi = voice3 + ext_in;
        Vnf = voice1 + voice2;
        break;

    case 0x0D:
        Vi = voice1 + voice3 + ext_in;
        Vnf = voice2;
        break;

    case 0x0E:
        Vi = voice2 + voice3 + ext_in;
        Vnf = voice1;
        break;

    case 0x0F:
        Vi = voice1 + voice2 + voice3 + ext_in;
        Vnf = 0;
        break;
    }
    int dVbp = (w0_ceil_1*Vhp>>20);
    int dVlp = (w0_ceil_1*Vbp>>20);
    Vbp-=dVbp;
    Vlp-=dVlp;
    Vhp=(Vbp*_1024_div_Q>>10)-Vlp-Vi;
}

inline void MOS6581_8085::FilterZyklus(int zyklen,int voice1,int voice2,int voice3,int ext_in)
{
    voice1 >>= 7;
    voice2 >>= 7;

    if(Voice3Off&&!(FilterKey&0x04)) voice3 = 0;
    else voice3 >>= 7;

    ext_in >>= 7;

    if(!FilterOn)
    {
        Vnf = voice1 + voice2 + voice3 + ext_in;
        Vhp = Vbp = Vlp = 0;
        return;
    }

    int Vi;

    switch (FilterKey)
    {

    default:

    case 0x00:
        Vi = 0;
        Vnf = voice1 + voice2 + voice3 + ext_in;
        break;

    case 0x01:
        Vi = voice1;
        Vnf = voice2 + voice3 + ext_in;
        break;

    case 0x02:
        Vi = voice2;
        Vnf = voice1 + voice3 + ext_in;
        break;

    case 0x03:
        Vi = voice1 + voice2;
        Vnf = voice3 + ext_in;
        break;

    case 0x04:
        Vi = voice3;
        Vnf = voice1 + voice2 + ext_in;
        break;

    case 0x05:
        Vi = voice1 + voice3;
        Vnf = voice2 + ext_in;
        break;

    case 0x06:
        Vi = voice2 + voice3;
        Vnf = voice1 + ext_in;
        break;

    case 0x07:
        Vi = voice1 + voice2 + voice3;
        Vnf = ext_in;
        break;

    case 0x08:
        Vi = ext_in;
        Vnf = voice1 + voice2 + voice3;
        break;

    case 0x09:
        Vi = voice1 + ext_in;
        Vnf = voice2 + voice3;
        break;

    case 0x0A:
        Vi = voice2 + ext_in;
        Vnf = voice1 + voice3;
        break;

    case 0x0B:
        Vi = voice1 + voice2 + ext_in;
        Vnf = voice3;
        break;

    case 0x0C:
        Vi = voice3 + ext_in;
        Vnf = voice1 + voice2;
        break;

    case 0x0D:
        Vi = voice1 + voice3 + ext_in;
        Vnf = voice2;
        break;

    case 0x0E:
        Vi = voice2 + voice3 + ext_in;
        Vnf = voice1;
        break;

    case 0x0F:
        Vi = voice1 + voice2 + voice3 + ext_in;
        Vnf = 0;
        break;
    }

    int delta_t_flt=8;
    while (zyklen)
    {
        if (zyklen < delta_t_flt)
        {
            delta_t_flt = zyklen;
        }
        int w0_delta_t = w0_ceil_dt*delta_t_flt>>6;
        int dVbp = (w0_delta_t*Vhp >> 14);
        int dVlp = (w0_delta_t*Vbp >> 14);
        Vbp-=dVbp;
        Vlp-=dVlp;
        Vhp=(Vbp*_1024_div_Q>>10)-Vlp-Vi;
        zyklen -= delta_t_flt;
    }
}

inline int MOS6581_8085::FilterOutput(void)
{
    if (!FilterOn)
    {
        return (Vnf + MixerDC)*static_cast<int>(Volume);
    }

    int Vf;

    switch(HpBpLp)
    {
    default:

    case 0x00:
        Vf=0;
        break;

    case 0x01:
        Vf=Vlp;
        break;

    case 0x02:
        Vf=Vbp;
        break;

    case 0x03:
        Vf=Vlp+Vbp;
        break;

    case 0x04:
        Vf=Vhp;
        break;

    case 0x05:
        Vf=Vlp+Vhp;
        break;

    case 0x06:
        Vf=Vbp+Vhp;
        break;

    case 0x07:
        Vf=Vlp+Vbp+Vhp;
        break;
    }
    return (Vnf+Vf+MixerDC)*static_cast<int>(Volume);
}

inline void MOS6581_8085::FilterReset(void)
{
    FilterFrequenz=0;
    FilterResonanz=0;
    FilterKey=0;
    Voice3Off=0;
    HpBpLp=0;
    Volume=0;

    Vhp=0;
    Vbp=0;
    Vlp=0;
    Vnf=0;

    SetW0();
    SetQ();
}

inline void MOS6581_8085::SetW0(void)
{
    const float pi = 3.1415926535897932385;
    w0=(int)(2*pi*f0[FilterFrequenz]*1.048576);
    const int w0_max_1 = static_cast<int>(2*pi*16000*1.048576);
    w0_ceil_1 = w0 <= w0_max_1 ? w0 : w0_max_1;
    const int w0_max_dt = static_cast<int>(2*pi*4000*1.048576);
    w0_ceil_dt = w0 <= w0_max_dt ? w0 : w0_max_dt;
}

inline void MOS6581_8085::SetQ(void)
{
    _1024_div_Q = static_cast<int>(1024.0/(0.707 + 1.0*FilterResonanz/0x0F));
}
