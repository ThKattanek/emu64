//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: mos6526_class.h                       //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 18.05.2014                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef MOS_6526_CLASS_H
#define MOS_6526_CLASS_H

#include <cstring>
#include "mos6526_port.h"
#include "structs.h"

#include "tr1/functional"
using namespace std::tr1;
using namespace std::tr1::placeholders;

#define UhrCounterPAL 98438
#define UhrCounterNTSC 118126
#define Byte2BCD(byte)  (((((byte) / 10) << 4) + ((byte) % 10)) & 0xFF)
#define BCD2Byte(bcd)   (((10*(((bcd) & 0xf0) >> 4)) + ((bcd) & 0xf)) & 0xFF)

class MOS6526
{
	// Timer states
	enum 
	{
            STOP,
            WAIT_THEN_COUNT,
            LOAD_THEN_STOP,
            LOAD_THEN_COUNT,
            LOAD_THEN_WAIT_THEN_COUNT,
            COUNT,
            COUNT_THEN_STOP,
	};
public:

    /// Funktionen ///

    MOS6526(unsigned char cia_nr);
    ~MOS6526();
    void Reset(void);
    void OneZyklus(void);
    void GetRegister(CIA_STRUCT *cia_reg);
    //bool SaveFreez(FILE* File);
    //bool LoadFreez(FILE *File,unsigned short Version);
    void SetSDR(unsigned char wert);
    void WriteIO(unsigned short adresse,unsigned char wert);
    unsigned char ReadIO(unsigned short adresse);

    /// Variablen ///

    bool *RESET;
    bool RESET_OLD;
    function<void(int)> CpuTriggerInterrupt;
    function<void(int)> CpuClearInterrupt;
    function<void(void)> VicTriggerLP;

    PORT *PA;
    PORT *PB;
    unsigned char   PALatch;
    unsigned char   PBLatch;
    unsigned char   SDR;

    unsigned char   *C64IEC;
    unsigned char   *FloppyIEC;
    bool            *FLAG_PIN;
    bool            FLAG_PIN_OLD;
    bool            CNT_PIN;

private:

    /// Funktionen ///
    inline void TriggerInterrupt(int bit);
    inline void TimerCount(void);
    inline void CheckLP(void);

    /// Variablen ///
    unsigned char   CiaNr;
    unsigned char   IO[16];
    unsigned char   IntMask;
    unsigned char   CRA,CRB,ICR;
    unsigned char   NEW_CRA,NEW_CRB;
    bool            WRITE_NEW_CRA,WRITE_NEW_CRB;
    bool            TimerA_CNT_PHI2;
    bool            TimerB_CNT_PHI2;
    bool            TimerB_CNT_TimerA;
    bool            TimerB_CNT_CNTPin;
    char            TimerAStatus;
    char            TimerBStatus;
    unsigned short  TimerA;
    unsigned short  TimerALatch;
    unsigned short  TimerB;
    unsigned short  TimerBLatch;

    int             UhrCounterLatch;
    int             UhrCounter;
    bool            UhrLatched;
    bool            UhrStopped;
    unsigned char   UhrLatch[4];
    unsigned char   UhrAlarm[4];

    unsigned char   DDRA;
    unsigned char   DDRB;
    bool            EnablePB6;
    bool            EnablePB7;
    int             PB6Mode;
    int             PB7Mode;
    unsigned char   PB6;
    unsigned char   PB7;
    unsigned char   Z1,Z2;
    unsigned char   InMode;
    bool            TimerANull;
    unsigned char   PrevLP;
};
#endif // MOS_6526_CLASS_H
