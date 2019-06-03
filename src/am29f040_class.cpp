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
// Letzte Änderung am 03.06.2019                //
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
    dirty = false;
}

AM29F040Class::~AM29F040Class(void)
{
}

uint8_t AM29F040Class::Read(uint32_t address)
{
    static uint8_t wert;

        switch (status)
        {
        case STATUS_AUTOSELECT:
            switch (address & 0xFF)
                        {
                case 0x00:
                    wert = FlashTypes[flash_type].manufactur_id;
                    break;
                case 0x01:
                    wert = FlashTypes[flash_type].device_id;
                    break;
                case 0x02:
                    wert = 0;
                    break;
                default:
                    wert = data[address];
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
            wert = data[address];
            break;
    }

    last_read = wert;
    return wert;
}

void AM29F040Class::Write(uint32_t address, uint8_t value)
{
    switch (status)
        {
        case STATUS_READ:
            if (FlashMagic1(address) && (value == 0xAA))
                        {
                status = STATUS_MAGIC1;
            }
            break;

        case STATUS_MAGIC1:
            if (FlashMagic2(address) && (value == 0x55))
                        {
                status = STATUS_MAGIC2;
            }
                        else
                        {
                status = STATUS_READ;
            }
            break;

        case STATUS_MAGIC2:
            if (FlashMagic1(address))
                        {
                switch (value)
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
            if (FlashProgramByte(address, value))
                        {
                status = STATUS_BYTE_PROGRAMM_HAPPENING;
            } else {
                status = STATUS_BYTE_PROGRAMM_ERROR;
            }
            break;

        case STATUS_ERASE_MAGIC1:
            if (FlashMagic1(address) && (value == 0xAA))
                        {
                status = STATUS_ERASE_MAGIC2;
            } else
                        {
                status = STATUS_READ;
            }
            break;

        case STATUS_ERASE_MAGIC2:
            if (FlashMagic2(address) && (value == 0x55))
                        {
                status = STATUS_ERASE_SELECT;
            } else
                        {
                status = STATUS_READ;
            }
            break;

        case STATUS_ERASE_SELECT:
            if (FlashMagic1(address) && (value == 0x10))
                        {
                /* TODO setup alarm to not have the operation complete instantly */
                FlashEraseChip();
                status = STATUS_CHIP_ERASE;
            }
                        else if (value == 0x30)
                        {
                /* TODO setup alarm to not have the operation complete instantly */
                FlashEraseSector(address);
                status = STATUS_SECTOR_ERASE;
            } else
                        {
                status = STATUS_READ;
            }
            break;

        case STATUS_SECTOR_ERASE:
            if (value == 0xB0)
                        {
                status = STATUS_SECTOR_ERASE_SUSPEND;
            }
            break;

        case STATUS_SECTOR_ERASE_SUSPEND:
            if (value == 0x30)
                        {
                status = STATUS_SECTOR_ERASE;
            }
            break;

        case STATUS_BYTE_PROGRAMM_ERROR:
        case STATUS_AUTOSELECT:
            if (value == 0xF0)
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

inline int AM29F040Class::FlashMagic1(uint32_t address)
{
    return ((address & FlashTypes[flash_type].magic1_mask) == FlashTypes[flash_type].magic1_addr);
}

inline int AM29F040Class::FlashMagic2(uint32_t address)
{
    return ((address & FlashTypes[flash_type].magic2_mask) == FlashTypes[flash_type].magic2_addr);
}

inline void AM29F040Class::FlashEraseSector(uint32_t address)
{
    uint32_t sector_addr = address & 0xF0000;

    memset(&(data[sector_addr]), 0xFF, 0x10000);
    dirty = true;
}

inline void AM29F040Class::FlashEraseChip()
{
    memset(data, 0xFF, FlashTypes[flash_type].size);
    dirty = true;
}

inline int AM29F040Class::FlashProgramByte(uint32_t address, uint8_t value)
{
    uint8_t old_data = data[address];
    uint8_t new_data = old_data & value;

    programm_byte = value;
    data[address] = new_data;
    dirty = true;

    return (new_data == value) ? 1 : 0;
}

inline int AM29F040Class::WriteOperationStatus()
{
        static int maincpu_clk = 0;

    return ((programm_byte ^ 0x80) & 0x80)   /* DQ7 = inverse of programmed data */
         | ((maincpu_clk & 2) << 5)         /* DQ6 = toggle bit (2 us) */
         | (1 << 5)                         /* DQ5 = timeout */
         ;
}
