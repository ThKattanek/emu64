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
// Letzte Änderung am 15.06.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "mos6569_class.h"
#include <stdio.h>

//PAL

#define Read(adresse) ReadProcTbl[(adresse)>>8](adresse)

///////////////////////////////////////////////////////////////////////////////////////////////////

// Rahmen Y Start und Stop
//RSEL=0
#define RAHMEN24_YSTART		55
#define RAHMEN24_YSTOP		247

// RSEL=1
#define RAHMEN25_YSTART		51
#define RAHMEN25_YSTOP		251

// Rahmen X Start und Stop
//CSEL=0
#define RAHMEN38_LINKS		18  // Eigtl XKoordinate 0x01f  (18)
#define RAHMEN38_RECHTS		56  // Eigtl XKoordinate 0x14f  (56)

// CSEL=1
#define RAHMEN40_LINKS		17  // Eigtl XKoordinate 0x018  (17)
#define RAHMEN40_RECHTS		57  // Eigtl XKoordinate 0x158  (57)

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

	for(int i=0;i<8;i++)
	{
        reg_mx[i] = 0;
        reg_my[i] = 0;
		MCOLOR[i] = 0;
        SpriteSeqOn[i] = false;
	}

    reg_mx8 = 0;
    reg_ctrl_1 = 0;
    reg_ctrl_2 = 0;
    reg_y_scroll = 0;
    reg_x_scroll = 0;
    VBASE = 0;
	IRQ_RASTER = 0;
	LPX = 0;
	LPY = 0;
	ME = 0;
	MYE = 0;
	MDP = 0;
	MMC = 0;
	MXE = 0;
	MM = 0;
	MD = 0;
	EC = 0;
	B0C = 0;
	B1C = 0;
	B2C = 0;
	B3C = 0;
	MM0 = 0;
	MM1 = 0;

	AktZyklus = 1;
	AktRZ = 0;
	GrafikMode = 0;
	IRQFlag = 0;
	IRQMask = 0;

    // XKoordinaten in Tabelle schreiben
    unsigned short xwert = 0x0194;
    for(int i=1;i<14;i++)
    {
        XKoordTbl[i] = xwert;
        xwert += 8;
    }

    xwert = 0x0004;
    for(int i=14;i<64;i++)
    {
        XKoordTbl[i] = xwert;
        xwert += 8;
    }

	SetVicType(0);

    draw_line_counter = 0;
    LPTriggered = draw_this_line = VBlanking = false;

	isWriteColorReg20 = false;
	isWriteColorReg21 = false;

	BadLineEnable = false;
	BadLineStatus = false;
	DisplayStatus = false;
	MatrixBase = 0;
	CharBase = 0;
	BitmapBase = 0;

	RC = 0;
	VC = 0;
	VCBASE = 0;
	VMLI = 0;

	cAdresse = 0;
	gAdresse = 0;

	for(int i=0;i<40;i++)
	{
            cDatenPuffer8Bit[i] = 0;
            cDatenPuffer4Bit[i] = 0;
	}

	for(int i=0;i<8;i++)
	{
		MC[i] = 0;
		MCBase[i] = 0;
	}
	SpriteExpYFlipFlop = 0xFF;
	SpriteDMA = 0;
	SpriteView = 0;

	unsigned short spr_x_start = 0x194;
	for(int x=0;x<0x200;x++)
	{
		SpriteXDisplayTbl[spr_x_start]=x;
		if(x<107) SpriteXDisplayTbl[spr_x_start] += 8;
		spr_x_start++;
		spr_x_start &= 0x1FF;
	}

    for(int i=0;i<VIC_CONFIG_NUM;i++) vic_config[i] = true;
    Write_xd011 = false;

    FirstDisplayLinePAL = 16;                       //VICE - 16, Emu64 - 26
    LastDisplayLinePAL = 288;                       //VICE - 288, Emu64 - 292

    FirstDisplayLineNTSC = 30;
    LastDisplayLineNTSC = 288;

    FirstDisplayLine = FirstDisplayLinePAL;
    LastDisplayLine = LastDisplayLinePAL;

    HoBorderCMP_L[0]=RAHMEN38_LINKS;
    HoBorderCMP_R[0]=RAHMEN38_RECHTS;
    VeBorderCMP_O[0]=RAHMEN24_YSTART;
    VeBorderCMP_U[0]=RAHMEN24_YSTOP;

    HoBorderCMP_L[1]=RAHMEN40_LINKS;
    HoBorderCMP_R[1]=RAHMEN40_RECHTS;
    VeBorderCMP_O[1]=RAHMEN25_YSTART;
    VeBorderCMP_U[1]=RAHMEN25_YSTOP;
}

VICII::~VICII()
{
}

void VICII::SwitchVideoBuffer()
{
    current_video_buffer++;
    video_buffer = video_buffer_back[current_video_buffer & 1];
}

void VICII::GetRegister(VIC_STRUCT *vic_reg)
{
	vic_reg->AktZyklus = AktZyklus;
        if(AktZyklus == 1) vic_reg->AktRasterzeile = AktRZ;
        else vic_reg->AktRasterzeile = AktRZ;
	vic_reg->RasterLatch = IRQ_RASTER;
	vic_reg->DisplayStatus = DisplayStatus;
	vic_reg->GrafikMode = GrafikMode;
    vic_reg->VicBank = *cia2_port_a & 3;
    vic_reg->MatrixBase = MatrixBase;
    vic_reg->CharBase = CharBase;
    vic_reg->IRQ = IRQFlag & 0x80;

	for(int i=0;i<8;i++)
	{
        vic_reg->SpriteX[i] = reg_mx[i];
        vic_reg->SpriteY[i] = reg_my[i];
    }
}

void VICII::SetVicVDisplayPalSize(uint16_t first_line, uint16_t last_line)
{
    if(first_line < MAX_RASTER_ROWS && last_line < MAX_RASTER_ROWS && first_line <= last_line)
    {
        FirstDisplayLinePAL = first_line;
        LastDisplayLinePAL = last_line;
        SetVicType(System);
    }
}

uint16_t VICII::GetVicFirstDisplayLinePal()
{
    return FirstDisplayLinePAL;
}

uint16_t VICII::GetVicLastDisplayLinePal()
{
    return LastDisplayLinePAL;
}

uint16_t VICII::GetAktVicDisplayFirstLine()
{
    return FirstDisplayLine;
}

uint16_t VICII::GetAktVicDisplayLastLine()
{
    return LastDisplayLine;
}

void VICII::SetVicVDisplayNtscSize(uint16_t first_line, uint16_t last_line)
{
    if(first_line < MAX_RASTER_ROWS && last_line < MAX_RASTER_ROWS && first_line <= last_line)
    {
        FirstDisplayLineNTSC = first_line;
        LastDisplayLineNTSC = last_line;
        SetVicType(System);
    }
}

uint16_t VICII::GetVicFirstDisplayLineNtsc()
{
    return FirstDisplayLineNTSC;
}

uint16_t VICII::GetVicLastDisplayLineNtsc()
{
    return LastDisplayLineNTSC;
}

void VICII::SetVicType(int system)
{
	System = system;

	switch(System)
	{
	case 0:
		TOTAL_RASTERS=312;
		TOTAL_ZYKLEN_LINE=63;
		TOTAL_X=504;
		RASTER_Y = TOTAL_RASTERS - 1;
        FirstDisplayLine = FirstDisplayLinePAL;
        LastDisplayLine = LastDisplayLinePAL;
		break;
	case 1:
		TOTAL_RASTERS=262;
		TOTAL_ZYKLEN_LINE=64;
		TOTAL_X=512;
		RASTER_Y = TOTAL_RASTERS - 1;
        FirstDisplayLine = FirstDisplayLineNTSC;
        LastDisplayLine = LastDisplayLineNTSC;
		break;
	case 2:
		TOTAL_RASTERS=263;
		TOTAL_ZYKLEN_LINE=65;
		TOTAL_X=520;
		RASTER_Y = TOTAL_RASTERS - 1;
        FirstDisplayLine = FirstDisplayLineNTSC;
        LastDisplayLine = LastDisplayLineNTSC;
		break;
	}
}

inline void VICII::RasterIRQ()
{
	IRQFlag |= 0x01;
	if (IRQMask & 0x01) 
	{
		IRQFlag |= 0x80;
		CpuTriggerInterrupt(VIC_IRQ);
	}
}

inline void VICII::SetBALow()
{
    if (*ba)
    {
		FirstBAZyklus = AktZyklus;
        *ba = false;
    }
}

inline void VICII::cAccess()
{
    if (*ba == false)
	{
		/// Grafikdaten ///
        if(AktZyklus >= 15 && AktZyklus <= 54 && DisplayStatus == true && BadLineStatus == true)
        {
            cAdresse = (*cia2_port_a<<14)|MatrixBase|(VC&0x3FF);
            cDatenPuffer8Bit[VMLI] = Read(cAdresse);
            cDatenPuffer4Bit[VMLI] = color_ram[(VC&0x3FF)]&0x0F;
        }
	}
}

