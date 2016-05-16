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
// Letzte Änderung am 16.05.2016                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef MOS_6569_CLASS_H
#define MOS_6569_CLASS_H

#include "structs.h"
#include "tr1/functional"
using namespace std::tr1;
using namespace std::tr1::placeholders;

#define NEW

#ifdef NEW
/////////////////////////// NEUE VERSION ////////////////////////////////

#define MAX_RASTER_ZEILEN 312

#define MAX_XW 504
#define MAX_YW MAX_RASTER_ZEILEN
#define VIDEO_PUFFER_SIZE MAX_XW*MAX_YW*4

class VICII
{
public:
/// Funktionen ///
    VICII();
    ~VICII();
    void SwitchVideoPuffer();
    void GetRegister(VIC_STRUCT *vic_reg);
    void SetVicType(int system);
    void EnableSpriteCollision(bool enabled);
    void EnableGreyDot(bool enabled);
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
    bool            *RESET;
    unsigned char   *CIA2_PA;
    unsigned char   *CIA1_PB;
    unsigned char   *FarbRam;

    unsigned short  *Breakpoints;
    unsigned short  *BreakStatus;
    unsigned short  *BreakWerte;

    bool            VicConfig[4];

    unsigned char   *VideoPuffer;
    int             AktVideoPuffer;
    unsigned char   VideoPufferBack[2][VIDEO_PUFFER_SIZE];


private:
    void Reset(void);
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

    bool            GreyDotEnable;
    bool            isWriteColorReg20;
    bool            isWriteColorReg21;

    int             System;
    int             TOTAL_RASTERS;
    int             TOTAL_ZYKLEN_LINE;
    int             TOTAL_X;
    int             RASTER_Y;
    int             FIRST_DISP_LINE;
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
    bool            SpriteCollisionEnable;
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

#else

/////////////////////////// ALTE VERSION ////////////////////////////////
#define MAX_XW 504
#define MAX_YW 312
#define VIDEO_PUFFER_SIZE MAX_XW*MAX_YW

class VICII
{
public:
/// Funktionen ///
    VICII();
    ~VICII();
    void GetRegister(VIC_STRUCT *vic_reg);
    void SetVicType(int system);
    void EnableGreyDot(bool enabled);
    void OneZyklus(void);
    //bool SaveFreez(FILE* File);
    //bool LoadFreez(FILE *File,unsigned short Version);
    void WriteIO(unsigned short adresse,unsigned char wert);
    unsigned char ReadIO(unsigned short adresse);
    void TriggerLightpen(void);

    function<unsigned char(unsigned short)> *ReadProcTbl;
    function<void(unsigned char*)> RefreshProc;
    function<void(int)> CpuTriggerInterrupt;
    function<void(int)> CpuClearInterrupt;

/// Variablen ///

    bool            VicConfig[4];

    bool            *BA;
    bool            *RESET;
    unsigned char   *CIA2_PA;

    unsigned short  *Breakpoints;
    unsigned short  *BreakStatus;
    unsigned short  *BreakWerte;

    unsigned char   *FarbRam;
private:
/// Funktionen ///
    void MatrixAccess(void);
    void GraphicsAccess(void);
    void DrawBackground(void);
    void DrawGraphics(void);
    void DrawSprites(void);
    void RasterIRQ(void);
    void MemSet8(unsigned char* p, unsigned char c);
/// Variablen ///
    int			  AktVideoPuffer;
    unsigned char *VideoPuffer;
    unsigned char VideoPuffer0[VIDEO_PUFFER_SIZE];
    unsigned char VideoPuffer1[VIDEO_PUFFER_SIZE];

    unsigned char	*ChunkyLineStart;
    unsigned char   *ForeMaskPtr;
    unsigned char	*ChunkyPtr;

   /// VIC II Interne VARIABLEN ///
    int                 System;
    int                 TOTAL_RASTERS;
    int                 TOTAL_ZYKLEN_LINE;
    int                 TOTAL_X;

    int                 FIRST_DISP_LINE;

    unsigned char	IRQFlag;
    unsigned char	IRQMask;
    unsigned short	DYStart;
    unsigned short	DYStop;
    bool                BadLinesEnabled;
    bool                IsBadLine;
    bool                DisplayState;
    int                 DisplayIDX;
    unsigned char	MatrixLine[40];
    unsigned char	ColorLine[40];
    unsigned char	GfxData;
    unsigned char	CharData;
    unsigned char	ColorData;
    unsigned char	LastCharData;
    int				MLIndex;
    unsigned int	FirstBaCycle;
    unsigned char	SprExpY;
    unsigned char	RefCnt;
    unsigned short	SprPtr[8];
    unsigned char	SprData[8][4];
    unsigned char	SprDrawData[8][4];
    unsigned char	SprDmaOn;
    unsigned char	SprDispOn;
    unsigned char	SprDraw;
    unsigned short	Mc[80];
    unsigned short	McBase[80];
    unsigned char	ClxSpr;
    unsigned char	ClxBgr;
    unsigned short	MatrixBase;
    unsigned short	CharBase;
    unsigned short	BitmapBase;
    unsigned short	CiaVabase;
    unsigned short	VcBase;
    unsigned short	Vc;
    unsigned short	Rc;
    bool                DrawThisLine;
    bool                BorderOnSample[5];
    unsigned char       BorderColorSample[520/8];
    bool                BorderGreyDotSample[520/8];
    bool                BorderOn;
    bool                UdBorderOn;
    bool                LPTriggered;
    bool                LPTriggerOld;
    bool                VBlanking;
    int                 Zyklus;
    int                 XMod;
    unsigned char       SprCollBuf[520];
    unsigned char       ForeMaskBuf[520/8];

    bool                GreyDotEnable;
    bool                isWriteColorReg20;
    bool                isWriteColorReg21;

/// VIC IO Register ///
    unsigned short      MX[8];			// X-Koordinate Sprite
    unsigned char       MY[8];			// Y-Koordinate Sprite
    unsigned char       MX8;				// 8te BIT für X-Koordinate Sprite
    unsigned char       CTRL1;			// Steuerregister 1
    unsigned char       ME;				// Sprite Anschalten
    unsigned char       MXE;			// Sprite X Expansion
    unsigned char       MYE;			// Sprite Y Expansion
    unsigned char       MDP;			// Sprite-Daten Priorität
    unsigned char       MMC;			// Sprite Multicolor Modus
    unsigned char       LPX;			// Ligthpen X
    unsigned char       LPY;			// Lightpen Y
    unsigned char       CTRL2;			// Steuerregister 2
    unsigned short      Y_SCROLL;		// Y Scroll
    unsigned short      X_SCROLL;		// X Scroll
    unsigned char       VBASE;			// Video Base
    unsigned short      IRQ_RASTER;		// IRQ Raster
    unsigned short      RASTER_Y;
    unsigned short      RASTER_X;
    unsigned char       EC;
    unsigned char       B0C;
    unsigned char       B1C;
    unsigned char       B2C;
    unsigned char       B3C;
    unsigned char       MM0;
    unsigned char       MM1;
    unsigned char       SC[8];
};
#endif
#endif // MOS6569_CLASS_H
