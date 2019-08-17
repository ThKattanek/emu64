//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: video_crt_class.cpp                   //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 02.06.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "video_crt_class.h"
#include "c64_colors.h"
#include <math.h>

#include <iostream>

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

VideoCrtClass::VideoCrtClass()
{
    AktFarbMode = 0;
    Double2x = false;
    Kontrast = 0.8;

    HoBlurWY = 0;
    HoBlurWUV = 0;
}

VideoCrtClass::~VideoCrtClass(void)
{
}

void VideoCrtClass::SetDisplayMode(int DisplayMode)
{
    DestDisplayMode = DisplayMode;
    UpdateParameter();
}

void VideoCrtClass::UpdateParameter(void)
{
    CreateVicIIColors();
}

void VideoCrtClass::SetPhaseAltLineOffset(int offset)
{
    if(offset < 0) offset = 0;
    if(offset > 2000) offset = 2000;
    PhaseAlternatingLine = offset;
}

void VideoCrtClass::SetHorizontalBlurY(int wblur)
{
    if(wblur > 5) wblur = 5;
    HoBlurWY = wblur;
    blur_y_mul = 1.0f/(HoBlurWY);
}

void VideoCrtClass::SetHorizontalBlurUV(int wblur)
{
    if(wblur > 5) wblur = 5;
    HoBlurWUV = wblur;
    blur_uv_mul = 1.0f/(HoBlurWUV);
}

void VideoCrtClass::SetScanline(int wert)
{
    Scanline = wert/100.0f;
}

void VideoCrtClass::SetSaturation(float wert)
{
    Saturation = wert*1.2f;
}

void VideoCrtClass::SetHelligkeit(float wert)
{
    Helligkeit = wert;
}

void VideoCrtClass::SetKontrast(float wert)
{
    Kontrast = wert+0.5f;
}

void VideoCrtClass::SetC64Palette(int palnr)
{
    AktFarbMode = palnr;

    //if(pixel_format == 0) return;

    int ij = 0;
    for(int j=0;j<16;j++)
    {
        uint8_t *color_rgba = (uint8_t*)&c64_colors[palnr*16*4];

        for(int i=0;i<16;i++)
        {
            /// Für 32Bit Video Display ///
            Palette32Bit[ij] = 0xFF000000 | color_rgba[2]<<16 | color_rgba[1]<<8 | color_rgba[0];

            /// Für 16Bit Video Display ///
            /// RGB-565
            unsigned char r = (color_rgba[0] * 31) / 255;
            unsigned char g = (color_rgba[1] * 63) / 255;
            unsigned char b = (color_rgba[2] * 31) / 255;
            Palette16Bit[ij] = r<<11 | g<<5 | b;

            color_rgba+=4;
            ij++;
        }
    }
}

void VideoCrtClass::EnableVideoDoubleSize(bool enabled)
{
        Double2x = enabled;
}

void VideoCrtClass::EnableCrtOutput(bool enabled)
{
        enable_crt_output = enabled;
}

float *VideoCrtClass::GetC64YUVPalette()
{
    return C64YUVPalette1;
}

inline void VideoCrtClass::RGB_To_YUV(float rgb[3], float yuv[3])
{
        yuv[0] = float(0.299*rgb[0]+0.587*rgb[1]+0.114*rgb[2]);  // Y
        yuv[1] = float(-0.147*rgb[0]-0.289*rgb[1]+0.436*rgb[2]); // U
        yuv[2] = float(0.615*rgb[0]-0.515*rgb[1]-0.100*rgb[2]);  // V
}

inline void VideoCrtClass::YUV_To_RGB(float yuv[3], float rgb[3])
{
        rgb[0] = float(yuv[0]+0.000*yuv[1]+1.140*yuv[2]); // Rot
        rgb[1] = float(yuv[0]-0.396*yuv[1]-0.581*yuv[2]); // Grün
        rgb[2] = float(yuv[0]+2.029*yuv[1]+0.000*yuv[2]); // Blau
}

const float PI = 3.14159265358979323846264338327950288419716939937510;

