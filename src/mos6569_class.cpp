//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: mos6569_class.cpp                     //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "mos6569_class.h"
#include <stdio.h>
#include <string.h>

//PAL

#define Read(adresse) ReadProcTbl[(adresse)>>8](adresse)

///////////////////////////////////////////////////////////////////////////////////////////////////

// Rahmen Y Start und Stop
//RSEL=0
#define BORDER_RSEL0_YSTART 55
#define BORDER_RSEL0_YSTOP  247

// RSEL=1
#define BORDER_RSEL1_YSTART 51
#define BORDER_RSEL1_YSTOP  251

// Rahmen X Start und Stop
//CSEL=0
#define BORDER_CSEL0_LEFT   18  // Eigtl XKoordinate 0x01f  (18)
#define BORDER_CSEL0_RIGHT  56  // Eigtl XKoordinate 0x14f  (56)

// CSEL=1
#define BORDER_CSEL1_LEFT   17  // Eigtl XKoordinate 0x018  (17)
#define BORDER_CSEL1_RIGHT  57  // Eigtl XKoordinate 0x158  (57)

// erste and letzte mögliche Rasterzeile für Bad Lines
#define FIRST_DMA_LINE		0x30
#define LAST_DMA_LINE		0xf7

// länge der X Werte der Destination BitMap
#define XMOD				504
#define YMOD                312

///////////////////////////////////////////////////////////////////////////////////////////////////

VICII::VICII()
{
    current_video_buffer = 0;
    video_buffer = video_buffer_back[0];

    for(int i=0; i<8; i++)
	{
        reg_mx[i] = 0;
        reg_my[i] = 0;
        reg_mcolor[i] = 0;
	}

    reg_mx8 = 0;
    reg_ctrl_1 = 0;
    reg_ctrl_2 = 0;
    reg_y_scroll = 0;
    reg_x_scroll = 0;
    reg_vbase = 0;
    reg_irq_raster = 0;
    reg_lpx = 0;
    reg_lpy = 0;
    reg_me = 0;
    reg_mye = 0;
    reg_mdp = 0;
    reg_mmc = 0;
    reg_mxe = 0;
    reg_mm = 0;
    reg_md = 0;
    reg_ec = 0;
    reg_b0c = 0;
    reg_b1c = 0;
    reg_b2c = 0;
    reg_b3c = 0;
    reg_mm0 = 0;
    reg_mm1 = 0;

    current_cycle = 1;
    current_rasterline = 0;
    graphic_mode = 0;
    irq_flag = 0;
    irq_mask = 0;

    // XKoordinaten in Tabelle schreiben
    uint16_t x_pos = 0x0194;
    for(int i=1; i<14; i++)
    {
        x_coordinates_table[i] = x_pos;
        x_pos += 8;
    }

    x_pos = 0x0004;
    for(int i=14; i<64; i++)
    {
        x_coordinates_table[i] = x_pos;
        x_pos += 8;
    }

	full_view = false;
	SetVicType(0);

    draw_line_counter = 0;
    lp_triggered = lp_line_low = lp_pending = draw_this_line = vertical_blanking = false;

    is_write_reg_ec = false;
    is_write_reg_b0c = false;

    badline_enable = false;
    badline_status = false;
    display_status = false;
    display_status_g = false;
    matrix_base = 0;
    char_base = 0;
    bitmap_base = 0;

    rc = 0;
    vc = 0;
    vc_base = 0;
    vmli = 0;

    c_address = 0;
    g_address = 0;

    for(int i=0; i<40; i++)
	{
            c_data_buffer_8bit[i] = 0;
            c_data_buffer_4bit[i] = 0;
	}

    for(int i=0; i<8; i++)
	{
        mc[i] = 0;
        mc_base[i] = 0;
	}

    sprite_y_exp_flip_flop = 0xFF;
    sprite_dma = 0;
    sprite_view = 0;

    for(int i=0; i<8; i++)
    {
        sprite_x_pipe[i] = 0;
        sbuf_reg[i] = 0;
        sbuf_pixel_reg[i] = 0;
        sprite_sequenzer[i] = 0;
    }
    sprite_pri_bits = sprite_mc_bits = sprite_expx_bits = 0;
    sprite_pending_bits = sprite_active_bits = sprite_halt_bits = 0;
    sbuf_expx_flops = sbuf_mc_flops = 0;

    for(int i=0; i<16; i++) creg[i] = 0;
    for(int c=0; c<66; c++)
    {
        for(int i=0; i<16; i++) creg_hist[c][i] = 0;
        creg_w_reg[c] = 0xFF;
    }
    creg_cur_reg = creg_pend_reg = 0xFF;
    creg_cur_val = creg_pend_val = 0;

    s_byte2_pending = 0;
    gfx_pipe0_g = gfx_pipe0_c = gfx_pipe0_v = 0;
    gfx_pipe1_g = gfx_pipe1_c = gfx_pipe1_v = 0;
    gfx_reg_g = gfx_reg_c = gfx_reg_v = 0;
    gfx_pixel_reg = 0;
    gfx_x_scroll = 0;
    gfx_vmode11 = gfx_vmode16 = gfx_vmode16_2 = 0;
    gfx_mc_flop = false;
    color_latency = true;
    for(int i=0; i<VIC_CONFIG_NUM; i++)
        vic_config[i] = true;
    vic_config[VIC_MODEL_8565] = false;
    reg11_delay = 0;

    write_reg_0x11 = false;

    first_display_line_pal = 16;                       //VICE - 16, Emu64 - 26
    last_display_line_pal = 288;                       //VICE - 288, Emu64 - 292

    first_display_line_ntsc = 30;
    last_display_line_ntsc = 288;

    first_display_line = first_display_line_pal;
    last_display_line = last_display_line_pal;

    h_border_compare_left[0]=BORDER_CSEL0_LEFT;
    h_border_compare_right[0]=BORDER_CSEL0_RIGHT;
    v_border_compare_top[0]=BORDER_RSEL0_YSTART;
    v_border_compare_bottom[0]=BORDER_RSEL0_YSTOP;

    h_border_compare_left[1]=BORDER_CSEL1_LEFT;
    h_border_compare_right[1]=BORDER_CSEL1_RIGHT;
    v_border_compare_top[1]=BORDER_RSEL1_YSTART;
    v_border_compare_bottom[1]=BORDER_RSEL1_YSTOP;
}

VICII::~VICII()
{
}

void VICII::SwitchVideoBuffer()
{
    current_video_buffer++;
    video_buffer = video_buffer_back[current_video_buffer & 1];
}

// Liefert für den Debugger das aktuelle Bild: Fertige Zeilen und der bisher gezeichnete Teil der aktuellen Zeile
// stammen aus dem Puffer im Aufbau, der Rest aus dem letzten fertigen Frame. Noch nicht aufgelöste Farbregister
// und der Rand der aktuellen Zeile werden wie in ResolveLine mit der Registerhistorie der bereits ausgeführten Zyklen aufgelöst.
// beam_row = Pufferzeile der aktuellen Rasterzeile (-1 wenn nicht sichtbar), beam_x = Anzahl bereits gezeichneter Pixel
void VICII::GetDebugFrame(uint8_t *dst, int *beam_row, int *beam_x)
{
    const uint8_t *cur = video_buffer;
    const uint8_t *prev = video_buffer_back[(current_video_buffer + 1) & 1];

    int row = -1;
    int x_end = 0;

    if(draw_this_line && draw_line_counter > 0)
    {
        row = draw_line_counter - 1;
        x_end = static_cast<int>(video_buffer_line - &video_buffer[row * MAX_XW]);
        if(x_end < 0) x_end = 0;
        if(x_end > MAX_XW) x_end = MAX_XW;
    }

    const int full_rows = (row >= 0) ? row : draw_line_counter;

    memcpy(dst, cur, static_cast<size_t>(full_rows) * MAX_XW);
    memcpy(dst + full_rows * MAX_XW, prev + full_rows * MAX_XW, VIDEO_BUFFER_SIZE - static_cast<size_t>(full_rows) * MAX_XW);

    if(row >= 0)
    {
        uint8_t *out = dst + row * MAX_XW;
        const uint8_t *in = cur + row * MAX_XW;
        const bool border = vic_config[VIC_BORDER_ON] && (current_rasterline >= first_display_line);

        // Registerzustand für Zyklen, die noch nicht ausgeführt wurden (inkl. Schreibzugriff des Vorzyklus)
        uint8_t last[16];
        for(int i=0; i<16; i++) last[i] = creg[i];
        if(creg_pend_reg != 0xFF) last[creg_pend_reg] = creg_pend_val;

        const int done_cycles = x_end >> 3;

        for(int x=0; x<x_end; x++)
        {
            uint8_t v = in[x];
            if(border && x < border_line_pos && !(border_line[x] & 128)) v = border_line[x];
            out[x] = ResolveColorPixel(v, x, last, done_cycles);
        }
    }

    *beam_row = row;
    *beam_x = x_end;
}

