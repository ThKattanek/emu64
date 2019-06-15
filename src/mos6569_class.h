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
// Letzte Änderung am 16.06.2019                //
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

#define MAX_RASTERLINES 312

#define MAX_XW 504
#define MAX_YW MAX_RASTERLINES
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
    void SetVicType(uint8_t system);
    void OneCycle();
    //bool SaveFreez(FILE* File);
    //bool LoadFreez(FILE *file,unsigned short version);
    void WriteIO(uint16_t address, uint8_t value);
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
    uint16_t    reg_mx[8];
    uint8_t     reg_my[8];
    uint8_t     reg_mx8;
    uint8_t     reg_ctrl_1;
    uint8_t     reg_ctrl_2;
    uint16_t    reg_y_scroll;
    uint16_t    reg_x_scroll;
    uint8_t     reg_vbase;
    uint16_t    reg_irq_raster;
    uint8_t     reg_lpx;
    uint8_t     reg_lpy;
    uint8_t     reg_me;
    uint8_t     reg_mye;
    uint8_t     reg_mdp;
    uint8_t     reg_mmc;
    uint8_t     reg_mxe;
    uint8_t     reg_mm;
    uint8_t     reg_md;
    uint8_t     reg_ec;
    uint8_t     reg_b0c;
    uint8_t     reg_b1c;
    uint8_t     reg_b2c;
    uint8_t     reg_b3c;
    uint8_t     reg_mm0;
    uint8_t     reg_mm1;
    uint8_t     reg_mcolor[8];

    uint8_t     current_cycle;
    uint16_t    current_rasterline;
    uint16_t    new_irq_raster;
    uint16_t    x_coordinates_table[64];
    uint16_t    current_x_coordinate;
    uint8_t     graphic_mode;
    uint8_t     irq_flag;
    uint8_t     irq_mask;

    bool        den;
    bool        vertical_blanking;
    uint8_t     first_ba_cycle;
    bool        badline_enable;
    bool        badline_status;
    bool        display_status;	// false = Idle / true = Display
    bool        lp_triggered;
    uint8_t     rc;
    uint16_t    vc;
    uint16_t    vc_base;
    uint16_t    matrix_base;
    uint16_t    char_base;
    uint16_t    bitmap_base;
    uint8_t     vmli;

    uint16_t    c_address;
    uint16_t    g_address;
    uint16_t    p_address;
    uint16_t    s_address;
    uint8_t     p_data[8];
    uint8_t     c_data_buffer_8bit[64];
    uint8_t     c_data_buffer_4bit[64];

    uint8_t     gfx_data;
    uint8_t     char_data;
    uint8_t     color_data;

    bool        is_write_reg_ec;
    bool        is_write_reg_b0c;

    uint8_t     system;
    uint16_t    total_rasterlines;
    uint16_t    total_cycles_per_rasterline;
    uint16_t    total_x_coordinate;
    uint16_t    raster_y;
    uint16_t    first_display_line_pal;
    uint16_t    first_display_line_ntsc;
    uint16_t    first_display_line;
    uint16_t    last_display_line_pal;
    uint16_t    last_display_line_ntsc;
    uint16_t    last_display_line;
    bool        write_reg_0x11;

    //////////////////////////////// Sprites //////////////////////////////////

    uint16_t    sprite_x_display_table[0x200];
    uint8_t     sprite_y_exp_flip_flop;
    uint8_t     mc[8];
    uint8_t     mc_base[8];
    uint8_t     sprite_dma;
    uint8_t     sprite_view;
    uint8_t     sprite_view_current_line;
    uint32_t    sprite_sequenzer[8];
    uint32_t    sprite_sequencer_current_line[8];
    uint8_t     sprite_collision_buffer[MAX_XW + 4*24];

    ///////////////////////////////////////////////////////////////////////////

    ////////////////////////////// Rahmenstufe ////////////////////////////////

    uint8_t     csel;
    uint8_t     rsel;
    bool        border_flip_flop0;                // Haupt Rahmen FlipFlop
    bool        border_flip_flop1;                // Vertikal Rahmen FlipFlop
    uint16_t    h_border_compare_left[2];
    uint16_t    h_border_compare_right[2];
    uint16_t    v_border_compare_top[2];
    uint16_t    v_border_compare_bottom[2];
    uint8_t     border_line[63*8];               // Bit7 gesetzt kein Rahmenpixel
    uint16_t    border_line_pos;

    ///////////////////////////////////////////////////////////////////////////

    uint8_t     colors[4];
    uint8_t     *video_buffer_line_xscroll;
    uint8_t     *sprite_buffer_line;            /// Erstes Sichtbares Pixel
    uint8_t     *border_buffer_line;            /// Erstes Sichtbares Pixel
};

#endif // MOS6569_CLASS_H
