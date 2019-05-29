//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: am29f040_class.cpp                    //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 29.05.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "am29f040_class.h"
#include <cstring>

AM29F040Class::AM29F040Class(uint8_t *flashbank, int flashtype)
{
    Data = flashbank;
    FlashType = flashtype;
    Status = STATUS_READ;
    Dirty = 0;
}

AM29F040Class::~AM29F040Class(void)
{
}

uint8_t AM29F040Class::Read(uint32_t adresse)
{
    static uint8_t wert;

        switch (Status)
        {
        case STATUS_AUTOSELECT:
            switch (adresse & 0xFF)
                        {
                case 0x00:
                    wert = FlashTypes[FlashType].ManufacturID;
                    break;
                case 0x01:
                    wert = FlashTypes[FlashType].DeviceID;
                    break;
                case 0x02:
                    wert = 0;
                    break;
                default:
                    wert = Data[adresse];
                    break;
            }
            break;

        case STATUS_BYTE_PROGRAMM_ERROR:
            wert = static_cast<uint8_t>(WriteOperationStatus());
            break;

        case STATUS_SECTOR_ERASE_SUSPEND:
        case STATUS_BYTE_PROGRAMM_HAPPENING:
        case STATUS_CHIP_ERASE:
        case STATUS_SECTOR_ERASE:
            /* TODO return write operation status. Until alarms are introduced, just reset the state and fall through */
            Status = STATUS_READ;

        [[clang::fallthrough]]; default:
            /* The state doesn't reset if a read occurs during a command sequence */
            /* fall through */
        case STATUS_READ:
            wert = Data[adresse];
            break;
    }

    LastRead = wert;
    return wert;
}

void AM29F040Class::Write(uint32_t adresse, uint8_t wert)
{
    switch (Status)
        {
        case STATUS_READ:
            if (FlashMagic1(adresse) && (wert == 0xAA))
                        {
                Status = STATUS_MAGIC1;
            }
            break;

        case STATUS_MAGIC1:
            if (FlashMagic2(adresse) && (wert == 0x55))
                        {
                Status = STATUS_MAGIC2;
            }
                        else
                        {
                Status = STATUS_READ;
            }
            break;

        case STATUS_MAGIC2:
            if (FlashMagic1(adresse))
                        {
                switch (wert)
                                {
                    case 0x90:
                        Status = STATUS_AUTOSELECT;
                        break;
                    case 0xa0:
                        Status = STATUS_BYTE_PROGRAMM;
                        break;
                    case 0x80:
                        Status = STATUS_ERASE_MAGIC1;
                        break;
                    default:
                        Status = STATUS_READ;
                        break;
                }
            }
                        else
                        {
                Status = STATUS_READ;
            }
            break;

        case STATUS_BYTE_PROGRAMM:
            /* TODO setup alarm to not have the operation complete instantly */
            if (FlashProgramByte(adresse, wert))
                        {
                Status = STATUS_BYTE_PROGRAMM_HAPPENING;
            } else {
                Status = STATUS_BYTE_PROGRAMM_ERROR;
            }
            break;

        case STATUS_ERASE_MAGIC1:
            if (FlashMagic1(adresse) && (wert == 0xAA))
                        {
                Status = STATUS_ERASE_MAGIC2;
            } else
                        {
                Status = STATUS_READ;
            }
            break;

        case STATUS_ERASE_MAGIC2:
            if (FlashMagic2(adresse) && (wert == 0x55))
                        {
                Status = STATUS_ERASE_SELECT;
            } else
                        {
                Status = STATUS_READ;
            }
            break;

        case STATUS_ERASE_SELECT:
            if (FlashMagic1(adresse) && (wert == 0x10))
                        {
                /* TODO setup alarm to not have the operation complete instantly */
                FlashEraseChip();
                Status = STATUS_CHIP_ERASE;
            }
                        else if (wert == 0x30)
                        {
                /* TODO setup alarm to not have the operation complete instantly */
                FlashEraseSector(adresse);
                Status = STATUS_SECTOR_ERASE;
            } else
                        {
                Status = STATUS_READ;
            }
            break;

        case STATUS_SECTOR_ERASE:
            if (wert == 0xB0)
                        {
                Status = STATUS_SECTOR_ERASE_SUSPEND;
            }
            break;

        case STATUS_SECTOR_ERASE_SUSPEND:
            if (wert == 0x30)
                        {
                Status = STATUS_SECTOR_ERASE;
            }
            break;

        case STATUS_BYTE_PROGRAMM_ERROR:
        case STATUS_AUTOSELECT:
            if (wert == 0xF0)
                        {
                Status = STATUS_READ;
            }
            break;

        case STATUS_BYTE_PROGRAMM_HAPPENING:
        case STATUS_CHIP_ERASE:
        default:
            break;
    }
}

inline int AM29F040Class::FlashMagic1(uint32_t adresse)
{
    return ((adresse & FlashTypes[FlashType].Magic1Mask) == FlashTypes[FlashType].Magic1Addr);
}

inline int AM29F040Class::FlashMagic2(uint32_t adresse)
{
    return ((adresse & FlashTypes[FlashType].Magic2Mask) == FlashTypes[FlashType].Magic2Addr);
}

inline void AM29F040Class::FlashEraseSector(uint32_t adresse)
{
    uint32_t sector_addr = adresse & 0xF0000;

    memset(&(Data[sector_addr]), 0xFF, 0x10000);
    Dirty = 1;
}

inline void AM29F040Class::FlashEraseChip(void)
{
    memset(Data, 0xFF, FlashTypes[FlashType].Size);
    Dirty = 1;
}

inline int AM29F040Class::FlashProgramByte(uint32_t adresse, uint8_t wert)
{
    uint8_t old_data = Data[adresse];
    uint8_t new_data = old_data & wert;

    ProgrammByte = wert;
    Data[adresse] = new_data;
    Dirty = 1;

    return (new_data == wert) ? 1 : 0;
}

inline int AM29F040Class::WriteOperationStatus(void)
{
        static int maincpu_clk = 0;

    return ((ProgrammByte ^ 0x80) & 0x80)   /* DQ7 = inverse of programmed data */
         | ((maincpu_clk & 2) << 5)         /* DQ6 = toggle bit (2 us) */
         | (1 << 5)                         /* DQ5 = timeout */
         ;
}