inline void VICII::gAccess()
{
    if (DisplayStatus) // DisplayModus
	{
        if (reg_ctrl_1 & 0x20) gAdresse = (*cia2_port_a<<14)|(BitmapBase)|((VC&0x3FF)<<3)|RC;
        else gAdresse = (*cia2_port_a<<14)|CharBase|(cDatenPuffer8Bit[VMLI]<<3)|RC;
        if (reg_ctrl_1 & 0x40) gAdresse &= 0xf9ff;

		switch(GrafikMode)
		{
		case 4:
			GfxData = Read(gAdresse & 0xF9FF);
			break;
		default:
			GfxData = Read(gAdresse);
			break;
		}

		CharData = cDatenPuffer8Bit[VMLI];
		ColorData = cDatenPuffer4Bit[VMLI];

		VMLI++;
		VC++;
	} 
    else    // IdleModus
	{
        gAdresse = reg_ctrl_1 & 0x40 ? 0x39FF : 0x3FFF;
        gAdresse |=(*cia2_port_a<<14);
		GfxData = Read(gAdresse);
		CharData = 0;
		ColorData = 0;
	}
}

inline void VICII::pAccess(uint8_t sp_nr)
{
    pAdresse = (*cia2_port_a<<14)|(MatrixBase)|0x3F8|sp_nr;
	pWert[sp_nr] = Read(pAdresse);
}

inline void VICII::sAccess(uint8_t sp_nr)
{
	//// Es werden gleich alle 3 Bytes geladen ////
    sAdresse = (*cia2_port_a<<14)|(pWert[sp_nr]<<6)|(MC[sp_nr]&0x3F);
	SpriteSeq[sp_nr] = (Read(sAdresse))<<16;sAdresse++;
	SpriteSeq[sp_nr] |= (Read(sAdresse))<<8;sAdresse++;
	SpriteSeq[sp_nr] |= Read(sAdresse);
}

inline void VICII::CheckBorder()
{
	///////////////// Rahmen ///////////////
	if(BorderFlipFlop0)
	{
        if(CSEL==1 && AktZyklus == RAHMEN40_RECHTS && BorderFlipFlop1 == false)
        {
            BorderLine[BorderLinePos++] = 128;
            BorderLine[BorderLinePos++] = 128;
            BorderLine[BorderLinePos++] = 128;
            BorderLine[BorderLinePos++] = 128;
            BorderLine[BorderLinePos++] = 128;
            BorderLine[BorderLinePos++] = 128;
            BorderLine[BorderLinePos++] = 128;
            BorderLine[BorderLinePos++] = 128;
        }
        else if(CSEL==0 && AktZyklus == RAHMEN38_RECHTS && BorderFlipFlop1 == false)
        {
            BorderLine[BorderLinePos++] = 128;
            BorderLine[BorderLinePos++] = 128;
            BorderLine[BorderLinePos++] = 128;
            BorderLine[BorderLinePos++] = 128;
            BorderLine[BorderLinePos++] = 128;
            BorderLine[BorderLinePos++] = 128;
            BorderLine[BorderLinePos++] = 128;
            BorderLine[BorderLinePos++] = EC;
        }
        else
        {
            if(isWriteColorReg20)
                BorderLine[BorderLinePos++] = 0x0f;
            else
                BorderLine[BorderLinePos++] = EC;

            int i=7;
            do{
                BorderLine[BorderLinePos++] = EC;
                i--;
            } while(i>0);

        }
	}
	else
	{
        if(CSEL==0 && AktZyklus == RAHMEN38_LINKS && BorderFlipFlop1 == true)
        {
            if(isWriteColorReg20)
                BorderLine[BorderLinePos++] = 0x0f;
            else
                BorderLine[BorderLinePos++] = EC;
            BorderLine[BorderLinePos++] = EC;
            BorderLine[BorderLinePos++] = EC;
            BorderLine[BorderLinePos++] = EC;
            BorderLine[BorderLinePos++] = EC;
            BorderLine[BorderLinePos++] = EC;
            BorderLine[BorderLinePos++] = 128;
            BorderLine[BorderLinePos++] = 128;
        }
        else
        {
            int i=8;
            do{
                BorderLine[BorderLinePos++] = 128;
                i--;
            } while(i>0);
        }
	}
}

