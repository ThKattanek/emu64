//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: structs.h                             //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 20.06.2023                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef STRUCTS_H
#define STRUCTS_H

#include <cstdint>

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
    FMV_RAM,
    FMV_VIA1,
    FMV_VIA2,
    FMV_DOS,
    FMV_OPEN
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

#define IntQuellenC64 10

enum
{
        VIC_IRQ,
        CIA_IRQ,
        CIA_NMI,
        REU_IRQ,
        CRT_NMI,
        CRT_IRQ,
        EXT_IRQ,
        EXT_NMI,
        RESTORE_NMI,
        NOP_INT
};

#define IntQuellenFloppy 2

enum
{
        VIA1_IRQ,
        VIA2_IRQ
};

typedef enum KeyStatus
{
    KEY_UP,
    KEY_DOWN,
}KeyStatus;

struct REG_STRUCT
{
    uint8_t     reg_mask;
    uint16_t    pc;
    uint8_t     ac;
    uint8_t     xr;
    uint8_t     yr;
    uint8_t     sp;
    uint8_t     sr;
    uint16_t    irq;
    uint16_t    nmi;
    uint16_t    _0314;
    uint16_t    _0318;
};

struct IREG_STRUCT
{
    uint8_t   pointer;
    uint16_t  address;
    uint16_t  branch_address;
    uint16_t  current_opcode_pc;
    uint16_t  current_opcode;
    uint8_t   current_micro_code;
    uint8_t   tmp_byte;
    bool      irq;
    bool      nmi;
    bool      rdy;
    bool      reset;
    bool      cpu_wait;
    bool      jam_flag;
    uint32_t  cycle_counter;
    bool      exrom;
    bool      game;
};

struct VIC_STRUCT
{
    uint16_t  current_rasterline;
    uint16_t  raster_latch;
    uint16_t  current_cycle;
    bool      irq;
    uint16_t  sprite_x[8];
    uint8_t   sprite_y[8];
    bool      display_status;
    uint8_t   graphic_mode;
    uint8_t   vic_bank;
    uint16_t  matrix_base;
    uint16_t  char_base;
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

struct BREAK_GROUP
{
    char            Name[128];
    boolean         Enable;
    boolean         bPC;
    uint16_t        iPC;
    boolean         bAC;
    uint16_t        iAC;
    boolean         bXR;
    uint16_t        iXR;
    boolean         bYR;
    uint16_t        iYR;
    boolean         bRAddress;
    uint16_t        iRAddress;
    uint16_t        iRAddressCount;
    boolean         bWAddress;
    uint16_t        iWAddress;
    uint16_t        iWAddressCount;
    boolean         bRWert;
    uint16_t        iRWert;
    boolean         bWWert;
    uint16_t        iWWert;
    boolean         bRZ;
    uint16_t        iRZ;
    boolean         bRZZyklus;
    uint16_t        iRZZyklus;
    boolean         bTrue;
};

struct FLOPPY_INFO
{
    bool            Motor;
    bool            Data;
	float			Data_RMS;
    unsigned char   Spur;
    unsigned char   Sektor;
	bool			ErrorFlag;
	char	    	ErrorMsg[36];
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
    uint8_t* BufferPointer;
};

struct CARTRIDGE_INFO_STRUCT
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

enum
{
    VJOY_TYPE_UNDEF,
    VJOY_TYPE_KEY,
    VJOY_TYPE_BUTTON,
    VJOY_TYPE_HAT,
    VJOY_TYPE_AXIS
};

struct VIRTUAL_JOY_STRUCT
{
    char            Name[256];
    unsigned char   Type[5];                // [0]=Keyboard [1]=JOYBUTTON [2]=JOYHAT [3]=JOYAXIS
    unsigned char   JoyIndex[5];

    // VJOY_TYPE_KEY
    unsigned char   KeyDown[5];             // [0]=Hoch [1]=Runter [2]=Links [3]=Rechts [4]=Feuer
    unsigned char   KeyUp[5];               // [0]=Hoch [1]=Runter [2]=Links [3]=Rechts [4]=Feuer

    // VJOY_TYPE_BUTTON
    unsigned char ButtonNr[5];

    // VJOY_TYPE_HAT
    unsigned char HatNr[5];
    unsigned char HatValue[5];

    // VJOY_TYPE_AXIS
    unsigned char AxisNr[5];
    unsigned char AxisValue[5];             // 0 = Plus Richtung // 1 = Minus Richtung
};

struct POINT_STRUCT
{
    float x;
    float y;
};

struct C64_KEYS
{
    unsigned char MatrixCode;
    int SDLKeyCode;
    bool          Shift;
};

struct VERSION_NUMBER
{
    int major;
    int minor;
    int micro;
    int build;
};

#endif // STRUCTS_H
