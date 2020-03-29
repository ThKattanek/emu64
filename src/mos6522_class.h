//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: mos6522_class.h                       //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 29.03.2020                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef MOS6522_CLASS_H
#define MOS6522_CLASS_H

#include <functional>

#include "./structs.h"

#define LO false
#define HI true

class MOS6522
{
public:

    /// Funktionen ///

    MOS6522(unsigned char via_nr);
    ~MOS6522();

    void Reset(void);
    unsigned char GetIO_Zero(void);
    void OneZyklus(void);
    void WriteIO(unsigned short adresse, unsigned char);
    unsigned char ReadIO(unsigned short adresse);

    std::function<bool(void)> SyncFound;
    std::function<unsigned char(void)> ReadGCRByte;
    std::function<void(unsigned char)> WriteGCRByte;
    std::function<void(void)> SpurInc;
    std::function<void(void)> SpurDec;
    std::function<void(int)> TriggerInterrupt;
    std::function<void(int)> ClearInterrupt;

    /// Variablen ///

    bool            *RESET;
    bool            *WriteProtect;
    bool            *DiskMotorOn;
    unsigned char   *FloppyIEC;
    unsigned char   *C64IEC;
    unsigned char   *Jumper;
    bool            CA2_Output;

private:

    /// Variablen ///

    unsigned char   VIANummer;
    unsigned char   IO[16];
    unsigned char   PA;
    unsigned char   PB;
    unsigned char   DDRA;
    unsigned char   DDRB;
    unsigned short  TimerA;
    unsigned short  TimerALatch;
    unsigned short  TimerB;
    unsigned short  TimerBLatch;
    bool            ATNState;
    bool            IECInterrupt;
    unsigned char   OldIECLines;
    bool            OldATNState;
    unsigned char   TmpByte;
};

#endif // MOS6522_CLASS_H