inline void VideoCrtClass::CreateVicIIColors(void)
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

                                                // Y BLUR
                                                float HoBlurWYIntension = 0.75f;
                                                if(HoBlurWY > 1)
                                                {
                                                    _y *= HoBlurWYIntension;
                                                    float intension_add = (1.0f - HoBlurWYIntension) / (HoBlurWY-1);
                                                    for(int i=1; i<HoBlurWY ; i++)
                                                    {
                                                         _y += C64YUVPalette1[x[i]*3] * i * intension_add;
                                                    }
                                                }

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

                                                ColorIn = ColorOut;
                                                ChangeContrast(&ColorIn, &ColorOut,Kontrast);

                                                if(DestDisplayMode == 32)
                                                {
                                                    RGB =  (unsigned long)ColorOut.r;       // Rot
                                                    RGB |= (unsigned long)ColorOut.g<<8;    // Grün
                                                    RGB |= (unsigned long)ColorOut.b<<16;   // Blau

                                                    BlurTable0[x[0]][x[1]][x[2]][x[3]] = RGB | 0xFF000000;
                                                }

                                                if(DestDisplayMode == 16)
                                                {
                                                    unsigned char r = ((unsigned char)ColorOut.r * 31) / 255;
                                                    unsigned char g = ((unsigned char)ColorOut.g * 63) / 255;
                                                    unsigned char b = ((unsigned char)ColorOut.b * 31) / 255;

                                                    BlurTable0[x[0]][x[1]][x[2]][x[3]] = (unsigned long)(r<<11 | g<<5 | b);
                                                }

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

                                                ColorIn = ColorOut;
                                                ChangeContrast(&ColorIn, &ColorOut,Kontrast);

                                                if(DestDisplayMode == 32)
                                                {
                                                    RGB =  (unsigned long)ColorOut.r;       // Rot
                                                    RGB |= (unsigned long)ColorOut.g<<8;    // Grün
                                                    RGB |= (unsigned long)ColorOut.b<<16;   // Blau

                                                    BlurTable0S[x[0]][x[1]][x[2]][x[3]] = RGB | 0xFF000000;
                                                }

                                                if(DestDisplayMode == 16)
                                                {
                                                    unsigned char r = ((unsigned char)ColorOut.r * 31) / 255;
                                                    unsigned char g = ((unsigned char)ColorOut.g * 63) / 255;
                                                    unsigned char b = ((unsigned char)ColorOut.b * 31) / 255;

                                                    BlurTable0S[x[0]][x[1]][x[2]][x[3]] = (unsigned long)(r<<11 | g<<5 | b);
                                                }

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

                                                // Y BLUR
                                                if(HoBlurWY > 1)
                                                {
                                                    _y *= HoBlurWYIntension;
                                                    float intension_add = (1.0f - HoBlurWYIntension) / (HoBlurWY-1);
                                                    for(int i=1; i<HoBlurWY ; i++)
                                                    {
                                                         _y += C64YUVPalette1[x[i]*3] * i * intension_add;
                                                    }
                                                }

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

                                                ColorIn = ColorOut;
                                                ChangeContrast(&ColorIn, &ColorOut,Kontrast);

                                                if(DestDisplayMode == 32)
                                                {
                                                    RGB =  (unsigned long)ColorOut.r;       // Rot
                                                    RGB |= (unsigned long)ColorOut.g<<8;    // Grün
                                                    RGB |= (unsigned long)ColorOut.b<<16;   // Blau

                                                    BlurTable1[x[0]][x[1]][x[2]][x[3]] = RGB | 0xFF000000;

                                                }

                                                if(DestDisplayMode == 16)
                                                {
                                                    unsigned char r = ((unsigned char)ColorOut.r * 31) / 255;
                                                    unsigned char g = ((unsigned char)ColorOut.g * 63) / 255;
                                                    unsigned char b = ((unsigned char)ColorOut.b * 31) / 255;

                                                    BlurTable1[x[0]][x[1]][x[2]][x[3]] = (unsigned long)(r<<11 | g<<5 | b);
                                                }

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

                                                ColorIn = ColorOut;
                                                ChangeContrast(&ColorIn, &ColorOut,Kontrast);

                                                if(DestDisplayMode == 32)
                                                {
                                                    RGB =  (unsigned long)ColorOut.r;       // Rot
                                                    RGB |= (unsigned long)ColorOut.g<<8;    // Grün
                                                    RGB |= (unsigned long)ColorOut.b<<16;   // Blau

                                                    BlurTable1S[x[0]][x[1]][x[2]][x[3]] = RGB | 0xFF000000;
                                                }

                                                if(DestDisplayMode == 16)
                                                {
                                                    unsigned char r = ((unsigned char)ColorOut.r * 31) / 255;
                                                    unsigned char g = ((unsigned char)ColorOut.g * 63) / 255;
                                                    unsigned char b = ((unsigned char)ColorOut.b * 31) / 255;

                                                    BlurTable1S[x[0]][x[1]][x[2]][x[3]] = (unsigned long)(r<<11 | g<<5 | b);
                                                }

                                                /*
                                                if(DestDisplayMode == 16)
                                                {
                                                    unsigned short r;
                                                    unsigned short g;
                                                    unsigned short b;

                                                    unsigned long RGBTmp = BlurTable0[x[0]][x[1]][x[2]][x[3]];
                                                    r = ((((RGBTmp&0x00FF0000)>>16)&0xFF) * 31) / 255;
                                                    g = ((((RGBTmp&0x0000FF00)>>8)&0xFF) * 63) / 255;
                                                    b = ((((RGBTmp&0x000000FF))&0xFF) * 31) / 255;
                                                    //BlurTable0[x[0]][x[1]][x[2]][x[3]] = r<<11 | g<<5 | b;
                                                    BlurTable0[x[0]][x[1]][x[2]][x[3]] = r<<11 | g<<5 | b;

                                                    RGBTmp = BlurTable1[x[0]][x[1]][x[2]][x[3]];
                                                    r = ((((RGBTmp&0x00FF0000)>>16)&0xFF) * 31) / 255;
                                                    g = ((((RGBTmp&0x0000FF00)>>8)&0xFF) * 63) / 255;
                                                    b = ((((RGBTmp&0x000000FF))&0xFF) * 31) / 255;
                                                    //BlurTable1[x[0]][x[1]][x[2]][x[3]] = r<<11 | g<<5 | b;
                                                    BlurTable1[x[0]][x[1]][x[2]][x[3]] = r<<11 | g<<5 | b;

                                                    RGBTmp = BlurTable0S[x[0]][x[1]][x[2]][x[3]];
                                                    r = ((((RGBTmp&0x00FF0000)>>16)&0xFF) * 31) / 255;
                                                    g = ((((RGBTmp&0x0000FF00)>>8)&0xFF) * 63) / 255;
                                                    b = ((((RGBTmp&0x000000FF))&0xFF) * 31) / 255;
                                                    //BlurTable0S[x[0]][x[1]][x[2]][x[3]] = r<<11 | g<<5 | b;
                                                    BlurTable0S[x[0]][x[1]][x[2]][x[3]] = r<<11 | g<<5 | b;

                                                    RGBTmp = BlurTable1S[x[0]][x[1]][x[2]][x[3]];
                                                    r = ((((RGBTmp&0x00FF0000)>>16)&0xFF) * 31) / 255;
                                                    g = ((((RGBTmp&0x0000FF00)>>8)&0xFF) * 63) / 255;
                                                    b = ((((RGBTmp&0x000000FF))&0xFF) * 31) / 255;
                                                    //BlurTable1S[x[0]][x[1]][x[2]][x[3]] = r<<11 | g<<5 | b;
                                                    BlurTable1S[x[0]][x[1]][x[2]][x[3]] = r<<11 | g<<5 | b;
                                                }
                                                */
                                        }
}

