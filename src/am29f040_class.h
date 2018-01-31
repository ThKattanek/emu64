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
// Letzte Änderung am 18.05.2014                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef AM29F040_CLASS_H
#define AM29F040_CLASS_H

struct FLASH_TYPES_STRUCT
{
    unsigned char ManufacturID;
    unsigned char DeviceID;
    unsigned int  Size;
    unsigned int  Magic1Addr;
    unsigned int  Magic2Addr;
    unsigned int  Magic1Mask;
    unsigned int  Magic2Mask;
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
    AM29F040Class(unsigned char* flashbank,int flashtype);
    ~AM29F040Class();

    unsigned char Read(unsigned int adresse);
    void Write(unsigned int adresse, unsigned char wert);

    unsigned char ProgrammByte;
    int	Dirty;

private:
        int	FlashType;
        int	Status;

        unsigned char* Data;
        unsigned char LastRead;

        int WriteOperationStatus(void);
        int FlashMagic1(unsigned int adresse);
        int FlashMagic2(unsigned int adresse);
        void FlashEraseSector(unsigned int adresse);
        void FlashEraseChip(void);
        int FlashProgramByte(unsigned int adresse,unsigned char wert);
};

#endif // AM29F040_CLASS_H
