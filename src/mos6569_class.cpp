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
// Letzte Änderung am 27.11.2018                //
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

// erste und letzte Display Zeile
#define FIRST_DISP_LINE_PAL	16              //16
#define FIRST_DISP_LINE_NTSC	0x10 + 14

#define LAST_DISP_LINE	288                 //288

// erste and letzte mögliche Rasterzeile für Bad Lines
#define FIRST_DMA_LINE		0x30
#define LAST_DMA_LINE		0xf7

// länge der X Werte der Destination BitMap
#define XMOD				504
#define YMOD                312

///////////////////////////////////////////////////////////////////////////////////////////////////

VICII::VICII()
{
    AktVideoPuffer = 0;
    VideoPuffer = VideoPufferBack[0];

	for(int i=0;i<8;i++)
	{
		MX[i] = 0;
		MY[i] = 0;
		MCOLOR[i] = 0;
        SpriteSeqOn[i] = false;
	}

	MX8 = 0;
	CTRL1 = 0;
	CTRL2 = 0;
    Y_SCROLL = 0;
    X_SCROLL = 0;
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

	DrawLineCounter = 0;
	LPTriggered = DrawThisLine = VBlanking = false;

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

	for(int i=0;i<VicConfigSizeof;i++) VicConfig[i] = true;
    Write_xd011 = false;

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

void VICII::SwitchVideoPuffer()
{
    AktVideoPuffer++;
    VideoPuffer = VideoPufferBack[AktVideoPuffer & 1];
}

void VICII::GetRegister(VIC_STRUCT *vic_reg)
{
	vic_reg->AktZyklus = AktZyklus;
        if(AktZyklus == 1) vic_reg->AktRasterzeile = AktRZ;
        else vic_reg->AktRasterzeile = AktRZ;
	vic_reg->RasterLatch = IRQ_RASTER;
	vic_reg->DisplayStatus = DisplayStatus;
	vic_reg->GrafikMode = GrafikMode;
	vic_reg->VicBank = *CIA2_PA & 3;
    vic_reg->MatrixBase = MatrixBase;
    vic_reg->CharBase = CharBase;
    vic_reg->IRQ = IRQFlag & 0x80;

	for(int i=0;i<8;i++)
	{
		vic_reg->SpriteX[i] = MX[i];
		vic_reg->SpriteY[i] = MY[i];
	}
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
		FIRST_DISP_LINE = FIRST_DISP_LINE_PAL;
		break;
	case 1:
		TOTAL_RASTERS=262;
		TOTAL_ZYKLEN_LINE=64;
		TOTAL_X=512;
		RASTER_Y = TOTAL_RASTERS - 1;
		FIRST_DISP_LINE = FIRST_DISP_LINE_NTSC;
		break;
	case 2:
		TOTAL_RASTERS=263;
		TOTAL_ZYKLEN_LINE=65;
		TOTAL_X=520;
		RASTER_Y = TOTAL_RASTERS - 1;
		FIRST_DISP_LINE = FIRST_DISP_LINE_NTSC;
		break;
	}
}

inline void VICII::RasterIRQ(void)
{
	IRQFlag |= 0x01;
	if (IRQMask & 0x01) 
	{
		IRQFlag |= 0x80;
		CpuTriggerInterrupt(VIC_IRQ);
	}
}

inline void VICII::SetBALow(void)
{
    if (*BA)
    {
		FirstBAZyklus = AktZyklus;
        *BA = false;
    }
}

inline void VICII::cZugriff(void)
{
    if (*BA == false)
	{
		/// Grafikdaten ///
        if(AktZyklus >= 15 && AktZyklus <= 54 && DisplayStatus == true && BadLineStatus == true)
        {
            cAdresse = (*CIA2_PA<<14)|MatrixBase|(VC&0x3FF);
            cDatenPuffer8Bit[VMLI] = Read(cAdresse);
            cDatenPuffer4Bit[VMLI] = FarbRam[(VC&0x3FF)]&0x0F;
        }
	}
}

