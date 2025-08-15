//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: mos6522_class.cpp                     //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "mos6522_class.h"



MOS6522::MOS6522(bool *reset_line, bool *irq_line)
{
    this->reset_line = reset_line;
    this->irq_line = irq_line;
}

MOS6522::~MOS6522()
{
}

void MOS6522::Reset()
{
    ora = orb = 0x00;
    ddra = ddrb = 0x00;
    ioa = iob = 0xff;
    ioa_old = iob_old = 0xff;
    latcha = latchb = 0x00;

    pcr = acr = 0;
}

void MOS6522::OneZyklus()
{
    if(!*reset_line) Reset();

    /*
    timera--;
    timerb--;

    if(timerb == 0)
    {
        io[0x0d] |= 0x40;
        timera = timera_latch;
    }

    if(timerb == 0)
    {
        io[0x0d] |= 0x20;
        timerb = timerb_latch;
    }

    if (((io[0x0d] & 127) & (io[0x0e] & 127)) != 0)
    {
        if(irq_line)
            *irq_line = true;

        io[0x0d] |= 0x80;
    }
    else
    {
        if(irq_line)
            *irq_line = false;
    }
    */
}

void MOS6522::WriteIO(uint16_t address, uint8_t value)
{
    address &= 0x0f; // Ensure address is within range

    io[address] = value;

    switch (address)
    {
    case VIA_PRA:
        [[fallthrough]];
    case VIA_PRA_NHS:
    case VIA_DDRA:
        break;
    case VIA_PRB:
        [[fallthrough]];
    case VIA_DDRB:
        if(address == VIA_PRB)
            orb = value; // Update Output Port B
        else
            ddrb = value; // Update Data Direction Register B

        uint8_t iob = orb | ~ddrb;
        this->iob = iob;

        WritePort(PORTS::B, this);

        iob_old = iob;
        break;
    }
}

uint8_t MOS6522::ReadIO(unsigned short address)
{
    address &= 0x0f; // Ensure address is within range
    uint8_t value = 0xff;

    switch (address)
    {
    case VIA_PRB: {
        uint8_t out;
        if ( acr & 2 )
            out = latchb;
        else
            out =  ReadPort( PORTS::B, this );

        return out;
    }
    default:
        break;
    }

    return io[address];
}
