//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: mos6581_8085_voice.h                  //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 12.04.2011		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#ifndef MOS6581_8085_VOICE_H
#define MOS6581_8085_VOICE_H

class VOICEClass
{
    public:
    /// Oscillator Register ///
    bool                OscEnable;
    bool		TestBit;
    bool		RingBit;
    bool		SyncBit;
    unsigned char	Waveform;
    unsigned int	Frequenz;
    unsigned int	Pulsweite;
    unsigned int	Accu;
    unsigned int	ShiftReg;
    unsigned int	MsbRising;
    int			SyncSource;
    int			SyncDestination;

    /// Envelope Register ///
    bool		KeyBit;
    int			State;
    unsigned int	RatePeriod;
    unsigned int	RateCounter;
    unsigned int	ExponentialCounterPeriod;
    unsigned int	ExponentialCounter;
    unsigned int	EnvCounter;
    bool		HoldZero;
    unsigned int	Attack;
    unsigned int	Decay;
    unsigned int	Sustain;
    unsigned int	Release;
};
#endif // MOS6581_8085_VOICE_H