void VICII::GetRegister(VIC_STRUCT *vic_reg)
{
    vic_reg->current_cycle = current_cycle;

    if(current_cycle == 1)
        vic_reg->current_rasterline = current_rasterline;
    else
        vic_reg->current_rasterline = current_rasterline;

    vic_reg->raster_latch = reg_irq_raster;
    vic_reg->display_status = display_status;
    vic_reg->graphic_mode = graphic_mode;
    vic_reg->vic_bank = *cia2_port_a & 3;
    vic_reg->matrix_base = matrix_base;
    vic_reg->char_base = char_base;
    vic_reg->irq = irq_flag & 0x80;

	for(int i=0;i<8;i++)
	{
        vic_reg->sprite_x[i] = reg_mx[i];
        vic_reg->sprite_y[i] = reg_my[i];
    }
}

void VICII::SetVicVDisplayPalSize(uint16_t first_line, uint16_t last_line)
{
    if(first_line < MAX_RASTERLINES && last_line < MAX_RASTERLINES && first_line <= last_line)
    {
        first_display_line_pal = first_line;
        last_display_line_pal = last_line;
        SetVicType(system);
    }
}

uint16_t VICII::GetVicFirstDisplayLinePal()
{
    return first_display_line_pal;
}

uint16_t VICII::GetVicLastDisplayLinePal()
{
    return last_display_line_pal;
}

uint16_t VICII::GetAktVicDisplayFirstLine()
{
    return first_display_line;
}

uint16_t VICII::GetAktVicDisplayLastLine()
{
    return last_display_line;
}

void VICII::SetVicVDisplayNtscSize(uint16_t first_line, uint16_t last_line)
{
    if(first_line < MAX_RASTERLINES && last_line < MAX_RASTERLINES && first_line <= last_line)
    {
        first_display_line_ntsc = first_line;
        last_display_line_ntsc = last_line;
        SetVicType(system);
    }
}

uint16_t VICII::GetVicFirstDisplayLineNtsc()
{
    return first_display_line_ntsc;
}

uint16_t VICII::GetVicLastDisplayLineNtsc()
{
    return last_display_line_ntsc;
}

void VICII::SetVicModel8565(bool enable)
{
    vic_config[VIC_MODEL_8565] = enable;
    color_latency = !enable;
}

void VICII::SetVicType(uint8_t system)
{
    this->system = system;

    switch(system)
	{
	case 0:
        total_rasterlines=312;
        total_cycles_per_rasterline=63;
        total_x_coordinate=504;
        raster_y = total_rasterlines - 1;
        first_display_line = first_display_line_pal;
        last_display_line = last_display_line_pal;
		break;
	case 1:
        total_rasterlines=262;
        total_cycles_per_rasterline=64;
        total_x_coordinate=512;
        raster_y = total_rasterlines - 1;
        first_display_line = first_display_line_ntsc;
        last_display_line = last_display_line_ntsc;
		break;
	case 2:
        total_rasterlines=263;
        total_cycles_per_rasterline=65;
        total_x_coordinate=520;
        raster_y = total_rasterlines - 1;
        first_display_line = first_display_line_ntsc;
        last_display_line = last_display_line_ntsc;
		break;
	}

    if(full_view) ApplyDisplayLines();
}

// Setzt die sichtbaren Rasterzeilen entsprechend dem Systemtyp, im Vollbild-Modus des Debuggers alle Zeilen
void VICII::ApplyDisplayLines()
{
    if(full_view)
    {
        first_display_line = 0;
        last_display_line = total_rasterlines;
        return;
    }

    if(system == 0)
    {
        first_display_line = first_display_line_pal;
        last_display_line = last_display_line_pal;
    }
    else
    {
        first_display_line = first_display_line_ntsc;
        last_display_line = last_display_line_ntsc;
    }
}

void VICII::SetFullView(bool status)
{
    full_view = status;
    ApplyDisplayLines();
}

inline void VICII::RasterIRQ()
{
    irq_flag |= 0x01;
    if (irq_mask & 0x01)
	{
        irq_flag |= 0x80;
		CpuTriggerInterrupt(VIC_IRQ);
	}
}

inline void VICII::SetBALow()
{
    if (*ba)
    {
        first_ba_cycle = current_cycle;
        *ba = false;
    }
}

inline void VICII::cAccess()
{
    if (*ba == false)
	{
		/// Grafikdaten ///
        if(current_cycle >= 15 && current_cycle <= 54 && display_status == true && badline_status == true)
        {
            if(current_cycle - first_ba_cycle < 3)
            {
                // FLI-Bug: Die CPU hat den Bus noch (BA ist erst weniger als 3 Zyklen low). Der VIC liest $ff
                // und als Farbe das untere Nibble des Bytes, das die CPU gerade am Bus hat.
                c_data_buffer_8bit[vmli] = 0xFF;
                c_data_buffer_4bit[vmli] = CpuBusByte() & 0x0F;
            }
            else
            {
                c_address = static_cast<uint16_t>((*cia2_port_a<<14)|matrix_base|(vc & 0x3FF));
                c_data_buffer_8bit[vmli] = Read(c_address);
                c_data_buffer_4bit[vmli] = color_ram[(vc & 0x3FF)] & 0x0F;
            }
        }
	}
}

inline void VICII::gAccess()
{
    if (display_status_g) // DisplayModus
	{
        // 8565: Der Videomodus für die Abrufe wirkt einen Zyklus später
        const uint8_t r11 = color_latency ? reg_ctrl_1 : reg11_delay;
        if (r11 & 0x20) g_address = static_cast<uint16_t>((*cia2_port_a<<14)|(bitmap_base)|((vc & 0x3FF)<<3)|rc);
        else g_address = static_cast<uint16_t>((*cia2_port_a<<14)|char_base|(c_data_buffer_8bit[vmli]<<3)|rc);
        if (r11 & 0x40) g_address &= 0xf9ff;

        switch(graphic_mode)
		{
		case 4:
            gfx_data = Read(g_address & 0xF9FF);
			break;
		default:
            gfx_data = Read(g_address);
			break;
		}

        char_data = c_data_buffer_8bit[vmli];
        color_data = c_data_buffer_4bit[vmli];

        vmli++;
        vc++;
	} 
    else    // IdleModus
	{
        g_address = (color_latency ? reg_ctrl_1 : reg11_delay) & 0x40 ? 0x39FF : 0x3FFF;
        g_address |=(*cia2_port_a<<14);
        gfx_data = Read(g_address);
        char_data = 0;
        color_data = 0;
	}

    last_read_gp_access = gfx_data;
}

inline void VICII::pAccess(uint8_t sp_nr)
{
    p_address = static_cast<uint16_t>((*cia2_port_a<<14)|(matrix_base)|0x3F8|sp_nr);
    p_data[sp_nr] = Read(p_address);

    last_read_gp_access = p_data[sp_nr];
}

inline void VICII::sAccess(uint8_t sp_nr)
{
    // Das 1. und 2. Byte werden im ersten Zyklus des Sprite-Zugriffs gelesen,
    // das 3. Byte erst im folgenden Zyklus (siehe OneCycle)
    // MC ist ein 6-Bit-Zaehler: Die Adressen der drei Bytes laufen innerhalb des 64-Byte-Blocks um
    uint16_t s_base = static_cast<uint16_t>((*cia2_port_a<<14)|(p_data[sp_nr]<<6));
    s_address = static_cast<uint16_t>(s_base | (mc[sp_nr] & 0x3F));
    sprite_sequenzer[sp_nr] = static_cast<uint32_t>((Read(s_address)) << 16);
    s_address = static_cast<uint16_t>(s_base | ((mc[sp_nr] + 1) & 0x3F));
    sprite_sequenzer[sp_nr] |= static_cast<uint32_t>(((Read(s_address)) << 8));
    s_address_byte2[sp_nr] = static_cast<uint16_t>(s_base | ((mc[sp_nr] + 2) & 0x3F));
    s_byte2_pending |= static_cast<uint8_t>(1 << sp_nr);
}

