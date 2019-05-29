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
// Letzte Änderung am 29.05.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef AM29F040_CLASS_H
#define AM29F040_CLASS_H

#include <cstdint>

struct FLASH_TYPES_STRUCT
{
    uint8_t     ManufacturID;
    uint8_t     DeviceID;
    uint32_t    Size;
    uint32_t    Magic1Addr;
    uint32_t    Magic2Addr;
    uint32_t    Magic1Mask;
    uint32_t    Magic2Mask;
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

    uint8_t Read(uint32_t adresse);
    void Write(uint32_t adresse, uint8_t wert);

    uint8_t ProgrammByte;
    int	Dirty;

private:
        int	FlashType;
        int	Status;

        uint8_t* Data;
        uint8_t LastRead;

        int WriteOperationStatus(void);
        int FlashMagic1(uint32_t adresse);
        int FlashMagic2(uint32_t adresse);
        void FlashEraseSector(uint32_t adresse);
        void FlashEraseChip(void);
        int FlashProgramByte(uint32_t adresse, uint8_t wert);
};

#endif // AM29F040_CLASS_H
