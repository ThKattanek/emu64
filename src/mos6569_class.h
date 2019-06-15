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
// Letzte Änderung am 15.06.2019                //
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

#define MAX_RASTER_ROWS 312

#define MAX_XW 504
#define MAX_YW MAX_RASTER_ROWS
#define VIDEO_BUFFER_SIZE MAX_XW*MAX_YW*4

enum
{
        VIC_BORDER_ON,
        VIC_SPRITES_ON,
        VIC_SPR_SPR_COLL_ON,
        VIC_SPR_BCK_COLL_ON,
        VIC_GREY_DOTS_ON,
        VIC_CONFIG_NUM
};

class VICII
{
public:
/// Funktionen ///
    VICII();
    ~VICII();
    void SwitchVideoBuffer();
    void GetRegister(VIC_STRUCT *vic_reg);
    void SetVicVDisplayPalSize(uint16_t first_line, uint16_t last_line);

    uint16_t GetVicFirstDisplayLinePal();
    uint16_t GetVicLastDisplayLinePal();
    uint16_t GetVicFirstDisplayLineNtsc();
    uint16_t GetVicLastDisplayLineNtsc();

    uint16_t GetAktVicDisplayFirstLine();
    uint16_t GetAktVicDisplayLastLine();

    void SetVicVDisplayNtscSize(uint16_t first_line, uint16_t last_line);
    uint16_t GetVicDisplayNtscFirstLine();
    uint16_t GetVicDisplayNtscLastLine();
    void SetVicType(int system);
    void OneCycle();
    //bool SaveFreez(FILE* File);
    //bool LoadFreez(FILE *file,unsigned short version);
    void WriteIO(uint16_t address, uint16_t value);
    uint8_t ReadIO(uint16_t address);
    void TriggerLightpen();

    function<uint8_t(uint16_t)> *ReadProcTbl;
    function<void(uint8_t*)> RefreshProc;
    function<void(int)> CpuTriggerInterrupt;
    function<void(int)> CpuClearInterrupt;

    /// Variablen ///
    bool        *ba;
    uint8_t     *cia2_port_a;
    uint8_t     *cia1_port_b;
    uint8_t     *color_ram;

    uint16_t    *breakpoints;
    uint16_t    *break_status;
    uint16_t    *break_values;

    bool        vic_config[VIC_CONFIG_NUM];

    uint8_t     *video_buffer;
    uint8_t     video_buffer_back[2][VIDEO_BUFFER_SIZE];
    int         current_video_buffer;

private:
    void RasterIRQ();
    void SetBALow();
    void cAccess();
    void gAccess();
    void pAccess(uint8_t sp_nr);
    void sAccess(uint8_t sp_nr);
    void CheckBorder();
    void DrawGraphics();
    void DrawGraphicsPseudo();
    void DrawSprites();
    void DrawBorder();

    /// Variablen ///

    uint8_t *video_buffer_line;
    int     draw_line_counter;
    bool    draw_this_line;

    //// Interne Register ////
    uint16_t  reg_mx[8];
    uint8_t   reg_my[8];
    uint8_t   reg_mx8;
    uint8_t   reg_ctrl_1;
    uint8_t   reg_ctrl_2;
    uint16_t  reg_y_scroll;
    uint16_t  reg_x_scroll;
    uint8_t   VBASE;
    uint16_t  IRQ_RASTER;
    uint8_t   LPX;
    uint8_t   LPY;
    uint8_t   ME;
    uint8_t   MYE;
    uint8_t   MDP;
    uint8_t   MMC;
    uint8_t   MXE;
    uint8_t   MM;
    uint8_t   MD;
    uint8_t   EC;
    uint8_t   B0C;
    uint8_t   B1C;
    uint8_t   B2C;
    uint8_t   B3C;
    uint8_t   MM0;
    uint8_t   MM1;
    uint8_t   MCOLOR[8];

    uint8_t   AktZyklus;
    uint16_t  AktRZ;
    uint16_t  XKoordTbl[64];
    uint16_t  AktXKoordinate;
    uint8_t   GrafikMode;
    uint8_t   IRQFlag;
    uint8_t   IRQMask;

    bool            DEN;
    bool            VBlanking;
    uint8_t   FirstBAZyklus;
    bool            BadLineEnable;
    bool            BadLineStatus;
    bool            DisplayStatus;	// false = Idle / true = Display
    bool            LPTriggered;
    uint8_t   RC;
    uint16_t  VC;
    uint16_t  VCBASE;
    uint16_t  MatrixBase;
    uint16_t  CharBase;
    uint16_t  BitmapBase;
    uint8_t   VMLI;

    uint16_t  cAdresse;
    uint16_t  gAdresse;
    uint16_t  pAdresse;
    uint16_t  sAdresse;
    uint8_t   pWert[8];
    uint8_t   cDatenPuffer8Bit[64];
    uint8_t   cDatenPuffer4Bit[64];

    uint8_t   GfxData;
    uint8_t   CharData;
    uint8_t   ColorData;

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

    uint16_t  SpriteXDisplayTbl[0x200];
    uint8_t   SpriteExpYFlipFlop;
    uint8_t   MC[8];
    uint8_t   MCBase[8];
    uint8_t   SpriteDMA;
    uint8_t   SpriteView;
    uint8_t   SpriteViewAktLine;
    uint32_t   SpriteSeq[8];
    uint32_t   SpriteSeqAktLine[8];
    bool            SpriteSeqOn[8];
    uint8_t	SpriteCollisionsPuffer[MAX_XW + 4*24];

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
    unsigned char   *video_buffer_line_xscroll;
    unsigned char   *SpritePufferLine;	/// Erstes Sichtbares Pixel
    unsigned char   *BorderPufferLine;	/// Erstes Sichtbares Pixel
};

#endif // MOS6569_CLASS_H
