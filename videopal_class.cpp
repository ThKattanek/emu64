//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: videopal_class.cpp                    //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 01.03.2013                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "videopal_class.h"
#include "c64_colors.h"
#include "math.h"
//#include "SDL/SDL.h"

#define VIC_SATURATION	48.0f		//48.0f
#define VIC_PHASE	-4.5f

#define ANGLE_RED   112.5f
#define ANGLE_GRN   -135.0f
#define ANGLE_BLU   0.0f
#define ANGLE_ORN   -45.0f
#define ANGLE_BRN   157.5f

#define LUMN0     0.0f
#define LUMN1    56.0f
#define LUMN2    74.0f
#define LUMN3    92.0f
#define LUMN4   117.0f
#define LUMN5   128.0f
#define LUMN6   163.0f
#define LUMN7   199.0f
#define LUMN8   256.0f

static VIC_COLOR_STRUCT VicFarben[16] =
{
    { LUMN0, ANGLE_ORN, -0},
    { LUMN8, ANGLE_BRN,  0},
    { LUMN2, ANGLE_RED,  1},
    { LUMN6, ANGLE_RED, -1},
    { LUMN3, ANGLE_GRN, -1},
    { LUMN5, ANGLE_GRN,  1},
    { LUMN1, ANGLE_BLU,  1},
    { LUMN7, ANGLE_BLU, -1},
    { LUMN3, ANGLE_ORN, -1},
    { LUMN1, ANGLE_BRN,  1},
    { LUMN5, ANGLE_RED,  1},
    { LUMN2, ANGLE_RED, -0},
    { LUMN4, ANGLE_GRN, -0},
    { LUMN7, ANGLE_GRN,  1},
    { LUMN4, ANGLE_BLU,  1},
    { LUMN6, ANGLE_BLU, -0}
};

#define MATH_PI	3.141592653589793238462643383279

VideoPalClass::VideoPalClass(void)
{
    AktFarbMode = 0;
    Double2x = false;
    Kontrast = 0.8;
}

VideoPalClass::~VideoPalClass(void)
{
}

/*
void VideoPalClass::SetPixelFormat()
{
    pixel_format = format;
    SetC64Palette(AktFarbMode);
    CreateVicIIColors();
}
*/

void VideoPalClass::SetDisplayMode(int DisplayMode)
{
    DestDisplayMode = DisplayMode;
    UpdateParameter();
}

void VideoPalClass::UpdateParameter(void)
{
    CreateVicIIColors();
}

void VideoPalClass::SetPhaseAltLineOffset(int offset)
{
    if(offset < 0) offset = 0;
    if(offset > 2000) offset = 2000;
    PhaseAlternatingLine = offset;
}

void VideoPalClass::SetHorizontalBlurY(int wblur)
{
    if(wblur > 5) wblur = 5;
    HoBlurWY = wblur;
    blur_y_mul = 1.0f/(HoBlurWY);
}

void VideoPalClass::SetHorizontalBlurUV(int wblur)
{
    if(wblur > 5) wblur = 5;
    HoBlurWUV = wblur;
    blur_uv_mul = 1.0f/(HoBlurWUV);
}

void VideoPalClass::SetScanline(int wert)
{
    Scanline = wert/100.0f;
}

void VideoPalClass::SetSaturation(float wert)
{
    Saturation = wert;
}

void VideoPalClass::SetHelligkeit(float wert)
{
    Helligkeit = wert;
}

void VideoPalClass::SetKontrast(float wert)
{
    Kontrast = wert;
}

void VideoPalClass::SetC64Palette(int palnr)
{
    AktFarbMode = palnr;

    //if(pixel_format == 0) return;

    int ij = 0;
    for(int j=0;j<16;j++)
    {
        uint8_t *COLOR_RGBA1 = (uint8_t*)&C64_FARBEN[palnr*16*4];

        for(int i=0;i<16;i++)
        {
            /// Für 32Bit Video Display ///

            Palette32Bit[ij] = 0xff000000 | COLOR_RGBA1[0]<<16 | COLOR_RGBA1[1]<<8 | COLOR_RGBA1[2];

            /// Für 16Bit Video Display ///
            //Palette16Bit[ij] = (uint16_t)SDL_MapRGB(pixel_format,COLOR_RGBA1[0],COLOR_RGBA1[1],COLOR_RGBA1[2]);
            COLOR_RGBA1+=4;
            ij++;
        }
    }
}

