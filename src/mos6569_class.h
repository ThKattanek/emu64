//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: mos6569_class.h                       //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 29.05.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef MOS_6569_CLASS_H
#define MOS_6569_CLASS_H

#include <cstdint>
#include "structs.h"
#include "tr1/functional"
using namespace std::tr1;
using namespace std::tr1::placeholders;

#define MAX_RASTER_ZEILEN 312

#define MAX_XW 504
#define MAX_YW MAX_RASTER_ZEILEN
#define VIDEO_PUFFER_SIZE MAX_XW*MAX_YW*4

#define VicConfigSizeof 5

enum
{
        VIC_BORDER_ON,
        VIC_SPRITES_ON,
        VIC_SPR_SPR_COLL_ON,
        VIC_SPR_BCK_COLL_ON,
        VIC_GREY_DOTS_ON
};

class VICII
{
public:
/// Funktionen ///
    VICII();
    ~VICII();
    void SwitchVideoPuffer();
    void GetRegister(VIC_STRUCT *vic_reg);
    void SetVicVDisplayPalSize(int first_line, int last_line);

    uint16_t GetVicFirstDisplayLinePal();
    uint16_t GetVicLastDisplayLinePal();
    uint16_t GetVicFirstDisplayLineNtsc();
    uint16_t GetVicLastDisplayLineNtsc();

    uint16_t GetAktVicDisplayFirstLine();
    uint16_t GetAktVicDisplayLastLine();

    void SetVicVDisplayNtscSize(int first_line, int last_line);
    uint16_t GetVicDisplayNtscFirstLine();
    uint16_t GetVicDisplayNtscLastLine();
    void SetVicType(int system);
    void OneZyklus(void);
    //bool SaveFreez(FILE* File);
    //bool LoadFreez(FILE *file,unsigned short version);
    void WriteIO(unsigned short adresse,unsigned char wert);
    unsigned char ReadIO(unsigned short adresse);
    void TriggerLightpen(void);

    function<unsigned char(unsigned short)> *ReadProcTbl;
    function<void(unsigned char*)> RefreshProc;
    function<void(int)> CpuTriggerInterrupt;
    function<void(int)> CpuClearInterrupt;

/// Variablen ///
    bool            *BA;
    unsigned char   *CIA2_PA;
    unsigned char   *CIA1_PB;
    unsigned char   *FarbRam;

    unsigned short  *Breakpoints;
    unsigned short  *BreakStatus;
    unsigned short  *BreakWerte;

    bool            VicConfig[VicConfigSizeof];

    unsigned char   *VideoPuffer;
    int             AktVideoPuffer;
    unsigned char   VideoPufferBack[2][VIDEO_PUFFER_SIZE];


private:
    void RasterIRQ(void);
    void SetBALow(void);
    void cZugriff(void);
    void gZugriff(void);
    void pZugriff(unsigned char sp_nr);
    void sZugriff(unsigned char sp_nr);
    void CheckBorder(void);
    void DrawGraphics(void);
    void DrawGraphicsPseudo(void);
    void DrawSprites();
    void DrawBorder(void);

    /// Variablen ///

    unsigned char   *VideoPufferLine;
    int             DrawLineCounter;
    bool            DrawThisLine;

    //// Interne Register ////
    unsigned short  MX[8];
    unsigned char   MY[8];
    unsigned char   MX8;
    unsigned char   CTRL1;
    unsigned char   CTRL2;
    unsigned short  Y_SCROLL;
    unsigned short  X_SCROLL;
    unsigned char   VBASE;
    unsigned short  IRQ_RASTER;
    unsigned char   LPX;
    unsigned char   LPY;
    unsigned char   ME;
    unsigned char   MYE;
    unsigned char   MDP;
    unsigned char   MMC;
    unsigned char   MXE;
    unsigned char   MM;
    unsigned char   MD;
    unsigned char   EC;
    unsigned char   B0C;
    unsigned char   B1C;
    unsigned char   B2C;
    unsigned char   B3C;
    unsigned char   MM0;
    unsigned char   MM1;
    unsigned char   MCOLOR[8];

    unsigned char   AktZyklus;
    unsigned short  AktRZ;
    unsigned short  XKoordTbl[64];
    unsigned short  AktXKoordinate;
    unsigned char   GrafikMode;
    unsigned char   IRQFlag;
    unsigned char   IRQMask;

    bool            DEN;
    bool            VBlanking;
    unsigned char   FirstBAZyklus;
    bool            BadLineEnable;
    bool            BadLineStatus;
    bool            DisplayStatus;	// false = Idle / true = Display
    bool            LPTriggered;
    unsigned char   RC;
    unsigned short  VC;
    unsigned short  VCBASE;
    unsigned short  MatrixBase;
    unsigned short  CharBase;
    unsigned short  BitmapBase;
    unsigned char   VMLI;

    unsigned short  cAdresse;
    unsigned short  gAdresse;
    unsigned short  pAdresse;
    unsigned short  sAdresse;
    unsigned char   pWert[8];
    unsigned char   cDatenPuffer8Bit[64];
    unsigned char   cDatenPuffer4Bit[64];

    unsigned char   GfxData;
    unsigned char   CharData;
    unsigned char   ColorData;

    bool            isWriteColorReg20;
    bool            isWriteColorReg21;

    int             System;
    int             TOTAL_RASTERS;
    int             TOTAL_ZYKLEN_LINE;
    int             TOTAL_X;
    int             RASTER_Y;
    int             FirstDisplayLinePAL;
    int             FirstDisplayLineNTSC;
    int             FirstDisplayLine;
    int             LastDisplayLinePAL;
    int             LastDisplayLineNTSC;
    int             LastDisplayLine;
    bool            Write_xd011;

    //////////////////////////////// Sprites //////////////////////////////////

    unsigned short  SpriteXDisplayTbl[0x200];
    unsigned char   SpriteExpYFlipFlop;
    unsigned char   MC[8];
    unsigned char   MCBase[8];
    unsigned char   SpriteDMA;
    unsigned char   SpriteView;
    unsigned char   SpriteViewAktLine;
    unsigned long   SpriteSeq[8];
    unsigned long   SpriteSeqAktLine[8];
    bool            SpriteSeqOn[8];
    unsigned char	SpriteCollisionsPuffer[MAX_XW + 4*24];

    ///////////////////////////////////////////////////////////////////////////

    ////////////////////////////// Rahmenstufe ////////////////////////////////

    unsigned char   CSEL;
    unsigned char   RSEL;
    int             BorderCmpYo;
    int             BorderCmpYu;
    bool            BorderFlipFlop0;	// Haupt Rahmen FlipFlop
    bool            BorderFlipFlop1;	// Vertikal Rahmen FlipFlop
    int             HoBorderCMP_L[2];
    int             HoBorderCMP_R[2];
    int             VeBorderCMP_O[2];
    int             VeBorderCMP_U[2];
    unsigned char   BorderLine[63*8]; // Bit7 gesetzt kein Rahmenpixel
    int             BorderLinePos;

    ///////////////////////////////////////////////////////////////////////////

    unsigned char   Colors[4];
    unsigned char   *VideoPufferLine_XScroll;
    unsigned char   *SpritePufferLine;	/// Erstes Sichtbares Pixel
    unsigned char   *BorderPufferLine;	/// Erstes Sichtbares Pixel
};

#endif // MOS6569_CLASS_H