inline void VICII::DrawGraphics()
{
	static unsigned short NEXT_X_SCROLL;

    if (!draw_this_line) return;

    video_buffer_line_xscroll = video_buffer_line + 16;

    if(AktZyklus == 14)
	{
        for(int i=0;i<8;i++) video_buffer_line_xscroll[i] = B0C | 0x40;
        NEXT_X_SCROLL = reg_x_scroll;
	}

    video_buffer_line_xscroll += NEXT_X_SCROLL;

    if(DisplayStatus)	// Display Mode
    {
        if((AktZyklus >= 16) && (AktZyklus <= 55)) // Grafiksequenzer --> muss um 16 Pixel verzögert werden
        {
			switch(GrafikMode)
			{
			case 0:
				/// Standard Text Modus (ECM/BMM/MCM = 0/0/0) ///
                Colors[0] = B0C | 0x40;
				Colors[1] = ColorData | 0x80;
					
                for(int i=7;i>-1;i--) *video_buffer_line_xscroll++ = Colors[(GfxData>>(i))&1];
                video_buffer_line += 8;
				break;
			case 1:
				/// Multicolor Text Modus (ECM/BMM/MCM = 0/0/1) ///
				if(!(ColorData&8))
				{
					/// MC-Flag = 0 ///
                    Colors[0] = B0C | 0x40;
					Colors[1] = (ColorData&7) | 0x80;

                    for(int i=7;i>-1;i--) *video_buffer_line_xscroll++ = Colors[(GfxData>>(i))&1];
                    video_buffer_line += 8;
				}
				else
				{
					/// MC-Flag = 1 ///
                    Colors[0] = B0C | 0x40;
					Colors[1] = B1C;
					Colors[2] = B2C | 0x80;
					Colors[3] = (ColorData&7) | 0x80;

					for(int i=6;i>-1;i-=2)
					{
                        *video_buffer_line_xscroll = Colors[(GfxData>>(i))&3];
                        video_buffer_line_xscroll++;
                        *video_buffer_line_xscroll = video_buffer_line_xscroll[-1];
                        video_buffer_line_xscroll++;
					}
                    video_buffer_line += 8;
				}
				break;
			case 2:
				/// Standard Bitmap Modus (ECM/BMM/MCM = 0/1/0) ///
				Colors[0] = CharData&0x0F;
				Colors[1] = (CharData>>4) | 0x80;
				
                for(int i=7;i>-1;i--) *video_buffer_line_xscroll++ = Colors[(GfxData>>(i))&1];
                video_buffer_line += 8;
				break;
			case 3:
				/// Multicolor Bitmap Modus (ECM/BMM/MCM = 0/1/1) ///
                Colors[0] = B0C | 0x40;
				Colors[1] = CharData>>4;
				Colors[2] = (CharData&0x0F) | 0x80;
				Colors[3] = ColorData | 0x80;
				
				for(int i=6;i>-1;i-=2)
				{
                    *video_buffer_line_xscroll = Colors[(GfxData>>(i))&3];
                    video_buffer_line_xscroll++;
                    *video_buffer_line_xscroll = video_buffer_line_xscroll[-1];
                    video_buffer_line_xscroll++;
				}
                video_buffer_line += 8;
				break;
			case 4:
				/// ECM Text Modus (ECM/BMM/MCM = 1/0/0) ///
				switch((CharData >> 6) & 0x03)
				{
				case 0:
                    Colors[0] = B0C | 0x40;
					break;
				case 1:
					Colors[0] = B1C;
					break;
				case 2:
					Colors[0] = B2C;
					break;
				case 3:
					Colors[0] = B3C;
					break;
				}
				Colors[1] = ColorData | 0x80;
					
                for(int i=7;i>-1;i--) *video_buffer_line_xscroll++ = Colors[(GfxData>>(i))&1];
                video_buffer_line += 8;
				break;
			default:
                *video_buffer_line_xscroll++ = 0;
                *video_buffer_line_xscroll++ = 0;
                *video_buffer_line_xscroll++ = 0;
                *video_buffer_line_xscroll++ = 0;
                *video_buffer_line_xscroll++ = 0;
                *video_buffer_line_xscroll++ = 0;
                *video_buffer_line_xscroll++ = 0;
                *video_buffer_line_xscroll++ = 0;
                video_buffer_line += 8;
				break;
			}
		}
        else
        {
                *video_buffer_line_xscroll++ = Colors[0];
                *video_buffer_line_xscroll++ = Colors[0];
                *video_buffer_line_xscroll++ = Colors[0];
                *video_buffer_line_xscroll++ = Colors[0];
                *video_buffer_line_xscroll++ = Colors[0];
                *video_buffer_line_xscroll++ = Colors[0];
                *video_buffer_line_xscroll++ = Colors[0];
                *video_buffer_line_xscroll++ = Colors[0];
                video_buffer_line += 8;
        }
	}
	else	// Idle Mode
    {
        Colors[0] = 0 | 0x40;

        if((AktZyklus >= 16) && (AktZyklus <= 55)) // Grafiksequenzer --> muss um 16 Pixel verzögert werden
		{
            // Grafiksequenzer --> muss um 16 Pixel verzögert werden
			switch(GrafikMode)
			{
			case 0:
                /// Standard Text Modus (ECM/BMM/MCM = 0/0/0) ///
                Colors[1] = 0 | 0x80;
                for(int i=7;i>-1;i--) *video_buffer_line_xscroll++ = Colors[(GfxData>>(i))&1];
                video_buffer_line += 8;
				break;
			case 1:
				/// Multicolor Text Modus (ECM/BMM/MCM = 0/0/1) ///
				if(!(ColorData&8))
				{
                    /// MC-Flag = 0 ///
					Colors[1] = 0 | 0x80;

                    for(int i=7;i>-1;i--) *video_buffer_line_xscroll++ = Colors[(GfxData>>(i))&1];
                    video_buffer_line += 8;
				}
				else
				{
                    /// MC-Flag = 1 ///
					Colors[1] = 0;
					Colors[2] = 0 | 0x80;
					Colors[3] = 0 | 0x80;

					for(int i=6;i>-1;i-=2)
					{
                        *video_buffer_line_xscroll = Colors[(GfxData>>(i))&3];
                        video_buffer_line_xscroll++;
                        *video_buffer_line_xscroll = video_buffer_line_xscroll[-1];
                        video_buffer_line_xscroll++;
					}
                    video_buffer_line += 8;
				}
				break;
			case 2:
				/// Standard Bitmap Modus (ECM/BMM/MCM = 0/1/0) ///
				Colors[1] = 0 | 0x80;
				
                for(int i=7;i>-1;i--) *video_buffer_line_xscroll++ = Colors[(GfxData>>(i))&1];
                video_buffer_line += 8;
				break;
			case 3:
                /// Multicolor Bitmap Modus (ECM/BMM/MCM = 1/0/0) ///
                Colors[1] = 0;
				Colors[2] = 0 | 0x80;
				Colors[3] = 0 | 0x80;
				
				for(int i=6;i>-1;i-=2)
				{
                    *video_buffer_line_xscroll = Colors[(GfxData>>(i))&3];
                    video_buffer_line_xscroll++;
                    *video_buffer_line_xscroll = video_buffer_line_xscroll[-1];
                    video_buffer_line_xscroll++;
				}
                video_buffer_line += 8;

				break;
			default:
                *video_buffer_line_xscroll++ = 0;
                *video_buffer_line_xscroll++ = 0;
                *video_buffer_line_xscroll++ = 0;
                *video_buffer_line_xscroll++ = 0;
                *video_buffer_line_xscroll++ = 0;
                *video_buffer_line_xscroll++ = 0;
                *video_buffer_line_xscroll++ = 0;
                *video_buffer_line_xscroll++ = 0;
                video_buffer_line += 8;
				break;
			}
		}
        else
        {
                *video_buffer_line_xscroll++ = Colors[0];
                *video_buffer_line_xscroll++ = Colors[0];
                *video_buffer_line_xscroll++ = Colors[0];
                *video_buffer_line_xscroll++ = Colors[0];
                *video_buffer_line_xscroll++ = Colors[0];
                *video_buffer_line_xscroll++ = Colors[0];
                *video_buffer_line_xscroll++ = Colors[0];
                *video_buffer_line_xscroll++ = Colors[0];
                video_buffer_line += 8;
        }
	}

    /// Im Aktuellen Zyklus Prüfen ob B0C Color verwendet wurde (16 Pixel vorlauf)
    /// Wenn ja mit aktuellen B0C überschreiben

    video_buffer_line_xscroll = video_buffer_line - 8;

    /// Grey Dot zeichnen
    ///
    if(isWriteColorReg21 && (video_buffer_line_xscroll[0] & 0x40) && DisplayStatus)
        video_buffer_line_xscroll[0] = 0x0f;


    for(int i=0;i<8;i++)
    {
        if(video_buffer_line_xscroll[i] & 0x40) video_buffer_line_xscroll[i] =  B0C;
    }

    ////////////////////////// Draw Border //////////////////////////
    CheckBorder();
}

inline void VICII::DrawGraphicsPseudo()
{
    /// Für Anzeige Aller Zyklen
    video_buffer_line += 8;

    //CheckBorder
    BorderLinePos += 8;
    return;
}