void VideoPalClass::EnableVideoDoubleSize(bool enabled)
{
        Double2x = enabled;
}

void VideoPalClass::EnablePALOutput(bool enabled)
{
        PALOutput = enabled;
}

inline void VideoPalClass::RGB_To_YUV(float rgb[3], float yuv[3])
{
        yuv[0] = float(0.299*rgb[0]+0.587*rgb[1]+0.114*rgb[2]);  // Y
        yuv[1] = float(-0.147*rgb[0]-0.289*rgb[1]+0.436*rgb[2]); // U
        yuv[2] = float(0.615*rgb[0]-0.515*rgb[1]-0.100*rgb[2]);  // V
}

inline void VideoPalClass::YUV_To_RGB(float yuv[3], float rgb[3])
{
        rgb[0] = float(yuv[0]+0.000*yuv[1]+1.140*yuv[2]); // Rot
        rgb[1] = float(yuv[0]-0.396*yuv[1]-0.581*yuv[2]); // Grün
        rgb[2] = float(yuv[0]+2.029*yuv[1]+0.000*yuv[2]); // Blau
}

inline void VideoPalClass::CreateVicIIColors(void)
{
        COLOR_STRUCT ColorOut;
        COLOR_STRUCT ColorIn;

        /// Für Phase Alternating Line
        float Offs=(((float)(PhaseAlternatingLine)) / (2000.0f / 90.0f))+(180.0f-45.0f);

        for(int i=0;i<16;i++)
        {
                C64YUVPalette1[i*3+0] = VicFarben[i].Luminace * Helligkeit;
                C64YUVPalette1[i*3+1] = (float)(VIC_SATURATION * cos((VicFarben[i].Angel + VIC_PHASE) * (MATH_PI / 180.0))) * Helligkeit;
                C64YUVPalette1[i*3+2] = (float)(VIC_SATURATION * sin((VicFarben[i].Angel + VIC_PHASE) * (MATH_PI / 180.0))) * Helligkeit;

                C64YUVPalette2[i*3+0] = VicFarben[i].Luminace * Helligkeit;
                C64YUVPalette2[i*3+1] = -(float)(VIC_SATURATION * cos((VicFarben[i].Angel + VIC_PHASE + Offs) * (MATH_PI / 180.0))) * Helligkeit;
                C64YUVPalette2[i*3+2] = -(float)(VIC_SATURATION * sin((VicFarben[i].Angel + VIC_PHASE + Offs) * (MATH_PI / 180.0))) * Helligkeit;

                if (VicFarben[i].Direction == 0)
                {
                        C64YUVPalette1[i*3+1] = 0.0f;
                        C64YUVPalette1[i*3+2] = 0.0f;
                        C64YUVPalette2[i*3+1] = 0.0f;
                        C64YUVPalette2[i*3+2] = 0.0f;
                }
                if (VicFarben[i].Direction < 0)
                {
                        C64YUVPalette1[i*3+1] = -C64YUVPalette1[i*3+1];
                        C64YUVPalette1[i*3+2] = -C64YUVPalette1[i*3+2];
                        C64YUVPalette2[i*3+1] = -C64YUVPalette2[i*3+1];
                        C64YUVPalette2[i*3+2] = -C64YUVPalette2[i*3+2];
                }
        }

        int x[4];
        for(x[0]=0;x[0]<16;x[0]++)
                for(x[1]=0;x[1]<16;x[1]++)
                        for(x[2]=0;x[2]<16;x[2]++)
                                for(x[3]=0;x[3]<16;x[3]++)
                                        {
                                                /// Tabelle 0 für Gerade Zeile
                                                _y = C64YUVPalette1[x[0]*3];
                                                _u = C64YUVPalette1[x[0]*3+1];
                                                _v = C64YUVPalette1[x[0]*3+2];

                                                // UV BLUR
                                                for(int i=1; i<HoBlurWUV ; i++)
                                                {
                                                        _u += C64YUVPalette1[x[i]*3+1];
                                                        _v += C64YUVPalette1[x[i]*3+2];
                                                }
                                                _u /= HoBlurWUV;
                                                _v /= HoBlurWUV;

                                                /// Y BLUR
                                                for(int i=1; i<HoBlurWY ; i++)
                                                {
                                                        _y += C64YUVPalette1[x[i]*3];

                                                }
                                                _y /= HoBlurWY;

                                                r = (short)(_y+0.000*_u+1.140*_v);
                                                g = (short)(_y-0.396*_u-0.581*_v);
                                                b = (short)(_y+2.029*_u+0.000*_v);

                                                if(r < 0) r=0;
                                                if(r > 255) r=255;
                                                if(g < 0) g=0;
                                                if(g > 255) g=255;
                                                if(b < 0) b=0;
                                                if(b > 255) b=255;

                                                ColorIn = COLOR_STRUCT((float)r,(float)g,(float)b,.0f);
                                                ChangeSaturation(&ColorIn,&ColorOut,Saturation);

                                                //ColorIn = ColorOut;
                                                //D3DXColorAdjustContrast(&ColorOut,&ColorIn,Kontrast);

                                                RGB =  (unsigned long)ColorOut.r<<16; // Rot
                                                RGB |= (unsigned long)ColorOut.g<<8;  // Grün
                                                RGB |= (unsigned long)ColorOut.b;     // Blau

                                                BlurTable0[x[0]][x[1]][x[2]][x[3]] = RGB | 0xFF000000;

                                                /// Tabelle 1 für Gerade Zeile
                                                _y *= Scanline;
                                                r = (short)(_y+0.000*_u+1.140*_v);
                                                g = (short)(_y-0.396*_u-0.581*_v);
                                                b = (short)(_y+2.029*_u+0.000*_v);

                                                if(r < 0) r=0;
                                                if(r > 255) r=255;
                                                if(g < 0) g=0;
                                                if(g > 255) g=255;
                                                if(b < 0) b=0;
                                                if(b > 255) b=255;

                                                ColorIn = COLOR_STRUCT((float)r,(float)g,(float)b,.0f);
                                                ChangeSaturation(&ColorIn,&ColorOut,Saturation);

                                                //ColorIn = ColorOut;
                                                //D3DXColorAdjustContrast(&ColorOut,&ColorIn,Kontrast);

                                                RGB =  (unsigned long)ColorOut.r<<16; // Rot
                                                RGB |= (unsigned long)ColorOut.g<<8;  // Grün
                                                RGB |= (unsigned long)ColorOut.b;     // Blau

                                                BlurTable0S[x[0]][x[1]][x[2]][x[3]] = RGB | 0xFF000000;

                                                /// Tabelle 1 für Ungerade Zeilen
                                                _y = C64YUVPalette2[x[0]*3];
                                                _u = C64YUVPalette2[x[0]*3+1];
                                                _v = C64YUVPalette2[x[0]*3+2];

                                                // UV BLUR
                                                for(int i=1; i<HoBlurWUV ; i++)
                                                {
                                                        _u += C64YUVPalette2[x[i]*3+1];
                                                        _v += C64YUVPalette2[x[i]*3+2];
                                                }
                                                _u /= HoBlurWUV;
                                                _v /= HoBlurWUV;

                                                /// Y BLUR
                                                for(int i=1; i<HoBlurWY ; i++)
                                                {
                                                        _y += C64YUVPalette2[x[i]*3];
                                                }
                                                _y /= HoBlurWY;

                                                r = (short)(_y+0.000*_u+1.140*_v);
                                                g = (short)(_y-0.396*_u-0.581*_v);
                                                b = (short)(_y+2.029*_u+0.000*_v);

                                                if(r < 0) r=0;
                                                if(r > 255) r=255;
                                                if(g < 0) g=0;
                                                if(g > 255) g=255;
                                                if(b < 0) b=0;
                                                if(b > 255) b=255;

                                                ColorIn = COLOR_STRUCT((float)r,(float)g,(float)b,.0f);
                                                ChangeSaturation(&ColorIn,&ColorOut,Saturation);

                                                //ColorIn = ColorOut;
                                                //D3DXColorAdjustContrast(&ColorOut,&ColorIn,Kontrast);

                                                RGB =  (unsigned long)ColorOut.r<<16; // Rot
                                                RGB |= (unsigned long)ColorOut.g<<8;  // Grün
                                                RGB |= (unsigned long)ColorOut.b;     // Blau

                                                BlurTable1[x[0]][x[1]][x[2]][x[3]] = RGB | 0xFF000000;

                                                _y *= Scanline;

                                                r = (short)(_y+0.000*_u+1.140*_v);
                                                g = (short)(_y-0.396*_u-0.581*_v);
                                                b = (short)(_y+2.029*_u+0.000*_v);

                                                if(r < 0) r=0;
                                                if(r > 255) r=255;
                                                if(g < 0) g=0;
                                                if(g > 255) g=255;
                                                if(b < 0) b=0;
                                                if(b > 255) b=255;

                                                ColorIn = COLOR_STRUCT((float)r,(float)g,(float)b,.0f);
                                                ChangeSaturation(&ColorIn,&ColorOut,Saturation);

                                                //ColorIn = ColorOut;
                                                //D3DXColorAdjustContrast(&ColorOut,&ColorIn,Kontrast);

                                                RGB =  (unsigned long)ColorOut.r<<16; // Rot
                                                RGB |= (unsigned long)ColorOut.g<<8;  // Grün
                                                RGB |= (unsigned long)ColorOut.b;     // Blau

                                                BlurTable1S[x[0]][x[1]][x[2]][x[3]] = RGB | 0xFF000000;

                                                if(DestDisplayMode == 16)
                                                {
                                                    /*
                                                    if(pixel_format != 0)
                                                    {
                                                        unsigned long RGBTmp = BlurTable0[x[0]][x[1]][x[2]][x[3]];
                                                        r=(((RGBTmp&0x00FF0000)>>16)&0xFF);	// ROT
                                                        g=(((RGBTmp&0x0000FF00)>>8)&0xFF);	// GRUEN
                                                        b=(((RGBTmp&0x000000FF))&0xFF);         // BLAU
                                                        BlurTable0[x[0]][x[1]][x[2]][x[3]] = (unsigned long)SDL_MapRGB(pixel_format,r,g,b);


                                                        RGBTmp = BlurTable1[x[0]][x[1]][x[2]][x[3]];
                                                        r=(((RGBTmp&0x00FF0000)>>16)&0xFF);	// ROT
                                                        g=(((RGBTmp&0x0000FF00)>>8)&0xFF);	// GRUEN
                                                        b=(((RGBTmp&0x000000FF))&0xFF);         // BLAU
                                                        BlurTable1[x[0]][x[1]][x[2]][x[3]] = (unsigned long)SDL_MapRGB(pixel_format,r,g,b);

                                                        RGBTmp = BlurTable0S[x[0]][x[1]][x[2]][x[3]];
                                                        r=(((RGBTmp&0x00FF0000)>>16)&0xFF);	// ROT
                                                        g=(((RGBTmp&0x0000FF00)>>8)&0xFF);	// GRUEN
                                                        b=(((RGBTmp&0x000000FF))&0xFF);         // BLAU
                                                        BlurTable0S[x[0]][x[1]][x[2]][x[3]] = (unsigned long)SDL_MapRGB(pixel_format,r,g,b);

                                                        RGBTmp = BlurTable1S[x[0]][x[1]][x[2]][x[3]];
                                                        r=(((RGBTmp&0x00FF0000)>>16)&0xFF);	// ROT
                                                        g=(((RGBTmp&0x0000FF00)>>8)&0xFF);	// GRUEN
                                                        b=(((RGBTmp&0x000000FF))&0xFF);         // BLAU
                                                        BlurTable1S[x[0]][x[1]][x[2]][x[3]] = (unsigned long)SDL_MapRGB(pixel_format,r,g,b);
                                                    }
                                                    */
                                                }
                                        }
}