void VideoCrtClass::ConvertVideo(void* Outpuffer,long Pitch,unsigned char* VICOutPuffer,int VICOutPufferOffset,int OutXW,int OutYW,int InXW,int,bool)
{
    static uint8_t w0,w1,w2,w3;
    VideoSource8 = (uint8_t*)VICOutPuffer;
    VideoSource8 += VICOutPufferOffset;

    if(enable_crt_output)
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

void VideoCrtClass::ChangeSaturation(COLOR_STRUCT *col_in, COLOR_STRUCT *col_out, float wert)
{
    float R = col_in->r;
    float G = col_in->g;
    float B = col_in->b;

    float grey = R * 0.2125f + G * 0.7154f + B * 0.0721f;
    col_out->r = grey + wert * (R - grey);
    col_out->g = grey + wert * (G - grey);
    col_out->b = grey + wert * (B - grey);

    if(col_out->r > 255.0f) col_out->r = 255.0f;
    else if(col_out->r < 0.0f) col_out->r = 0.0f;

    if(col_out->g > 255.0f) col_out->g = 255.0f;
    else if(col_out->g < 0.0f) col_out->g = 0.0f;

    if(col_out->b > 255.0f) col_out->b = 255.0f;
    else if(col_out->b < 0.0f) col_out->b = 0.0f;
}

void VideoCrtClass::ChangeContrast(COLOR_STRUCT *col_in, COLOR_STRUCT *col_out, float wert)
{
    float R = col_in->r;
    float G = col_in->g;
    float B = col_in->b;

    col_out->r = 0.5f + wert * (R - 0.5f);
    col_out->g = 0.5f + wert * (G - 0.5f);
    col_out->b = 0.5f + wert * (B - 0.5f);

    if(col_out->r > 255.0f) col_out->r = 255.0f;
    else if(col_out->r < 0.0f) col_out->r = 0.0f;

    if(col_out->g > 255.0f) col_out->g = 255.0f;
    else if(col_out->g < 0.0f) col_out->g = 0.0f;

    if(col_out->b > 255.0f) col_out->b = 255.0f;
    else if(col_out->b < 0.0f) col_out->b = 0.0f;
}