inline void VICII::gZugriff(void)
{
    if (DisplayStatus) // DisplayModus
	{
		if (CTRL1 & 0x20) gAdresse = (*CIA2_PA<<14)|(BitmapBase)|((VC&0x3FF)<<3)|RC;
		else gAdresse = (*CIA2_PA<<14)|CharBase|(cDatenPuffer8Bit[VMLI]<<3)|RC;			
		if (CTRL1 & 0x40) gAdresse &= 0xf9ff;

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
		gAdresse = CTRL1 & 0x40 ? 0x39FF : 0x3FFF;
		gAdresse |=(*CIA2_PA<<14);
		GfxData = Read(gAdresse);
		CharData = 0;
		ColorData = 0;
	}
}

inline void VICII::pZugriff(unsigned char sp_nr)
{
	pAdresse = (*CIA2_PA<<14)|(MatrixBase)|0x3F8|sp_nr;
	pWert[sp_nr] = Read(pAdresse);
}

inline void VICII::sZugriff(unsigned char sp_nr)
{
	//// Es werden gleich alle 3 Bytes geladen ////
	sAdresse = (*CIA2_PA<<14)|(pWert[sp_nr]<<6)|(MC[sp_nr]&0x3F);
	SpriteSeq[sp_nr] = (Read(sAdresse))<<16;sAdresse++;
	SpriteSeq[sp_nr] |= (Read(sAdresse))<<8;sAdresse++;
	SpriteSeq[sp_nr] |= Read(sAdresse);
}

inline void VICII::CheckBorder(void)
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

