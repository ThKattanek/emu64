//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: structs.h                             //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 17.09.2011		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#ifndef STRUCTS_H
#define STRUCTS_H

#define	MAX_JOYS 10
#define MAX_JOYNAME_LENGTH 260

#define REG_MASK_ALL 255
#define REG_MASK_PC 1
#define REG_MASK_AC 2
#define REG_MASK_XR 4
#define REG_MASK_YR 8
#define REG_MASK_SP 16
#define REG_MASK_SR 32
#define REG_MASK_IRQ 64
#define REG_MASK_NMI 128

#define TAPE_KEY_STOP  1
#define TAPE_KEY_PLAY  2
#define TAPE_KEY_REW   4
#define TAPE_KEY_FFW   8
#define TAPE_KEY_REC   16
#define TAPE_KEY_PAUSE 32

typedef unsigned char boolean;

enum
{
    MV_RAM,
    MV_KERNAL_ROM,
    MV_BASIC_ROM,
    MV_VIC,
    MV_FARB_RAM,
    MV_SID,
    MV_CIA1,
    MV_CIA2,
    MV_IO1,
    MV_IO2,
    MV_CHAR_ROM,
    MV_CRT_1,
    MV_CRT_2,
    MV_CRT_3,
    MV_OPEN
};

enum
{
        TAPE_IS_STOP,
        TAPE_IS_PLAY,
        TAPE_IS_REW,
        TAPE_IS_FFW,
        TAPE_IS_REC,
        TAPE_IS_PAUSE
};

#define IntQuellenC64 6

enum
{
        VIC_IRQ,
        CIA_IRQ,
        CIA_NMI,
        REU_IRQ,
        CRT_NMI,
        CRT_IRQ
};

#define IntQuellenFloppy 2

enum
{
        VIA1_IRQ,
        VIA2_IRQ
};

#define VicConfigSizeof 4

enum
{
        VIC_BORDER_ON,
        VIC_SPRITES_ON,
        VIC_SPR_SPR_COLL_ON,
        VIC_SPR_BCK_COLL_ON
};

typedef enum KeyStatus
{
    KEY_UP,
    KEY_DOWN,
}KeyStatus;

struct REG_STRUCT
{
    unsigned char  REG_MASK;
    unsigned short PC;
    unsigned short AC;
    unsigned short XR;
    unsigned short YR;
    unsigned short SP;
    unsigned short SR;
    unsigned short IRQ;
    unsigned short NMI;
    unsigned short _0314;
    unsigned short _0318;
};

struct VIC_STRUCT
{
    unsigned short  AktRasterzeile;
    unsigned short  RasterLatch;
    unsigned short  AktZyklus;
    bool            IRQ;
    unsigned short  SpriteX[8];
    unsigned char   SpriteY[8];
    bool            DisplayStatus;
    unsigned char   GrafikMode;
    unsigned char   VicBank;
    unsigned short  MatrixBase;
    unsigned short  CharBase;
};

struct CIA_STRUCT
{
    unsigned short  TimerA;
    unsigned short  TimerALatch;
    unsigned short  TimerB;
    unsigned short  TimerBLatch;
    unsigned char   CtrlA;
    unsigned char   CtrlB;
    unsigned char   Tod10;
    unsigned char   TodSec;
    unsigned char   TodMin;
    unsigned char   TodHr;
    unsigned char   PRA;
    unsigned char   PRB;
    unsigned char   DDRA;
    unsigned char   DDRB;
    unsigned char   SDR;

    unsigned char   IntData;
    unsigned char   IntMask;
    bool            IRQ;
};

struct IREG_STRUCT
{
    unsigned char   Pointer;
    unsigned short  Adresse;
    unsigned short  BranchAdresse;
    unsigned short  AktOpcodePC;
    unsigned short  AktOpcode;
    unsigned char   AktMicroCode;
    unsigned char   TMPByte;
    bool            IRQ;
    bool            NMI;
    bool            BA;
    bool            RESET;
    bool            CpuWait;
    bool            JAMFlag;
    unsigned long   CycleCounter;
    bool            EXROM;
    bool            GAME;
};

struct BREAK_GROUP
{
    char            Name[128];
    boolean         Enable;
    boolean         bPC;
    unsigned short  iPC;
    boolean         bAC;
    unsigned short  iAC;
    boolean         bXR;
    unsigned short  iXR;
    boolean         bYR;
    unsigned short  iYR;
    boolean         bRAdresse;
    unsigned short  iRAdresse;
    boolean         bWAdresse;
    unsigned short  iWAdresse;
    boolean         bRWert;
    unsigned short  iRWert;
    boolean         bWWert;
    unsigned short  iWWert;
    boolean         bRZ;
    unsigned short  iRZ;
    boolean         bRZZyklus;
    unsigned short  iRZZyklus;
    boolean         bTrue;
};

struct FLOPPY_INFO
{
    bool            Motor;
    bool            Data;
    unsigned char   Spur;
    unsigned char   Sektor;
};

struct D64_FILES
{
    char            Name[17];
    unsigned char   Track;
    unsigned char   Sektor;
    unsigned short  Laenge;
    unsigned char   Typ;
    unsigned short  Adresse;
};

struct T64_FILES
{
    char Name[17];
    unsigned short  StartAdresse;
    unsigned short  EndAdresse;
    unsigned char   Typ;
};

struct PSID_STRUCT
{
    unsigned short int	Version;
    unsigned short int	DataOffset;
    unsigned short int	LoadAdress;
    unsigned short int	InitAdress;
    unsigned short int  PlayAdress;
    unsigned short int	Songs;
    unsigned short int	StartSong;
    unsigned long int	Speed;
    unsigned char       Name[32];
    unsigned char       Autor[32];
    unsigned char       Copyright[32];
    unsigned short int	DataSize;
};

struct IEC_STRUCT
{
    bool    ATN_OUT;
    bool    DATA_OUT;
    bool    CLOCK_OUT;
    bool    DATA_IN;
    bool    CLOCK_IN;
};

struct CHIP_INFO_STRUCT
{
    unsigned short Type;			// 0 = ROM 1 = RAM
    unsigned short BankLocation;	// $0000 Normal Cartridge
    unsigned short LoadAdress;
    unsigned short ChipSize;
    unsigned char* BufferPointer;
};

struct CRT_INFO_STRUCT
{
    char                Name[32];
    char                Version[10];
    char*               HardwareTypeString;
    unsigned short      HardwareType;
    unsigned char       EXROM;
    unsigned char       GAME;
    int                 ChipCount;
    CHIP_INFO_STRUCT    ChipInfo[128];
    CHIP_INFO_STRUCT    ChipInfoHi[128];
};

struct FREEZ_RETURN_STRUCT
{
    bool    ReuInstert;
};

struct CPU_TUNING_STRUCT
{
    int     InterruptCounterMin;
};

struct EMU_TUNING_STRUCT
{
    CPU_TUNING_STRUCT	cpu;
};

struct JOY_STRUCT
{
    int     Anzahl;
    char    Name[MAX_JOYS][MAX_JOYNAME_LENGTH];
};
#endif // STRUCTS_H
