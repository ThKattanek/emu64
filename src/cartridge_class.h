//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: crt_class.h                           //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 05.06.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef CARTRIDGE_CLASS_H
#define CARTRIDGE_CLASS_H

#include "structs.h"
#include "am29f040_class.h"
#include "mk7pla.h"

#include "tr1/functional"
using namespace std::tr1;
using namespace std::tr1::placeholders;

class CartridgeClass
{
public:
    CartridgeClass();
    ~CartridgeClass();

    // Funktionen
    bool *GetFlash040Dirty(uint16_t nr);
    uint8_t* GetFlash040Byte(uint16_t nr);
    int LoadCRTImage(const char* filename);
    void RemoveCRTImage();
    int CreateNewEasyFlashImage(const char* filename, const char* crt_name);
    int WriteEasyFlashImage(const char* filename);
    int GetCRTInfo(const char* filename, CRT_INFO_STRUCT* crt_info);
    void SetEasyFlashJumper(bool enable);
    void Reset();
    void Freeze();
    uint16_t ConvertDWord(uint32_t value);

    void WriteIO1(uint16_t adresse, uint8_t value);
    uint8_t ReadIO1(uint16_t address);
    void WriteIO2(uint16_t address, uint8_t value);
    uint8_t ReadIO2(uint16_t address);
    void WriteRom1(uint16_t address, uint8_t value);
    void WriteRom2(uint16_t address, uint8_t value);
    void WriteRom3(uint16_t address, uint8_t value);
    uint8_t ReadRom1(uint16_t address);
    uint8_t ReadRom2(uint16_t address);
    uint8_t ReadRom3(uint16_t address);

    function<void(void)> ChangeMemMapProc;
    function<void(int)> CpuTriggerInterrupt;
    function<void(int)> CpuClearInterrupt;
    function<void(int,bool)> ChangeLED;

    // Variablen
    bool        *exrom;
    bool        *game;

    uint8_t     *lo_rom;                      // 0x8000
    uint8_t     *hi_rom;                      // 0xA000 oder 0xE000

    uint8_t     rom_bank1[64*0x2000];         // Alle ChipRoms für 0x8000	(max 64 x 0x2000)
    uint8_t     rom_bank2[64*0x2000];         // Alle ChipRoms für 0xA000 oder 0xE000 (max 64 x 0x2000)

    uint8_t     rom_bank1_tmp[64*0x2000];     // Alle ChipRoms für 0x8000	(max 64 x 0x2000)
    uint8_t     rom_bank2_tmp[64*0x2000];     // Alle ChipRoms für 0xA000 oder 0xE000 (max 64 x 0x2000)

    uint8_t     *RAM_C64;                     // Kompletter C64 RAM

    bool        led_00;                 // LED FCIII
    bool        led_00_old;
    bool        led_01;                 // LED EF
    bool        led_01_old;

    private:
    // Funktionen
    void ResetAllLEDS(void);
    void SetMemLogicAR(uint16_t address);

    // Variablen
    bool        cartridge_exrom;
    bool        cartridge_game;

    bool        cartridge_is_insert;
    uint16_t    cartridge_type;

    //// EASY FLASH (32)
    bool            easyflash_jumper;
    uint8_t         easyflash_ram[256];		// Speziller Speicher für das EasyFlash Cartridge
    uint8_t         easyflash_bank_reg;		// Bank Register
    AM29F040Class   *am29f040Hi;
    AM29F040Class   *am29f040Lo;

    uint32_t    rom_lo_bank;

    //// Action Replay 4/5/6 ////
    uint8_t     ar_reg;                     // Inhalt des zuletzt geschriebenen $DExx Wert
    bool        ar_freez;
    bool        ar_active;
    bool        ar_enable_ram;
    uint8_t     ar_ram[0x2000];             // 8KB
    uint8_t     pla_adresse;
};

#endif // CARTRIDGE_CLASS_H