inline void VICII::CheckBorder()
{
	///////////////// Rahmen ///////////////
    if(border_flip_flop0)
	{
        if(csel==1 && current_cycle == BORDER_CSEL1_RIGHT && border_flip_flop1 == false)
        {
            border_line[border_line_pos++] = 128;
            border_line[border_line_pos++] = 128;
            border_line[border_line_pos++] = 128;
            border_line[border_line_pos++] = 128;
            border_line[border_line_pos++] = 128;
            border_line[border_line_pos++] = 128;
            border_line[border_line_pos++] = 128;
            border_line[border_line_pos++] = 128;
        }
        else if(csel==0 && current_cycle == BORDER_CSEL0_RIGHT && border_flip_flop1 == false)
        {
            border_line[border_line_pos++] = 128;
            border_line[border_line_pos++] = 128;
            border_line[border_line_pos++] = 128;
            border_line[border_line_pos++] = 128;
            border_line[border_line_pos++] = 128;
            border_line[border_line_pos++] = 128;
            border_line[border_line_pos++] = 128;
            border_line[border_line_pos++] = BORDER_COLOR_REG;
        }
        else
        {
            int i=8;
            do{
                border_line[border_line_pos++] = BORDER_COLOR_REG;
                i--;
            } while(i>0);

        }
	}
	else
	{
        if(csel==0 && current_cycle == BORDER_CSEL0_LEFT && border_flip_flop1 == true)
        {
            border_line[border_line_pos++] = BORDER_COLOR_REG;
            border_line[border_line_pos++] = BORDER_COLOR_REG;
            border_line[border_line_pos++] = BORDER_COLOR_REG;
            border_line[border_line_pos++] = BORDER_COLOR_REG;
            border_line[border_line_pos++] = BORDER_COLOR_REG;
            border_line[border_line_pos++] = BORDER_COLOR_REG;
            border_line[border_line_pos++] = 128;
            border_line[border_line_pos++] = 128;
        }
        else
        {
            int i=8;
            do{
                border_line[border_line_pos++] = 128;
                i--;
            } while(i>0);
        }
	}
}

// Farbquellen der Pixel je Videomodus (MCM=4, BMM=8, ECM=16) und Pixelwert, wie im VIC
enum { GFX_D021, GFX_D022, GFX_D023, GFX_CBUF, GFX_CBUF_MC, GFX_VBUF_L, GFX_VBUF_H, GFX_D02X_EXT, GFX_NONE };

static const uint8_t gfx_color_source[32] =
{
    GFX_D021, GFX_D021, GFX_CBUF, GFX_CBUF,                 // ECM=0 BMM=0 MCM=0
    GFX_D021, GFX_D022, GFX_D023, GFX_CBUF_MC,              // ECM=0 BMM=0 MCM=1
    GFX_VBUF_L, GFX_VBUF_L, GFX_VBUF_H, GFX_VBUF_H,         // ECM=0 BMM=1 MCM=0
    GFX_D021, GFX_VBUF_H, GFX_VBUF_L, GFX_CBUF,             // ECM=0 BMM=1 MCM=1
    GFX_D02X_EXT, GFX_D02X_EXT, GFX_CBUF, GFX_CBUF,         // ECM=1 BMM=0 MCM=0
    GFX_NONE, GFX_NONE, GFX_NONE, GFX_NONE,                 // ECM=1 BMM=0 MCM=1
    GFX_NONE, GFX_NONE, GFX_NONE, GFX_NONE,                 // ECM=1 BMM=1 MCM=0
    GFX_NONE, GFX_NONE, GFX_NONE, GFX_NONE                  // ECM=1 BMM=1 MCM=1
};

// Erzeugt die 8 Pixel eines Zyklus aus dem Schieberegister. Der Videomodus wird in der Mitte
// des Zyklus gelatcht (MCM bei Pixel 4, ECM/BMM je nach Chip bei Pixel 4 bzw. 6 bzw. am Ende).
// Danach rutschen die im Zyklus abgerufenen Daten in die Pipeline.
inline void VICII::RenderGfxCycle(uint8_t *out)
{
    for(int i=0;i<8;i++)
    {
        if(i == 4)
        {
            gfx_vmode16 = static_cast<uint8_t>((reg_ctrl_2 & 0x10) >> 2);
            if(color_latency) gfx_vmode11 |= static_cast<uint8_t>((reg_ctrl_1 & 0x60) >> 2);
        }
        else if(i == 6)
        {
            if(color_latency) gfx_vmode11 &= static_cast<uint8_t>((reg_ctrl_1 & 0x60) >> 2);
        }
        else if(i == 7)
        {
            if(gfx_vmode16 && !gfx_vmode16_2) gfx_mc_flop = false;
            gfx_vmode16_2 = gfx_vmode16;
        }

        // Neue Zellendaten werden beim X-Scroll Offset ins Schieberegister geladen
        if(i == gfx_x_scroll)
        {
            gfx_reg_v = gfx_pipe1_v;
            gfx_reg_c = gfx_pipe1_c;
            gfx_reg_g = gfx_pipe1_g;
            gfx_mc_flop = true;
        }

        // Pixelwert lesen
        const bool mc_cell = ((gfx_vmode11 & 0x08) != 0) || ((gfx_reg_c & 0x08) != 0);
        if(gfx_vmode16_2)
        {
            if(mc_cell)
            {
                if(gfx_mc_flop) gfx_pixel_reg = gfx_reg_g >> 6;
            }
            else gfx_pixel_reg = (gfx_reg_g & 0x80) ? 3 : 0;
        }
        else
        {
            if(mc_cell) gfx_pixel_reg = (gfx_reg_g & 0x80) ? 2 : 0;
            else gfx_pixel_reg = (gfx_reg_g & 0x80) ? 3 : 0;
        }

        const uint8_t px = gfx_pixel_reg;
        gfx_reg_g <<= 1;
        gfx_mc_flop = !gfx_mc_flop;

        // Farbe bestimmen. Die Hintergrundfarben sind Registerindizes und werden am Zeilenende aufgelöst, Vordergrund bekommt 0x80
        uint8_t color;
        switch(gfx_color_source[gfx_vmode11 | gfx_vmode16 | px])
        {
        case GFX_D021:      color = COLOR_REG(0x21);                break;
        case GFX_D022:      color = COLOR_REG(0x22);                break;
        case GFX_D023:      color = COLOR_REG(0x23);                break;
        case GFX_CBUF:      color = gfx_reg_c;                      break;
        case GFX_CBUF_MC:   color = gfx_reg_c & 0x07;               break;
        case GFX_VBUF_L:    color = gfx_reg_v & 0x0F;               break;
        case GFX_VBUF_H:    color = gfx_reg_v >> 4;                 break;
        case GFX_D02X_EXT:  color = static_cast<uint8_t>(COLOR_REG(0x21) + (gfx_reg_v >> 6)); break;
        default:            color = 0;                              break;
        }

        if(px & 0x02) color |= 0x80;
        out[i] = color;
    }

    if(!color_latency) gfx_vmode11 = static_cast<uint8_t>((reg_ctrl_1 & 0x60) >> 2);

    // Pipeline weiterschieben und die im Zyklus abgerufenen Daten übernehmen
    gfx_pipe1_v = gfx_pipe0_v;
    gfx_pipe1_c = gfx_pipe0_c;
    gfx_pipe1_g = gfx_pipe0_g;

    if((current_cycle >= 16) && (current_cycle <= 55))
    {
        gfx_pipe0_g = gfx_data;
        gfx_x_scroll = static_cast<uint8_t>(reg_x_scroll);

        if(display_status)
        {
            gfx_pipe0_v = char_data;
            gfx_pipe0_c = color_data;
        }
        else
        {
            gfx_pipe0_v = 0;
            gfx_pipe0_c = 0;
        }
    }
    else gfx_pipe0_g = 0;
}

// Farbregister $d020-$d02e: Der Schreibzugriff eines Zyklus wird erst im Folgezyklus übernommen (wie cregs im VICE).
// Der Zustand je Zyklus wird für die Auflösung der Pixel am Zeilenende gemerkt.
inline void VICII::UpdateColorRegs()
{
    if(creg_pend_reg != 0xFF) creg[creg_pend_reg] = creg_pend_val;

    if(current_cycle < 66)
    {
        for(int i=0; i<16; i++) creg_hist[current_cycle][i] = creg[i];
        creg_w_reg[current_cycle] = creg_cur_reg;
    }

    creg_pend_reg = creg_cur_reg;
    creg_pend_val = creg_cur_val;
    creg_cur_reg = 0xFF;
}