inline void VICII::DrawSprites()
{
    unsigned char AktSpriteBit=0x80;    // Sprites werden hier von 7 nach 0 gezeichnet (Sprite 0 hat die höchste Priorität und 7 die niedrigste)
    unsigned char AktSpriteColl=0;      // Noch keine Kollision Sprite-Sprite
    unsigned char AktDataColl=0;        // Noch keine Kollision Sprite-Hintergrund

    /*
    if(!vic_config[VIC_SPRITES_ON]) return;

    unsigned char mask=1;

    for(int SpriteNr=0; SpriteNr<8; SpriteNr++)
    {
        if(SpriteView & mask)
        {
            if((reg_mx[SpriteNr] >= AktXKoordinate) && (reg_mx[SpriteNr] < AktXKoordinate+8))
            {
                video_buffer_line_xscroll = (video_buffer_line-8) + (reg_mx[SpriteNr]-AktXKoordinate);
                *video_buffer_line_xscroll++ = SpriteNr+1;
            }
        }
        mask<<=1;
    }
    return;
    */

    /////////////////////////////////////////
    /////////////////////////////////////////

    if(!vic_config[VIC_SPRITES_ON]) return;
    if(AktRZ < FirstDisplayLine+1) return;

    // Spritekollisionspuffer löschen
    for (int i=0; i < 520; i++) SpriteCollisionsPuffer[i] = 0;

    int SpriteNr = 7;
    while(SpriteNr >= 0)
    {
        // Ist Sprite Sichtbar ?
        if(((SpriteViewAktLine & AktSpriteBit) == AktSpriteBit) && (reg_mx[SpriteNr]<0x1F8))
		{
            SpritePufferLine = (video_buffer_line - (62 * 8)) -4 + SpriteXDisplayTbl[reg_mx[SpriteNr]];

            /// Temporärer Zeiger inerhalb des SpriteCollisionsPuffers auf das Aktuellen Sprites
            unsigned char *q = SpriteCollisionsPuffer + reg_mx[SpriteNr] + 8;

            if((MDP & AktSpriteBit) == 0)	/// Prüfen auf Sprite Hinter Vordergrund
			{
				/// Sprites vor der Vordergrundgrafik
                if((MXE & AktSpriteBit) == AktSpriteBit)
				{
                    /// Expandiertes Sprite
                    if((MMC & AktSpriteBit) == AktSpriteBit)
					{
                        /// MultiColor Sprite
                        for(int i=0;i<12;i++)
						{
                            switch(SpriteSeqAktLine[SpriteNr] & 0xC00000)
							{
							case 0x000000:
								SpritePufferLine += 4;
								break;
							case 0x400000:
                                /// Sprite-Grafik Kollision
                                if(*SpritePufferLine & 0x80) AktDataColl |= AktSpriteBit;
                                if(*(SpritePufferLine+1) & 0x80) AktDataColl |= AktSpriteBit;
                                if(*(SpritePufferLine+2) & 0x80) AktDataColl |= AktSpriteBit;
                                if(*(SpritePufferLine+3) & 0x80) AktDataColl |= AktSpriteBit;

								*SpritePufferLine++ = MM0;
								*SpritePufferLine++ = MM0;
								*SpritePufferLine++ = MM0;
								*SpritePufferLine++ = MM0;

                                /// Sprite-Sprite Kollision (MuliColor Expand)
                                if (q[i<<2]) AktSpriteColl |= q[i<<2] | AktSpriteBit;
                                else q[i<<2] = AktSpriteBit;
                                if (q[(i<<2) + 1]) AktSpriteColl |= q[(i<<2) + 1] | AktSpriteBit;
                                else q[(i<<2) + 1] = AktSpriteBit;
                                if (q[(i<<2) + 2]) AktSpriteColl |= q[(i<<2) + 2] | AktSpriteBit;
                                else q[(i<<2) + 2] = AktSpriteBit;
                                if (q[(i<<2) + 3]) AktSpriteColl |= q[(i<<2) + 3] | AktSpriteBit;
                                else q[(i<<2) + 3] = AktSpriteBit;

								break;
							case 0x800000:
                                /// Sprite-Grafik Kollision
                                if(*SpritePufferLine & 0x80) AktDataColl |= AktSpriteBit;
                                if(*(SpritePufferLine+1) & 0x80) AktDataColl |= AktSpriteBit;
                                if(*(SpritePufferLine+2) & 0x80) AktDataColl |= AktSpriteBit;
                                if(*(SpritePufferLine+3) & 0x80) AktDataColl |= AktSpriteBit;

                                *SpritePufferLine++ = MCOLOR[SpriteNr];
                                *SpritePufferLine++ = MCOLOR[SpriteNr];
                                *SpritePufferLine++ = MCOLOR[SpriteNr];
                                *SpritePufferLine++ = MCOLOR[SpriteNr];

                                /// Sprite-Sprite Kollision (MuliColor Expand)
                                if (q[i<<2]) AktSpriteColl |= q[i<<2] | AktSpriteBit;
                                else q[i<<2] = AktSpriteBit;
                                if (q[(i<<2) + 1]) AktSpriteColl |= q[(i<<2) + 1] | AktSpriteBit;
                                else q[(i<<2) + 1] = AktSpriteBit;
                                if (q[(i<<2) + 2]) AktSpriteColl |= q[(i<<2) + 2] | AktSpriteBit;
                                else q[(i<<2) + 2] = AktSpriteBit;
                                if (q[(i<<2) + 3]) AktSpriteColl |= q[(i<<2) + 3] | AktSpriteBit;
                                else q[(i<<2) + 3] = AktSpriteBit;

								break;
							case 0xC00000:
                                /// Sprite-Grafik Kollision
                                if(*SpritePufferLine & 0x80) AktDataColl |= AktSpriteBit;
                                if(*(SpritePufferLine+1) & 0x80) AktDataColl |= AktSpriteBit;
                                if(*(SpritePufferLine+2) & 0x80) AktDataColl |= AktSpriteBit;
                                if(*(SpritePufferLine+3) & 0x80) AktDataColl |= AktSpriteBit;

								*SpritePufferLine++ = MM1;
								*SpritePufferLine++ = MM1;
								*SpritePufferLine++ = MM1;
								*SpritePufferLine++ = MM1;

                                /// Sprite-Sprite Kollision (MuliColor Expand)
                                if (q[i<<2]) AktSpriteColl |= q[i<<2] | AktSpriteBit;
                                else q[i<<2] = AktSpriteBit;
                                if (q[(i<<2) + 1]) AktSpriteColl |= q[(i<<2) + 1] | AktSpriteBit;
                                else q[(i<<2) + 1] = AktSpriteBit;
                                if (q[(i<<2) + 2]) AktSpriteColl |= q[(i<<2) + 2] | AktSpriteBit;
                                else q[(i<<2) + 2] = AktSpriteBit;
                                if (q[(i<<2) + 3]) AktSpriteColl |= q[(i<<2) + 3] | AktSpriteBit;
                                else q[(i<<2) + 3] = AktSpriteBit;

								break;
							}
                            SpriteSeqAktLine[SpriteNr] = SpriteSeqAktLine[SpriteNr] << 2;
						}
					}
					else
					{
                        /// SingleColor Sprite
                        for(int i=0;i<24;i++)
						{
                            if(SpriteSeqAktLine[SpriteNr] & 0x800000)
							{
                                /// Sprite-Grafik Kollision
                                if(*SpritePufferLine & 0x80) AktDataColl |= AktSpriteBit;
                                if(*(SpritePufferLine+1) & 0x80) AktDataColl |= AktSpriteBit;

                                *SpritePufferLine++ = MCOLOR[SpriteNr];
                                *SpritePufferLine++ = MCOLOR[SpriteNr];

                                /// Sprite-Sprite Kollision (SingleColor Expand)
                                if (q[i<<1]) AktSpriteColl |= q[i<<1] | AktSpriteBit;
                                else q[i<<1] = AktSpriteBit;
                                if (q[(i<<1) + 1]) AktSpriteColl |= q[(i<<1) + 1] | AktSpriteBit;
                                else q[(i<<1) + 1] = AktSpriteBit;
							}
							else SpritePufferLine += 2;
                            SpriteSeqAktLine[SpriteNr] = SpriteSeqAktLine[SpriteNr] << 1;
						}
					}
				}
				else
				{
                    /// Nicht Expandiertes Sprite
                    if((MMC & AktSpriteBit) == AktSpriteBit)
					{
                        /// MultiColor Sprite
                        for(int i=0;i<12;i++)
						{
                            switch(SpriteSeqAktLine[SpriteNr] & 0xC00000)
							{
							case 0x000000:
								SpritePufferLine += 2;
								break;
							case 0x400000:
                                /// Sprite-Grafik Kollision
                                if(*SpritePufferLine & 0x80) AktDataColl |= AktSpriteBit;
                                if(*(SpritePufferLine+1) & 0x80) AktDataColl |= AktSpriteBit;

								*SpritePufferLine++ = MM0;
								*SpritePufferLine++ = MM0;

                                /// Sprite-Sprite Kollision (MuliColor)
                                if (q[i<<1]) AktSpriteColl |= q[i<<1] | AktSpriteBit;
                                else q[i<<1] = AktSpriteBit;

                                if (q[(i<<1) + 1]) AktSpriteColl |= q[(i<<1) + 1] | AktSpriteBit;
                                else q[(i<<1) + 1] = AktSpriteBit;

								break;
							case 0x800000:
                                /// Sprite-Grafik Kollision
                                if(*SpritePufferLine & 0x80) AktDataColl |= AktSpriteBit;
                                if(*(SpritePufferLine+1) & 0x80) AktDataColl |= AktSpriteBit;

                                *SpritePufferLine++ = MCOLOR[SpriteNr];
                                *SpritePufferLine++ = MCOLOR[SpriteNr];

                                /// Sprite-Sprite Kollision (MuliColor)
                                if (q[i<<1]) AktSpriteColl |= q[i<<1] | AktSpriteBit;
                                else q[i<<1] = AktSpriteBit;

                                if (q[(i<<1) + 1]) AktSpriteColl |= q[(i<<1) + 1] | AktSpriteBit;
                                else q[(i<<1) + 1] = AktSpriteBit;

								break;
							case 0xC00000:
                                /// Sprite-Grafik Kollision
                                if(*SpritePufferLine & 0x80) AktDataColl |= AktSpriteBit;
                                if(*(SpritePufferLine+1) & 0x80) AktDataColl |= AktSpriteBit;

								*SpritePufferLine++ = MM1;
								*SpritePufferLine++ = MM1;

                                /// Sprite-Sprite Kollision (MuliColor)
                                if (q[i<<1]) AktSpriteColl |= q[i<<1] | AktSpriteBit;
                                else q[i<<1] = AktSpriteBit;

                                if (q[(i<<1) + 1]) AktSpriteColl |= q[(i<<1) + 1] | AktSpriteBit;
                                else q[(i<<1) + 1] = AktSpriteBit;

								break;
							}
                            SpriteSeqAktLine[SpriteNr] = SpriteSeqAktLine[SpriteNr] << 2;
						}
					}
					else
					{
                        /// SingleColor Sprite
                        for(int i=0;i<24;i++)
						{                            
                            if(SpriteSeqAktLine[SpriteNr] & 0x800000)
                            {
                                /// Sprite-Grafik Kollision
                                if(*SpritePufferLine & 0x80) AktDataColl |= AktSpriteBit;

                                *SpritePufferLine++ = MCOLOR[SpriteNr];

                                /// Sprite-Sprite Kollision
                                if (q[i]) AktSpriteColl |= q[i] | AktSpriteBit;
                                else q[i] = AktSpriteBit;
                            }
							else SpritePufferLine++;
                            SpriteSeqAktLine[SpriteNr] = SpriteSeqAktLine[SpriteNr] << 1;
						}
					}
				}
			}
			else
			{
				/// Sprites hinter der Vordergrundgrafik
                if((MXE & AktSpriteBit) == AktSpriteBit)
				{
                    /// Expandiertes Sprite
                    if((MMC & AktSpriteBit) == AktSpriteBit)
					{
                        /// MultiColor Sprite
                        for(int i=0;i<12;i++)
						{
                            switch(SpriteSeqAktLine[SpriteNr] & 0xC00000)
							{
							case 0x000000:
								SpritePufferLine += 4;
								break;
							case 0x400000:
                                /// Sprite-Grafik Kollision
                                if(*SpritePufferLine & 0x80) AktDataColl |= AktSpriteBit;
                                if(*(SpritePufferLine+1) & 0x80) AktDataColl |= AktSpriteBit;
                                if(*(SpritePufferLine+2) & 0x80) AktDataColl |= AktSpriteBit;
                                if(*(SpritePufferLine+3) & 0x80) AktDataColl |= AktSpriteBit;

								if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MM0;
								else SpritePufferLine++;
								if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MM0;
								else SpritePufferLine++;
								if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MM0;
								else SpritePufferLine++;
								if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MM0;
								else SpritePufferLine++;

                                /// Sprite-Sprite Kollision (MuliColor Expand)
                                if (q[i<<2]) AktSpriteColl |= q[i<<2] | AktSpriteBit;
                                else q[i<<2] = AktSpriteBit;
                                if (q[(i<<2) + 1]) AktSpriteColl |= q[(i<<2) + 1] | AktSpriteBit;
                                else q[(i<<2) + 1] = AktSpriteBit;
                                if (q[(i<<2) + 2]) AktSpriteColl |= q[(i<<2) + 2] | AktSpriteBit;
                                else q[(i<<2) + 2] = AktSpriteBit;
                                if (q[(i<<2) + 3]) AktSpriteColl |= q[(i<<2) + 3] | AktSpriteBit;
                                else q[(i<<2) + 3] = AktSpriteBit;

								break;
							case 0x800000:
                                /// Sprite-Grafik Kollision
                                if(*SpritePufferLine & 0x80) AktDataColl |= AktSpriteBit;
                                if(*(SpritePufferLine+1) & 0x80) AktDataColl |= AktSpriteBit;
                                if(*(SpritePufferLine+2) & 0x80) AktDataColl |= AktSpriteBit;
                                if(*(SpritePufferLine+3) & 0x80) AktDataColl |= AktSpriteBit;

                                if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MCOLOR[SpriteNr];
								else SpritePufferLine++;
                                if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MCOLOR[SpriteNr];
								else SpritePufferLine++;
                                if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MCOLOR[SpriteNr];
								else SpritePufferLine++;
                                if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MCOLOR[SpriteNr];
								else SpritePufferLine++;

                                /// Sprite-Sprite Kollision (MuliColor Expand)
                                if (q[i<<2]) AktSpriteColl |= q[i<<2] | AktSpriteBit;
                                else q[i<<2] = AktSpriteBit;
                                if (q[(i<<2) + 1]) AktSpriteColl |= q[(i<<2) + 1] | AktSpriteBit;
                                else q[(i<<2) + 1] = AktSpriteBit;
                                if (q[(i<<2) + 2]) AktSpriteColl |= q[(i<<2) + 2] | AktSpriteBit;
                                else q[(i<<2) + 2] = AktSpriteBit;
                                if (q[(i<<2) + 3]) AktSpriteColl |= q[(i<<2) + 3] | AktSpriteBit;
                                else q[(i<<2) + 3] = AktSpriteBit;

								break;
							case 0xC00000:
                                /// Sprite-Grafik Kollision
                                if(*SpritePufferLine & 0x80) AktDataColl |= AktSpriteBit;
                                if(*(SpritePufferLine+1) & 0x80) AktDataColl |= AktSpriteBit;
                                if(*(SpritePufferLine+2) & 0x80) AktDataColl |= AktSpriteBit;
                                if(*(SpritePufferLine+3) & 0x80) AktDataColl |= AktSpriteBit;

								if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MM1;
								else SpritePufferLine++;
								if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MM1;
								else SpritePufferLine++;
								if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MM1;
								else SpritePufferLine++;
								if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MM1;
								else SpritePufferLine++;

                                /// Sprite-Sprite Kollision (MuliColor Expand)
                                if (q[i<<2]) AktSpriteColl |= q[i<<2] | AktSpriteBit;
                                else q[i<<2] = AktSpriteBit;
                                if (q[(i<<2) + 1]) AktSpriteColl |= q[(i<<2) + 1] | AktSpriteBit;
                                else q[(i<<2) + 1] = AktSpriteBit;
                                if (q[(i<<2) + 2]) AktSpriteColl |= q[(i<<2) + 2] | AktSpriteBit;
                                else q[(i<<2) + 2] = AktSpriteBit;
                                if (q[(i<<2) + 3]) AktSpriteColl |= q[(i<<2) + 3] | AktSpriteBit;
                                else q[(i<<2) + 3] = AktSpriteBit;

								break;
							}
                            SpriteSeqAktLine[SpriteNr] = SpriteSeqAktLine[SpriteNr] << 2;
						}
					}
					else
					{
                        /// SingleColor Sprite
                        for(int i=0;i<24;i++)
						{
                            if(SpriteSeqAktLine[SpriteNr] & 0x800000)
							{
                                /// Sprite-Grafik Kollision
                                if(*SpritePufferLine & 0x80) AktDataColl |= AktSpriteBit;
                                if(*(SpritePufferLine+1) & 0x80) AktDataColl |= AktSpriteBit;

                                if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MCOLOR[SpriteNr];
								else SpritePufferLine++;
                                if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MCOLOR[SpriteNr];
								else SpritePufferLine++;

                                /// Sprite-Sprite Kollision (SingleColor Expand)
                                if (q[i<<1]) AktSpriteColl |= q[i<<1] | AktSpriteBit;
                                else q[i<<1] = AktSpriteBit;
                                if (q[(i<<1) + 1]) AktSpriteColl |= q[(i<<1) + 1] | AktSpriteBit;
                                else q[(i<<1) + 1] = AktSpriteBit;
							}
							else SpritePufferLine += 2;
                            SpriteSeqAktLine[SpriteNr] = SpriteSeqAktLine[SpriteNr] << 1;
						}
					}
				}
				else
				{
                    /// Nicht Expandiertes Sprite
                    if((MMC & AktSpriteBit) == AktSpriteBit)
					{
                        /// MultiColor Sprite
                        for(int i=0;i<12;i++)
						{
                            switch(SpriteSeqAktLine[SpriteNr] & 0xC00000)
							{
							case 0x000000:
								SpritePufferLine += 2;
								break;
							case 0x400000:
                                /// Sprite-Grafik Kollision
                                if(*SpritePufferLine & 0x80) AktDataColl |= AktSpriteBit;
                                if(*(SpritePufferLine+1) & 0x80) AktDataColl |= AktSpriteBit;

								if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MM0;
								else SpritePufferLine++;
								if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MM0;
								else SpritePufferLine++;

                                /// Sprite-Sprite Kollision (MuliColor)
                                if (q[i<<1]) AktSpriteColl |= q[i<<1] | AktSpriteBit;
                                else q[i<<1] = AktSpriteBit;

                                if (q[(i<<1) + 1]) AktSpriteColl |= q[(i<<1) + 1] | AktSpriteBit;
                                else q[(i<<1) + 1] = AktSpriteBit;

								break;
							case 0x800000:
                                /// Sprite-Grafik Kollision
                                if(*SpritePufferLine & 0x80) AktDataColl |= AktSpriteBit;
                                if(*(SpritePufferLine+1) & 0x80) AktDataColl |= AktSpriteBit;

                                if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MCOLOR[SpriteNr];
								else SpritePufferLine++;
                                if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MCOLOR[SpriteNr];
								else SpritePufferLine++;

                                /// Sprite-Sprite Kollision (MuliColor)
                                if (q[i<<1]) AktSpriteColl |= q[i<<1] | AktSpriteBit;
                                else q[i<<1] = AktSpriteBit;

                                if (q[(i<<1) + 1]) AktSpriteColl |= q[(i<<1) + 1] | AktSpriteBit;
                                else q[(i<<1) + 1] = AktSpriteBit;

								break;
							case 0xC00000:
                                /// Sprite-Grafik Kollision
                                if(*SpritePufferLine & 0x80) AktDataColl |= AktSpriteBit;
                                if(*(SpritePufferLine+1) & 0x80) AktDataColl |= AktSpriteBit;

								if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MM1;
								else SpritePufferLine++;
								if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MM1;
								else SpritePufferLine++;

                                /// Sprite-Sprite Kollision (MuliColor)
                                if (q[i<<1]) AktSpriteColl |= q[i<<1] | AktSpriteBit;
                                else q[i<<1] = AktSpriteBit;

                                if (q[(i<<1) + 1]) AktSpriteColl |= q[(i<<1) + 1] | AktSpriteBit;
                                else q[(i<<1) + 1] = AktSpriteBit;

								break;
							}
                            SpriteSeqAktLine[SpriteNr] = SpriteSeqAktLine[SpriteNr] << 2;
						}
					}
					else
					{
                        /// SingleColor Sprite
                        for(int i=0;i<24;i++)
						{
                            if(SpriteSeqAktLine[SpriteNr] & 0x800000)
							{
                                if(!(*SpritePufferLine&0x80))
                                {                                    
                                    /// Sprite-Grafik Kollision
                                    if(*SpritePufferLine & 0x80) AktDataColl |= AktSpriteBit;

                                    *SpritePufferLine++ = MCOLOR[SpriteNr];

                                    /// Sprite-Sprite Kollision
                                    if (q[i]) AktSpriteColl |= q[i] | AktSpriteBit;
                                    else q[i] = AktSpriteBit;
                                }
                                else SpritePufferLine++;
							}
							else SpritePufferLine++;
                            SpriteSeqAktLine[SpriteNr] = SpriteSeqAktLine[SpriteNr] << 1;
						}
					}
				}
			}
		}

        SpriteNr--;
        AktSpriteBit >>= 1;
    }

    //////////////////////// Sprite Collision ///////////////////////////

    // Prüfe sprite-sprite kollisions
    if(vic_config[VIC_SPR_SPR_COLL_ON])
    {
        if(MM == 0)
        {
            // IRQ kann ausgelöst werden
            MM |= AktSpriteColl;
            if(MM != 0)
            {
                IRQFlag |= 0x04;
                if (IRQMask & 0x04)
                {
                    IRQFlag |= 0x80;
                    CpuTriggerInterrupt(VIC_IRQ);
                }
            }
        }
        else
        {
            // kein IRQ auslösen
            MM |= AktSpriteColl;
        }
    }

    // Prüfe sprite-gfx kollision
    if(vic_config[VIC_SPR_BCK_COLL_ON])
    {
        if(MD == 0)
        {
            // IRQ kann ausgelöst werden
            MD |= AktDataColl;
            if(MD != 0)
            {
                IRQFlag |= 0x02;
                if (IRQMask & 0x02)
                {
                    IRQFlag |= 0x80;
                    CpuTriggerInterrupt(VIC_IRQ);
                }
            }
        }
        else
        {
            // kein IRQ auslösen
            MD |= AktDataColl;
        }
    }
}

