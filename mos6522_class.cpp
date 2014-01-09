//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: mos6522_class.cpp                     //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 20.07.2011		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#include "mos6522_class.h"

MOS6522::MOS6522(unsigned char via_nr):
    DiskMotorOn(0),
    FloppyIEC(0),
    C64IEC(0),
    Jumper(0)
{
    VIANummer = via_nr;
    Reset();
}

MOS6522::~MOS6522()
{
}

void MOS6522::Reset()
{
    for(int i=0;i<16;i++) IO[i] = 0x00;

    IO[0]=0xFF;
    IO[1]=0xFF;
    IO[2]=0xFF;
    IO[3]=0xFF;

    PA = 0xFF;
    PB = 0xFF;
    DDRA = 0xFF;
    DDRB = 0xFF;

    if(DiskMotorOn) *DiskMotorOn = true;

    ATNState = false;
    IECInterrupt = false;
}

void MOS6522::OneZyklus()
{
    if(!*RESET) Reset();

    TimerA--;
    TimerB--;

    if(TimerA == 0)
    {
        IO[0x0D] |= 0x40;
        TimerA = TimerALatch;
    }

    if(TimerB == 0)
    {
        IO[0x0D] |= 0x20;
        TimerB = TimerBLatch;
    }

    if(VIANummer == 0)
    {
        if (((*C64IEC) ^ OldIECLines) & 0x10)
        {
            ATNState = true;
            if (OldIECLines & 0x10) // ATN 1->0
            IECInterrupt = true;
        }
        OldIECLines = *C64IEC;

        OldATNState = ATNState;
        ATNState = false;

        if(OldATNState)
        {
            TmpByte = ~PB & DDRB;
            *FloppyIEC = ((TmpByte << 6) & ((~TmpByte ^ *C64IEC) << 3) & 0x80) |    // DATA (incl. ATN acknowledge)
                         ((TmpByte << 3) & 0x40);                                   // CLK

        }

        if(IECInterrupt)
        {
            IECInterrupt = false;
            IO[0x0D]|=0x02;
        }
    }

    if (((IO[0x0D]&127)&(IO[0x0E]&127))!=0)
    {
        if(VIANummer == 0)TriggerInterrupt(VIA1_IRQ);
        else TriggerInterrupt(VIA2_IRQ);
        IO[0x0D]|=0x80;
    }
    else
    {
        if(VIANummer == 0)ClearInterrupt(VIA1_IRQ);
        else ClearInterrupt(VIA2_IRQ);
    }
}

unsigned char MOS6522::GetIO_Zero(void)
{
    return IO[0];
}

void MOS6522::WriteIO(unsigned short adresse, unsigned char wert)
{
    IO[adresse & 0xF] = wert;

    switch(VIANummer)
    {
    case 0: // VIA 1 // Komunitkation via IEC BUS
    {
        switch (adresse & 0xF)
        {
            case 0x00:
            {
                PB = wert;
                wert = ~PB & DDRB;
                *FloppyIEC = ((wert << 6) & ((~wert ^ *C64IEC) << 3) & 0x80) | ((wert << 3) & 0x40);
                break;
            }
            case 1:
            case 15:
            {
                PA = wert;
                break;
            }
            case 0x02:
            {
                DDRB = wert;
                wert &= ~PB;
                *FloppyIEC = ((wert << 6) & ((~wert ^ *C64IEC) << 3) & 0x80) | ((wert << 3) & 0x40);
                break;
            }
            case 0x03:
            {
                DDRA = wert;
                break;
            }
            case 0x04:
            {
                TimerALatch = (TimerALatch & 0xFF00) | wert;
                break;
            }
            case 0x05:
            {
                TimerALatch = (TimerALatch & 0x00FF) | (wert << 8);
                IO[0x0D] &=0xBF;
                TimerA = TimerALatch;
                break;
            }
            case 0x06:
            {
                TimerALatch = (TimerALatch & 0xFF00) | wert;
                break;
            }
            case 0x07:
            {
                TimerALatch = (TimerALatch & 0x00FF) | (wert << 8);
                break;
            }
            case 0x08:
            {
                TimerBLatch = (TimerBLatch & 0xFF00) | wert;
                break;
            }
            case 0x09:
            {
                TimerBLatch = (TimerBLatch & 0x00FF) | (wert << 8);
                IO[0x0D] &=0xDF;
                TimerB = TimerBLatch;
                break;
            }
            case 0x0D:
            {
                IO[0x0D]&= ~wert;
                break;
            }
        }
        break;
    }
    case 1: // VIA 2 // Schreib/Lese Einheit FloppyDisk
    {
        switch (adresse & 0xF)
        {
            case 0x00:
            {
                if ((PB ^ wert) & 3)	// Bits 0/1: Stepper motor
                {
                    if ((PB & 3) == ((wert+1) & 3))
                    {
                        SpurDec();
                    }
                    else if ((PB & 3) == ((wert-1) & 3))
                    {
                        SpurInc();
                    }
                }
                PB = wert & 0xEF;
                *DiskMotorOn = !!(wert&4);
                break;
            }
            case 0x01:
            case 0x0F:
            {
                PA=wert;
                WriteGCRByte(PA);
                break;
            }
            case 0x04:
            {
                TimerALatch = (TimerALatch & 0xFF00) | wert;
                break;
            }
            case 0x05:
            {
                TimerALatch = (TimerALatch & 0x00FF) | (wert << 8);
                IO[0x0D] &=0xBF;
                TimerA = TimerALatch;
                break;
            }
            case 0x06:
            {
                TimerALatch = (TimerALatch & 0xFF00) | wert;
                break;
            }
            case 0x07:
            {
                TimerALatch = (TimerALatch & 0x00FF) | (wert << 8);
                break;
            }
            case 0x08:
            {
                TimerBLatch = (TimerBLatch & 0xFF00) |wert;
                break;
            }
            case 0x09:
            {
                TimerBLatch = (TimerBLatch & 0x00FF) | (wert << 8);
                IO[0x0D] &=0xDF;
                TimerB = TimerBLatch;
                break;
            }
            case 0x0D:
            {
                IO[0x0D]&= ~wert;
                break;
            }
        }
        break;
    }
    default:
        break;
    }
}