inline void VICII::DrawGraphics(void)
{
	static unsigned short NEXT_X_SCROLL;

    if (!DrawThisLine) return;

    VideoPufferLine_XScroll = VideoPufferLine + 16;

    if(AktZyklus == 14)
	{
        for(int i=0;i<8;i++) VideoPufferLine_XScroll[i] = B0C | 0x40;
		NEXT_X_SCROLL = X_SCROLL;
	}

	VideoPufferLine_XScroll += NEXT_X_SCROLL;

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
					
				for(int i=7;i>-1;i--) *VideoPufferLine_XScroll++ = Colors[(GfxData>>(i))&1];
				VideoPufferLine += 8;
				break;
			case 1:
				/// Multicolor Text Modus (ECM/BMM/MCM = 0/0/1) ///
				if(!(ColorData&8))
				{
					/// MC-Flag = 0 ///
                    Colors[0] = B0C | 0x40;
					Colors[1] = (ColorData&7) | 0x80;

					for(int i=7;i>-1;i--) *VideoPufferLine_XScroll++ = Colors[(GfxData>>(i))&1];
					VideoPufferLine += 8;
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
                        *VideoPufferLine_XScroll = Colors[(GfxData>>(i))&3];
                        VideoPufferLine_XScroll++;
                        *VideoPufferLine_XScroll = VideoPufferLine_XScroll[-1];
                        VideoPufferLine_XScroll++;
					}
					VideoPufferLine += 8;
				}
				break;
			case 2:
				/// Standard Bitmap Modus (ECM/BMM/MCM = 0/1/0) ///
				Colors[0] = CharData&0x0F;
				Colors[1] = (CharData>>4) | 0x80;
				
				for(int i=7;i>-1;i--) *VideoPufferLine_XScroll++ = Colors[(GfxData>>(i))&1];			
				VideoPufferLine += 8;
				break;
			case 3:
				/// Multicolor Bitmap Modus (ECM/BMM/MCM = 0/1/1) ///
                Colors[0] = B0C | 0x40;
				Colors[1] = CharData>>4;
				Colors[2] = (CharData&0x0F) | 0x80;
				Colors[3] = ColorData | 0x80;
				
				for(int i=6;i>-1;i-=2)
				{
                    *VideoPufferLine_XScroll = Colors[(GfxData>>(i))&3];
                    VideoPufferLine_XScroll++;
                    *VideoPufferLine_XScroll = VideoPufferLine_XScroll[-1];
                    VideoPufferLine_XScroll++;
				}
				VideoPufferLine += 8;
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
					
				for(int i=7;i>-1;i--) *VideoPufferLine_XScroll++ = Colors[(GfxData>>(i))&1];
				VideoPufferLine += 8;
				break;
			default:
				*VideoPufferLine_XScroll++ = 0;
				*VideoPufferLine_XScroll++ = 0;
				*VideoPufferLine_XScroll++ = 0;
				*VideoPufferLine_XScroll++ = 0;
				*VideoPufferLine_XScroll++ = 0;
				*VideoPufferLine_XScroll++ = 0;
				*VideoPufferLine_XScroll++ = 0;
				*VideoPufferLine_XScroll++ = 0;
				VideoPufferLine += 8;
				break;
			}
		}
        else
        {
                *VideoPufferLine_XScroll++ = Colors[0];
                *VideoPufferLine_XScroll++ = Colors[0];
                *VideoPufferLine_XScroll++ = Colors[0];
                *VideoPufferLine_XScroll++ = Colors[0];
                *VideoPufferLine_XScroll++ = Colors[0];
                *VideoPufferLine_XScroll++ = Colors[0];
                *VideoPufferLine_XScroll++ = Colors[0];
                *VideoPufferLine_XScroll++ = Colors[0];
                VideoPufferLine += 8;
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
                for(int i=7;i>-1;i--) *VideoPufferLine_XScroll++ = Colors[(GfxData>>(i))&1];
				VideoPufferLine += 8;
				break;
			case 1:
				/// Multicolor Text Modus (ECM/BMM/MCM = 0/0/1) ///
				if(!(ColorData&8))
				{
                    /// MC-Flag = 0 ///
					Colors[1] = 0 | 0x80;

					for(int i=7;i>-1;i--) *VideoPufferLine_XScroll++ = Colors[(GfxData>>(i))&1];
					VideoPufferLine += 8;
				}
				else
				{
                    /// MC-Flag = 1 ///
					Colors[1] = 0;
					Colors[2] = 0 | 0x80;
					Colors[3] = 0 | 0x80;

					for(int i=6;i>-1;i-=2)
					{
                        *VideoPufferLine_XScroll = Colors[(GfxData>>(i))&3];
                        VideoPufferLine_XScroll++;
                        *VideoPufferLine_XScroll = VideoPufferLine_XScroll[-1];
                        VideoPufferLine_XScroll++;
					}
					VideoPufferLine += 8;
				}
				break;
			case 2:
				/// Standard Bitmap Modus (ECM/BMM/MCM = 0/1/0) ///
				Colors[1] = 0 | 0x80;
				
				for(int i=7;i>-1;i--) *VideoPufferLine_XScroll++ = Colors[(GfxData>>(i))&1];			
				VideoPufferLine += 8;
				break;
			case 3:
                /// Multicolor Bitmap Modus (ECM/BMM/MCM = 1/0/0) ///
                Colors[1] = 0;
				Colors[2] = 0 | 0x80;
				Colors[3] = 0 | 0x80;
				
				for(int i=6;i>-1;i-=2)
				{
                    *VideoPufferLine_XScroll = Colors[(GfxData>>(i))&3];
                    VideoPufferLine_XScroll++;
                    *VideoPufferLine_XScroll = VideoPufferLine_XScroll[-1];
                    VideoPufferLine_XScroll++;
				}
				VideoPufferLine += 8;

				break;
			default:
				*VideoPufferLine_XScroll++ = 0;
				*VideoPufferLine_XScroll++ = 0;
				*VideoPufferLine_XScroll++ = 0;
				*VideoPufferLine_XScroll++ = 0;
				*VideoPufferLine_XScroll++ = 0;
				*VideoPufferLine_XScroll++ = 0;
				*VideoPufferLine_XScroll++ = 0;
				*VideoPufferLine_XScroll++ = 0;
                VideoPufferLine += 8;
				break;
			}
		}
        else
        {
                *VideoPufferLine_XScroll++ = Colors[0];
                *VideoPufferLine_XScroll++ = Colors[0];
                *VideoPufferLine_XScroll++ = Colors[0];
                *VideoPufferLine_XScroll++ = Colors[0];
                *VideoPufferLine_XScroll++ = Colors[0];
                *VideoPufferLine_XScroll++ = Colors[0];
                *VideoPufferLine_XScroll++ = Colors[0];
                *VideoPufferLine_XScroll++ = Colors[0];
                VideoPufferLine += 8;
        }
	}

    /// Im Aktuellen Zyklus Prüfen ob B0C Color verwendet wurde (16 Pixel vorlauf)
    /// Wenn ja mit aktuellen B0C überschreiben

    VideoPufferLine_XScroll = VideoPufferLine - 8;

    /// Grey Dot zeichnen
    ///
    if(isWriteColorReg21 && (VideoPufferLine_XScroll[0] & 0x40) && DisplayStatus)
        VideoPufferLine_XScroll[0] = 0x0f;


    for(int i=0;i<8;i++)
    {
        if(VideoPufferLine_XScroll[i] & 0x40) VideoPufferLine_XScroll[i] =  B0C;
    }

    ////////////////////////// Draw Border //////////////////////////
    CheckBorder();
}