inline void VICII::DrawBorder()
{
    if(!vic_config[VIC_BORDER_ON]) return;
    if(AktRZ < FirstDisplayLine) return;
    BorderPufferLine = video_buffer_line - 504;	/// Erstes Sichtbares Pixel

    for(int x=0;x<504;x++)
	{
		if(BorderLine[x] & 128) BorderPufferLine++;
		else *BorderPufferLine++ = BorderLine[x];
	}
}

void VICII::OneCycle()
{
    unsigned char SpriteBit = 0x01;

    AktXKoordinate = XKoordTbl[AktZyklus];

    // Prüfen ob Badlines zugelassen sind
    if(AktRZ == 0x30) BadLineEnable = DEN;

    // Prüfen auf Badline zustand
    if((AktRZ>=0x30) && (AktRZ<=0xF7) && (reg_y_scroll == (AktRZ&7)) && (BadLineEnable == true))
    {
        BadLineStatus = true;
        DisplayStatus = true;
    }
    else BadLineStatus = false;

	switch (AktZyklus)
	{
    case 1:

		if (AktRZ == (TOTAL_RASTERS-1)) 
		{
			VBlanking = true;
			AktRZ = 0;
            RefreshProc(video_buffer);
            draw_line_counter = 0;
			VCBASE = 0;
		}
		else 
		{

            AktRZ++;

			// Prüfen auf Raster IRQ
            //if (AktRZ == IRQ_RASTER) RasterIRQ();
            draw_this_line = (AktRZ >= FirstDisplayLine && AktRZ <= LastDisplayLine);
		}

        if(draw_this_line)
        {
            video_buffer_line = &video_buffer[draw_line_counter++*MAX_XW];		// Zeiger für Aktuelle Zeile setzen
            BorderLinePos = 0;
        }

		// Sprite 2 //
        if((SpriteDMA & 0x18) == 0) *ba = true;

		// Sprite 3 //
        pAccess(3);
        if(!*ba) sAccess(3);

        DrawGraphicsPseudo();
		break;
	
    case 2:
        if (VBlanking)
        {
            LPTriggered = VBlanking = false;
            //if (IRQ_RASTER == 0) RasterIRQ();
        }

        // Prüfen auf Raster IRQ
        if (AktRZ == IRQ_RASTER) RasterIRQ();

		/// Sprite 5 ///
        if(SpriteDMA & 0x20)  SetBALow();

        DrawGraphicsPseudo();
		break;

	case 3:
		// Sprite 3 //
        if((SpriteDMA & 0x30) == 0) *ba = true;

		// Sprite 4 //
        pAccess(4);
        if(!*ba) sAccess(4);

        DrawGraphicsPseudo();
		break;

	case 4:
		/// Sprite 6 ///
		if(SpriteDMA & 0x40)  SetBALow();

        DrawGraphicsPseudo();
		break;

	case 5:
        // Sprite 4 //
        if((SpriteDMA & 0x60) == 0) *ba = true;

		// Sprite 5 //
        pAccess(5);
        if(!*ba) sAccess(5);

        DrawGraphicsPseudo();
		break;

	case 6:
		/// Sprite 7 ///
		if(SpriteDMA & 0x80)  SetBALow();

        DrawGraphicsPseudo();
		break;

	case 7:
        // Sprite 5 //
        if((SpriteDMA & 0xC0) == 0) *ba = true;

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
        if((SpriteDMA & 0x80) == 0) *ba = true;

		// Sprite 7 //
        pAccess(7);
        if(!*ba) sAccess(7);

        VMLI = 0;

        DrawGraphicsPseudo();
        break;

    case 10:
        VMLI = 0;

        DrawGraphicsPseudo();
		break;

	case 11:
		// Sprite 7 //
        *ba = true;

        VC = VCBASE;

        DrawGraphics();
        break;

    case 12:
        VC = VCBASE;

        if(BadLineStatus) SetBALow();
        DrawGraphics();
        break;

    case 13:
        VC = VCBASE;

        if(BadLineStatus) SetBALow();
        DrawGraphics();
		break;

	case 14:
        if(BadLineStatus) SetBALow();
        if((BadLineStatus == true) && (DisplayStatus == true)) RC = 0;

        /// Sprite MC + 3
        for(int i=0;i<8;i++,SpriteBit<<=1)
        {
            if(SpriteDMA & SpriteBit) MC[i] += 3;
        }

        DrawGraphics();
		break;

    case 15:
        if (BadLineStatus) SetBALow();
        DrawGraphics();
        break;

	case 16:
		SpriteViewAktLine = SpriteView;
		SpriteSeqAktLine[0] = SpriteSeq[0];
		SpriteSeqAktLine[1] = SpriteSeq[1];
		SpriteSeqAktLine[2] = SpriteSeq[2];
		SpriteSeqAktLine[3] = SpriteSeq[3];
		SpriteSeqAktLine[4] = SpriteSeq[4];
		SpriteSeqAktLine[5] = SpriteSeq[5];
		SpriteSeqAktLine[6] = SpriteSeq[6];
		SpriteSeqAktLine[7] = SpriteSeq[7];

        /// Sprite ///
        for(int i=0;i<8;i++,SpriteBit<<=1)
        {
            if((SpriteExpYFlipFlop & SpriteBit) == SpriteBit) MCBase[i] += 2;
        }

        gAccess();
        if (BadLineStatus) SetBALow();

        DrawGraphics();
		break;

    case 17:
        /// Sprite ///
        for(int i=0;i<8;i++,SpriteBit<<=1)
        {
            if((SpriteExpYFlipFlop & SpriteBit) == SpriteBit) MCBase[i] ++;
            if(MCBase[i] >= 63)
            {
                SpriteDMA  &= ~SpriteBit;
                SpriteView &= ~SpriteBit;
            }
        }

        gAccess();
        if (BadLineStatus) SetBALow();

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
        if (BadLineStatus) SetBALow();

        DrawGraphics();
		break;

    case 55:
        *ba = true;

		/// Sprite ///
		// In der ersten Phase von Zyklus 55 wird das Expansions-Flipflop
		// invertiert, wenn das MxYE-Bit gesetzt ist.

        for (int i=0; i<8; i++, SpriteBit<<=1) if (MYE & SpriteBit) SpriteExpYFlipFlop ^= SpriteBit;

		// In der ersten Phasen von Zyklus 55 wird für Sprite 0-3 geprüft, 
		// ob das entsprechende MxE-Bit in Register $d015 gesetzt und die Y-Koordinate 
		// des Sprites (ungerade Register $d001-$d00f) gleich den unteren 8 Bits 
		// von RASTER ist. Ist dies der Fall und der DMA für das Sprite noch 
		// ausgeschaltet, wird der DMA angeschaltet, MCBASE gelöscht und, 
		// wenn das MxYE-Bit gesetzt ist, das Expansions-Flipflop gelöscht.
        SpriteBit = 0x01;
        for (int i=0; i<4; i++, SpriteBit<<=1)
		{
            if ((ME & SpriteBit) && ((AktRZ & 0xff) == reg_my[i]))
			{
                if((SpriteDMA & SpriteBit) == 0)
				{
                    SpriteDMA |= SpriteBit;
					MCBase[i] = 0;
                    if (MYE & SpriteBit) SpriteExpYFlipFlop &= ~SpriteBit;
				}
			}
		}

		/// Sprite 0 ///
		if(SpriteDMA & 0x01)  SetBALow();

        gAccess();
        DrawGraphics();
		break;
	
	case 56:
		/// Sprite ///
		// In der ersten Phasen von Zyklus 56 wird für Sprite 4-7 geprüft, 
		// ob das entsprechende MxE-Bit in Register $d015 gesetzt und die Y-Koordinate 
		// des Sprites (ungerade Register $d001-$d00f) gleich den unteren 8 Bits 
		// von RASTER ist. Ist dies der Fall und der DMA für das Sprite noch 
		// ausgeschaltet, wird der DMA angeschaltet, MCBASE gelöscht und, 
		// wenn das MxYE-Bit gesetzt ist, das Expansions-Flipflop gelöscht.
        SpriteBit = 0x10;
        for (int i=4; i<8; i++, SpriteBit<<=1)
		{
            if ((ME & SpriteBit) && (AktRZ & 0xff) == reg_my[i])
			{
                if((SpriteDMA & SpriteBit) == 0)
				{
                    SpriteDMA |= SpriteBit;
					MCBase[i] = 0;
                    if (MYE & SpriteBit) SpriteExpYFlipFlop &= ~SpriteBit;
				}
			}
		}	

        DrawGraphics();
		break;
	
	case 57:
		/// Sprite 1 ///
		if(SpriteDMA & 0x02)  SetBALow();

        DrawGraphics();
		break;

	case 58:
        if(DisplayStatus == false)
        {
            VCBASE = VC;
        }
        else if(BadLineStatus)
        {
            if(RC == 7) VCBASE = VC;
            RC = (RC + 1) & 7;
        }
        else
        {
            if(RC == 7)
            {
                VCBASE = VC;
                DisplayStatus = false;
            }
            if(RC < 7) RC = (RC + 1) & 7;
        }

        /// Sprite ///
		// In der ersten Phase von Zyklus 58 wird für jedes Sprite MC mit MCBASE
		// geladen (MCBASE->MC) und geprüft, ob der DMA für das Sprite angeschaltet
		// und die Y-Koordinate des Sprites gleich den unteren 8 Bits von RASTER ist.
		// Ist dies der Fall, wird die Darstellung des Sprites angeschaltet.
        for(int i=0;i<8;i++, SpriteBit<<=1)
		{
			MC[i] = MCBase[i];
            //if(((SpriteDMA & bitc) == bitc) && ((AktRZ&0xFF) == reg_my[i])) SpriteView |= bitc;
            if(((SpriteDMA & SpriteBit) == SpriteBit)) SpriteView |= SpriteBit;
		}

		// Sprite 0 //
        pAccess(0);
        if(!*ba) sAccess(0);

        DrawGraphics();
		break;

	case 59:
		/// Sprite 2 ///
		if(SpriteDMA & 0x04)  SetBALow();

        DrawGraphics();
		break;
	
	case 60:
		// Sprite 0 //
        if((SpriteDMA & 0x06) == 0) *ba = true;

		// Sprite 1 //
        pAccess(1);
        if(!*ba) sAccess(1);

        DrawGraphics();
		break;

	case 61:
		/// Sprite 3 ///
        if(SpriteDMA & 0x08)  SetBALow();

        DrawGraphics();
		break;

	case 62:
		// Sprite 1 //
        if((SpriteDMA & 0x0C) == 0) *ba = true;

		// Sprite 2 //
        pAccess(2);
        if(!*ba) sAccess(2);

        DrawGraphicsPseudo();
		break;
	
	case 63:
		/// Rahmen ///
        if(RSEL == 0)
		{
            if(AktRZ == RAHMEN24_YSTOP) BorderFlipFlop1 = true;
            if((AktRZ == RAHMEN24_YSTART) && (DEN == true)) BorderFlipFlop1 = false;
		}
		else
		{
            if(AktRZ == RAHMEN25_YSTOP) BorderFlipFlop1 = true;
            if((AktRZ == RAHMEN25_YSTART) && (DEN == true)) BorderFlipFlop1 = false;
		}

		/// Sprite 4 ///
        if(SpriteDMA & 0x10)  SetBALow();


        DrawGraphicsPseudo();
        DrawSprites();
        DrawBorder();

        if(TOTAL_ZYKLEN_LINE == 63) AktZyklus = 0;

        break;

    case 64:
        if(TOTAL_ZYKLEN_LINE == 64) AktZyklus = 0;

        DrawGraphicsPseudo();
        break;

    case 65:
        AktZyklus = 0;

        DrawGraphicsPseudo();
        break;
	}

    cAccess();     // cZugriffe immer im 2.Teil eines Zyklus wenn BA Low ist

    // Rahmen
    if(AktZyklus == HoBorderCMP_R[CSEL])
    {
        BorderFlipFlop0 = true;
    }

    if(AktZyklus == HoBorderCMP_L[CSEL])
    {
        if(AktRZ == VeBorderCMP_U[RSEL]) BorderFlipFlop1 = true;
        if(AktRZ == VeBorderCMP_O[RSEL]  && (DEN == true)) BorderFlipFlop1 = false;
        if(BorderFlipFlop1 == false) BorderFlipFlop0 = false;
    }

    // Ist ein Spritesequenzer aktiv ?
    // NEU
    //DrawSprites();

    isWriteColorReg20 = false;
    isWriteColorReg21 = false;

    ///   Auf nächsten Zyklus stellen   ///
    /// Aktueller Zyklus ist nun zuende ///

	AktZyklus++;

    if(AktZyklus == 1)
    {
        if(breakpoints[AktRZ+1] & 256)
        {
                *break_status |= 256;
                break_values[8] = AktRZ+1;
        }
    }
    else
    {
        if(breakpoints[AktRZ] & 256)
        {
                *break_status |= 256;
                break_values[8] = AktRZ;
        }
    }

    if(breakpoints[AktZyklus] & 512)
	{
        *break_status |= 512;
        break_values[9] = AktZyklus;
    }
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
    fw(VBASE);
	fw(IRQ_RASTER);
	fw(LPX);
	fw(LPY);
	fw(ME);
	fw(MYE);
	fw(MDP);
	fw(MMC);
	fw(MXE);
	fw(MM);
	fw(MD);
	fw(EC);
	fw(B0C);
	fw(B1C);
	fw(B2C);
	fw(B3C);
	fw(MM0);
	fw(MM1);
	fw1(MCOLOR,8);
	fw(AktZyklus);
	fw(AktRZ);
	fw(AktXKoordinate);
	fw(GrafikMode);
	fw(IRQFlag);
	fw(IRQMask);
	fw(DEN);
	fw(VBlanking);
	fw(FirstBAZyklus);
	fw(BadLineEnable);
	fw(BadLineStatus);
	fw(DisplayStatus);
	fw(LPTriggered);
	fw(RC);
	fw(VC);
	fw(VCBASE);
	fw(MatrixBase);
	fw(CharBase);
	fw(BitmapBase);
	fw(VMLI);
	fw(cAdresse);
	fw(gAdresse);
	fw1(cDatenPuffer8Bit,64);
	fw1(cDatenPuffer4Bit,64);
	fw(GfxData);
	fw(CharData);
	fw(ColorData);
	fw(GreyDotEnable);
	fw(isWriteColorReg20);
	fw(isWriteColorReg21);

	//////////////////////////////// Sprites //////////////////////////////////

	fw(SpriteExpYFlipFlop);
	fw1(MC,8);
	fw1(MCBase,8);
	fw(SpriteDMA);
	fw(SpriteView);

	///////////////////////////////////////////////////////////////////////////

	////////////////////////////// Rahmenstufe ////////////////////////////////

	fw(CSEL);
	fw(RSEL);
	fw(BorderCmpYo);
	fw(BorderCmpYu);
	fw(BorderFlipFlop0);
	fw(BorderFlipFlop1);
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
		fr(VBASE);
		fr(IRQ_RASTER);
		fr(LPX);
		fr(LPY);
		fr(ME);
		fr(MYE);
		fr(MDP);
		fr(MMC);
		fr(MXE);
		fr(MM);
		fr(MD);
		fr(EC);
		fr(B0C);
		fr(B1C);
		fr(B2C);
		fr(B3C);
		fr(MM0);
		fr(MM1);
		fr1(MCOLOR,8);
		fr(AktZyklus);
		fr(AktRZ);
		fr(AktXKoordinate);
		fr(GrafikMode);
		fr(IRQFlag);
		fr(IRQMask);
		fr(DEN);
		fr(VBlanking);
		fr(FirstBAZyklus);
		fr(BadLineEnable);
		fr(BadLineStatus);
		fr(DisplayStatus);
		fr(LPTriggered);
		fr(RC);
		fr(VC);
		fr(VCBASE);
		fr(MatrixBase);
		fr(CharBase);
		fr(BitmapBase);
		fr(VMLI);
		fr(cAdresse);
		fr(gAdresse);
		fr1(cDatenPuffer8Bit,64);
		fr1(cDatenPuffer4Bit,64);
		fr(GfxData);
		fr(CharData);
		fr(ColorData);
		fr(GreyDotEnable);
		fr(isWriteColorReg20);
		fr(isWriteColorReg21);

		//////////////////////////////// Sprites //////////////////////////////////

		fr(SpriteExpYFlipFlop);
		fr1(MC,8);
		fr1(MCBase,8);
		fr(SpriteDMA);
		fr(SpriteView);

		///////////////////////////////////////////////////////////////////////////

		////////////////////////////// Rahmenstufe ////////////////////////////////

		fr(CSEL);
		fr(RSEL);
		fr(BorderCmpYo);
		fr(BorderCmpYu);
		fr(BorderFlipFlop0);
		fr(BorderFlipFlop1);
		fr(BorderCMP_Re);
		fr(BorderCMP_Li);
		fr(BorderCMP_Ob);
		fr(BorderCMP_Un);
		break;
	}
	return true;
}
*/

