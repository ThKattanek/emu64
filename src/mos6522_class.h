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
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef MOS6522_CLASS_H
#define MOS6522_CLASS_H

#include <cstdint>

class MOS6522
{
public:

    MOS6522(bool *reset_line, bool *irq_line = nullptr);
    ~MOS6522();

    void Reset();
    void OneZyklus(void);
    void WriteIO(uint16_t address, uint8_t value);
    uint8_t ReadIO(unsigned short address);
    uint8_t GetIOZero(void);

private:

    bool *reset_line = nullptr;
    bool *irq_line = nullptr;

    uint8_t   io[16];
    uint8_t   pa;
    uint8_t   pb;
    uint8_t   ddra;
    uint8_t   ddrb;
    uint16_t  timera;
    uint16_t  timera_latch;
    uint16_t  timerb;
    uint16_t  timerb_latch;
};

#endif // MOS6522_CLASS_H