// Löst die Registerindizes der 504 Pixel einer Zeile in Farben auf.
// 6569: Pixel 1-7 sehen den Zustand des Folgezyklus, Pixel 0 den des eigenen Zyklus (1 Pixel Verzögerung).
// 8565: Alle Pixel sehen den Zustand des Folgezyklus. Pixel 0 wird bei Schreibzugriff auf das Register grau (Grey Dot).
void VICII::ResolveLine()
{
    uint8_t *line = video_buffer_line - 504;

    // Zustand nach dem letzten Zyklus der Zeile
    uint8_t last[16];
    for(int i=0; i<16; i++) last[i] = creg[i];
    if(creg_pend_reg != 0xFF) last[creg_pend_reg] = creg_pend_val;

    const int total = total_cycles_per_rasterline;

    for(int n=0; n<504; n++)
        line[n] = ResolveColorPixel(line[n], n, last, total);
}

// Löst ein Pixel auf. valid_cycles = Anzahl der Zyklen der Zeile, deren Registerzustand in creg_hist bereits vorliegt,
// last = Registerzustand für alle späteren Zyklen
uint8_t VICII::ResolveColorPixel(uint8_t v, int n, const uint8_t *last, int valid_cycles) const
{
    if(!(v & 0x40)) return v & 0x0F;

    const int e = (n >> 3) + 1;
    const int p = n & 7;
    const int k = v & 0x0F;
    const uint8_t *next = (e + 1 <= valid_cycles) ? creg_hist[e+1] : last;

    // Grey Dots gehören beim 8565 zum Chip, beim 6569 nur wenn sie im Setup eingeschaltet sind
    if(p == 0 && creg_w_reg[e] == k && (!color_latency || vic_config[VIC_GREY_DOTS_ON]))
        return 0x0F;
    if(color_latency)
        return (p == 0) ? creg_hist[e][k] : next[k];
    return next[k];
}

inline void VICII::DrawGraphics()
{
    uint8_t tmp[8];
    uint8_t *slot = draw_this_line ? video_buffer_line : tmp;

    RenderGfxCycle(slot);
    RenderSpriteCycle(slot);

    if (!draw_this_line) return;

    video_buffer_line += 8;

    ////////////////////////// Draw Border //////////////////////////
    CheckBorder();
}

inline void VICII::DrawGraphicsPseudo()
{
    // Auch in den Zyklen ohne Grafikabruf läuft das Schieberegister weiter (z.B. bei geöffnetem Rand)
    uint8_t tmp[8];
    uint8_t *slot = draw_this_line ? video_buffer_line : tmp;

    RenderGfxCycle(slot);
    RenderSpriteCycle(slot);

    /// Für Anzeige Aller Zyklen
    video_buffer_line += 8;

    // Rand auch in diesen Zyklen füllen (sichtbar im Gesamtbild des Debuggers): Hier gibt es keine Rand-Flanken,
    // der Rand ist an wenn das Haupt-Flipflop gesetzt ist
    if(draw_this_line && (border_line_pos + 8 <= static_cast<int>(sizeof(border_line))))
    {
        const uint8_t v = border_flip_flop0 ? BORDER_COLOR_REG : 128;
        for(int i=0; i<8; i++) border_line[border_line_pos + i] = v;
    }

    border_line_pos += 8;
    return;
}

// Zyklen in denen der Sprite-Pointer (Phi1) bzw. das 2./3. Byte (Phi1/Phi2) geladen wird (PAL)
static const uint8_t sprite_ptr_cycle[8] = {58, 60, 62, 1, 3, 5, 7, 9};
static const uint8_t sprite_dma12_cycle[8] = {59, 61, 63, 2, 4, 6, 8, 10};

// Sprite-Ausgabe wie im VICE (draw_sprites8): Pro Zyklus werden 8 Pixel erzeugt. Die Zyklusdaten (X-Position,
// DMA-Zyklen) sind wie bei der Grafik um einen Zyklus verzögert, das X-Register wird einen Zyklus verzögert
// übernommen. Sprite-Pixel tragen den Farbregisterindex und werden am Zeilenende aufgelöst.
inline void VICII::RenderSpriteCycle(uint8_t *slot)
{
    if(!vic_config[VIC_SPRITES_ON])
    {
        for(int s=0; s<8; s++) sprite_x_pipe[s] = reg_mx[s];
        return;
    }

    // Daten des Vorzyklus
    const int pc = (current_cycle == 1) ? total_cycles_per_rasterline : current_cycle - 1;
    // Die X-Position eines Zyklus ist wie im VICE auf ein Vielfaches von 8 ausgerichtet
    const int xpos = ((pc < 64) ? x_coordinates_table[pc] : (x_coordinates_table[63] + 8 * (pc - 63))) & 0x1F8;

    uint8_t dma_cycle_0 = 0;
    uint8_t dma_cycle_2 = 0;
    int dma2_sprite = 0;
    for(int s=0; s<8; s++)
    {
        if(pc == sprite_ptr_cycle[s]) dma_cycle_0 = static_cast<uint8_t>(1 << s);
        if(pc == sprite_dma12_cycle[s])
        {
            dma_cycle_2 = static_cast<uint8_t>(1 << s);
            dma2_sprite = s;
        }
    }
    const bool spr_en = (pc == 58);

    uint8_t candidate_bits = 0;
    for(int s=0; s<8; s++)
        if((xpos & 0x1F8) == (sprite_x_pipe[s] & 0x1F8)) candidate_bits |= static_cast<uint8_t>(1 << s);

    uint8_t sprite_collision = 0;
    uint8_t gfx_collision = 0;

    for(int k=0; k<8; k++)
    {
        if(k == 2) sprite_active_bits &= static_cast<uint8_t>(~dma_cycle_2);
        if(k == 3) sprite_halt_bits |= dma_cycle_0;
        if(k == 4)
        {
            if(spr_en) sprite_pending_bits = sprite_view;
            if(dma_cycle_2) sbuf_reg[dma2_sprite] = sprite_sequenzer[dma2_sprite];
        }
        if(k == 6)
        {
            if(!color_latency)
            {
                // 8565: MC-Bits werden bei Pixel 6 übernommen
                const uint8_t toggled = static_cast<uint8_t>(reg_mmc ^ sprite_mc_bits);
                sbuf_mc_flops ^= static_cast<uint8_t>(toggled & ~sbuf_expx_flops);
                sbuf_mc_flops |= static_cast<uint8_t>(toggled & ~sbuf_expx_flops & ~reg_mmc);
                sprite_mc_bits = reg_mmc;
            }
            sprite_pri_bits = reg_mdp;
            sprite_expx_bits = reg_mxe;
        }
        if(k == 7)
        {
            if(color_latency)
            {
                // 6569: MC-Bits werden bei Pixel 7 übernommen
                const uint8_t toggled = static_cast<uint8_t>(reg_mmc ^ sprite_mc_bits);
                sbuf_mc_flops &= static_cast<uint8_t>(~toggled);
                sprite_mc_bits = reg_mmc;
            }
            sprite_halt_bits &= static_cast<uint8_t>(~dma_cycle_2);
        }

        // Sprites starten wenn die X-Position erreicht ist
        if(candidate_bits && sprite_pending_bits)
        {
            for(int s=0; s<8; s++)
            {
                const uint8_t m = static_cast<uint8_t>(1 << s);
                if((candidate_bits & m) && (sprite_pending_bits & m) && !(sprite_active_bits & m) && !(sprite_halt_bits & m))
                {
                    if(xpos + k == sprite_x_pipe[s])
                    {
                        sbuf_expx_flops |= m;
                        sbuf_mc_flops |= m;
                        sprite_active_bits |= m;
                    }
                }
            }
        }

        // Pixel der aktiven Sprites erzeugen
        if(sprite_active_bits)
        {
            int active_sprite = -1;
            uint8_t collision_mask = 0;

            for(int s=7; s>=0; s--)
            {
                const uint8_t m = static_cast<uint8_t>(1 << s);
                if(!(sprite_active_bits & m)) continue;

                if(sbuf_reg[s] || sbuf_pixel_reg[s])
                {
                    if(!(sprite_halt_bits & m))
                    {
                        if(sbuf_expx_flops & m)
                        {
                            if(sprite_mc_bits & m)
                            {
                                if(sbuf_mc_flops & m) sbuf_pixel_reg[s] = static_cast<uint8_t>((sbuf_reg[s] >> 22) & 0x03);
                                sbuf_mc_flops ^= m;
                            }
                            else if((sbuf_mc_flops & m) || color_latency)
                                sbuf_pixel_reg[s] = static_cast<uint8_t>(((sbuf_reg[s] >> 23) & 0x01) << 1);
                            else
                                sbuf_mc_flops |= m;
                        }

                        // Sprite-Puffer schieben und Expansions-Flags behandeln
                        if(sbuf_expx_flops & m) sbuf_reg[s] <<= 1;
                        if(sprite_expx_bits & m) sbuf_expx_flops ^= m;
                        else sbuf_expx_flops |= m;
                    }

                    if(sbuf_pixel_reg[s])
                    {
                        active_sprite = s;
                        collision_mask |= m;
                    }
                }
                else sprite_active_bits &= static_cast<uint8_t>(~m);
            }

            if(collision_mask)
            {
                const bool pixel_pri = (slot[k] & 0x80) != 0;
                const bool spri = (sprite_pri_bits & (1 << active_sprite)) != 0;

                if(!(pixel_pri && spri))
                {
                    switch(sbuf_pixel_reg[active_sprite])
                    {
                    case 1: slot[k] = COLOR_REG(0x25);                                  break;
                    case 2: slot[k] = static_cast<uint8_t>(COLOR_REG(0x27) + active_sprite); break;
                    case 3: slot[k] = COLOR_REG(0x26);                                  break;
                    default: break;
                    }
                }

                // Treffer auf Vordergrundgrafik
                if(pixel_pri) gfx_collision |= collision_mask;
            }

            // Mehr als ein Sprite mit Pixel
            if(collision_mask & (collision_mask - 1)) sprite_collision |= collision_mask;
        }
    }

    for(int s=0; s<8; s++) sprite_x_pipe[s] = reg_mx[s];

    //////////////////////// Sprite Collision ///////////////////////////

    if(sprite_collision && vic_config[VIC_SPR_SPR_COLL_ON])
    {
        if(reg_mm == 0)
        {
            // IRQ kann ausgelöst werden
            reg_mm |= sprite_collision;
            irq_flag |= 0x04;
            if (irq_mask & 0x04)
            {
                irq_flag |= 0x80;
                CpuTriggerInterrupt(VIC_IRQ);
            }
        }
        else reg_mm |= sprite_collision;
    }

    if(gfx_collision && vic_config[VIC_SPR_BCK_COLL_ON])
    {
        if(reg_md == 0)
        {
            // IRQ kann ausgelöst werden
            reg_md |= gfx_collision;
            irq_flag |= 0x02;
            if (irq_mask & 0x02)
            {
                irq_flag |= 0x80;
                CpuTriggerInterrupt(VIC_IRQ);
            }
        }
        else reg_md |= gfx_collision;
    }
}

