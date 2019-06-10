//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: am29f040_class.h                      //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 10.06.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef AM29F040_CLASS_H
#define AM29F040_CLASS_H

#include <cstdint>

struct FLASH_TYPES_STRUCT
{
    uint8_t     manufactur_id;
    uint8_t     device_id;
    uint32_t    size;
    uint32_t    magic1_addr;
    uint32_t    magic2_addr;
    uint32_t    magic1_mask;
    uint32_t    magic2_mask;
};

const FLASH_TYPES_STRUCT FlashTypes[2] =
{
        { 0x01, 0xa4, 0x80000, 0x5555, 0x2aaa, 0x7fff, 0x7fff },
        { 0x01, 0xa4, 0x80000, 0x555,  0x2aa,  0x7ff,  0x7ff  }
};

class AM29F040Class
{
    enum FlashStatus {
        STATUS_READ,
        STATUS_MAGIC1,
        STATUS_MAGIC2,
        STATUS_AUTOSELECT,
        STATUS_BYTE_PROGRAMM,
        STATUS_BYTE_PROGRAMM_HAPPENING,
        STATUS_BYTE_PROGRAMM_ERROR,
        STATUS_ERASE_MAGIC1,
        STATUS_ERASE_MAGIC2,
        STATUS_ERASE_SELECT,
        STATUS_CHIP_ERASE,
        STATUS_SECTOR_ERASE,
        STATUS_SECTOR_ERASE_SUSPEND
    };

public:
    AM29F040Class(uint8_t* flashbank,int flashtype);
    ~AM29F040Class();

    uint8_t Read(uint32_t address);
    void Write(uint32_t address, uint8_t value);

    uint8_t programm_byte;
    bool	dirty;

private:
        int	flash_type;
        int	status;

        uint8_t* data;
        uint8_t last_read;

        int WriteOperationStatus();
        int FlashMagic1(uint32_t address);
        int FlashMagic2(uint32_t address);
        void FlashEraseSector(uint32_t address);
        void FlashEraseChip();
        int FlashProgramByte(uint32_t address, uint8_t value);
};

#endif // AM29F040_CLASS_H