inline void VICII::DrawGraphicsPseudo()
{
    /// Für Anzeige Aller Zyklen
    VideoPufferLine += 8;

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
    if(!VicConfig[VIC_SPRITES_ON]) return;

    unsigned char mask=1;

    for(int SpriteNr=0; SpriteNr<8; SpriteNr++)
    {
        if(SpriteView & mask)
        {
            if((MX[SpriteNr] >= AktXKoordinate) && (MX[SpriteNr] < AktXKoordinate+8))
            {
                VideoPufferLine_XScroll = (VideoPufferLine-8) + (MX[SpriteNr]-AktXKoordinate);
                *VideoPufferLine_XScroll++ = SpriteNr+1;
            }
        }
        mask<<=1;
    }
    return;
    */

    /////////////////////////////////////////
    /////////////////////////////////////////

    if(!VicConfig[VIC_SPRITES_ON]) return;
	if(AktRZ < FIRST_DISP_LINE_PAL+1) return;

    // Spritekollisionspuffer löschen
    for (int i=0; i < 520; i++) SpriteCollisionsPuffer[i] = 0;

    int SpriteNr = 7;
    while(SpriteNr >= 0)
    {
        // Ist Sprite Sichtbar ?
        if(((SpriteViewAktLine & AktSpriteBit) == AktSpriteBit) && (MX[SpriteNr]<0x1F8))
		{
            SpritePufferLine = (VideoPufferLine - (62 * 8)) -4 + SpriteXDisplayTbl[MX[SpriteNr]];

            /// Temporärer Zeiger inerhalb des SpriteCollisionsPuffers auf das Aktuellen Sprites
            unsigned char *q = SpriteCollisionsPuffer + MX[SpriteNr] + 8;

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
    if(VicConfig[VIC_SPR_SPR_COLL_ON])
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
    if(VicConfig[VIC_SPR_BCK_COLL_ON])
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

inline void VICII::DrawBorder(void)
{
	if(!VicConfig[VIC_BORDER_ON]) return;
	if(AktRZ < FIRST_DISP_LINE_PAL) return;
    BorderPufferLine = VideoPufferLine - 504;	/// Erstes Sichtbares Pixel

    for(int x=0;x<504;x++)
	{
		if(BorderLine[x] & 128) BorderPufferLine++;
		else *BorderPufferLine++ = BorderLine[x];
	}
}

void VICII::OneZyklus(void)
{
    unsigned char SpriteBit = 0x01;

    AktXKoordinate = XKoordTbl[AktZyklus];

    // Prüfen ob Badlines zugelassen sind
    if(AktRZ == 0x30) BadLineEnable = DEN;

    // Prüfen auf Badline zustand
    if((AktRZ>=0x30) && (AktRZ<=0xF7) && (Y_SCROLL == (AktRZ&7)) && (BadLineEnable == true))
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
			RefreshProc(VideoPuffer);
			DrawLineCounter = 0;
			VCBASE = 0;
		}
		else 
		{

            AktRZ++;

			// Prüfen auf Raster IRQ
            //if (AktRZ == IRQ_RASTER) RasterIRQ();
			DrawThisLine = (AktRZ >= FIRST_DISP_LINE && AktRZ <= LAST_DISP_LINE);
		}

        if(DrawThisLine)
        {
            VideoPufferLine = &VideoPuffer[DrawLineCounter++*MAX_XW];		// Zeiger für Aktuelle Zeile setzen
            BorderLinePos = 0;
        }

		// Sprite 2 //
        if((SpriteDMA & 0x18) == 0) *BA = true;

		// Sprite 3 //
		pZugriff(3);
        if(!*BA) sZugriff(3);

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
        if((SpriteDMA & 0x30) == 0) *BA = true;

		// Sprite 4 //
		pZugriff(4);
        if(!*BA) sZugriff(4);

        DrawGraphicsPseudo();
		break;

	case 4:
		/// Sprite 6 ///
		if(SpriteDMA & 0x40)  SetBALow();

        DrawGraphicsPseudo();
		break;

	case 5:
        // Sprite 4 //
        if((SpriteDMA & 0x60) == 0) *BA = true;

		// Sprite 5 //
		pZugriff(5);
        if(!*BA) sZugriff(5);

        DrawGraphicsPseudo();
		break;

	case 6:
		/// Sprite 7 ///
		if(SpriteDMA & 0x80)  SetBALow();

        DrawGraphicsPseudo();
		break;

	case 7:
        // Sprite 5 //
        if((SpriteDMA & 0xC0) == 0) *BA = true;

		// Sprite 6 //
		pZugriff(6);
        if(!*BA) sZugriff(6);

        DrawGraphicsPseudo();
		break;

	case 8:
        DrawGraphicsPseudo();
		break;

	case 9:
		// Sprite 6 //
        if((SpriteDMA & 0x80) == 0) *BA = true;

		// Sprite 7 //
		pZugriff(7);
        if(!*BA) sZugriff(7);

        VMLI = 0;

        DrawGraphicsPseudo();
        break;

    case 10:
        VMLI = 0;

        DrawGraphicsPseudo();
		break;

	case 11:
		// Sprite 7 //
        *BA = true;

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

        gZugriff();
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

        gZugriff();
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

		gZugriff();
        if (BadLineStatus) SetBALow();

        DrawGraphics();
		break;

    case 55:
        *BA = true;

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
            if ((ME & SpriteBit) && ((AktRZ & 0xff) == MY[i]))
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

        gZugriff();
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
            if ((ME & SpriteBit) && (AktRZ & 0xff) == MY[i])
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
			//if(((SpriteDMA & bitc) == bitc) && ((AktRZ&0xFF) == MY[i])) SpriteView |= bitc;
            if(((SpriteDMA & SpriteBit) == SpriteBit)) SpriteView |= SpriteBit;
		}

		// Sprite 0 //
		pZugriff(0);
        if(!*BA) sZugriff(0);

        DrawGraphics();
		break;

	case 59:
		/// Sprite 2 ///
		if(SpriteDMA & 0x04)  SetBALow();

        DrawGraphics();
		break;
	
	case 60:
		// Sprite 0 //
        if((SpriteDMA & 0x06) == 0) *BA = true;

		// Sprite 1 //
		pZugriff(1);
        if(!*BA) sZugriff(1);

        DrawGraphics();
		break;

	case 61:
		/// Sprite 3 ///
        if(SpriteDMA & 0x08)  SetBALow();

        DrawGraphics();
		break;

	case 62:
		// Sprite 1 //
        if((SpriteDMA & 0x0C) == 0) *BA = true;

		// Sprite 2 //
		pZugriff(2);
        if(!*BA) sZugriff(2);

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

    cZugriff();     // cZugriffe immer im 2.Teil eines Zyklus wenn BA Low ist

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
        if(Breakpoints[AktRZ+1] & 256)
        {
                *BreakStatus |= 256;
                BreakWerte[8] = AktRZ+1;
        }
    }
    else
    {
        if(Breakpoints[AktRZ] & 256)
        {
                *BreakStatus |= 256;
                BreakWerte[8] = AktRZ;
        }
    }

	if(Breakpoints[AktZyklus] & 512) 
	{
		*BreakStatus |= 512;
		BreakWerte[9] = AktZyklus;
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
	Offset = VideoPufferLine - VideoPuffer;
	fw(Offset);
	
	fw(DrawLineCounter);
	fw(DrawThisLine);
	fw(bitc);
	fw1(MX,8);
	fw1(MY,8);
	fw(MX8);
	fw(CTRL1);
	fw(CTRL2);
    fw(Y_SCROLL);
    fw(X_SCROLL);
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
		VideoPufferLine = Offset + VideoPuffer;
		
		fr(DrawLineCounter);
		fr(DrawThisLine);
		fr(bitc);
		fr1(MX,8);
		fr1(MY,8);
		fr(MX8);
		fr(CTRL1);
		fr(CTRL2);
		fr(Y_SCROLL);
		fr(X_SCROLL);
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
void VICII::WriteIO(unsigned short adresse,unsigned char wert)
{
	adresse &= 63;
	switch (adresse) 
	{
		/// Sprite X Position
		case 0x00: case 0x02: case 0x04: case 0x06:
		case 0x08: case 0x0A: case 0x0C: case 0x0E:
                        MX[adresse >> 1] = (MX[adresse >> 1] & 0xFF00) | wert;
		break;

		/// Sprite Y Position
		case 0x01: case 0x03: case 0x05: case 0x07:
		case 0x09: case 0x0B: case 0x0D: case 0x0F:
                        MY[adresse >> 1] = wert;
		break;

		/// Sprite X Position MSB
		case 0x10:
			int i, j;
                        MX8 = wert;
			for (i=0, j=1; i<8; i++, j<<=1)
			{
                                if (MX8 & j) MX[i] |= 0x100;
                                else MX[i] &= 0xFF;
			}
			break;

		// Control Register 1
		case 0x11:
                        CTRL1 = wert;
                        Y_SCROLL = wert & 7;

                        NEW_IRQ_RASTER = (IRQ_RASTER & 0xFF) | ((wert & 0x80) << 1);
                        if (IRQ_RASTER != NEW_IRQ_RASTER && AktRZ == NEW_IRQ_RASTER) RasterIRQ();
                        IRQ_RASTER = NEW_IRQ_RASTER;
			
                        GrafikMode = ((CTRL1 & 0x60) | (CTRL2 & 0x10)) >> 4;
		
			// Prüfen ob Badlines zugelassen sind
                        if ((AktRZ == 0x30) && (wert & 0x10)) BadLineEnable = true;

			// Prüfen auf Badline zustand
                        if((AktRZ>=0x30) && (AktRZ<=0xF7) && (Y_SCROLL == (AktRZ&7)) && (BadLineEnable == true))
			{
                                BadLineStatus = true;
			}
            else BadLineStatus = false;

			// RSEL
            RSEL = (wert & 0x08)>>3;

			// DEN
            DEN = (wert & 0x10)>>4;
            Write_xd011 = true;
			break;

		/// Rasterzähler
		case 0x12:
                        //IRQ_RASTER = (IRQ_RASTER & 0xFF00) | wert;

                        NEW_IRQ_RASTER = (IRQ_RASTER & 0xFF00) | wert;
                        if (IRQ_RASTER != NEW_IRQ_RASTER && AktRZ == NEW_IRQ_RASTER) RasterIRQ();
                        IRQ_RASTER = NEW_IRQ_RASTER;
			break;

		/// Sprite Enable
		case 0x15:
            ME = wert;
			break;

		// Control Register 2
		case 0x16:
            CTRL2 = wert;
            X_SCROLL = wert & 7;
            GrafikMode = ((CTRL1 & 0x60) | (CTRL2 & 0x10)) >> 4;
			
			// CSEL
            CSEL = (wert & 0x08)>>3;
			
			break;

		/// Sprite Y-Expansion
		case 0x17:
                        MYE = wert;
                        SpriteExpYFlipFlop |= ~wert;
			break;

		/// Speicher Pointer
		case 0x18:
                        VBASE = wert;
                        MatrixBase = (wert & 0xf0) << 6;
                        CharBase = (wert & 0x0e) << 10;
                        BitmapBase = (wert & 0x08) << 10;
			break;

		/// IRQ Flags
		case 0x19: 
                        IRQFlag = IRQFlag & (~wert & 0x0F);
                        if (IRQFlag & IRQMask) IRQFlag |= 0x80;
                        else CpuClearInterrupt(VIC_IRQ);
			break;
		
		/// IRQ Mask
		case 0x1A:
                        IRQMask = wert & 0x0F;
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
                        MDP = wert;
			break;

		/// Sprite Multicolor
		case 0x1C:
                        MMC = wert;
			break;

		/// Sprite X-Expansion
		case 0x1D:
                        MXE = wert;
			break;

		/// Rahmenfarbe
		case 0x20:
                        EC = wert&15;
                        if(VicConfig[VIC_GREY_DOTS_ON]) isWriteColorReg20 = true;
			break;
		/// Hintergrundfarbe 0
        case 0x21:
                        B0C = wert&15;
                        if(VicConfig[VIC_GREY_DOTS_ON]) isWriteColorReg21 = true;
			break;
		/// Hintergrundfarbe 1
                case 0x22: B1C	= wert&15;
                        //isWriteColorReg = true;
			break;
		/// Hintergrundfarbe 2
                case 0x23: B2C	= wert&15;
                        //isWriteColorReg = true;
			break;
		/// Hintergrundfarbe 3
                case 0x24: B3C	= wert&15;
                        //isWriteColorReg = true;
			break;
		/// Sprite Multicolor 0
                case 0x25: MM0  = wert&15;
                        //isWriteColorReg = true;
			break;
		/// Sprite Multicolor 1
                case 0x26: MM1  = wert&15;
                        //isWriteColorReg = true;
			break;

		/// Sprite Farbe
		case 0x27: case 0x28: case 0x29: case 0x2A:
		case 0x2B: case 0x2C: case 0x2D: case 0x2E:
                        MCOLOR[adresse - 0x27] = wert&15;
			break;
	}
}

unsigned char VICII::ReadIO(unsigned short adresse)
{
	adresse &= 63;
	switch (adresse) 
	{
		/// Sprite X Position
		case 0x00: case 0x02: case 0x04: case 0x06:
		case 0x08: case 0x0A: case 0x0C: case 0x0E:
                        return (unsigned char)MX[adresse >> 1];

		/// Sprite Y Position
		case 0x01: case 0x03: case 0x05: case 0x07:
		case 0x09: case 0x0B: case 0x0D: case 0x0F:
                        return MY[adresse >> 1];

		/// Sprite X Position MSB
		case 0x10:	
                        return MX8;

		/// Control Register 1
		case 0x11:	
                    if(Write_xd011) return (CTRL1 & 0x7F) | ((AktRZ & 0x100) >> 1);
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
                        return CTRL2 | 0xC0;

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
            return MCOLOR[adresse - 0x27] | 0xf0;

		default:
			return 0xFF;
	}
}

void VICII::TriggerLightpen(void)
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
