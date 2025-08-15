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
#include <functional>

// VIA (VIA = Versatile Interface Adapter) register definitions
#define VIA_PRB         0  // Port B
#define VIA_PRA         1  // Port A
#define VIA_DDRB        2  // Data direction register for port B
#define VIA_DDRA        3  // Data direction register for port A
#define VIA_T1CL        4  // Timer 1 count low
#define VIA_T1CH        5  // Timer 1 count high
#define VIA_T1LL        6  // Timer 1 latch low
#define VIA_T1LH        7  // Timer 1 latch high
#define VIA_T2CL        8  // Timer 2 count low - read only
#define VIA_T2LL        8  // Timer 2 latch low - write only
#define VIA_T2CH        9  // Timer 2 count high - read only
#define VIA_T2LH        9  // Timer 2 latch high - write only
#define VIA_SR          10 // Serial port shift register
#define VIA_ACR         11 // Auxiliary control register
#define VIA_PCR         12 // Peripheral control register
#define VIA_IFR         13 // Interrupt flag register
#define VIA_IER         14 // Interrupt control register
#define VIA_PRA_NHS     15 // Port A with no handshake

// Interrupt Mask
#define VIA_IM_IRQ      128     // Control Bit
#define VIA_IM_T1       64      // Timer 1 underflow
#define VIA_IM_T2       32      // Timer 2 underflow
#define VIA_IM_CB1      16      // Handshake
#define VIA_IM_CB2      8       // Handshake
#define VIA_IM_SR       4       // Shift Register completion
#define VIA_IM_CA1      2       // Handshake
#define VIA_IM_CA2      1       // Handshake

class MOS6522
{
public:

    MOS6522(bool *reset_line, bool *irq_line = nullptr);
    ~MOS6522();

    void Reset();
    void OneZyklus(void);
    void WriteIO(uint16_t address, uint8_t value);
    uint8_t ReadIO(unsigned short address);

    void SetExternalInputPA(uint8_t value) { ira = value; }
    void SetExternalInputPB(uint8_t value) { irb = value; }

    uint8_t GetOutputPA() { return ora & ddra; }
    uint8_t GetOutputPB() { return iob; }

    // Port A and B states
    uint8_t ioa;
    uint8_t ioa_old;
    uint8_t iob;
    uint8_t iob_old;

    enum class PORTS : uint8_t
    {
        A = 0,
        B = 1
    };

    std::function<uint8_t(PORTS port, MOS6522 *via)> ReadPort;
    std::function<void(PORTS port, MOS6522 *via)> WritePort;

private:

    bool *reset_line = nullptr;
    bool *irq_line = nullptr;

    uint8_t   io[16]; // IO registers

    uint8_t   orb = 0x00; // Output Port B                  // Regnr. 0x00
    uint8_t   irb = 0x00; // External Input Port B
    uint8_t   ora = 0x00; // Output Port A                  // Regnr. 0x01
    uint8_t   ira = 0x00; // External Input Port A
    uint8_t   ddrb = 0x00; // Data Direction Register B     // Regnr. 0x02
    uint8_t   ddra = 0x00; // Data Direction Register A     // Regnr. 0x03
    uint8_t   t1cl = 0x00; // T1 Low-Order Counter          // Regnr. 0x04
    uint8_t   t1ch = 0x00; // T1 High-Order Counter         // Regnr. 0x05
    uint8_t   t1ll = 0x00; // T1 Low-Order Latch            // Regnr. 0x06
    uint8_t   t1lh = 0x00; // T1 High-Order Latch           // Regnr. 0x07
    uint8_t   t2cl = 0x00; // T2 Low-Order Latch/Counter    // Regnr. 0x08
    uint8_t   t2ch = 0x00; // T2 High-Order Counter         // Regnr. 0x09
    uint8_t   sr = 0x00; // Shift Register                  // Regnr. 0x0A
    uint8_t   acr = 0x00; // Auxiliary Control Register     // Regnr. 0x0B
    uint8_t   pcr = 0x00; // Peripheral Control Register    // Regnr. 0x0C
    uint8_t   ifr = 0x00; // Interrupt Flag Register        // Regnr. 0x0D
    uint8_t   ier = 0x00; // Interrupt Enable Register      // Regnr. 0x0E
    // ora/ira without handshake                            // Regnr. 0x0F

    // Ports

    uint8_t latcha;
    uint8_t latchb;

    uint8_t   last_read = 0x00; // Last read register address

    bool ca2_out_state = false; // CA2 output state
};

#endif // MOS6522_CLASS_H
