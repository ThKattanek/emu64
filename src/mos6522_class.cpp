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
    for(int i=0; i<16; i++) io[i] = 0x00;

    io[0]=0xff;
    io[1]=0xff;
    io[2]=0xff;
    io[3]=0xff;

    pa = 0xff;
    pb = 0xff;
    ddra = 0xff;
    ddrb = 0xff;
}

void MOS6522::OneZyklus()
{
    if(!*reset_line) Reset();

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
}

uint8_t MOS6522::GetIOZero(void)
{
    return io[0];
}

void MOS6522::WriteIO(uint16_t address, uint8_t value)
{
    io[address & 0x0f] = value;

    switch (address & 0x0f)
    {
        case 0x00:
        {
            pa = value;
            break;
        }
        case 0x01:
    [[fallthrough]];
        case 0x0f:
        {
            pa = value;
            break;
        }
        case 0x02:
        {
            ddrb = value;
            break;
        }
        case 0x03:
        {
            ddra = value;
            break;
        }
        case 0x04:
        {
            timera_latch = (timera_latch & 0xff00) | value;
            break;
        }
        case 0x05:
        {
            timera_latch = (timera_latch & 0x00ff) | (value << 8);
            io[0x0d] &=0xbf;
            timera = timera_latch;
            break;
        }
        case 0x06:
        {
            timera_latch = (timera_latch & 0xff00) | value;
            break;
        }
        case 0x07:
        {
            timera_latch = (timera_latch & 0x00ff) | (value << 8);
            break;
        }
        case 0x08:
        {
            timerb_latch = (timerb_latch & 0xFF00) | value;
            break;
        }
        case 0x09:
        {
            timerb_latch = (timerb_latch & 0x00FF) | (value << 8);
            io[0x0D] &=0xDF;
            timerb = timerb_latch;
            break;
        }
        case 0x0D:
        {
            io[0x0d] &= ~value;
            break;
        }
    }
}

uint8_t MOS6522::ReadIO(unsigned short address)
{
    switch (address & 0x0f)
    {
        case 0x00:
        {
            return pa | (ddra & 0x80 ? 0 : 0xff);
            break;
        }
        case 0x01:
        {
            io[0x0D] &= 253;
        }
    [[fallthrough]];
        case 0x0f:
        {
            return 0xff;
        }

        case 0x02:
        {
            return ddrb;
        }
        case 0x03:
        {
            return ddra;
        }
        case 0x04:
        {
            io[0x0d] &= 0xbf;
            return (unsigned char)timera;
        }
        case 0x05:
        {
            return timera >> 8;
        }
        case 0x06:
        {
            return (unsigned char)timera_latch;
        }
        case 0x07:
        {
            return timera_latch >> 8;
        }
        case 0x08:
        {
            io[0x0D] &= 0xDF;
            return (unsigned char)timerb;
        }
        case 0x09:
        {
            return timerb>>8;
        }
        case 0x0D:
        {
            return io[0x0d] | (io[0x0d] & io[0x0e] ? 0x80 : 0);
        }
    }
    return io[address & 0x0f];
}
