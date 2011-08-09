//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: crt_class.h                            //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 09.07.2011		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#ifndef CRT_CLASS_H
#define CRT_CLASS_H

#include "structs.h"
#include "am29f040_class.h"

#include "tr1/functional"
using namespace std::tr1;
using namespace std::tr1::placeholders;

class CRTClass
{
public:
    CRTClass();
    ~CRTClass();

    // Funktionen
    int* GetFlash040Dirty(int nr);
    unsigned char* GetFlash040Byte(int nr);
    int LoadCRTImage(char* filename);
    void RemoveCRTImage(void);
    int NewEasyFlashImage(char* filename,char* crt_name);
    int WriteEasyFlashImage(char* filename);
    int GetCRTInfo(char* filename,CRT_INFO_STRUCT* crtinfo);
    void SetEasyFlashJumper(bool enabled);
    void Reset(void);
    void Freeze(void);
    unsigned long conv_dword(unsigned long wert);

    void WriteIO1(unsigned short adresse,unsigned char wert);
    unsigned char ReadIO1(unsigned short adresse);
    void WriteIO2(unsigned short adresse,unsigned char wert);
    unsigned char ReadIO2(unsigned short adresse);
    void WriteRom1(unsigned short adresse,unsigned char wert);
    void WriteRom2(unsigned short adresse,unsigned char wert);
    void WriteRom3(unsigned short adresse,unsigned char wert);
    unsigned char ReadRom1(unsigned short adresse);
    unsigned char ReadRom2(unsigned short adresse);
    unsigned char ReadRom3(unsigned short adresse);

    function<void(void)> ChangeMemMapProc;
    function<void(int)> CpuTriggerInterrupt;
    function<void(int)> CpuClearInterrupt;

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

    bool            LED_00;

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

    //// Action Replay 4 ////
    bool            ActionReplayAktiv;
};

#endif // CRT_CLASS_H