unsigned char MOS6522::ReadIO(unsigned short adresse)
{
    switch(VIANummer)
    {
        case 0: // VIA 1 // Komunitkation via IEC BUS
        {
            switch (adresse & 0xF)
            {
                case 0x00:
                {
                return (((PB & 0x1A)
                    | ((*FloppyIEC & *C64IEC) >> 7)             // DATA
                    | (((*FloppyIEC & *C64IEC) >> 4) & 0x04)	// CLK
                    | ((*C64IEC << 3) & 0x80)) ^ 0x85)          // ATN
                    | ((*Jumper<<5)&64)                         // Schalter S1 für Geräte Adresse
                    | ((*Jumper<<5)&32);                        // Schalter S2
                    break;
                }
                case 0x01:
                {
                    IO[0x0D]&=253;
                }
                case 15:
                {
                    return 0xFF;
                }

                case 0x02:
                {
                    return DDRB;
                }
                case 0x03:
                {
                    return DDRA;
                }
                case 0x04:
                {
                    IO[0x0D] &= 0xBF;
                    return (unsigned char)TimerA;
                }
                case 0x05:
                {
                    return TimerA>>8;
                }
                case 0x06:
                {
                    return (unsigned char)TimerALatch;
                }
                case 0x07:
                {
                    return TimerALatch>>8;
                }
                case 0x08:
                {
                    IO[0x0D] &= 0xDF;
                    return (unsigned char)TimerB;
                }
                case 0x09:
                {
                    return TimerB>>8;
                }
                case 0x0D:
                {
                    return IO[0x0D] | (IO[0x0D] & IO[0x0E] ? 0x80 : 0);
                }
            }
            return IO[adresse & 0xF];
            break;
        }

        case 1: // VIA 2 // Schreib/Lese Einheit FloppyDisk
        {
            switch (adresse & 0xF)
            {
                case 0x00:
                {
                    static unsigned char WP;
                    if (*WriteProtected) WP = 0;
                    else WP = 0x10;

                    if (SyncFound != 0)
                    {
                        if (SyncFound()) return (PB & 0x7F) | WP;
                        else return PB | 0x80 | WP;
                    }
                }
                case 0x01:
                case 0x0F:
                {
                    return ReadGCRByte();
                }
                case 0x04:
                {
                    IO[0x0D] &= 0xBF;
                    return (unsigned char)TimerA;
                }
                case 0x05:
                {
                    return TimerA>>8;
                }
                case 0x06:
                {
                    return (unsigned char)TimerALatch;
                }
                case 0x07:
                {
                    return TimerALatch>>8;
                }
                case 0x08:
                {
                    IO[0x0D] &= 0xDF;
                    return (unsigned char)TimerB;
                }
                case 0x09:
                {
                    return TimerB>>8;
                }
                case 0x0D:
                {
                    return IO[0x0D] | (IO[0x0D] & IO[0x0E] ? 0x80 : 0);
                }
                default:
                    return IO[adresse & 0xF];
                break;
            }
            break;
        }
        default:
            return 0;
            break;
    }
}
