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
// Letzte Änderung am 02.06.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "am29f040_class.h"
#include <cstring>

AM29F040Class::AM29F040Class(uint8_t *flashbank, int flashtype)
{
    data = flashbank;
    flash_type = flashtype;
    status = STATUS_READ;
    dirty = 0;
}

AM29F040Class::~AM29F040Class(void)
{
}

uint8_t AM29F040Class::Read(uint32_t adresse)
{
    static uint8_t wert;

        switch (status)
        {
        case STATUS_AUTOSELECT:
            switch (adresse & 0xFF)
                        {
                case 0x00:
                    wert = FlashTypes[flash_type].ManufacturID;
                    break;
                case 0x01:
                    wert = FlashTypes[flash_type].DeviceID;
                    break;
                case 0x02:
                    wert = 0;
                    break;
                default:
                    wert = data[adresse];
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
            status = STATUS_READ;

        [[clang::fallthrough]]; default:
            /* The state doesn't reset if a read occurs during a command sequence */
            /* fall through */
        case STATUS_READ:
            wert = data[adresse];
            break;
    }

    last_read = wert;
    return wert;
}

void AM29F040Class::Write(uint32_t adresse, uint8_t wert)
{
    switch (status)
        {
        case STATUS_READ:
            if (FlashMagic1(adresse) && (wert == 0xAA))
                        {
                status = STATUS_MAGIC1;
            }
            break;

        case STATUS_MAGIC1:
            if (FlashMagic2(adresse) && (wert == 0x55))
                        {
                status = STATUS_MAGIC2;
            }
                        else
                        {
                status = STATUS_READ;
            }
            break;

        case STATUS_MAGIC2:
            if (FlashMagic1(adresse))
                        {
                switch (wert)
                                {
                    case 0x90:
                        status = STATUS_AUTOSELECT;
                        break;
                    case 0xa0:
                        status = STATUS_BYTE_PROGRAMM;
                        break;
                    case 0x80:
                        status = STATUS_ERASE_MAGIC1;
                        break;
                    default:
                        status = STATUS_READ;
                        break;
                }
            }
                        else
                        {
                status = STATUS_READ;
            }
            break;

        case STATUS_BYTE_PROGRAMM:
            /* TODO setup alarm to not have the operation complete instantly */
            if (FlashProgramByte(adresse, wert))
                        {
                status = STATUS_BYTE_PROGRAMM_HAPPENING;
            } else {
                status = STATUS_BYTE_PROGRAMM_ERROR;
            }
            break;

        case STATUS_ERASE_MAGIC1:
            if (FlashMagic1(adresse) && (wert == 0xAA))
                        {
                status = STATUS_ERASE_MAGIC2;
            } else
                        {
                status = STATUS_READ;
            }
            break;

        case STATUS_ERASE_MAGIC2:
            if (FlashMagic2(adresse) && (wert == 0x55))
                        {
                status = STATUS_ERASE_SELECT;
            } else
                        {
                status = STATUS_READ;
            }
            break;

        case STATUS_ERASE_SELECT:
            if (FlashMagic1(adresse) && (wert == 0x10))
                        {
                /* TODO setup alarm to not have the operation complete instantly */
                FlashEraseChip();
                status = STATUS_CHIP_ERASE;
            }
                        else if (wert == 0x30)
                        {
                /* TODO setup alarm to not have the operation complete instantly */
                FlashEraseSector(adresse);
                status = STATUS_SECTOR_ERASE;
            } else
                        {
                status = STATUS_READ;
            }
            break;

        case STATUS_SECTOR_ERASE:
            if (wert == 0xB0)
                        {
                status = STATUS_SECTOR_ERASE_SUSPEND;
            }
            break;

        case STATUS_SECTOR_ERASE_SUSPEND:
            if (wert == 0x30)
                        {
                status = STATUS_SECTOR_ERASE;
            }
            break;

        case STATUS_BYTE_PROGRAMM_ERROR:
        case STATUS_AUTOSELECT:
            if (wert == 0xF0)
                        {
                status = STATUS_READ;
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
    return ((adresse & FlashTypes[flash_type].Magic1Mask) == FlashTypes[flash_type].Magic1Addr);
}

inline int AM29F040Class::FlashMagic2(uint32_t adresse)
{
    return ((adresse & FlashTypes[flash_type].Magic2Mask) == FlashTypes[flash_type].Magic2Addr);
}

inline void AM29F040Class::FlashEraseSector(uint32_t adresse)
{
    uint32_t sector_addr = adresse & 0xF0000;

    memset(&(data[sector_addr]), 0xFF, 0x10000);
    dirty = 1;
}

inline void AM29F040Class::FlashEraseChip(void)
{
    memset(data, 0xFF, FlashTypes[flash_type].Size);
    dirty = 1;
}

inline int AM29F040Class::FlashProgramByte(uint32_t adresse, uint8_t wert)
{
    uint8_t old_data = data[adresse];
    uint8_t new_data = old_data & wert;

    programm_byte = wert;
    data[adresse] = new_data;
    dirty = 1;

    return (new_data == wert) ? 1 : 0;
}

inline int AM29F040Class::WriteOperationStatus()
{
        static int maincpu_clk = 0;

    return ((programm_byte ^ 0x80) & 0x80)   /* DQ7 = inverse of programmed data */
         | ((maincpu_clk & 2) << 5)         /* DQ6 = toggle bit (2 us) */
         | (1 << 5)                         /* DQ5 = timeout */
         ;
}
