//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: mos6581_8085_class.h                  //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 18.07.2011		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#ifndef MOS6581_8085_CLASS_H
#define MOS6581_8085_CLASS_H

#include "siddump.h"
#include "mos6581_8085_voice.h"

//// Sid Typen ////
#define MOS_6581 0
#define MOS_8580 1

#define	ATTACK		0
#define	DECAY_SUSTAIN   1
#define RELEASE		2

#define	PUFFER_TIME	1

typedef int fc_point[2];

class MOS6581_8085
{
  public:
    /* Variable */
    short               *SoundBuffer;
    short               *SoundBufferV0;
    short               *SoundBufferV1;
    short               *SoundBufferV2;
    int                 SoundBufferPos;
    int                 SoundBufferSize;
    bool                CycleExact;
    bool                FilterOn;
    bool                SoundOutputEnable;
    bool                *RESET;

    unsigned char	WriteReg;
    SIDDumpClass	*IoDump;

    bool		Mouse1351Enable;
    unsigned char	MouseX;
    unsigned char	MouseY;

    /// Recording ///
    bool		Recording;
    int                 RecSampleCounter;
    short		RecSampleBuffer[19656];

    /* Funktionen */
    MOS6581_8085(int nummer, int samplerate,int puffersize,int *error);
    virtual ~MOS6581_8085(void);
    void ChangeSampleRate(int samplerate,int puffersize);
    void Reset(void);
    void SetChipType(bool type);
    void SetC64Zyklen(double ZyklenSek);
    void SetVoiceEnable(int nr, bool enable);
    bool OneZyklus(void);
    bool SaveFreez(FILE* File);
    bool LoadFreez(FILE *File,unsigned short Version);
    void WriteIO(unsigned short adresse,unsigned char wert);
    unsigned char ReadIO(unsigned short adresse);
    
  private:
    /* Variable */
    int                 Zyklencounter;
    bool                ret;

    double              Samplerate;
    VOICEClass          *Voice[3];
    unsigned char       IO[32];

    VOICEClass          *v;     // Für Temporären zugriff auf die Voices
    VOICEClass          *vs;    // Für Temporären zugriff auf die Voices (Source Voice)
    VOICEClass          *vd;    // Für Temporären zugriff auf die Voices (Destination Voice)

    int	SidNummer;
    bool                SidModel;

    double              FreqConvCounter;	// interner Counter für Fast Fast Fast Resampling ;-)
    double              FreqConvAddWert;

    /// Zeiger auf Sonder Wellen (Mischformen) ///
    unsigned int	*Wave0;
    unsigned int	*Wave1;
    unsigned int	*Wave2;
    unsigned int	*Wave3;

    unsigned int	VoiceDC;
    unsigned int	WaveZero;

    /// Filter Register ///
    unsigned int	FilterKey;
    unsigned int	FilterFrequenz;
    unsigned int	FilterResonanz;
    unsigned int	Voice3Off;
    unsigned int	HpBpLp;
    unsigned int	Volume;

    /// Filter interne Register
    int                 MixerDC;
    int                 Vhp;
    int                 Vbp;
    int                 Vlp;
    int                 Vnf;
    int                 w0,w0_ceil_1,w0_ceil_dt;
    int                 _1024_div_Q;
    int                 f0_6581[2048];
    int                 f0_8580[2048];
    int*		f0;
    fc_point*           f0_points;
    int			f0_count;
	
    /* Funktionen */
    int VoiceOutput(int voice);
    void OscZyklus(void);
    void OscZyklus(int zyklen);
    unsigned int OscOutput(int voice);
    unsigned int WaveDreieck(VOICEClass *v,VOICEClass *vs);
    unsigned int WaveSaegezahn(VOICEClass *v);
    unsigned int WaveRechteck(VOICEClass *v);
    unsigned int WaveRauschen(VOICEClass *v);
    void OscReset(void);
    void EnvZyklus(void);
    void EnvZyklus(int zyklen);
    void EnvReset(void);
    unsigned int EnvOutput(int voice);
    void FilterZyklus(int voice1,int voice2,int voice3,int ext_in);
    void FilterZyklus(int zyklen,int voice1,int voice2,int voice3,int ext_in);
    int FilterOutput(void);
    void FilterReset(void);
    void SetW0(void);
    void SetQ(void);
};
#endif // MOS6581_8085CLASS_H
