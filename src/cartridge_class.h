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
// Letzte Änderung am 04.06.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef CRT_CLASS_H
#define CRT_CLASS_H

#include "structs.h"
#include "am29f040_class.h"
#include "mk7pla.h"

#include "tr1/functional"
using namespace std::tr1;
using namespace std::tr1::placeholders;

class CRTClass
{
public:
    CRTClass();
    ~CRTClass();

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
    unsigned char ReadIO1(uint16_t address);
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
    bool            *EXROM;
    bool            *GAME;

    unsigned char   *ROM_LO;		// 0x8000
    unsigned char   *ROM_HI;		// 0xA000 oder 0xE000

    unsigned char   CRT_ROM_BANK1[64*0x2000];		// Alle ChipRoms für 0x8000	(max 64 x 0x2000)
    unsigned char   CRT_ROM_BANK2[64*0x2000];		// Alle ChipRoms für 0xA000 oder 0xE000 (max 64 x 0x2000)

    unsigned char   CRT_ROM_BANK1_TMP[64*0x2000];	// Alle ChipRoms für 0x8000	(max 64 x 0x2000)
    unsigned char   CRT_ROM_BANK2_TMP[64*0x2000];	// Alle ChipRoms für 0xA000 oder 0xE000 (max 64 x 0x2000)

    unsigned char   *RAM_C64;	// Kompletter C64 RAM

    bool            LED_00;             // LED FCIII
    bool            LED_00_OLD;
    bool            LED_01;             // LED EF
    bool            LED_01_OLD;

    private:
    // Variablen

    bool            CRT_EXROM;
    bool            CRT_GAME;

    bool            CRTInsert;
    unsigned short  CRTTyp;

    //// EASY FLASH (32)
    bool            EasyFlashJumper;
    unsigned char   EasyFlashRam[256];		// Speziller Speicher für das EasyFlash Cartridge
    unsigned char   EasyFlashBankReg;		// Bank Register
    AM29F040Class   *am29f040Hi;
    AM29F040Class   *am29f040Lo;

    int             RomLBank;

    //// Action Replay 4/5/6 ////
    unsigned char   ARRegister;             // Inhalt des zuletzt geschriebenen $DExx Wert
    bool            ARFreez;
    bool            ActionReplayAktiv;
    bool            EnableActionReplayRam;
    unsigned char   ActionReplayRam[0x2000];    // 8KB

private:
    void ResetAllLEDS(void);
    void SetMemLogicAR(unsigned short adresse);

    unsigned char pla_adresse;
};

#endif // CRT_CLASS_H
