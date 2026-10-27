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
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef MOS_6526_CLASS_H
#define MOS_6526_CLASS_H

#include <functional>

#include "./mos6526_port.h"
#include "./structs.h"

#define ClockCounterPAL 98438
#define ClockCounterNTSC 118126
#define Byte2BCD(byte)  (((((byte) / 10) << 4) + ((byte) % 10)) & 0xFF)
#define BCD2Byte(bcd)   (((10*(((bcd) & 0xf0) >> 4)) + ((bcd) & 0xf)) & 0xFF)

class MOS6526
{
    // Zustand eines Timers (Timer A und Timer B benutzen die gleiche Struktur)
    struct TIMER
    {
        uint16_t    counter;
        uint16_t    latch;
        bool        cr_start;           // Bit 0 vom Steuerregister (Timer laeuft)
        bool        cr_oneshot;         // Bit 3 vom Steuerregister
        bool        cr_force_load;      // Bit 4 vom Steuerregister (gilt nur einen Takt)
        bool        count_phi2;         // Zaehlt mit PHI2 (sonst durch externe Impulse)
        bool        count_step;         // Externer Zaehlimpuls (CNT Pin / Unterlauf von Timer A)
        bool        count2;             // Zaehlpipeline Stufe 2
        bool        count3;             // Zaehlpipeline Stufe 3 (in dieser Stufe wird der Zaehler verringert)
        bool        load1;              // Ladepipeline Stufe 1
        bool        load;               // Ladepipeline Stufe 2 (Latch wird in den Zaehler geladen)
        bool        oneshot0;           // OneShot Pipeline Stufe 1
        bool        oneshot;            // OneShot Pipeline Stufe 2
    };
public:

    /// Funktionen ///

    MOS6526(uint8_t cia_nr);
    ~MOS6526();
    void Reset();
    void OneZyklus();
    void GetRegister(CIA_STRUCT *cia_reg);
    //bool SaveFreez(FILE* File);
    //bool LoadFreez(FILE *File,unsigned short Version);
    void SetSDR(uint8_t value);
    void WriteIO(uint16_t address, uint8_t value);
    uint8_t ReadIO(uint16_t address);

    /// Variablen ///

    bool *reset_wire;
    bool reset_wire_old;
    std::function<void(int)> CpuTriggerInterrupt;
    std::function<void(int)> CpuClearInterrupt;
    std::function<void(void)> VicTriggerLP;

    std::function<void(void)> ChangePOTSwitch;

    PORT        *pa;
    PORT        *pb;
    uint8_t     pa_latch;
    uint8_t     pb_latch;
    uint8_t     sdr;

    uint8_t     *c64_iec_wire;
    uint8_t     *floppy_iec_wire;
    bool        *flag_pin;
    bool        flag_pin_old;
    bool        cnt_pin;

private:

    /// Funktionen ///
    inline void TriggerInterrupt(int bit);
    inline void TimerCount();
    inline void TimerReset(TIMER *timer);
    inline bool TimerStep(TIMER *timer);
    inline void CheckLP();

    /// Variablen ///
    uint8_t     cia_nr;
    uint8_t     io[16];
    uint8_t     interupt_mask;
    uint8_t     cra, crb, icr;
    uint8_t     timer_b_mode;       // Bit 5 und 6 aus CRB (0=PHI2, 1=CNT Pin, 2/3=Unterlauf von Timer A)
    TIMER       timer_a;
    TIMER       timer_b;
    bool        timer_a_underflow_old;      // Timer A hatte im vorigen Takt einen Unterlauf
    bool        timer_b_underflow_old;      // Timer B hatte im vorigen Takt einen Unterlauf
    bool        cnt_pin_old;

    uint32_t    clock_counter_latch;
    uint32_t    clock_counter;
    bool        clock_is_latched;
    bool        clock_is_stopped;
    uint8_t     clock_latch[4];
    uint8_t     clock_alarm[4];

    uint8_t     ddr_a;
    uint8_t     ddr_b;
    bool        enable_pb6;
    bool        enable_pb7;
    uint8_t     pb6_mode;
    uint8_t     pb7_mode;
    uint8_t     pb_6;               // Zustand des Toggle Ausgangs von Timer A (PB6)
    uint8_t     pb_7;               // Zustand des Toggle Ausgangs von Timer B (PB7)
    uint8_t     prev_lp;
};
#endif // MOS_6526_CLASS_H