unsigned short NEW_IRQ_RASTER = 0;
void VICII::WriteIO(uint16_t address, uint16_t value)
{
    address &= 63;
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

                        NEW_IRQ_RASTER = (IRQ_RASTER & 0xFF) | ((value & 0x80) << 1);
                        if (IRQ_RASTER != NEW_IRQ_RASTER && AktRZ == NEW_IRQ_RASTER) RasterIRQ();
                        IRQ_RASTER = NEW_IRQ_RASTER;
			
                        GrafikMode = ((reg_ctrl_1 & 0x60) | (reg_ctrl_2 & 0x10)) >> 4;
		
			// Prüfen ob Badlines zugelassen sind
                        if ((AktRZ == 0x30) && (value & 0x10)) BadLineEnable = true;

			// Prüfen auf Badline zustand
                        if((AktRZ>=0x30) && (AktRZ<=0xF7) && (reg_y_scroll == (AktRZ&7)) && (BadLineEnable == true))
			{
                                BadLineStatus = true;
			}
            else BadLineStatus = false;

			// RSEL
            RSEL = (value & 0x08)>>3;

			// DEN
            DEN = (value & 0x10)>>4;
            Write_xd011 = true;
			break;

		/// Rasterzähler
		case 0x12:
                        //IRQ_RASTER = (IRQ_RASTER & 0xFF00) | wert;

                        NEW_IRQ_RASTER = (IRQ_RASTER & 0xFF00) | value;
                        if (IRQ_RASTER != NEW_IRQ_RASTER && AktRZ == NEW_IRQ_RASTER) RasterIRQ();
                        IRQ_RASTER = NEW_IRQ_RASTER;
			break;

		/// Sprite Enable
		case 0x15:
            ME = value;
			break;

		// Control Register 2
		case 0x16:
            reg_ctrl_2 = value;
            reg_x_scroll = value & 7;
            GrafikMode = ((reg_ctrl_1 & 0x60) | (reg_ctrl_2 & 0x10)) >> 4;
			
			// CSEL
            CSEL = (value & 0x08)>>3;
			
			break;

		/// Sprite Y-Expansion
		case 0x17:
                        MYE = value;
                        SpriteExpYFlipFlop |= ~value;
			break;

		/// Speicher Pointer
		case 0x18:
                        VBASE = value;
                        MatrixBase = (value & 0xf0) << 6;
                        CharBase = (value & 0x0e) << 10;
                        BitmapBase = (value & 0x08) << 10;
			break;

		/// IRQ Flags
		case 0x19: 
                        IRQFlag = IRQFlag & (~value & 0x0F);
                        if (IRQFlag & IRQMask) IRQFlag |= 0x80;
                        else CpuClearInterrupt(VIC_IRQ);
			break;
		
		/// IRQ Mask
		case 0x1A:
                        IRQMask = value & 0x0F;
                        if (IRQFlag & IRQMask)
			{
                                IRQFlag |= 0x80;
                                CpuTriggerInterrupt(VIC_IRQ);
			} else {
                                IRQFlag &= 0x7F;
                                CpuClearInterrupt(VIC_IRQ);
			}
			break;

		/// Sprite Daten Priorität
		case 0x1B:
                        MDP = value;
			break;

		/// Sprite Multicolor
		case 0x1C:
                        MMC = value;
			break;

		/// Sprite X-Expansion
		case 0x1D:
                        MXE = value;
			break;

		/// Rahmenfarbe
		case 0x20:
                        EC = value&15;
                        if(vic_config[VIC_GREY_DOTS_ON]) isWriteColorReg20 = true;
			break;
		/// Hintergrundfarbe 0
        case 0x21:
                        B0C = value&15;
                        if(vic_config[VIC_GREY_DOTS_ON]) isWriteColorReg21 = true;
			break;
		/// Hintergrundfarbe 1
                case 0x22: B1C	= value&15;
                        //isWriteColorReg = true;
			break;
		/// Hintergrundfarbe 2
                case 0x23: B2C	= value&15;
                        //isWriteColorReg = true;
			break;
		/// Hintergrundfarbe 3
                case 0x24: B3C	= value&15;
                        //isWriteColorReg = true;
			break;
		/// Sprite Multicolor 0
                case 0x25: MM0  = value&15;
                        //isWriteColorReg = true;
			break;
		/// Sprite Multicolor 1
                case 0x26: MM1  = value&15;
                        //isWriteColorReg = true;
			break;

		/// Sprite Farbe
		case 0x27: case 0x28: case 0x29: case 0x2A:
		case 0x2B: case 0x2C: case 0x2D: case 0x2E:
                        MCOLOR[address - 0x27] = value&15;
			break;
	}
}

