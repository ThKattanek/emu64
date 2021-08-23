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
// Letzte Änderung am 22.09.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef MOS_6526_CLASS_H
#define MOS_6526_CLASS_H

#include <cstring>
#include <functional>

#include "./cia_port.h"
#include "./structs.h"

#define ClockCounterPAL 98438
#define ClockCounterNTSC 118126
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
    inline void CheckLP();

    /// Variablen ///
    uint8_t     cia_nr;
    uint8_t     io[16];
    uint8_t     interupt_mask;
    uint8_t     cra, crb, icr;
    uint8_t     new_cra, new_crb;
    bool        writing_new_cra, writing_new_crb;
    bool        timer_a_cnt_phi2;
    bool        timer_b_cnt_phi2;
    bool        timer_b_cnt_timer_a;
    bool        timer_b_cnt_cnt_pin;
    uint8_t     timer_a_status;
    uint8_t     timer_b_status;
    uint16_t    timer_a;
    uint16_t    timer_a_latch;
    uint16_t    timer_b;
    uint16_t    timer_b_latch;

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
    uint8_t     pb_6;
    uint8_t     pb_7;
    uint8_t     prev_lp;
};
#endif // MOS_6526_CLASS_H