inline void VICII::DrawBorder()
{
    if(!vic_config[VIC_BORDER_ON]) return;
    if(current_rasterline < first_display_line) return;
    border_buffer_line = video_buffer_line - 504;	/// Erstes Sichtbares Pixel

    for(int x=0;x<504;x++)
	{
        if(border_line[x] & 128) border_buffer_line++;
        else *border_buffer_line++ = border_line[x];
	}
}

void VICII::OneCycle()
{
    uint8_t sprite_bit = 0x01;

    current_x_coordinate = x_coordinates_table[current_cycle];

    // Lightpen: Auslösung einen Takt nach dem Pegelwechsel (wie VICE)
    if(lp_pending)
    {
        lp_pending = false;
        TriggerLightpen(false);
    }

    // Rasterzeile für die Badline-Prüfung: In Zyklus 1 wird die Zeile erst weiter unten hochgezählt,
    // die Prüfung gehört aber schon zur neuen Zeile (wie VICE, dort zählt raster_line vor check_badline).
    const int badline_check_line = (current_cycle == 1) ? ((current_rasterline == total_rasterlines - 1) ? 0 : current_rasterline + 1) : current_rasterline;

    // Prüfen ob Badlines zugelassen sind
    if(badline_check_line == 0x30) badline_enable = den;

    // Der g-Abruf dieses Zyklus sieht den Display-Zustand noch vor der Badline-Prüfung (wie VICE, wichtig für VSP)
    display_status_g = display_status;

    // Prüfen auf Badline zustand
    if((badline_check_line>=0x30) && (badline_check_line<=0xF7) && (reg_y_scroll == (badline_check_line&7)) && (badline_enable == true))
    {
        badline_status = true;
        display_status = true;
    }
    else badline_status = false;

    // Farbregister: Schreibzugriff des Vorzyklus übernehmen und Zustand für die Pixelauflösung merken
    UpdateColorRegs();

    // 3. Byte der Sprite-Daten lesen
    if(s_byte2_pending)
    {
        for(int i=0; i<8; i++)
        {
            if(s_byte2_pending & (1 << i)) sprite_sequenzer[i] |= Read(s_address_byte2[i]);
        }
        s_byte2_pending = 0;
    }

    switch (current_cycle)
	{
    case 1:

        if (current_rasterline == (total_rasterlines-1))
		{
            vertical_blanking = true;
            current_rasterline = 0;
            RefreshProc(video_buffer);
            draw_line_counter = 0;
            vc_base = 0;
		}
		else 
		{

            current_rasterline++;

			// Prüfen auf Raster IRQ
            //if (current_rasterline == reg_irq_raster) RasterIRQ();
            draw_this_line = (current_rasterline >= first_display_line && current_rasterline <= last_display_line);
		}

        if(draw_this_line)
        {
            video_buffer_line = &video_buffer[draw_line_counter++*MAX_XW];		// Zeiger für Aktuelle Zeile setzen
            border_line_pos = 0;
        }

		// Sprite 2 //
        if((sprite_dma & 0x18) == 0) *ba = true;

		// Sprite 3 //
        pAccess(3);
        if(!*ba) sAccess(3);

        DrawGraphicsPseudo();
		break;
	
    case 2:
        if (vertical_blanking)
        {
            lp_triggered = vertical_blanking = false;

            // Liegt die Lightpen-Leitung beim Bildanfang noch auf Low, wird sofort erneut ausgelöst
            if(lp_line_low) TriggerLightpen(true);
            //if (reg_irq_raster == 0) RasterIRQ();
        }

        // Prüfen auf Raster IRQ
        if (current_rasterline == reg_irq_raster) RasterIRQ();

		/// Sprite 5 ///
        if(sprite_dma & 0x20)  SetBALow();

        DrawGraphicsPseudo();
		break;

	case 3:
		// Sprite 3 //
        if((sprite_dma & 0x30) == 0) *ba = true;

		// Sprite 4 //
        pAccess(4);
        if(!*ba) sAccess(4);

        DrawGraphicsPseudo();
		break;

	case 4:
		/// Sprite 6 ///
        if(sprite_dma & 0x40)  SetBALow();

        DrawGraphicsPseudo();
		break;

	case 5:
        // Sprite 4 //
        if((sprite_dma & 0x60) == 0) *ba = true;

		// Sprite 5 //
        pAccess(5);
        if(!*ba) sAccess(5);

        DrawGraphicsPseudo();
		break;

	case 6:
		/// Sprite 7 ///
        if(sprite_dma & 0x80)  SetBALow();

        DrawGraphicsPseudo();
		break;

	case 7:
        // Sprite 5 //
        if((sprite_dma & 0xC0) == 0) *ba = true;

		// Sprite 6 //
        pAccess(6);
        if(!*ba) sAccess(6);

        DrawGraphicsPseudo();
		break;

	case 8:
        DrawGraphicsPseudo();
		break;

	case 9:
		// Sprite 6 //
        if((sprite_dma & 0x80) == 0) *ba = true;

		// Sprite 7 //
        pAccess(7);
        if(!*ba) sAccess(7);

        vmli = 0;

        DrawGraphicsPseudo();
        break;

    case 10:
        vmli = 0;

        DrawGraphicsPseudo();
		break;

	case 11:
		// Sprite 7 //
        *ba = true;

        vc = vc_base;

        /// Sprite MC + 3 (Sprite-DMA ist abgeschlossen), MC ist ein 6-Bit-Zähler
        for(int i=0;i<8;i++,sprite_bit<<=1)
        {
            if(sprite_dma & sprite_bit) mc[i] = static_cast<uint8_t>((mc[i] + 3) & 0x3F);
        }

        DrawGraphics();
        break;

    case 12:
        vc = vc_base;

        if(badline_status) SetBALow();
        DrawGraphics();
        break;

    case 13:
        vc = vc_base;

        if(badline_status) SetBALow();
        DrawGraphics();
		break;

	case 14:
        if(badline_status) SetBALow();
        if((badline_status == true) && (display_status == true)) rc = 0;

        DrawGraphics();
		break;

    case 15:
        if (badline_status) SetBALow();
        DrawGraphics();
        break;

	case 16:
        /// Sprite ///
        // MCBASE wird mit MC geladen wenn das Expansions-Flipflop gesetzt ist.
        // Hier wirkt auch der Sprite-Crunch (siehe Schreibzugriff auf $d017).
        for(int i=0;i<8;i++,sprite_bit<<=1)
        {
            if((sprite_y_exp_flip_flop & sprite_bit) == sprite_bit)
            {
                mc_base[i] = mc[i];
                if(mc_base[i] == 63)
                {
                    sprite_dma  &= ~sprite_bit;
                    sprite_view &= ~sprite_bit;
                }
            }
        }

        gAccess();
        if (badline_status) SetBALow();

        DrawGraphics();
		break;

    case 17:
        gAccess();
        if (badline_status) SetBALow();

        DrawGraphics();
        break;

    case 18:
    case 19: case 20: case 21: case 22: case 23: case 24:
    case 25: case 26: case 27: case 28: case 29: case 30:
    case 31: case 32: case 33: case 34: case 35: case 36:
    case 37: case 38: case 39: case 40: case 41: case 42:
    case 43: case 44: case 45: case 46: case 47: case 48:
    case 49: case 50: case 51: case 52: case 53: case 54:

        gAccess();
        if (badline_status) SetBALow();

        DrawGraphics();
		break;

    case 55:
        *ba = true;

		/// Sprite ///
		// In der ersten Phase von Zyklus 55 wird für alle Sprites geprüft, 
		// ob das entsprechende MxE-Bit in Register $d015 gesetzt und die Y-Koordinate 
		// des Sprites (ungerade Register $d001-$d00f) gleich den unteren 8 Bits 
		// von RASTER ist. Ist dies der Fall und der DMA für das Sprite noch 
		// ausgeschaltet, wird der DMA angeschaltet, MCBASE gelöscht und, 
		// wenn das MxYE-Bit gesetzt ist, das Expansions-Flipflop gelöscht.
        sprite_bit = 0x01;
        for (int i=0; i<8; i++, sprite_bit<<=1)
		{
            if ((reg_me & sprite_bit) && ((current_rasterline & 0xff) == reg_my[i]))
			{
                if((sprite_dma & sprite_bit) == 0)
				{
                    sprite_dma |= sprite_bit;
                    mc_base[i] = 0;
                    sprite_y_exp_flip_flop |= sprite_bit;
				}
			}
		}

		/// Sprite 0 ///
        if(sprite_dma & 0x01)  SetBALow();

        gAccess();
        DrawGraphics();
		break;
	
	case 56:
		/// Sprite ///
		// In der ersten Phase von Zyklus 56 wird (wie in Zyklus 55) für alle Sprites geprüft, 
		// ob das entsprechende MxE-Bit in Register $d015 gesetzt und die Y-Koordinate 
		// des Sprites (ungerade Register $d001-$d00f) gleich den unteren 8 Bits 
		// von RASTER ist. Ist dies der Fall und der DMA für das Sprite noch 
		// ausgeschaltet, wird der DMA angeschaltet, MCBASE gelöscht und, 
		// wenn das MxYE-Bit gesetzt ist, das Expansions-Flipflop gelöscht.
        sprite_bit = 0x01;
        for (int i=0; i<8; i++, sprite_bit<<=1)
		{
            if ((reg_me & sprite_bit) && (current_rasterline & 0xff) == reg_my[i])
			{
                if((sprite_dma & sprite_bit) == 0)
				{
                    sprite_dma |= sprite_bit;
                    mc_base[i] = 0;
                    sprite_y_exp_flip_flop |= sprite_bit;
				}
			}
		}	

		// Danach wird das Expansions-Flipflop invertiert, wenn der DMA des Sprites
		// läuft und das MxYE-Bit gesetzt ist (wie bei VICE in Zyklus 56). Ein $d017-Schreibzugriff
		// in Zyklus 55 wirkt daher noch auf das Flipflop (Sprite-Crunch).
        sprite_bit = 0x01;
        for (int i=0; i<8; i++, sprite_bit<<=1) if ((sprite_dma & sprite_bit) && (reg_mye & sprite_bit)) sprite_y_exp_flip_flop ^= sprite_bit;

        DrawGraphics();
		break;
	
	case 57:
		/// Sprite 1 ///
        if(sprite_dma & 0x02)  SetBALow();

        DrawGraphics();
		break;

	case 58:
        if(display_status == false)
        {
            vc_base = vc;
        }
        else if(badline_status)
        {
            if(rc == 7) vc_base = vc;
            rc = (rc + 1) & 7;
        }
        else
        {
            if(rc == 7)
            {
                vc_base = vc;
                display_status = false;
            }
            if(rc < 7) rc = (rc + 1) & 7;
        }

        /// Sprite ///
		// In der ersten Phase von Zyklus 58 wird für jedes Sprite MC mit MCBASE
		// geladen (MCBASE->MC) und geprüft, ob der DMA für das Sprite angeschaltet
		// und die Y-Koordinate des Sprites gleich den unteren 8 Bits von RASTER ist.
		// Ist dies der Fall, wird die Darstellung des Sprites angeschaltet.
        for(int i=0;i<8;i++, sprite_bit<<=1)
		{
            mc[i] = mc_base[i];
            if(sprite_dma & sprite_bit)
            {
                if((reg_me & sprite_bit) && ((current_rasterline & 0xff) == reg_my[i])) sprite_view |= sprite_bit;
            }
            else sprite_view &= ~sprite_bit;
		}

		// Sprite 0 //
        pAccess(0);
        if(!*ba) sAccess(0);

        DrawGraphics();
		break;

	case 59:
		/// Sprite 2 ///
        if(sprite_dma & 0x04)  SetBALow();

        DrawGraphics();
		break;
	
	case 60:
		// Sprite 0 //
        if((sprite_dma & 0x06) == 0) *ba = true;

		// Sprite 1 //
        pAccess(1);
        if(!*ba) sAccess(1);

        DrawGraphics();
		break;

	case 61:
		/// Sprite 3 ///
        if(sprite_dma & 0x08)  SetBALow();

        DrawGraphics();
		break;

	case 62:
		// Sprite 1 //
        if((sprite_dma & 0x0C) == 0) *ba = true;

		// Sprite 2 //
        pAccess(2);
        if(!*ba) sAccess(2);

        DrawGraphicsPseudo();
		break;
	
	case 63:
		/// Rahmen ///
        if(rsel == 0)
		{
            if(current_rasterline == BORDER_RSEL0_YSTOP) border_flip_flop1 = true;
            if((current_rasterline == BORDER_RSEL0_YSTART) && (den == true)) border_flip_flop1 = false;
		}
		else
		{
            if(current_rasterline == BORDER_RSEL1_YSTOP) border_flip_flop1 = true;
            if((current_rasterline == BORDER_RSEL1_YSTART) && (den == true)) border_flip_flop1 = false;
		}

		/// Sprite 4 ///
        if(sprite_dma & 0x10)  SetBALow();

        DrawGraphicsPseudo();
        DrawBorder();
        if(draw_this_line) ResolveLine();

        if(total_cycles_per_rasterline == 63) current_cycle = 0;

        break;

    case 64:
        if(total_cycles_per_rasterline == 64) current_cycle = 0;

        DrawGraphicsPseudo();
        break;

    case 65:
        current_cycle = 0;

        DrawGraphicsPseudo();
        break;
	}

    cAccess();     // cZugriffe immer im 2.Teil eines Zyklus wenn BA Low ist

    // Rahmen
    if(current_cycle == h_border_compare_right[csel])
    {
        border_flip_flop0 = true;
    }

    if(current_cycle == h_border_compare_left[csel])
    {
        if(current_rasterline == v_border_compare_bottom[rsel]) border_flip_flop1 = true;
        if(current_rasterline == v_border_compare_top[rsel]  && (den == true)) border_flip_flop1 = false;
        if(border_flip_flop1 == false) border_flip_flop0 = false;
    }

    // Oberer Rahmen: Wie im VICE (check_vborder_top) wird in jedem Zyklus geprüft, nicht nur am Zeilenende.
    // Wird RSEL/DEN in der Startzeile erst im Laufe der Zeile umgeschaltet, öffnet der Rahmen trotzdem (#328).
    if(current_rasterline == v_border_compare_top[rsel] && (den == true)) border_flip_flop1 = false;

    // Ist ein Spritesequenzer aktiv ?
    // NEU
    //DrawSprites();

    is_write_reg_ec = false;
    is_write_reg_b0c = false;

    ///   Auf nächsten Zyklus stellen   ///
    /// Aktueller Zyklus ist nun zuende ///

    current_cycle++;

    if(current_cycle == 1)
    {
        if(breakpoints[current_rasterline+1] & 256)
        {
                *break_status |= 256;
                break_values[8] = current_rasterline+1;
        }
    }
    else
    {
        if(breakpoints[current_rasterline] & 256)
        {
                *break_status |= 256;
                break_values[8] = current_rasterline;
        }
    }

    if(breakpoints[current_cycle] & 512)
	{
        *break_status |= 512;
        break_values[9] = current_cycle;
    }

    reg11_delay = reg_ctrl_1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*
#define fw(var) fwrite(&var,1,sizeof(var),File)
#define fw1(var,size) fwrite(var,1,size,File)
bool VICII::SaveFreez(FILE* File)
{

	int Offset;
    Offset = video_buffer_line - video_buffer;
	fw(Offset);
	
    fw(draw_line_counter);
    fw(draw_this_line);
	fw(bitc);
    fw1(reg_mx,8);
    fw1(reg_my,8);
    fw(reg_mx8);
    fw(reg_ctrl_1);
    fw(reg_ctrl_2);
    fw(reg_y_scroll);
    fw(reg_x_scroll);
    fw(reg_vbase);
    fw(reg_irq_raster);
    fw(reg_lpx);
    fw(reg_lpy);
    fw(reg_me);
    fw(reg_mye);
    fw(reg_mdp);
    fw(reg_mmc);
    fw(reg_mxe);
	fw(MM);
    fw(reg_md);
	fw(EC);
    fw(reg_b0c);
    fw(reg_b1c);
    fw(reg_b2c);
    fw(reg_b3c);
    fw(reg_mm0);
    fw(reg_mm1);
    fw1(reg_mcolor,8);
    fw(current_cycle);
    fw(current_rasterline);
	fw(AktXKoordinate);
    fw(graphic_mode);
    fw(irq_flag);
    fw(irq_mask);
	fw(DEN);
	fw(VBlanking);
	fw(FirstBAZyklus);
    fw(badline_enable);
    fw(badline_status);
    fw(display_status);
    fw(lp_triggered);
	fw(RC);
    fw(vc);
    fw(vc_base);
    fw(matrix_base);
    fw(char_base);
    fw(bitmap_base);
    fw(vmli);
    fw(c_address);
    fw(g_address);
    fw1(c_value_buffer_8bit,64);
    fw1(c_value_buffer_4bit,64);
    fw(gfx_data);
    fw(char_data);
    fw(color_data);
	fw(GreyDotEnable);
    fw(is_write_reg_ec);
    fw(is_write_reg_b0c);

	//////////////////////////////// Sprites //////////////////////////////////

    fw(sprite_y_exp_flip_flop);
	fw1(MC,8);
    fw1(mc_base,8);
    fw(sprite_dma);
	fw(SpriteView);

	///////////////////////////////////////////////////////////////////////////

	////////////////////////////// Rahmenstufe ////////////////////////////////

    fw(csel);
    fw(rsel);
	fw(BorderCmpYo);
	fw(BorderCmpYu);
    fw(border_flip_flop0);
    fw(border_flip_flop1);
	fw(BorderCMP_Re);
	fw(BorderCMP_Li);
	fw(BorderCMP_Ob);
	fw(BorderCMP_Un);
	return true;
}

#define fr(var) fread(&var,1,sizeof(var),File)
#define fr1(var,size) fread(var,1,size,File)
bool VICII::LoadFreez(FILE *File,unsigned short Version)
{
	int Offset;

	switch(Version)
	{
	case 0x0100:
	case 0x0101:

		fr(Offset);
        video_buffer_line = Offset + video_buffer;
		
        fr(draw_line_counter);
        fr(draw_this_line);
		fr(bitc);
        fr1(reg_mx,8);
        fr1(reg_my,8);
        fr(reg_mx8);
        fr(reg_ctrl_1);
        fr(reg_ctrl_2);
        fr(reg_y_scroll);
        fr(reg_x_scroll);
        fr(reg_vbase);
        fr(reg_irq_raster);
        fr(reg_lpx);
        fr(reg_lpy);
        fr(reg_me);
        fr(reg_mye);
        fr(reg_mdp);
        fr(reg_mmc);
        fr(reg_mxe);
		fr(MM);
        fr(reg_md);
		fr(EC);
        fr(reg_b0c);
        fr(reg_b1c);
        fr(reg_b2c);
        fr(reg_b3c);
        fr(reg_mm0);
        fr(reg_mm1);
        fr1(reg_mcolor,8);
        fr(current_cycle);
        fr(current_rasterline);
		fr(AktXKoordinate);
        fr(graphic_mode);
        fr(irq_flag);
        fr(irq_mask);
		fr(DEN);
		fr(VBlanking);
		fr(FirstBAZyklus);
        fr(badline_enable);
        fr(badline_status);
        fr(display_status);
        fr(lp_triggered);
		fr(RC);
        fr(vc);
        fr(vc_base);
        fr(matrix_base);
        fr(char_base);
        fr(bitmap_base);
        fr(vmli);
        fr(c_address);
        fr(g_address);
        fr1(c_value_buffer_8bit,64);
        fr1(c_value_buffer_4bit,64);
        fr(gfx_data);
        fr(char_data);
        fr(color_data);
		fr(GreyDotEnable);
        fr(is_write_reg_ec);
        fr(is_write_reg_b0c);

		//////////////////////////////// Sprites //////////////////////////////////

        fr(sprite_y_exp_flip_flop);
		fr1(MC,8);
        fr1(mc_base,8);
        fr(sprite_dma);
		fr(SpriteView);

		///////////////////////////////////////////////////////////////////////////

		////////////////////////////// Rahmenstufe ////////////////////////////////

        fr(csel);
        fr(rsel);
		fr(BorderCmpYo);
		fr(BorderCmpYu);
        fr(border_flip_flop0);
        fr(border_flip_flop1);
		fr(BorderCMP_Re);
		fr(BorderCMP_Li);
		fr(BorderCMP_Ob);
		fr(BorderCMP_Un);
		break;
	}
	return true;
}
*/

void VICII::WriteIO(uint16_t address, uint8_t value)
{
    address &= 63;

    // Schreibzugriff auf ein Farbregister merken (wirkt erst im Folgezyklus)
    if(address >= 0x20 && address <= 0x2E)
    {
        creg_cur_reg = static_cast<uint8_t>(address - 0x20);
        creg_cur_val = value & 0x0F;
    }

    switch (address)
	{
		/// Sprite X Position
		case 0x00: case 0x02: case 0x04: case 0x06:
		case 0x08: case 0x0A: case 0x0C: case 0x0E:
            reg_mx[address >> 1] = (reg_mx[address >> 1] & 0xFF00) | value;
		break;

		/// Sprite Y Position
		case 0x01: case 0x03: case 0x05: case 0x07:
		case 0x09: case 0x0B: case 0x0D: case 0x0F:
            reg_my[address >> 1] = value;
		break;

		/// Sprite X Position MSB
		case 0x10:
			int i, j;
                        reg_mx8 = value;
			for (i=0, j=1; i<8; i++, j<<=1)
			{
                                if (reg_mx8 & j) reg_mx[i] |= 0x100;
                                else reg_mx[i] &= 0xFF;
			}
			break;

		// Control Register 1
		case 0x11:
                        reg_ctrl_1 = value;
                        reg_y_scroll = value & 7;

                        new_irq_raster = static_cast<uint16_t>((reg_irq_raster & 0xFF) | ((value & 0x80) << 1));
                        if (reg_irq_raster != new_irq_raster && current_rasterline == new_irq_raster) RasterIRQ();
                        reg_irq_raster = new_irq_raster;
			
                        graphic_mode = ((reg_ctrl_1 & 0x60) | (reg_ctrl_2 & 0x10)) >> 4;
		
			// Prüfen ob Badlines zugelassen sind
                        if ((current_rasterline == 0x30) && (value & 0x10)) badline_enable = true;

			// Prüfen auf Badline zustand
                        if((current_rasterline>=0x30) && (current_rasterline<=0xF7) && (reg_y_scroll == (current_rasterline&7)) && (badline_enable == true))
			{
                                badline_status = true;
			}
            else badline_status = false;

			// RSEL
            rsel = (value & 0x08)>>3;

			// DEN
            den = (value & 0x10)>>4;
            write_reg_0x11 = true;
			break;

		/// Rasterzähler
		case 0x12:
                        //reg_irq_raster = (reg_irq_raster & 0xFF00) | wert;

                        new_irq_raster = (reg_irq_raster & 0xFF00) | value;
                        if (reg_irq_raster != new_irq_raster && current_rasterline == new_irq_raster) RasterIRQ();
                        reg_irq_raster = new_irq_raster;
			break;

		/// Sprite Enable
		case 0x15:
            reg_me = value;
			break;

		// Control Register 2
		case 0x16:
            reg_ctrl_2 = value;
            reg_x_scroll = value & 7;
            graphic_mode = ((reg_ctrl_1 & 0x60) | (reg_ctrl_2 & 0x10)) >> 4;
			
            // csel
            csel = (value & 0x08)>>3;
			
			break;

		/// Sprite Y-Expansion
		case 0x17:
                        // Sprite-Crunch: Wird das Expansions-Flipflop in Zyklus 15 durch
                        // Löschen von MxYE gesetzt, wird MC verfälscht. Die CPU läuft vor dem VIC,
                        // der Schreibzugriff in Zyklus 15 sieht daher current_cycle == 16.
                        if(current_cycle == 16)
                        {
                            uint8_t bit = 0x01;
                            for(int sp=0;sp<8;sp++,bit<<=1)
                            {
                                if(!(value & bit) && !(sprite_y_exp_flip_flop & bit))
                                    mc[sp] = (0x2a & (mc_base[sp] & mc[sp])) | (0x15 & (mc_base[sp] | mc[sp]));
                            }
                        }
                        reg_mye = value;
                        sprite_y_exp_flip_flop |= ~value;
			break;

		/// Speicher Pointer
		case 0x18:
                        reg_vbase = value;
                        matrix_base = static_cast<uint16_t>((value & 0xf0) << 6);
                        char_base = static_cast<uint16_t>((value & 0x0e) << 10);
                        bitmap_base = static_cast<uint16_t>((value & 0x08) << 10);
			break;

		/// IRQ Flags
		case 0x19: 
                        irq_flag = irq_flag & (~value & 0x0F);
                        if (irq_flag & irq_mask) irq_flag |= 0x80;
                        else CpuClearInterrupt(VIC_IRQ);
			break;
		
		/// IRQ Mask
		case 0x1A:
                        irq_mask = value & 0x0F;
                        if (irq_flag & irq_mask)
			{
                                irq_flag |= 0x80;
                                CpuTriggerInterrupt(VIC_IRQ);
			} else {
                                irq_flag &= 0x7F;
                                CpuClearInterrupt(VIC_IRQ);
			}
			break;

		/// Sprite Daten Priorität
		case 0x1B:
                        reg_mdp = value;
			break;

		/// Sprite Multicolor
		case 0x1C:
                        reg_mmc = value;
			break;

		/// Sprite X-Expansion
		case 0x1D:
                        reg_mxe = value;
			break;

		/// Rahmenfarbe
		case 0x20:
                        reg_ec = value&15;
                        if(vic_config[VIC_GREY_DOTS_ON]) is_write_reg_ec = true;
			break;
		/// Hintergrundfarbe 0
        case 0x21:
                        reg_b0c = value&15;
                        if(vic_config[VIC_GREY_DOTS_ON]) is_write_reg_b0c = true;
			break;
		/// Hintergrundfarbe 1
                case 0x22: reg_b1c	= value&15;
                        //isWriteColorReg = true;
			break;
		/// Hintergrundfarbe 2
                case 0x23: reg_b2c	= value&15;
                        //isWriteColorReg = true;
			break;
		/// Hintergrundfarbe 3
                case 0x24: reg_b3c	= value&15;
                        //isWriteColorReg = true;
			break;
		/// Sprite Multicolor 0
                case 0x25: reg_mm0  = value&15;
                        //isWriteColorReg = true;
			break;
		/// Sprite Multicolor 1
                case 0x26: reg_mm1  = value&15;
                        //isWriteColorReg = true;
			break;

		/// Sprite Farbe
		case 0x27: case 0x28: case 0x29: case 0x2A:
		case 0x2B: case 0x2C: case 0x2D: case 0x2E:
                        reg_mcolor[address - 0x27] = value&15;
			break;
	}
}

uint8_t VICII::ReadIO(uint16_t address)
{
    uint8_t ret;
    address &= 63;

    switch (address)
	{
		/// Sprite X Position
		case 0x00: case 0x02: case 0x04: case 0x06:
		case 0x08: case 0x0A: case 0x0C: case 0x0E:
            return static_cast<uint8_t>(reg_mx[address >> 1]);

		/// Sprite Y Position
		case 0x01: case 0x03: case 0x05: case 0x07:
		case 0x09: case 0x0B: case 0x0D: case 0x0F:
            return reg_my[address >> 1];

		/// Sprite X Position MSB
        case 0x10:
            return reg_mx8;

		/// Control Register 1
		case 0x11:	
            if(write_reg_0x11) return (reg_ctrl_1 & 0x7F) | ((current_rasterline & 0x100) >> 1);
            else return 0;

		/// Rasterzähler
		case 0x12:
            return static_cast<uint8_t>(current_rasterline);

		/// Ligthpen X
        case 0x13:
            return reg_lpx;

		/// Ligthpen Y
		case 0x14:
            return reg_lpy;

		case 0x15:	// Sprite enable
            return reg_me;

		// Control Register 2
		case 0x16:
            return reg_ctrl_2 | 0xC0;

		/// Sprite Y-Expansion
		case 0x17:
            return reg_mye;

		/// Speicher Pointer
		case 0x18:
            return reg_vbase | 0x01;

		/// IRQ Flags
		case 0x19:
            return irq_flag | 0x70;

		/// IRQ Mask
		case 0x1A:
            return irq_mask | 0xF0;

		/// Sprite Daten Priorität
		case 0x1B:
            return reg_mdp;

		/// Sprite Multicolor
		case 0x1C:
            return reg_mmc;

		/// Sprite X-Expansion
		case 0x1D:
            return reg_mxe;

		/// Sprite - Sprite Kollision
		case 0x1E:
            ret = reg_mm;
            reg_mm = 0;
            return ret;

		/// Sprite - Daten Kollision
		case 0x1F:
            ret = reg_md;
            reg_md = 0;
            return ret;

        case 0x20: return reg_ec | 0xf0;
        case 0x21: return reg_b0c | 0xf0;
        case 0x22: return reg_b1c | 0xf0;
        case 0x23: return reg_b2c | 0xf0;
        case 0x24: return reg_b3c | 0xf0;
        case 0x25: return reg_mm0 | 0xf0;
        case 0x26: return reg_mm1 | 0xf0;

		case 0x27: case 0x28: case 0x29: case 0x2a:
		case 0x2b: case 0x2c: case 0x2d: case 0x2e:
            return reg_mcolor[address - 0x27] | 0xf0;

		default:
			return 0xFF;
	}
}

void VICII::SetLightpen(bool low)
{
    lp_line_low = low;

    // Nur die fallende Flanke löst aus. Das Latchen erfolgt einen Takt später.
    if(low) lp_pending = true;
}

void VICII::TriggerLightpen(bool retrigger)
{
    if(lp_triggered) return;
    lp_triggered = true;

    uint8_t x;
    uint8_t y;

    if(retrigger)
    {
        // Leitung liegt am Bildanfang noch auf Low
        x = (total_cycles_per_rasterline == 65) ? 0xd5 : 0xd1;
        y = 0;
    }
    else
    {
        // Im ersten Zyklus ist die Rasterzeile noch nicht weitergezählt
        const int line = (current_cycle == 1) ? ((current_rasterline + 1) % total_rasterlines) : current_rasterline;

        // In der letzten Zeile wird nur im ersten Zyklus ausgelöst
        if(line == total_rasterlines - 1 && current_cycle > 1) return;

        // X-Position auf 8 Pixel genau (wie VICE) plus Offset je nach VIC-Modell (6569: +2, 8565: +1)
        x = static_cast<uint8_t>(((current_x_coordinate & ~7) >> 1) + (color_latency ? 2 : 1));
        y = static_cast<uint8_t>(line);
    }

    reg_lpx = x;
    reg_lpy = y;

    irq_flag |= 0x08;
    if (irq_mask & 0x08)
    {
        irq_flag |= 0x80;
        CpuTriggerInterrupt(VIC_IRQ);
    }
}
