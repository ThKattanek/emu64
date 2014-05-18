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
// Letzte Änderung am 18.05.2014                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef MOS6522_CLASS_H
#define MOS6522_CLASS_H

#include "structs.h"

#include "tr1/functional"
using namespace std::tr1;
using namespace std::tr1::placeholders;

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

    function<bool(void)> SyncFound;
    function<unsigned char(void)> ReadGCRByte;
    function<void(unsigned char)> WriteGCRByte;
    function<void(void)> SpurInc;
    function<void(void)> SpurDec;
    function<void(int)> TriggerInterrupt;
    function<void(int)> ClearInterrupt;

    /// Variablen ///

    bool            *RESET;
    bool            *WriteProtected;
    bool            *DiskMotorOn;
    unsigned char   *FloppyIEC;
    unsigned char   *C64IEC;
    unsigned char   *Jumper;

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
