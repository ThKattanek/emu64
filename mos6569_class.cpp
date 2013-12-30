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
// Letzte Änderung am 30.12.2013                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "mos6569_class.h"
#include <stdio.h>


#ifdef NEW
/////////////////////////// NEUE VERSION ////////////////////////////////

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

    GreyDotEnable = false;
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
    SpriteCollisionEnable = true;

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

void VICII::EnableSpriteCollision(bool enabled)
{
    SpriteCollisionEnable = enabled;
}

void VICII::EnableGreyDot(bool enabled)
{
	GreyDotEnable = enabled;
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
            int i=8;
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
    if(SpriteCollisionEnable)
    {
        unsigned int *lp = (unsigned int *)SpriteCollisionsPuffer - 1;
        for (int i=0; i < TOTAL_X/4; i++) *++lp = 0;
    }

	bitc = 0x80;
    int SpriteNr = 7;
    while(SpriteNr >= 0)
    {
        // Ist Sprite Sichtbar ?
        if(((SpriteViewAktLine & bitc) == bitc) && (MX[SpriteNr]<0x1F8))
		{
            SpritePufferLine = (VideoPufferLine - (62 * 8)) -4 + SpriteXDisplayTbl[MX[SpriteNr]];
			if((MDP & bitc) == 0)	/// Prüfen auf Sprite Hinter Vordergrund
			{
				/// Sprites vor der Vordergrundgrafik
				if((MXE & bitc) == bitc)
				{
					if((MMC & bitc) == bitc)
					{
                        for(int i=0;i<12;i++)
						{
                            switch(SpriteSeqAktLine[SpriteNr] & 0xC00000)
							{
							case 0x000000:
								SpritePufferLine += 4;
								break;
							case 0x400000:
								*SpritePufferLine++ = MM0;
								*SpritePufferLine++ = MM0;
								*SpritePufferLine++ = MM0;
								*SpritePufferLine++ = MM0;
								break;
							case 0x800000:
                                *SpritePufferLine++ = MCOLOR[SpriteNr];
                                *SpritePufferLine++ = MCOLOR[SpriteNr];
                                *SpritePufferLine++ = MCOLOR[SpriteNr];
                                *SpritePufferLine++ = MCOLOR[SpriteNr];
								break;
							case 0xC00000:
								*SpritePufferLine++ = MM1;
								*SpritePufferLine++ = MM1;
								*SpritePufferLine++ = MM1;
								*SpritePufferLine++ = MM1;
								break;
							}
                            SpriteSeqAktLine[SpriteNr] = SpriteSeqAktLine[SpriteNr] << 2;
						}
					}
					else
					{
                        for(int i=0;i<24;i++)
						{
                            if(SpriteSeqAktLine[SpriteNr] & 0x800000)
							{
                                *SpritePufferLine++ = MCOLOR[SpriteNr];
                                *SpritePufferLine++ = MCOLOR[SpriteNr];
							}
							else SpritePufferLine += 2;
                            SpriteSeqAktLine[SpriteNr] = SpriteSeqAktLine[SpriteNr] << 1;
						}
					}
				}
				else
				{
					if((MMC & bitc) == bitc)
					{
                        for(int i=0;i<12;i++)
						{
                            switch(SpriteSeqAktLine[SpriteNr] & 0xC00000)
							{
							case 0x000000:
								SpritePufferLine += 2;
								break;
							case 0x400000:
								*SpritePufferLine++ = MM0;
								*SpritePufferLine++ = MM0;
								break;
							case 0x800000:
                                *SpritePufferLine++ = MCOLOR[SpriteNr];
                                *SpritePufferLine++ = MCOLOR[SpriteNr];
								break;
							case 0xC00000:
								*SpritePufferLine++ = MM1;
								*SpritePufferLine++ = MM1;
								break;
							}
                            SpriteSeqAktLine[SpriteNr] = SpriteSeqAktLine[SpriteNr] << 2;
						}
					}
					else
					{
                        for(int i=0;i<24;i++)
						{
                            if(SpriteSeqAktLine[SpriteNr] & 0x800000) *SpritePufferLine++ = MCOLOR[SpriteNr];
							else SpritePufferLine++;
                            SpriteSeqAktLine[SpriteNr] = SpriteSeqAktLine[SpriteNr] << 1;
						}
					}
				}
			}
			else
			{
				/// Sprites hinter der Vordergrundgrafik
				if((MXE & bitc) == bitc)
				{
					if((MMC & bitc) == bitc)
					{
                        for(int i=0;i<12;i++)
						{
                            switch(SpriteSeqAktLine[SpriteNr] & 0xC00000)
							{
							case 0x000000:
								SpritePufferLine += 4;
								break;
							case 0x400000:
								if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MM0;
								else SpritePufferLine++;
								if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MM0;
								else SpritePufferLine++;
								if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MM0;
								else SpritePufferLine++;
								if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MM0;
								else SpritePufferLine++;
								break;
							case 0x800000:
                                if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MCOLOR[SpriteNr];
								else SpritePufferLine++;
                                if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MCOLOR[SpriteNr];
								else SpritePufferLine++;
                                if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MCOLOR[SpriteNr];
								else SpritePufferLine++;
                                if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MCOLOR[SpriteNr];
								else SpritePufferLine++;
								break;
							case 0xC00000:
								if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MM1;
								else SpritePufferLine++;
								if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MM1;
								else SpritePufferLine++;
								if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MM1;
								else SpritePufferLine++;
								if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MM1;
								else SpritePufferLine++;
								break;
							}
                            SpriteSeqAktLine[SpriteNr] = SpriteSeqAktLine[SpriteNr] << 2;
						}
					}
					else
					{
                        for(int i=0;i<24;i++)
						{
                            if(SpriteSeqAktLine[SpriteNr] & 0x800000)
							{
                                if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MCOLOR[SpriteNr];
								else SpritePufferLine++;
                                if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MCOLOR[SpriteNr];
								else SpritePufferLine++;
							}
							else SpritePufferLine += 2;
                            SpriteSeqAktLine[SpriteNr] = SpriteSeqAktLine[SpriteNr] << 1;
						}
					}
				}
				else
				{
					if((MMC & bitc) == bitc)
					{
                        for(int i=0;i<12;i++)
						{
                            switch(SpriteSeqAktLine[SpriteNr] & 0xC00000)
							{
							case 0x000000:
								SpritePufferLine += 2;
								break;
							case 0x400000:
								if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MM0;
								else SpritePufferLine++;
								if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MM0;
								else SpritePufferLine++;
								break;
							case 0x800000:
                                if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MCOLOR[SpriteNr];
								else SpritePufferLine++;
                                if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MCOLOR[SpriteNr];
								else SpritePufferLine++;
								break;
							case 0xC00000:
								if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MM1;
								else SpritePufferLine++;
								if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MM1;
								else SpritePufferLine++;
								break;
							}
                            SpriteSeqAktLine[SpriteNr] = SpriteSeqAktLine[SpriteNr] << 2;
						}
					}
					else
					{
                        for(int i=0;i<24;i++)
						{
                            if(SpriteSeqAktLine[SpriteNr] & 0x800000)
							{
                                if(!(*SpritePufferLine&0x80))*SpritePufferLine++ = MCOLOR[SpriteNr];
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
		bitc = bitc >> 1;
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

inline void VICII::Reset(void)
{
    //WriteIO(0x11,0);
    //WriteIO(0x12,0);
    Write_xd011 = false;
}

void VICII::OneZyklus(void)
{
    static bool OLD_RESET = *RESET;

    AktXKoordinate = XKoordTbl[AktZyklus];

    if(!*RESET)
	{
		CTRL2 = 5;
		CSEL = false;
		X_SCROLL = CTRL2 & 7;
		GrafikMode = ((CTRL1 & 0x60) | (CTRL2 & 0x10)) >> 4;
	}
	else if(!OLD_RESET) Reset();
        OLD_RESET = *RESET;

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
        bitc = 1;
        for(int i=0;i<8;i++,bitc<<=1)
        {
            if(SpriteDMA & bitc) MC[i] += 3;
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
        bitc = 0x01;
        for(int i=0;i<8;i++,bitc<<=1)
        {
            if((SpriteExpYFlipFlop & bitc) == bitc) MCBase[i] += 2;
        }

        gZugriff();
        if (BadLineStatus) SetBALow();

        DrawGraphics();
		break;

    case 17:

        /// Sprite ///
        bitc = 0x01;
        for(int i=0;i<8;i++,bitc<<=1)
        {
            if((SpriteExpYFlipFlop & bitc) == bitc) MCBase[i] ++;
            if(MCBase[i] >= 63)
            {
                SpriteDMA  &= ~bitc;
                SpriteView &= ~bitc;
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
		bitc = 0x01;
		for (int i=0; i<8; i++, bitc<<=1) if (MYE & bitc) SpriteExpYFlipFlop ^= bitc;

		// In der ersten Phasen von Zyklus 55 wird für Sprite 0-3 geprüft, 
		// ob das entsprechende MxE-Bit in Register $d015 gesetzt und die Y-Koordinate 
		// des Sprites (ungerade Register $d001-$d00f) gleich den unteren 8 Bits 
		// von RASTER ist. Ist dies der Fall und der DMA für das Sprite noch 
		// ausgeschaltet, wird der DMA angeschaltet, MCBASE gelöscht und, 
		// wenn das MxYE-Bit gesetzt ist, das Expansions-Flipflop gelöscht.
		bitc = 0x01;
		for (int i=0; i<4; i++, bitc<<=1)
		{
			if ((ME & bitc) && ((AktRZ & 0xff) == MY[i]))
			{
				if((SpriteDMA & bitc) == 0)
				{
					SpriteDMA |= bitc;
					MCBase[i] = 0;
					if (MYE & bitc) SpriteExpYFlipFlop &= ~bitc;
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
		bitc = 0x10;
		for (int i=4; i<8; i++, bitc<<=1)
		{
			if ((ME & bitc) && (AktRZ & 0xff) == MY[i])
			{
				if((SpriteDMA & bitc) == 0)
				{
					SpriteDMA |= bitc;
					MCBase[i] = 0;
					if (MYE & bitc) SpriteExpYFlipFlop &= ~bitc;
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
		bitc = 0x01;
		for(int i=0;i<8;i++)
		{
			MC[i] = MCBase[i];
			//if(((SpriteDMA & bitc) == bitc) && ((AktRZ&0xFF) == MY[i])) SpriteView |= bitc;
			if(((SpriteDMA & bitc) == bitc)) SpriteView |= bitc;
			bitc = bitc << 1;
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
                        if(GreyDotEnable)isWriteColorReg20 = true;
			break;
		/// Hintergrundfarbe 0
                case 0x21: B0C = wert&15;
                        if(GreyDotEnable)isWriteColorReg21 = true;
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
			return 0;

		/// Sprite - Daten Kollision
		case 0x1F:
			return 0;

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

#else

/////////////////////////// ALTE VERSION ////////////////////////////////

#define Read(adresse) ReadProcTbl[(adresse)>>8](adresse)

///////////////////////////////////////////////////////////////////////////////////////////////////

// Rahmen Y Start und Stop
#define RAHMEN25_YSTART		0x33
#define RAHMEN25_YSTOP		0xfb
#define RAHMEN24_YSTART		0x37
#define RAHMEN24_YSTOP		0xf7

// erste und letzte Display Zeile
#define FIRST_DISP_LINE_PAL		0x10
#define FIRST_DISP_LINE_NTSC	0x10 + 14

#define LAST_DISP_LINE		0x11F

// erste and letzte mögliche Rasterzeile für Bad Lines
#define FIRST_DMA_LINE		0x30
#define LAST_DMA_LINE		0xf7


// länge der X Werte der Destination BitMap
#define XMOD				504
#define YMOD                312

///////////////////////////////////////////////////////////////////////////////////////////////////

// Tables for sprite X expansion
unsigned short int ExpTable[256] = {
	0x0000, 0x0003, 0x000C, 0x000F, 0x0030, 0x0033, 0x003C, 0x003F,
	0x00C0, 0x00C3, 0x00CC, 0x00CF, 0x00F0, 0x00F3, 0x00FC, 0x00FF,
	0x0300, 0x0303, 0x030C, 0x030F, 0x0330, 0x0333, 0x033C, 0x033F,
	0x03C0, 0x03C3, 0x03CC, 0x03CF, 0x03F0, 0x03F3, 0x03FC, 0x03FF,
	0x0C00, 0x0C03, 0x0C0C, 0x0C0F, 0x0C30, 0x0C33, 0x0C3C, 0x0C3F,
	0x0CC0, 0x0CC3, 0x0CCC, 0x0CCF, 0x0CF0, 0x0CF3, 0x0CFC, 0x0CFF,
	0x0F00, 0x0F03, 0x0F0C, 0x0F0F, 0x0F30, 0x0F33, 0x0F3C, 0x0F3F,
	0x0FC0, 0x0FC3, 0x0FCC, 0x0FCF, 0x0FF0, 0x0FF3, 0x0FFC, 0x0FFF,
	0x3000, 0x3003, 0x300C, 0x300F, 0x3030, 0x3033, 0x303C, 0x303F,
	0x30C0, 0x30C3, 0x30CC, 0x30CF, 0x30F0, 0x30F3, 0x30FC, 0x30FF,
	0x3300, 0x3303, 0x330C, 0x330F, 0x3330, 0x3333, 0x333C, 0x333F,
	0x33C0, 0x33C3, 0x33CC, 0x33CF, 0x33F0, 0x33F3, 0x33FC, 0x33FF,
	0x3C00, 0x3C03, 0x3C0C, 0x3C0F, 0x3C30, 0x3C33, 0x3C3C, 0x3C3F,
	0x3CC0, 0x3CC3, 0x3CCC, 0x3CCF, 0x3CF0, 0x3CF3, 0x3CFC, 0x3CFF,
	0x3F00, 0x3F03, 0x3F0C, 0x3F0F, 0x3F30, 0x3F33, 0x3F3C, 0x3F3F,
	0x3FC0, 0x3FC3, 0x3FCC, 0x3FCF, 0x3FF0, 0x3FF3, 0x3FFC, 0x3FFF,
	0xC000, 0xC003, 0xC00C, 0xC00F, 0xC030, 0xC033, 0xC03C, 0xC03F,
	0xC0C0, 0xC0C3, 0xC0CC, 0xC0CF, 0xC0F0, 0xC0F3, 0xC0FC, 0xC0FF,
	0xC300, 0xC303, 0xC30C, 0xC30F, 0xC330, 0xC333, 0xC33C, 0xC33F,
	0xC3C0, 0xC3C3, 0xC3CC, 0xC3CF, 0xC3F0, 0xC3F3, 0xC3FC, 0xC3FF,
	0xCC00, 0xCC03, 0xCC0C, 0xCC0F, 0xCC30, 0xCC33, 0xCC3C, 0xCC3F,
	0xCCC0, 0xCCC3, 0xCCCC, 0xCCCF, 0xCCF0, 0xCCF3, 0xCCFC, 0xCCFF,
	0xCF00, 0xCF03, 0xCF0C, 0xCF0F, 0xCF30, 0xCF33, 0xCF3C, 0xCF3F,
	0xCFC0, 0xCFC3, 0xCFCC, 0xCFCF, 0xCFF0, 0xCFF3, 0xCFFC, 0xCFFF,
	0xF000, 0xF003, 0xF00C, 0xF00F, 0xF030, 0xF033, 0xF03C, 0xF03F,
	0xF0C0, 0xF0C3, 0xF0CC, 0xF0CF, 0xF0F0, 0xF0F3, 0xF0FC, 0xF0FF,
	0xF300, 0xF303, 0xF30C, 0xF30F, 0xF330, 0xF333, 0xF33C, 0xF33F,
	0xF3C0, 0xF3C3, 0xF3CC, 0xF3CF, 0xF3F0, 0xF3F3, 0xF3FC, 0xF3FF,
	0xFC00, 0xFC03, 0xFC0C, 0xFC0F, 0xFC30, 0xFC33, 0xFC3C, 0xFC3F,
	0xFCC0, 0xFCC3, 0xFCCC, 0xFCCF, 0xFCF0, 0xFCF3, 0xFCFC, 0xFCFF,
	0xFF00, 0xFF03, 0xFF0C, 0xFF0F, 0xFF30, 0xFF33, 0xFF3C, 0xFF3F,
	0xFFC0, 0xFFC3, 0xFFCC, 0xFFCF, 0xFFF0, 0xFFF3, 0xFFFC, 0xFFFF
};

unsigned short int MultiExpTable[256] = {
	0x0000, 0x0005, 0x000A, 0x000F, 0x0050, 0x0055, 0x005A, 0x005F,
	0x00A0, 0x00A5, 0x00AA, 0x00AF, 0x00F0, 0x00F5, 0x00FA, 0x00FF,
	0x0500, 0x0505, 0x050A, 0x050F, 0x0550, 0x0555, 0x055A, 0x055F,
	0x05A0, 0x05A5, 0x05AA, 0x05AF, 0x05F0, 0x05F5, 0x05FA, 0x05FF,
	0x0A00, 0x0A05, 0x0A0A, 0x0A0F, 0x0A50, 0x0A55, 0x0A5A, 0x0A5F,
	0x0AA0, 0x0AA5, 0x0AAA, 0x0AAF, 0x0AF0, 0x0AF5, 0x0AFA, 0x0AFF,
	0x0F00, 0x0F05, 0x0F0A, 0x0F0F, 0x0F50, 0x0F55, 0x0F5A, 0x0F5F,
	0x0FA0, 0x0FA5, 0x0FAA, 0x0FAF, 0x0FF0, 0x0FF5, 0x0FFA, 0x0FFF,
	0x5000, 0x5005, 0x500A, 0x500F, 0x5050, 0x5055, 0x505A, 0x505F,
	0x50A0, 0x50A5, 0x50AA, 0x50AF, 0x50F0, 0x50F5, 0x50FA, 0x50FF,
	0x5500, 0x5505, 0x550A, 0x550F, 0x5550, 0x5555, 0x555A, 0x555F,
	0x55A0, 0x55A5, 0x55AA, 0x55AF, 0x55F0, 0x55F5, 0x55FA, 0x55FF,
	0x5A00, 0x5A05, 0x5A0A, 0x5A0F, 0x5A50, 0x5A55, 0x5A5A, 0x5A5F,
	0x5AA0, 0x5AA5, 0x5AAA, 0x5AAF, 0x5AF0, 0x5AF5, 0x5AFA, 0x5AFF,
	0x5F00, 0x5F05, 0x5F0A, 0x5F0F, 0x5F50, 0x5F55, 0x5F5A, 0x5F5F,
	0x5FA0, 0x5FA5, 0x5FAA, 0x5FAF, 0x5FF0, 0x5FF5, 0x5FFA, 0x5FFF,
	0xA000, 0xA005, 0xA00A, 0xA00F, 0xA050, 0xA055, 0xA05A, 0xA05F,
	0xA0A0, 0xA0A5, 0xA0AA, 0xA0AF, 0xA0F0, 0xA0F5, 0xA0FA, 0xA0FF,
	0xA500, 0xA505, 0xA50A, 0xA50F, 0xA550, 0xA555, 0xA55A, 0xA55F,
	0xA5A0, 0xA5A5, 0xA5AA, 0xA5AF, 0xA5F0, 0xA5F5, 0xA5FA, 0xA5FF,
	0xAA00, 0xAA05, 0xAA0A, 0xAA0F, 0xAA50, 0xAA55, 0xAA5A, 0xAA5F,
	0xAAA0, 0xAAA5, 0xAAAA, 0xAAAF, 0xAAF0, 0xAAF5, 0xAAFA, 0xAAFF,
	0xAF00, 0xAF05, 0xAF0A, 0xAF0F, 0xAF50, 0xAF55, 0xAF5A, 0xAF5F,
	0xAFA0, 0xAFA5, 0xAFAA, 0xAFAF, 0xAFF0, 0xAFF5, 0xAFFA, 0xAFFF,
	0xF000, 0xF005, 0xF00A, 0xF00F, 0xF050, 0xF055, 0xF05A, 0xF05F,
	0xF0A0, 0xF0A5, 0xF0AA, 0xF0AF, 0xF0F0, 0xF0F5, 0xF0FA, 0xF0FF,
	0xF500, 0xF505, 0xF50A, 0xF50F, 0xF550, 0xF555, 0xF55A, 0xF55F,
	0xF5A0, 0xF5A5, 0xF5AA, 0xF5AF, 0xF5F0, 0xF5F5, 0xF5FA, 0xF5FF,
	0xFA00, 0xFA05, 0xFA0A, 0xFA0F, 0xFA50, 0xFA55, 0xFA5A, 0xFA5F,
	0xFAA0, 0xFAA5, 0xFAAA, 0xFAAF, 0xFAF0, 0xFAF5, 0xFAFA, 0xFAFF,
	0xFF00, 0xFF05, 0xFF0A, 0xFF0F, 0xFF50, 0xFF55, 0xFF5A, 0xFF5F,
	0xFFA0, 0xFFA5, 0xFFAA, 0xFFAF, 0xFFF0, 0xFFF5, 0xFFFA, 0xFFFF
};

VICII *_vic;

VICII::VICII()
{
	_vic = this;
	SetVicType(0);

	Zyklus=1;

	ChunkyPtr = ChunkyLineStart = VideoPuffer;
	XMod = XMOD;

	MatrixBase = 0;
	CharBase = 0;
	BitmapBase = 0;

	MX8 = 0;
	CTRL1 = CTRL2 = 0;
	LPX = LPY = 0;
	ME = MXE = MYE = MDP = MMC = 0;
	VBASE = IRQFlag = IRQMask = 0;
	ClxSpr = ClxBgr = 0;
	CiaVabase = 0;

	Rc = 7;
	IRQ_RASTER = Vc = VcBase = X_SCROLL = Y_SCROLL = 0;
	DYStart = RAHMEN24_YSTART;
	DYStop = RAHMEN24_YSTOP;
	MLIndex = 0;

	DisplayIDX = 0;
	DisplayState = false;
	BorderOn = UdBorderOn = VBlanking = false;
	LPTriggered = DrawThisLine = false;

	SprDmaOn = SprDispOn = 0;
	for (int i=0; i<8; i++) 
	{
		Mc[i] = 63;
		SprPtr[i] = 0;
	}

	memset(SprCollBuf, 0, TOTAL_X);
	memset(ForeMaskBuf, 0, TOTAL_X/8);

	EC = B0C = B1C = B2C = B3C = MM0 = MM1 = 0;
	for (int i=0; i<8; i++) MX[i] = MY[i] = SC[i] = 0;

	GreyDotEnable = false;
	isWriteColorReg20 = false;
	isWriteColorReg21 = false;

	AktVideoPuffer = 0;
	VideoPuffer = VideoPuffer0;
}

VICII::~VICII()
{
}

void VICII::GetRegister(VIC_STRUCT *vic_reg)
{
	vic_reg->AktRasterzeile = RASTER_Y;
	vic_reg->AktZyklus = Zyklus;
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

void VICII::EnableGreyDot(bool enabled)
{
	GreyDotEnable = enabled;
}

#define SetBALow if (*BaLine) {FirstBaCycle = Zyklus; *BaLine = false;}
//#define SetBAHigh if (*RESET) *BaLine = true
#define SetBAHigh *BaLine = true
#define DisplayIfBadLine if(IsBadLine) DisplayState = true;
#define FetchIfBadLine if (IsBadLine) {DisplayState = true; SetBALow;}
#define IdleAccess Read(0x3fff | CiaVabase)
#define RefreshAccess Read(0x3f00 | RefCnt-- | CiaVabase)
#define SprPtrAccess(num) SprPtr[num] = Read(MatrixBase | 0x03f8 | num | CiaVabase) << 6;
#define SprDataAccess(num, bytenum) if (SprDmaOn & (1 << num)) {SprData[num][bytenum] = Read(Mc[num] & 0x3f | SprPtr[num] | CiaVabase); Mc[num]++;} else if (bytenum == 1) IdleAccess;
#define SampleBorder if (DrawThisLine) { if(BorderOn){ BorderColorSample[Zyklus-13] = EC;if(isWriteColorReg20)BorderGreyDotSample[Zyklus-13] = true;else BorderGreyDotSample[Zyklus-13]=false;} ChunkyPtr += 8; ForeMaskPtr++;}
#define RCIfBadLine if (IsBadLine) {DisplayState = true; Rc = 0; SetBALow;}
#define CheckSpriteDMA \
	mask = 1; \
	for (i=0; i<8; i++, mask<<=1) \
		if ((ME & mask) && (RASTER_Y & 0xff) == MY[i]) { \
			SprDmaOn |= mask; \
			McBase[i] = 0; \
			if (MYE & mask) \
				SprExpY &= ~mask; \
		}

void VICII::OneZyklus(void)
{	
	unsigned char mask;
	int i;

	CiaVabase = (*CIA2_PA) << 14;
	
        if(!*RESET)
	{
		WriteIO(0x11,0);
		WriteIO(0x12,0);
		/*
		CTRL2 = 5;
		X_SCROLL = CTRL2 & 7;
		DisplayIDX = ((CTRL1 & 0x60) | (CTRL2 & 0x10)) >> 4;
		*/
		IRQMask = 0;
		IRQFlag = 0;
	}

	switch (Zyklus)
	{
		case 1:
			if (RASTER_Y == TOTAL_RASTERS-1) VBlanking = true;
			else 
			{
				RASTER_Y++;
	
				// Raster IRQ setzen wenn Rasterzeile mit Rasterlatch(IRQ_RASTER) übereinstimmt
				if (RASTER_Y == IRQ_RASTER) RasterIRQ();

				// Prüfen ob DEN Bit gesetzt ist ---> Badline zulassen
				if (RASTER_Y == 0x30)BadLinesEnabled = !(~CTRL1 & 0x10);

				// Prüfen auf Badline
				IsBadLine = (RASTER_Y >= FIRST_DMA_LINE && RASTER_Y <= LAST_DMA_LINE && ((RASTER_Y & 7) == Y_SCROLL) && BadLinesEnabled);

				DrawThisLine = (RASTER_Y >= FIRST_DISP_LINE && RASTER_Y <= LAST_DISP_LINE);
			}

			// First sample of border state
			BorderOnSample[0] = BorderOn;
			SprPtrAccess(3);
			SprDataAccess(3,0);
			DisplayIfBadLine;
			if (!(SprDmaOn & 0x18)) SetBAHigh;
			//if((SprDmaOn & 0xF8) == 0) SetBAHigh; //NEU
			break;

		case 2:
			if (VBlanking) 
			{
				// Vertical Blank, Reset Counters
				RASTER_Y = VcBase = 0;
				RefCnt = 0xff;
				LPTriggered = VBlanking = false;

                //// Bild fertig !!! /////

				RefreshProc(VideoPuffer);

				/// Nur für Double Buffer !! ///
				/*
				if(AktVideoPuffer == 0)
				{
					AktVideoPuffer = 1;
					VideoPuffer = VideoPuffer1;
				}
				else
				{
					AktVideoPuffer = 0;
					VideoPuffer = VideoPuffer0;
				}
				*/
				/////////////////////////

				ChunkyLineStart = VideoPuffer;
				XMod = XMOD;

				// Trigger raster IRQ if IRQ in line 0
				if (IRQ_RASTER == 0) RasterIRQ();
			}

			ChunkyPtr = ChunkyLineStart;

			memset(ForeMaskBuf, 0, TOTAL_X/8);
			ForeMaskPtr = ForeMaskBuf;

			SprDataAccess(3,1);
			SprDataAccess(3,2);
			DisplayIfBadLine;

			if (SprDmaOn & 0x20) SetBALow;
			break;

		case 3:
			SprPtrAccess(4);
			SprDataAccess(4, 0);
			DisplayIfBadLine;
			if (!(SprDmaOn & 0x30)) SetBAHigh;
			//if((SprDmaOn & 0xF0) == 0) SetBAHigh; // NEU
			break;

		case 4:
			SprDataAccess(4, 1);
			SprDataAccess(4, 2);
			DisplayIfBadLine;

			if (SprDmaOn & 0x40) SetBALow;
			break;

		case 5:
			SprPtrAccess(5);
			SprDataAccess(5, 0);
			DisplayIfBadLine;
			if (!(SprDmaOn & 0x60)) SetBAHigh;
			//if((SprDmaOn & 0xE0) == 0) SetBAHigh; // NEU
			break;

		case 6:
			SprDataAccess(5, 1);
			SprDataAccess(5, 2);
			DisplayIfBadLine;

			if (SprDmaOn & 0x80) SetBALow;
			break;

		case 7:
			SprPtrAccess(6);
			SprDataAccess(6, 0);
			DisplayIfBadLine;
			if (!(SprDmaOn & 0xc0)) SetBAHigh;
			//if((SprDmaOn & 0xC0) == 0) SetBAHigh; // NEU
			break;

		case 8:
			SprDataAccess(6, 1);
			SprDataAccess(6, 2);
			DisplayIfBadLine;
			break;

		case 9:
			SprPtrAccess(7);
			SprDataAccess(7, 0);
			DisplayIfBadLine;

			if (!(SprDmaOn & 0x80)) SetBAHigh;
			//if((SprDmaOn & 0x80) == 0) SetBAHigh; // NEU
			break;

		case 10:
			SprDataAccess(7, 1);
			SprDataAccess(7, 2);
			DisplayIfBadLine;
			break;

		case 11:
			RefreshAccess;
			DisplayIfBadLine;
			SetBAHigh;
			break;

		case 12:
			RefreshAccess;
			FetchIfBadLine;
			break;

		case 13:
			DrawBackground();
			SampleBorder;
			RefreshAccess;
			FetchIfBadLine;
			RASTER_X = 0xfffc;
			break;

		case 14:
			DrawBackground();
			SampleBorder;
			RefreshAccess;
			RCIfBadLine;
			Vc = VcBase;
			break;

		case 15:
			DrawBackground();
			SampleBorder;
			RefreshAccess;
			FetchIfBadLine;
			for (i=0; i<8; i++) if (SprExpY & (1 << i)) McBase[i] += 2;
			MLIndex = 0;
			MatrixAccess();
			break;

		case 16:
			DrawBackground();
			SampleBorder;
			GraphicsAccess();
			FetchIfBadLine;
			mask = 1;
			for (i=0; i<8; i++, mask<<=1) 
			{
				if (SprExpY & mask) McBase[i]++;
				if ((McBase[i] & 0x3f) == 0x3f) SprDmaOn &= ~mask;
			}
			MatrixAccess();
			break;

		case 17:
			if (CTRL2 & 8)
			{
				if (RASTER_Y == DYStop) UdBorderOn = true;
				else 
				{
					if (CTRL1 & 0x10) 
					{
						if (RASTER_Y == DYStart) BorderOn = UdBorderOn = false;
						else if (!UdBorderOn) BorderOn = false;
					} 
					else if (!UdBorderOn) BorderOn = false;
				}
			}

			// Second sample of border state
			BorderOnSample[1] = BorderOn;

			DrawBackground();
			DrawGraphics();
			SampleBorder;
			GraphicsAccess();
			FetchIfBadLine;
			MatrixAccess();
			break;

		case 18:
			if (!(CTRL2 & 8)) 
			{
				if (RASTER_Y == DYStop) UdBorderOn = true;
				else 
				{
					if (CTRL1 & 0x10) 
					{
						if (RASTER_Y == DYStart) BorderOn = UdBorderOn = false;
						else if (!UdBorderOn) BorderOn = false;
					} 
					else if (!UdBorderOn) BorderOn = false;
				}
			}

			// Third sample of border state
			BorderOnSample[2] = BorderOn;

		case 19: case 20: case 21: case 22: case 23: case 24:
		case 25: case 26: case 27: case 28: case 29: case 30:
		case 31: case 32: case 33: case 34: case 35: case 36:
		case 37: case 38: case 39: case 40: case 41: case 42:
		case 43: case 44: case 45: case 46: case 47: case 48:
		case 49: case 50: case 51: case 52: case 53: case 54:	
			DrawGraphics();
			SampleBorder;
			GraphicsAccess();
			FetchIfBadLine;
			MatrixAccess();
			LastCharData = CharData;
			break;

		case 55:
			DrawGraphics();
			SampleBorder;
			GraphicsAccess();
			DisplayIfBadLine;
			SetBAHigh;

			// Invert y expansion flipflop if bit in MYE is set
			mask = 1;
			for (i=0; i<8; i++, mask<<=1) if (MYE & mask) SprExpY ^= mask;
			CheckSpriteDMA;
			
			if (SprDmaOn & 0x01) SetBALow;
			break;

		case 56:
			if (!(CTRL2 & 8)) BorderOn = true;

			// Fourth sample of border state
			BorderOnSample[3] = BorderOn;

			DrawGraphics();
			SampleBorder;
			IdleAccess;
			DisplayIfBadLine;
			break;

		case 57:
			if (CTRL2 & 8) BorderOn = true;

			// Fifth sample of border state
			BorderOnSample[4] = BorderOn;

			// Sample spr_disp_on and spr_data for sprite drawing
			if ((SprDraw = SprDispOn)) memcpy(SprDrawData, SprData, 8*4);

			// Turn off sprite display if DMA is off
			mask = 1;
			for (i=0; i<8; i++, mask<<=1)
				if ((SprDispOn & mask) && !(SprDmaOn & mask))
					SprDispOn &= ~mask;

			DrawBackground();
			SampleBorder;
			IdleAccess;
			DisplayIfBadLine;

			if (SprDmaOn & 0x02) SetBALow;
			break;

		case 58:
			DrawBackground();
			SampleBorder;

			mask = 1;
			for (i=0; i<8; i++, mask<<=1) 
			{
				Mc[i] = McBase[i];
				if ((SprDmaOn & mask) && (RASTER_Y & 0xff) == MY[i])
					SprDispOn |= mask;
			}
			SprPtrAccess(0);
			SprDataAccess(0,0);

			if (Rc == 7) 
			{
				VcBase = Vc;
				DisplayState = false;
			}
			if (IsBadLine || DisplayState) 
			{
				DisplayState = true;
				Rc = (Rc + 1) & 7;
			}
			break;

		case 59:
			DrawBackground();
			SampleBorder;
			SprDataAccess(0,1);
			SprDataAccess(0,2);
			DisplayIfBadLine;

			if (SprDmaOn & 0x04) SetBALow;
			break;

		case 60:
			DrawBackground();
			SampleBorder;

			if (DrawThisLine) 
			{
				// Draw sprites
				if (SprDraw) DrawSprites();
				
				
				// Draw border
				
				if (BorderOnSample[0]) 
					for (i=0; i<4; i++)
					{
						MemSet8((ChunkyLineStart+i*8)-0, BorderColorSample[i]);
						if(BorderGreyDotSample[i]) ChunkyLineStart[(i*8)-0] = 0x0F;
					}
				if (BorderOnSample[1])
				{
					MemSet8((ChunkyLineStart+4*8)-0, BorderColorSample[4]);
					if(BorderGreyDotSample[4]) ChunkyLineStart[(4*8)-0] = 0x0F;
				}
				if (BorderOnSample[2])
					for (i=5; i<43; i++) 	
					{
						MemSet8((ChunkyLineStart+i*8)-0, BorderColorSample[i]);
						if(BorderGreyDotSample[i]) ChunkyLineStart[(i*8)-0] = 0x0F;
					}
				if (BorderOnSample[3])
				{
					MemSet8((ChunkyLineStart+43*8)-0, BorderColorSample[43]);
					if(BorderGreyDotSample[43]) ChunkyLineStart[(43*8)-0] = 0x0F;
				}
				BorderColorSample[48] = EC;
				if(isWriteColorReg20)BorderGreyDotSample[48] = true;
				else BorderGreyDotSample[48] = false;
				BorderColorSample[49] = EC;
				if (BorderOnSample[4])
					for (i=44; i<0x190/8; i++) 
					{
						MemSet8((ChunkyLineStart+i*8)-0, BorderColorSample[i]);
						if((BorderGreyDotSample[i] == true) && (i<48)) ChunkyLineStart[(i*8)-0] = 0x0F;
					}
				
				// Increment pointer in chunky buffer
				ChunkyLineStart += XMod;
			}

			SprPtrAccess(1);
			SprDataAccess(1,0);
			DisplayIfBadLine;
			if (!(SprDmaOn & 0x06)) SetBAHigh;
			//if((SprDmaOn & 0xFE) == 0) SetBAHigh; // NEU
			break;

		case 61:
			SprDataAccess(1,1);
			SprDataAccess(1,2);
			DisplayIfBadLine;

			if (SprDmaOn & 0x08) SetBALow;
			break;

		case 62:
			SprPtrAccess(2);
			SprDataAccess(2,0);
			DisplayIfBadLine;
			if (!(SprDmaOn & 0x0c)) SetBAHigh;
			//if((SprDmaOn & 0xFC) == 0) SetBAHigh; // NEU
			break;

		case 63:

			SprDataAccess(2,1);
			SprDataAccess(2,2);
			DisplayIfBadLine;

			if (RASTER_Y == DYStop) UdBorderOn = true;
			else if (CTRL1 & 0x10 && RASTER_Y == DYStart) UdBorderOn = false;

			if (SprDmaOn & 0x10) SetBALow;

			if(System == 0)
			{
				// Last cycle PAL
				Zyklus = 0;
			}

			if(Breakpoints[RASTER_Y] & 256) 
			{
				*BreakStatus |= 256;
				BreakWerte[8] = RASTER_Y;
			}

			if(Breakpoints[Zyklus] & 512) 
			{
				*BreakStatus |= 512;
				BreakWerte[9] = Zyklus;
			}
			break;

		case 64:
			if(System == 1)
			{
				// Last cycle NTSC0
				Zyklus = 0;
			}
			break;
		case 65:
				// Last cycle NTSC1
				Zyklus = 0;
			break;
	}
	RASTER_X += 8;
	Zyklus++;

	isWriteColorReg20 = false;
	isWriteColorReg21 = false;
	
	if(Breakpoints[RASTER_Y] & 256) 
	{
		*BreakStatus |= 256;
		BreakWerte[8] = RASTER_Y;
	}

	if(Breakpoints[Zyklus] & 512) 
	{
		*BreakStatus |= 512;
		BreakWerte[9] = Zyklus;
	}
}

bool VICII::SaveFreez(FILE* File)
{
	int Offset;
	Offset = ChunkyLineStart - VideoPuffer;
	fwrite(&Offset,1,sizeof(Offset),File);
	Offset = ForeMaskPtr - VideoPuffer;
	fwrite(&Offset,1,sizeof(Offset),File);
	Offset = ChunkyPtr - VideoPuffer;
	fwrite(&Offset,1,sizeof(Offset),File);

	fwrite(&System,1,sizeof(System),File);
	fwrite(&TOTAL_RASTERS,1,sizeof(TOTAL_RASTERS),File);
	fwrite(&TOTAL_ZYKLEN_LINE,1,sizeof(TOTAL_ZYKLEN_LINE),File);
	fwrite(&TOTAL_X,1,sizeof(TOTAL_X),File);
	fwrite(&IRQFlag,1,sizeof(IRQFlag),File);
	fwrite(&IRQMask,1,sizeof(IRQMask),File);
	fwrite(&DYStart,1,sizeof(DYStart),File);
	fwrite(&DYStop,1,sizeof(DYStop),File);
	fwrite(&BadLinesEnabled,1,sizeof(BadLinesEnabled),File);
	fwrite(&IsBadLine,1,sizeof(IsBadLine),File);
	fwrite(&DisplayState,1,sizeof(DisplayState),File);
	fwrite(&DisplayIDX,1,sizeof(DisplayIDX),File);
	fwrite(MatrixLine,1,40,File);
	fwrite(ColorLine,1,40,File);
	fwrite(&GfxData,1,sizeof(GfxData),File);
	fwrite(&CharData,1,sizeof(CharData),File);
	fwrite(&ColorData,1,sizeof(ColorData),File);
	fwrite(&LastCharData,1,sizeof(LastCharData),File);
	fwrite(&MLIndex,1,sizeof(MLIndex),File);
	fwrite(&FirstBaCycle,1,sizeof(FirstBaCycle),File);
	fwrite(&SprExpY,1,sizeof(SprExpY),File);
	fwrite(&RefCnt,1,sizeof(RefCnt),File);
	fwrite(SprPtr,1,8,File);
	fwrite(SprData,1,32,File);
	fwrite(SprDrawData,1,32,File);
	fwrite(&SprDmaOn,1,sizeof(SprDmaOn),File);
	fwrite(&SprDispOn,1,sizeof(SprDispOn),File);
	fwrite(&SprDraw,1,sizeof(SprDraw),File);
	fwrite(Mc,1,80,File);
	fwrite(McBase,1,80,File);
	fwrite(&ClxSpr,1,sizeof(ClxSpr),File);
	fwrite(&ClxBgr,1,sizeof(ClxBgr),File);
	fwrite(&MatrixBase,1,sizeof(MatrixBase),File);
	fwrite(&CharBase,1,sizeof(CharBase),File);
	fwrite(&BitmapBase,1,sizeof(BitmapBase),File);
	fwrite(&CiaVabase,1,sizeof(CiaVabase),File);
	fwrite(&VcBase,1,sizeof(VcBase),File);
	fwrite(&Vc,1,sizeof(Vc),File);
	fwrite(&Rc,1,sizeof(Rc),File);
	fwrite(&DrawThisLine,1,sizeof(DrawThisLine),File);
	fwrite(BorderOnSample,1,5,File);
	fwrite(BorderColorSample,1,65,File);
	fwrite(&BorderOn,1,sizeof(BorderOn),File);
	fwrite(&UdBorderOn,1,sizeof(UdBorderOn),File);
	fwrite(&LPTriggered,1,sizeof(LPTriggered),File);
	fwrite(&LPTriggerOld,1,sizeof(LPTriggerOld),File);
	fwrite(&VBlanking,1,sizeof(VBlanking),File);
	fwrite(&Zyklus,1,sizeof(Zyklus),File);
	fwrite(&XMod,1,sizeof(XMod),File);
	fwrite(SprCollBuf,1,520,File);
	fwrite(ForeMaskBuf,1,65,File);

	fwrite(MX,1,8,File);
	fwrite(MY,1,8,File);
	fwrite(&MX8,1,sizeof(MX8),File);
	fwrite(&CTRL1,1,sizeof(CTRL1),File);
	fwrite(&ME,1,sizeof(ME),File);
	fwrite(&MXE,1,sizeof(MXE),File);
	fwrite(&MYE,1,sizeof(MYE),File);
	fwrite(&MDP,1,sizeof(MDP),File);
	fwrite(&MMC,1,sizeof(MMC),File);
	fwrite(&LPX,1,sizeof(LPX),File);
	fwrite(&LPY,1,sizeof(LPY),File);
	fwrite(&CTRL2,1,sizeof(CTRL2),File);
	fwrite(&Y_SCROLL,1,sizeof(Y_SCROLL),File);
	fwrite(&X_SCROLL,1,sizeof(X_SCROLL),File);
	fwrite(&VBASE,1,sizeof(VBASE),File);
	fwrite(&IRQ_RASTER,1,sizeof(IRQ_RASTER),File);
	fwrite(&RASTER_Y,1,sizeof(RASTER_Y),File);
	fwrite(&RASTER_X,1,sizeof(RASTER_X),File);
	fwrite(&EC,1,sizeof(EC),File);
	fwrite(&B0C,1,sizeof(B0C),File);
	fwrite(&B1C,1,sizeof(B1C),File);
	fwrite(&B2C,1,sizeof(B2C),File);
	fwrite(&B3C,1,sizeof(B3C),File);
	fwrite(&MM0,1,sizeof(MM0),File);
	fwrite(&MM1,1,sizeof(MM1),File);
	fwrite(SC,1,8,File);
	return true;
}

bool VICII::LoadFreez(FILE *File,unsigned short Version)
{
	int Offset;

	switch(Version)
	{
	case 0x0100:
	case 0x0101:
		fread(&Offset,1,sizeof(Offset),File);
		ChunkyLineStart = Offset + VideoPuffer;
		fread(&Offset,1,sizeof(Offset),File);
		ForeMaskPtr = Offset + VideoPuffer;
		fread(&Offset,1,sizeof(Offset),File);
		ChunkyPtr = Offset + VideoPuffer;

		fread(&System,1,sizeof(System),File);
		fread(&TOTAL_RASTERS,1,sizeof(TOTAL_RASTERS),File);
		fread(&TOTAL_ZYKLEN_LINE,1,sizeof(TOTAL_ZYKLEN_LINE),File);
		fread(&TOTAL_X,1,sizeof(TOTAL_X),File);
		fread(&IRQFlag,1,sizeof(IRQFlag),File);
		fread(&IRQMask,1,sizeof(IRQMask),File);
		fread(&DYStart,1,sizeof(DYStart),File);
		fread(&DYStop,1,sizeof(DYStop),File);
		fread(&BadLinesEnabled,1,sizeof(BadLinesEnabled),File);
		fread(&IsBadLine,1,sizeof(IsBadLine),File);
		fread(&DisplayState,1,sizeof(DisplayState),File);
		fread(&DisplayIDX,1,sizeof(DisplayIDX),File);
		fread(MatrixLine,1,40,File);
		fread(ColorLine,1,40,File);
		fread(&GfxData,1,sizeof(GfxData),File);
		fread(&CharData,1,sizeof(CharData),File);
		fread(&ColorData,1,sizeof(ColorData),File);
		fread(&LastCharData,1,sizeof(LastCharData),File);
		fread(&MLIndex,1,sizeof(MLIndex),File);
		fread(&FirstBaCycle,1,sizeof(FirstBaCycle),File);
		fread(&SprExpY,1,sizeof(SprExpY),File);
		fread(&RefCnt,1,sizeof(RefCnt),File);
		fread(SprPtr,1,8,File);
		fread(SprData,1,32,File);
		fread(SprDrawData,1,32,File);
		fread(&SprDmaOn,1,sizeof(SprDmaOn),File);
		fread(&SprDispOn,1,sizeof(SprDispOn),File);
		fread(&SprDraw,1,sizeof(SprDraw),File);
		fread(Mc,1,80,File);
		fread(McBase,1,80,File);
		fread(&ClxSpr,1,sizeof(ClxSpr),File);
		fread(&ClxBgr,1,sizeof(ClxBgr),File);
		fread(&MatrixBase,1,sizeof(MatrixBase),File);
		fread(&CharBase,1,sizeof(CharBase),File);
		fread(&BitmapBase,1,sizeof(BitmapBase),File);
		fread(&CiaVabase,1,sizeof(CiaVabase),File);
		fread(&VcBase,1,sizeof(VcBase),File);
		fread(&Vc,1,sizeof(Vc),File);
		fread(&Rc,1,sizeof(Rc),File);
		fread(&DrawThisLine,1,sizeof(DrawThisLine),File);
		fread(BorderOnSample,1,5,File);
		fread(BorderColorSample,1,65,File);
		fread(&BorderOn,1,sizeof(BorderOn),File);
		fread(&UdBorderOn,1,sizeof(UdBorderOn),File);
		fread(&LPTriggered,1,sizeof(LPTriggered),File);
		fread(&LPTriggerOld,1,sizeof(LPTriggerOld),File);
		fread(&VBlanking,1,sizeof(VBlanking),File);
		fread(&Zyklus,1,sizeof(Zyklus),File);
		fread(&XMod,1,sizeof(XMod),File);
		fread(SprCollBuf,1,520,File);
		fread(ForeMaskBuf,1,65,File);

		fread(MX,1,8,File);
		fread(MY,1,8,File);
		fread(&MX8,1,sizeof(MX8),File);
		fread(&CTRL1,1,sizeof(CTRL1),File);
		fread(&ME,1,sizeof(ME),File);
		fread(&MXE,1,sizeof(MXE),File);
		fread(&MYE,1,sizeof(MYE),File);
		fread(&MDP,1,sizeof(MDP),File);
		fread(&MMC,1,sizeof(MMC),File);
		fread(&LPX,1,sizeof(LPX),File);
		fread(&LPY,1,sizeof(LPY),File);
		fread(&CTRL2,1,sizeof(CTRL2),File);
		fread(&Y_SCROLL,1,sizeof(Y_SCROLL),File);
		fread(&X_SCROLL,1,sizeof(X_SCROLL),File);
		fread(&VBASE,1,sizeof(VBASE),File);
		fread(&IRQ_RASTER,1,sizeof(IRQ_RASTER),File);
		fread(&RASTER_Y,1,sizeof(RASTER_Y),File);
		fread(&RASTER_X,1,sizeof(RASTER_X),File);
		fread(&EC,1,sizeof(EC),File);
		fread(&B0C,1,sizeof(B0C),File);
		fread(&B1C,1,sizeof(B1C),File);
		fread(&B2C,1,sizeof(B2C),File);
		fread(&B3C,1,sizeof(B3C),File);
		fread(&MM0,1,sizeof(MM0),File);
		fread(&MM1,1,sizeof(MM1),File);
		fread(SC,1,8,File);
		break;
	}
	return true;
}

void VICII::WriteIO(unsigned short adresse,unsigned char wert)
{
   adresse &= 63;
   switch (adresse) 
	{
		case 0x00: case 0x02: case 0x04: case 0x06:
		case 0x08: case 0x0a: case 0x0c: case 0x0e:
		
                        MX[adresse >> 1] = (MX[adresse >> 1] & 0xff00) | wert;
			break;

		case 0x10:{
			int i, j;
                        MX8 = wert;
			for (i=0, j=1; i<8; i++, j<<=1) {
                                if (MX8 & j)
                                        MX[i] |= 0x100;
				else
                                        MX[i] &= 0xff;
			}
			break;
		}

		case 0x01: case 0x03: case 0x05: case 0x07:
		case 0x09: case 0x0b: case 0x0d: case 0x0f:
                        MY[adresse >> 1] = wert;
			break;

		case 0x11:{	// Control register 1
                        CTRL1 = wert;
                        Y_SCROLL = wert & 7;

                        unsigned short int NEW_IRQ_RASTER = (IRQ_RASTER & 0xff) | ((wert & 0x80) << 1);
                        if (IRQ_RASTER != NEW_IRQ_RASTER && RASTER_Y == NEW_IRQ_RASTER) RasterIRQ();
                        IRQ_RASTER = NEW_IRQ_RASTER;

			if (wert & 8) {
                                DYStart = RAHMEN25_YSTART;
                                DYStop = RAHMEN25_YSTOP;
			} else {
                                DYStart = RAHMEN24_YSTART;
                                DYStop = RAHMEN24_YSTOP;
			}

			// In line $30, the DEN bit controls if Bad Lines can occur
                        if (RASTER_Y == 0x30 && wert & 0x10)
                                BadLinesEnabled = true;

			// Bad Line condition?
                        IsBadLine = (RASTER_Y >= FIRST_DMA_LINE && RASTER_Y <= LAST_DMA_LINE && ((RASTER_Y & 7) == Y_SCROLL) && BadLinesEnabled);

                        DisplayIDX = ((CTRL1 & 0x60) | (CTRL2 & 0x10)) >> 4;
			break;
		}

		case 0x12:{	// Raster counter
                        unsigned short int NEW_IRQ_RASTER = (IRQ_RASTER & 0xff00) | wert;
                        if (IRQ_RASTER != NEW_IRQ_RASTER && RASTER_Y == NEW_IRQ_RASTER) RasterIRQ();
                        IRQ_RASTER = NEW_IRQ_RASTER;
			break;
		}

		case 0x15:	// Sprite enable
                        ME = wert;
			break;

		case 0x16:	// Control register 2
                        CTRL2 = wert;
                        X_SCROLL = wert & 7;
                        DisplayIDX = (((CTRL1 & 0x60) | (CTRL2 & 0x10)) >> 4);
			break;

		case 0x17:	// Sprite Y expansion
                        MYE = wert;
                        SprExpY |= ~wert;
			break;

		case 0x18:	// Memory pointers
                        VBASE = wert;
                        MatrixBase = (wert & 0xf0) << 6;
                        CharBase = (wert & 0x0e) << 10;
                        BitmapBase = (wert & 0x08) << 10;
			break;

		case 0x19: // IRQ flags
                        IRQFlag = IRQFlag & (~wert & 0x0f);
                        if (IRQFlag & IRQMask)	// Set master bit if allowed interrupt still pending
                                IRQFlag |= 0x80;
			else
                                CpuClearInterrupt(VIC_IRQ);
			break;
		
		case 0x1a:	// IRQ mask
                        IRQMask = wert & 0x0f;
                        if (IRQFlag & IRQMask) {	// Trigger interrupt if pending and now allowed
                                IRQFlag |= 0x80;
                                CpuTriggerInterrupt(VIC_IRQ);
			} else {
                                IRQFlag &= 0x7f;
                                CpuClearInterrupt(VIC_IRQ);
			}
			break;

		case 0x1b:	// Sprite data priority
                        MDP = wert;
			break;

		case 0x1c:	// Sprite multicolor
                        MMC = wert;
			break;

		case 0x1d:	// Sprite X expansion
                        MXE = wert;
			break;

                case 0x20: EC	= wert&15;
                        if(GreyDotEnable)isWriteColorReg20 = true;
			break;
                case 0x21: B0C	= wert&15;
                        if(GreyDotEnable)isWriteColorReg21 = true;
			break;
                case 0x22: B1C	= wert&15;
			break;
                case 0x23: B2C	= wert&15;
			break;
                case 0x24: B3C	= wert&15;
			break;
                case 0x25: MM0  = wert&15;
			break;
                case 0x26: MM1  = wert&15;
			break;

		case 0x27: case 0x28: case 0x29: case 0x2a:
		case 0x2b: case 0x2c: case 0x2d: case 0x2e:
                        SC[adresse - 0x27] = wert&15;
			break;
	}
}

unsigned char VICII::ReadIO(unsigned short adresse)
{
	adresse &= 63;
	unsigned char tmp;

	switch (adresse) 
	{
		case 0x00: case 0x02: case 0x04: case 0x06:
		case 0x08: case 0x0a: case 0x0c: case 0x0e:
                        return (unsigned char)MX[adresse >> 1];

		case 0x01: case 0x03: case 0x05: case 0x07:
		case 0x09: case 0x0b: case 0x0d: case 0x0f:
                        return MY[adresse >> 1];

		case 0x10:	// Sprite X position MSB
                        return MX8;

		case 0x11:	// Control register 1
                        return (CTRL1 & 0x7f) | ((RASTER_Y & 0x100) >> 1);

		case 0x12:	// Raster counter
                        return (unsigned char)RASTER_Y;

		case 0x13:	// Light pen X
                        return LPX;

		case 0x14:	// Light pen Y
                        return LPY;

		case 0x15:	// Sprite enable
                        return ME;

		case 0x16:	// Control register 2
                        return CTRL2 | 0xc0;

		case 0x17:	// Sprite Y expansion
                        return MYE;

		case 0x18:	// Memory pointers
                        return VBASE | 0x01;

		case 0x19:	// IRQ flags

                if (RASTER_Y == IRQ_RASTER)
        {
                        if (IRQMask & 0x01) tmp = IRQFlag | 0xf1;
                        else tmp = IRQFlag | 0x71;
		} 
		else 
		{
                        tmp = IRQFlag | 0x70;
		}

		return tmp;
	
		case 0x1a:	// IRQ mask
                        return IRQMask | 0xf0;

		case 0x1b:	// Sprite data priority
                        return MDP;

		case 0x1c:	// Sprite multicolor
                        return MMC;

		case 0x1d:	// Sprite X expansion
                        return MXE;

		case 0x1e:{	// Sprite-sprite collision
                        unsigned char ret = ClxSpr;
                        ClxSpr = 0;	// Read and clear
			return ret;
		}

		case 0x1f:{	// Sprite-background collision
                        unsigned char ret = ClxBgr;
                        ClxBgr = 0;	// Read and clear
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
                        return SC[adresse - 0x27] | 0xf0;

		default:
		return 0xff;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

inline void VICII::MatrixAccess(void)
{
	if (BA) 
	{
		if (Zyklus-FirstBaCycle < 3)
			MatrixLine[MLIndex] = ColorLine[MLIndex] = 0xff;
		
		else 
		{
			unsigned short int adr = (Vc & 0x03ff) | MatrixBase;
			MatrixLine[MLIndex] = Read(adr|CiaVabase);
			ColorLine[MLIndex] = FarbRam[adr & 0x03ff];
		}
	}
}

inline void VICII::GraphicsAccess(void)
{
	if (DisplayState) 
	{
		unsigned short int adr;
		if (CTRL1 & 0x20)	// Bitmap
			adr = ((Vc & 0x03ff) << 3) | BitmapBase | Rc;
		else				// Text
			adr = (MatrixLine[MLIndex] << 3) | CharBase | Rc;
		if (CTRL1 & 0x40) adr &= 0xf9ff;
		GfxData = Read(adr|CiaVabase);
		CharData = MatrixLine[MLIndex];
		ColorData = ColorLine[MLIndex];
		MLIndex++;
		Vc++;

	} 
	else 
	{
		// Display is off
		GfxData = Read((CTRL1 & 0x40 ? 0x39ff : 0x3fff)|CiaVabase);
		CharData = ColorData = 0;
	}
}

inline void VICII::DrawBackground(void)
{
	if(VideoPuffer == NULL) return;

	unsigned char *p = ChunkyPtr;
	unsigned char c;

	if (!DrawThisLine) return;

	switch (DisplayIDX) 
	{
		case 0:		// Standard text
		case 1:		// Multicolor text
		case 3:		// Multicolor bitmap
			c = B0C;
			break;
		case 2:		// Standard bitmap
			c = LastCharData;
			break;
		case 4:		// ECM text
			if (LastCharData & 0x80)
				if (LastCharData & 0x40) c = B3C;
				else c = B2C;
			else if (LastCharData & 0x40) c = B1C;
				 else c = B0C;
			break;
		default:
			c = 0;
			break;
	}
	MemSet8(p, c);
	if(isWriteColorReg21) p[0] = 0x0F;
}

inline void VICII::DrawGraphics(void)
{
	if(VideoPuffer == NULL) return;

	unsigned char *p = ChunkyPtr + X_SCROLL;
	unsigned char c[4];
	unsigned char data;

	if (!DrawThisLine) return;
	if (UdBorderOn) 
	{
		DrawBackground();
		return;
	}

	switch (DisplayIDX) 
	{
		case 0:		// Standard text
			c[0] = B0C&15;
			c[1] = ColorData&15;
			goto draw_std;

		case 1:		// Multicolor text
			if (ColorData & 8) 
			{
				c[0] = B0C&15;
				c[1] = B1C&15;
				c[2] = B2C&15;
				c[3] = ColorData & 7;
				goto draw_multi;
			} 
			else 
			{
				c[0] = B0C&15;
				c[1] = ColorData&15;
				goto draw_std;
			}

		case 2:		// Standard bitmap
			c[0] = CharData&15;
			c[1] = CharData >> 4;
			goto draw_std;

		case 3:		// Multicolor bitmap
			c[0] = B0C&15;
			c[1] = CharData >> 4;
			c[2] = CharData&15;
			c[3] = ColorData&15;
			goto draw_multi;

		case 4:		// ECM text
			if (CharData & 0x80)
				if (CharData & 0x40) c[0] = B3C&15;
				else c[0] = B2C&15;
			else
				if (CharData & 0x40) c[0] = B1C&15;
				else c[0] = B0C&15;
			
			c[1] = ColorData&15;
			goto draw_std;

		case 5:		// Invalid multicolor text
			MemSet8(p,0);
			if (ColorData & 8) {
				ForeMaskPtr[0] |= ((GfxData & 0xaa) | (GfxData & 0xaa) >> 1) >> X_SCROLL;
				ForeMaskPtr[1] |= ((GfxData & 0xaa) | (GfxData & 0xaa) >> 1) << (8-X_SCROLL);
			} else {
				ForeMaskPtr[0] |= GfxData >> X_SCROLL;
				ForeMaskPtr[1] |= GfxData << (7-X_SCROLL);
			}
			return;

		case 6:		// Invalid standard bitmap
			MemSet8(p,0);
			ForeMaskPtr[0] |= GfxData >> X_SCROLL;
			ForeMaskPtr[1] |= GfxData << (7-X_SCROLL);
			return;

		case 7:		// Invalid multicolor bitmap
			MemSet8(p,0);
			ForeMaskPtr[0] |= ((GfxData & 0xaa) | (GfxData & 0xaa) >> 1) >> X_SCROLL;
			ForeMaskPtr[1] |= ((GfxData & 0xaa) | (GfxData & 0xaa) >> 1) << (8-X_SCROLL);
			return;

			default:	// Can't happen

			if(isWriteColorReg21) p[0] = 0x0F;

			return;
	}

draw_std:

	ForeMaskPtr[0] |= GfxData >> X_SCROLL;
	ForeMaskPtr[1] |= GfxData << (7-X_SCROLL);

	data = GfxData;
	p[7] = c[data & 1]; data >>= 1;
	p[6] = c[data & 1]; data >>= 1;
	p[5] = c[data & 1]; data >>= 1;
	p[4] = c[data & 1]; data >>= 1;
	p[3] = c[data & 1]; data >>= 1;
	p[2] = c[data & 1]; data >>= 1;
	p[1] = c[data & 1]; data >>= 1;
	p[0] = c[data];
	return;

draw_multi:
	
	ForeMaskPtr[0] |= ((GfxData & 0xaa) | (GfxData & 0xaa) >> 1) >> X_SCROLL;
	ForeMaskPtr[1] |= ((GfxData & 0xaa) | (GfxData & 0xaa) >> 1) << (8-X_SCROLL);
	data = GfxData;
	p[7] = p[6] = c[data & 3]; data >>= 2;
	p[5] = p[4] = c[data & 3]; data >>= 2;
	p[3] = p[2] = c[data & 3]; data >>= 2;
	p[1] = p[0] = c[data];
	return;
}

inline void VICII::DrawSprites(void)
{
    int i;
	int snum, sbit;		// Sprite number/bit mask
	int spr_coll=0, gfx_coll=0;

	// Clear sprite collision buffer
	{
		unsigned int *lp = (unsigned int *)SprCollBuf - 1;
		for (i=0; i<TOTAL_X/4; i++)
			*++lp = 0;
	}

	// Loop for all sprites
	for (snum=0, sbit=1; snum<8; snum++, sbit<<=1) 
	{
		// Is sprite visible?
		if ((SprDraw & sbit) && MX[snum] <= TOTAL_X-32) 
        {
            unsigned char *p = ChunkyLineStart + MX[snum] + 8;

			unsigned char *q = SprCollBuf + MX[snum] + 8;
			unsigned char color = SC[snum];

			// Fetch sprite data and mask
			unsigned int sdata = (SprDrawData[snum][0] << 24) | (SprDrawData[snum][1] << 16) | (SprDrawData[snum][2] << 8);

			int spr_mask_pos = MX[snum] + 8;	// Sprite bit position in fore_mask_buf
			
			unsigned char *fmbp = ForeMaskBuf + (spr_mask_pos / 8);
			int sshift = spr_mask_pos & 7;
			unsigned int fore_mask = (((*(fmbp+0) << 24) | (*(fmbp+1) << 16) | (*(fmbp+2) << 8)
				  		    | (*(fmbp+3))) << sshift) | (*(fmbp+4) >> (8-sshift));

			if (MXE & sbit) 
            {		// X-expanded
				if (MX[snum] > TOTAL_X-56)
					continue;

				unsigned int sdata_l = 0, sdata_r = 0, fore_mask_r;
				fore_mask_r = (((*(fmbp+4) << 24) | (*(fmbp+5) << 16) | (*(fmbp+6) << 8)
						| (*(fmbp+7))) << sshift) | (*(fmbp+8) >> (8-sshift));

				if (MMC & sbit) 
                {	// Multicolor mode
					unsigned int plane0_l, plane0_r, plane1_l, plane1_r;

					// Expand sprite data
					sdata_l = MultiExpTable[sdata >> 24 & 0xff] << 16 | MultiExpTable[sdata >> 16 & 0xff];
					sdata_r = MultiExpTable[sdata >> 8 & 0xff] << 16;

					// Convert sprite chunky pixels to bitplanes
					plane0_l = (sdata_l & 0x55555555) | (sdata_l & 0x55555555) << 1;
					plane1_l = (sdata_l & 0xaaaaaaaa) | (sdata_l & 0xaaaaaaaa) >> 1;
					plane0_r = (sdata_r & 0x55555555) | (sdata_r & 0x55555555) << 1;
					plane1_r = (sdata_r & 0xaaaaaaaa) | (sdata_r & 0xaaaaaaaa) >> 1;

					// Collision with graphics?
					if ((fore_mask & (plane0_l | plane1_l)) || (fore_mask_r & (plane0_r | plane1_r))) 
                    {
						gfx_coll |= sbit;
						if (MDP & sbit)	
                        {
							plane0_l &= ~fore_mask;	// Mask sprite if in background
							plane1_l &= ~fore_mask;
							plane0_r &= ~fore_mask_r;
							plane1_r &= ~fore_mask_r;
						}
					}

					// Paint sprite
					for (i=0; i<32; i++, plane0_l<<=1, plane1_l<<=1) 
                    {
						unsigned char col;
						if (plane1_l & 0x80000000) 
                        {
							if (plane0_l & 0x80000000)
								col = MM1;
							else
								col = color;
						} 
                        else 
                        {
							if (plane0_l & 0x80000000)
								col = MM0;
							else
								continue;
						}
						if (q[i])
							spr_coll |= q[i] | sbit;
						else
                        {
							p[i] = col&15;
							q[i] = sbit;
						}
					}
					for (; i<48; i++, plane0_r<<=1, plane1_r<<=1) 
                    {
						unsigned char col;
						if (plane1_r & 0x80000000) 
                        {
							if (plane0_r & 0x80000000)
								col = MM1;
							else
								col = color;
						} 
                        else 
                        {
							if (plane0_r & 0x80000000)
								col = MM0;
							else
								continue;
						}
						if (q[i])
							spr_coll |= q[i] | sbit;
						else
                        {
							p[i] = col&15;
							q[i] = sbit;
						}
					}
				} 
                else 
                {			// Standard mode

					// Expand sprite data
					sdata_l = ExpTable[sdata >> 24 & 0xff] << 16 | ExpTable[sdata >> 16 & 0xff];
					sdata_r = ExpTable[sdata >> 8 & 0xff] << 16;

					// Collision with graphics?
					if ((fore_mask & sdata_l) || (fore_mask_r & sdata_r)) 
                    {
						gfx_coll |= sbit;
						if (MDP & sbit)	
                        {
							sdata_l &= ~fore_mask;	// Mask sprite if in background
							sdata_r &= ~fore_mask_r;
						}
					}

					// Paint sprite
					for (i=0; i<32; i++, sdata_l<<=1)
					if (sdata_l & 0x80000000) 
                    {
						if (q[i])	// Collision with sprite?
							spr_coll |= q[i] | sbit;
						
						else
                        {		// Draw pixel if no collision
							p[i] = color&15;
							q[i] = sbit;
						}
					}
					for (; i<48; i++, sdata_r<<=1)
					if (sdata_r & 0x80000000) 
                    {
						if (q[i]) 	// Collision with sprite?
							spr_coll |= q[i] | sbit;
						else
                        {		// Draw pixel if no collision
							p[i] = color&15;
							q[i] = sbit;
						}
					}
			    }
            } 
            else 
            {				// Unexpanded

			if (MMC & sbit) 
            {	// Multicolor mode
				unsigned int plane0, plane1;

				// Convert sprite chunky pixels to bitplanes
				plane0 = (sdata & 0x55555555) | (sdata & 0x55555555) << 1;
				plane1 = (sdata & 0xaaaaaaaa) | (sdata & 0xaaaaaaaa) >> 1;

				// Collision with graphics?
				if (fore_mask & (plane0 | plane1)) 
                {
					gfx_coll |= sbit;
					if (MDP & sbit) 
                    {
						plane0 &= ~fore_mask;	// Mask sprite if in background
						plane1 &= ~fore_mask;
					}
				}

				// Paint sprite
				for (i=0; i<24; i++, plane0<<=1, plane1<<=1) 
                {
					unsigned char col;
					if (plane1 & 0x80000000) 
                    {
						if (plane0 & 0x80000000)
							col = MM1;
						else
							col = color;
					} else {
						if (plane0 & 0x80000000)
							col = MM0;
						else
							continue;
					}
					if (q[i])
						spr_coll |= q[i] | sbit;
					else
                    {
						p[i] = col&15;
						q[i] = sbit;
					}
				}
			} 
            else 
            {			// Standard mode

				// Collision with graphics?
				if (fore_mask & sdata) 
                {
					gfx_coll |= sbit;
					if (MDP & sbit)
						sdata &= ~fore_mask;	// Mask sprite if in background
				}

				// Paint sprite
				for (i=0; i<24; i++, sdata<<=1)
					if (sdata & 0x80000000) 
                    {
						if (q[i]) {	// Collision with sprite?
							spr_coll |= q[i] | sbit;
						} else {		// Draw pixel if no collision
							p[i] = color;
							q[i] = sbit;
						}
					}
				}
			}
		}
	}

    ////////// Sprite Collision kann man auch weglassen / abschalten

	// Check sprite-sprite collisions

	if(ClxSpr == 0)
	{
		// IRQ kann ausgelöst werden
		ClxSpr |= spr_coll;
		if(ClxSpr != 0)
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
		ClxSpr |= spr_coll;
	}

	if(ClxBgr == 0)
	{
		// IRQ kann ausgelöst werden
		ClxBgr |= gfx_coll;
		if(ClxBgr != 0)
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
		ClxBgr |= gfx_coll;
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

void VICII::TriggerLightpen(void)
{
        if(!LPTriggered)
	{
                LPTriggered = true;

                LPX = RASTER_X >> 1;
                LPY = RASTER_Y;

                IRQFlag |= 0x08;
                if (IRQMask & 0x08)
		{
                        IRQFlag |= 0x80;
                        CpuTriggerInterrupt(VIC_IRQ);
		}
	}
}

inline void VICII::MemSet8(unsigned char* p, unsigned char c)
{
	p[0] = p[1] = p[2] = p[3] = p[4] = p[5] = p[6] = p[7] = c;
}
#endif