uint8_t VICII::ReadIO(uint16_t address)
{
    address &= 63;
    switch (address)
	{
		/// Sprite X Position
		case 0x00: case 0x02: case 0x04: case 0x06:
		case 0x08: case 0x0A: case 0x0C: case 0x0E:
                        return (unsigned char)reg_mx[address >> 1];

		/// Sprite Y Position
		case 0x01: case 0x03: case 0x05: case 0x07:
		case 0x09: case 0x0B: case 0x0D: case 0x0F:
                        return reg_my[address >> 1];

		/// Sprite X Position MSB
		case 0x10:	
                        return reg_mx8;

		/// Control Register 1
		case 0x11:	
                    if(Write_xd011) return (reg_ctrl_1 & 0x7F) | ((AktRZ & 0x100) >> 1);
                    else return 0;

		/// Rasterzähler
		case 0x12:
                        return (unsigned char)AktRZ;

		/// Ligthpen X
		case 0x13:
                        return LPX;

		/// Ligthpen Y
		case 0x14:
                        return LPY;

		case 0x15:	// Sprite enable
                        return ME;

		// Control Register 2
		case 0x16:
                        return reg_ctrl_2 | 0xC0;

		/// Sprite Y-Expansion
		case 0x17:
                        return MYE;

		/// Speicher Pointer
		case 0x18:
                        return VBASE | 0x01;

		/// IRQ Flags
		case 0x19:
                        return IRQFlag | 0x70;

		/// IRQ Mask
		case 0x1A:
                        return IRQMask | 0xF0;

		/// Sprite Daten Priorität
		case 0x1B:
                        return MDP;

		/// Sprite Multicolor
		case 0x1C:
                        return MMC;

		/// Sprite X-Expansion
		case 0x1D:
                        return MXE;

		/// Sprite - Sprite Kollision
		case 0x1E:
        {
            unsigned char ret = MM;
            MM = 0;
            return ret;
        }

		/// Sprite - Daten Kollision
		case 0x1F:
        {
            unsigned char ret = MD;
            MD = 0;
            return ret;
        }

        case 0x20: return EC | 0xf0;
        case 0x21: return B0C | 0xf0;
        case 0x22: return B1C | 0xf0;
        case 0x23: return B2C | 0xf0;
        case 0x24: return B3C | 0xf0;
        case 0x25: return MM0 | 0xf0;
        case 0x26: return MM1 | 0xf0;

		case 0x27: case 0x28: case 0x29: case 0x2a:
		case 0x2b: case 0x2c: case 0x2d: case 0x2e:
            return MCOLOR[address - 0x27] | 0xf0;

		default:
			return 0xFF;
	}
}

void VICII::TriggerLightpen()
{
    if(!LPTriggered)
	{
        LPTriggered = true;

        LPX = AktXKoordinate >> 1;
        LPY = AktRZ;

        IRQFlag |= 0x08;
        if (IRQMask & 0x08)
		{
            IRQFlag |= 0x80;
            CpuTriggerInterrupt(VIC_IRQ);
		}
	}
}