void VideoPalClass::ConvertVideo(void* Outpuffer,long Pitch,unsigned char* VICOutPuffer,int OutXW,int OutYW,int InXW,int,bool)
{
    static uint8_t w0,w1,w2,w3;
    VideoSource8 = (uint8_t*)VICOutPuffer;

    if(PALOutput)
    {
        if(Double2x)
        {
            switch(DestDisplayMode)
            {
            case 16:
                for(int y=0;y<OutYW/2;y++)
                {
                    Outpuffer16 = ((uint16_t*)Outpuffer + (((y*2)+1)*Pitch/2));
                    Outpuffer16Scanline = ((uint16_t*)Outpuffer + ((y*2)*(Pitch/2)));

                    w0 = w1 = w2 = w3 = *VideoSource8 & 0x0F;
                    switch(y&1)
                    {
                    case 0:
                        for(int x=0;x<(OutXW/2);x++)
                        {
                            *(Outpuffer16++) = (unsigned short)BlurTable0[w0][w1][w2][w3];
                            *(Outpuffer16++) = (unsigned short)BlurTable0[w0][w1][w2][w3];
                            *(Outpuffer16Scanline++) = (unsigned short)BlurTable0S[w0][w1][w2][w3];
                            *(Outpuffer16Scanline++) = (unsigned short)BlurTable0S[w0][w1][w2][w3];
                            w3 = w2;
                            w2 = w1;
                            w1 = w0;
                            w0 = *(VideoSource8+x+2) & 0x0F;
                        }
                        break;
                    case 1:
                        for(int x=0;x<(OutXW/2);x++)
                        {
                            *(Outpuffer16++) = (unsigned short)BlurTable1[w0][w1][w2][w3];
                            *(Outpuffer16++) = (unsigned short)BlurTable1[w0][w1][w2][w3];
                            *(Outpuffer16Scanline++) = (unsigned short)BlurTable1S[w0][w1][w2][w3];
                            *(Outpuffer16Scanline++) = (unsigned short)BlurTable1S[w0][w1][w2][w3];
                            w3 = w2;
                            w2 = w1;
                            w1 = w0;
                            w0 = *(VideoSource8+x+2) & 0x0F;
                        }
                        break;
                    }
                    VideoSource8 = VideoSource8+InXW;
                }
                break;
            case 32:
                for(int y=0;y<(OutYW/2);y++)
                {
                    Outpuffer32 = ((uint32_t*)Outpuffer + ((y*2)*Pitch/4));
                    Outpuffer32Scanline = ((uint32_t*)Outpuffer + (((y*2)+1)*(Pitch/4)));

                    w0 = w1 = w2 = w3 = *VideoSource8 & 0x0F;

                    switch(y&1)
                    {
                    case 0:
                        for(int x=0;x<(OutXW/2);x++)
                        {
                            *(Outpuffer32++) = BlurTable0[w0][w1][w2][w3];
                            *(Outpuffer32++) = BlurTable0[w0][w1][w2][w3];
                            *(Outpuffer32Scanline++) = BlurTable0S[w0][w1][w2][w3];
                            *(Outpuffer32Scanline++) = BlurTable0S[w0][w1][w2][w3];
                            w3 = w2;
                            w2 = w1;
                            w1 = w0;
                            w0 = *(VideoSource8+x+1) & 0x0F;
                        }
                        break;
                    case 1:
                        for(int x=0;x<(OutXW/2);x++)
                        {
                            *(Outpuffer32++) = BlurTable1[w0][w1][w2][w3];
                            *(Outpuffer32++) = BlurTable1[w0][w1][w2][w3];
                            *(Outpuffer32Scanline++) = BlurTable1S[w0][w1][w2][w3];
                            *(Outpuffer32Scanline++) = BlurTable1S[w0][w1][w2][w3];
                            w3 = w2;
                            w2 = w1;
                            w1 = w0;
                            w0 = *(VideoSource8+x+1) & 0x0F;
                        }
                        break;
                    }
                    VideoSource8 = VideoSource8+InXW;
                }
                break;
            }
        }
        else
        {
            switch(DestDisplayMode)
            {
            case 16:
                for(int y=0;y<OutYW;y++)
                {
                    Outpuffer16 = ((unsigned short*)Outpuffer + ((y)*Pitch/2));

                    w0 = w1 = w2 = w3 = *VideoSource8 & 0x0F;

                    switch(y&1)
                    {
                    case 0:
                        for(int x=0;x<(OutXW);x++)
                        {
                            *(Outpuffer16++) = (unsigned short)BlurTable0[w0][w1][w2][w3];
                            w3 = w2;
                            w2 = w1;
                            w1 = w0;
                            w0 = *(VideoSource8+x+1) & 0x0F;
                        }
                        break;
                    case 1:
                        for(int x=0;x<(OutXW);x++)
                        {
                            *(Outpuffer16++) = (unsigned short)BlurTable1[w0][w1][w2][w3];
                            w3 = w2;
                            w2 = w1;
                            w1 = w0;
                            w0 = *(VideoSource8+x+1) & 0x0F;
                        }
                        break;
                    }
                    VideoSource8 = VideoSource8+InXW;
                }
                break;
            case 32:

                for(int y=0;y<OutYW;y++)
                {
                    Outpuffer32 = ((uint32_t*)Outpuffer + ((y)*Pitch/4));
                    Outpuffer32Scanline = ((uint32_t*)Outpuffer + (((y*2)+1)*(Pitch/4)));

                    w0 = w1 = w2 = w3 = *VideoSource8 & 0x0F;

                    switch(y&1)
                    {
                    case 0:
                        for(int x=0;x<(OutXW);x++)
                        {
                                *(Outpuffer32++) = BlurTable0[w0][w1][w2][w3];
                                w3 = w2;
                                w2 = w1;
                                w1 = w0;
                                w0 = *(VideoSource8+x+1) & 0x0F;
                        }
                        break;
                    case 1:
                        for(int x=0;x<(OutXW);x++)
                        {
                                *(Outpuffer32++) = BlurTable1[w0][w1][w2][w3];
                                w3 = w2;
                                w2 = w1;
                                w1 = w0;
                                w0 = *(VideoSource8+x+1) & 0x0F;
                        }
                        break;
                    }
                    VideoSource8 = VideoSource8+InXW;
                }

                break;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    ///////////////////// AUSGABE ÜBER NORMALE FARBPALETTE ///////////////////
    else
    if(Double2x)
    {
        switch(DestDisplayMode)
        {
        case 16: /// OK
            for(int y=0;y<(OutYW/2);y++)
            {
                Outpuffer16 = ((uint16_t*)Outpuffer + (((y*2)+1)*Pitch/2));
                Outpuffer16Scanline = ((uint16_t*)Outpuffer + ((y*2)*(Pitch/2)));

                for(int x=0;x<(OutXW/2);x++)
                {
                    *(Outpuffer16++) = Palette16Bit[VideoSource8[x] & 0x0F];
                    *(Outpuffer16++) = Palette16Bit[VideoSource8[x] & 0x0F];
                    *(Outpuffer16Scanline++) = Palette16Bit[VideoSource8[x] & 0x0F];
                    *(Outpuffer16Scanline++) = Palette16Bit[VideoSource8[x] & 0x0F];
                }
                VideoSource8 = VideoSource8+InXW;
            }
            break;
        case 32: /// OK
            for(int y=0;y<(OutYW/2);y++)
            {
                Outpuffer32 = ((uint32_t*)Outpuffer + ((y*2)*Pitch/4));
                Outpuffer32Scanline = ((uint32_t*)Outpuffer + (((y*2)+1)*(Pitch/4)));
                for(int x=0;x<(OutXW/2);x++)
                {
                    *(Outpuffer32++) = Palette32Bit[VideoSource8[x] & 0x0F];
                    *(Outpuffer32++) = Palette32Bit[VideoSource8[x] & 0x0F];
                    *(Outpuffer32Scanline++) = Palette32Bit[VideoSource8[x] & 0x0F];
                    *(Outpuffer32Scanline++) = Palette32Bit[VideoSource8[x] & 0x0F];
                }
                VideoSource8 = VideoSource8+InXW;
            }
            break;
        }
    }
    else
    {
        switch(DestDisplayMode)
        {
        case 16: /// OK
            for(int y=0;y<(OutYW);y++)
            {
                Outpuffer16 = ((uint16_t*)Outpuffer + (((y)+1)*Pitch/2));

                for(int x=0;x<(OutXW);x++)
                {
                    *(Outpuffer16++) = Palette16Bit[VideoSource8[x] & 0x0F];
                }
                VideoSource8 = VideoSource8+InXW;
            }
            break;
        case 32: /// OK
            for(int y=0;y<(OutYW);y++)
            {
                Outpuffer32 = ((uint32_t*)Outpuffer + ((y)*Pitch/4));
                for(int x=0;x<(OutXW);x++)
                {
                    *(Outpuffer32++) = Palette32Bit[VideoSource8[x] & 0x0F];
                }
                VideoSource8 = VideoSource8+InXW;
            }
            break;
        }
    }
}

#define Pr .299
#define Pg .587
#define Pb .114

void VideoPalClass::ChangeSaturation(COLOR_STRUCT *col_in, COLOR_STRUCT *col_out, float wert)
{
    float R = col_in->r;
    float G = col_in->g;
    float B = col_in->b;

    float p=sqrt((R*R*Pr)+(G*G*Pg)+(B*B*Pb));

    col_out->r = p+(R-p)*wert;
    col_out->g = p+(G-p)*wert;
    col_out->b = p+(B-p)*wert;
}
