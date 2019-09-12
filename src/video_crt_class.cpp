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
// Letzte Änderung am 12.09.2019                //
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

static VIC_COLOR_STRUCT VicColors[16] =
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
    current_color_palette_mumber = 0;
    is_double2x = false;
    contrast = 0.8f;

    hor_blur_wy = 0;
    hor_blur_wuv = 0;
}

VideoCrtClass::~VideoCrtClass(void)
{
}

void VideoCrtClass::UpdateParameter(void)
{
    CreateVicIIColors();
}

void VideoCrtClass::SetPhaseAlternatingLineOffset(int offset)
{
    if(offset < 0) offset = 0;
    if(offset > 2000) offset = 2000;
    phase_alternating_line = offset;
}

void VideoCrtClass::SetHorizontalBlurY(int wblur)
{
    if(wblur > 5) wblur = 5;
    hor_blur_wy = wblur;
    blur_y_mul = 1.0f/(hor_blur_wy);
}

void VideoCrtClass::SetHorizontalBlurUV(int wblur)
{
    if(wblur > 5) wblur = 5;
    hor_blur_wuv = wblur;
    blur_uv_mul = 1.0f/(hor_blur_wuv);
}

void VideoCrtClass::SetScanline(int value)
{
    scanline = value/100.0f;
}

void VideoCrtClass::SetSaturation(float value)
{
    saturation = value*1.2f;
}

void VideoCrtClass::SetBrightness(float value)
{
    brightness = value;
}

void VideoCrtClass::SetContrast(float value)
{
    contrast = value+0.5f;
}

void VideoCrtClass::SetC64Palette(uint8_t palnr)
{
    current_color_palette_mumber = palnr;

    int ij = 0;
    for(int j=0;j<16;j++)
    {
        uint8_t *color_rgba = &c64_colors[palnr*16*4];

        for(int i=0;i<16;i++)
        {
            /// Für 32Bit Video Display ///
            palette[ij] = 0xFF000000 | static_cast<uint32_t>((color_rgba[2]<<16 | color_rgba[1]<<8 | color_rgba[0]));
            color_rgba+=4;
            ij++;
        }
    }
}

void VideoCrtClass::EnableVideoDoubleSize(bool enabled)
{
        is_double2x = enabled;
}

void VideoCrtClass::EnableCrtOutput(bool enabled)
{
        is_crt_output = enabled;
}

float *VideoCrtClass::GetC64YUVPalette()
{
    return c64_yuv_palette0;
}

inline void VideoCrtClass::ConvertYUVToRGB()
{
    // from yuv to rgb
    r = static_cast<int16_t>(_y + 0.0f *_u + 1.140f *_v);
    g = static_cast<int16_t>(_y - 0.396f * _u - 0.581f * _v);
    b = static_cast<int16_t>(_y + 2.029f * _u);

    if(r<0) r=0;
    if(r>255) r=255;
    if(g<0) g=0;
    if(g>255) g=255;
    if(b<0) b=0;
    if(b>255) b=255;
}

inline void VideoCrtClass::CreateVicIIColors(void)
{
        COLOR_STRUCT ColorOut;
        COLOR_STRUCT ColorIn;

        /// Für Phase Alternating Line
        float Offs=((phase_alternating_line) / (2000.0f / 90.0f))+(180.0f-45.0f);

        for(int i=0;i<16;i++)
        {
                c64_yuv_palette0[i*3+0] = VicColors[i].luminace * brightness;
                c64_yuv_palette0[i*3+1] = VIC_SATURATION * static_cast<float>(cos((VicColors[i].angel + VIC_PHASE) * (static_cast<float>(MATH_PI) / 180.0f))) * brightness;
                c64_yuv_palette0[i*3+2] = VIC_SATURATION * sin((VicColors[i].angel + VIC_PHASE) * (static_cast<float>(MATH_PI) / 180.0f)) * brightness;

                c64_yuv_palette1[i*3+0] = VicColors[i].luminace * brightness;
                c64_yuv_palette1[i*3+1] = -(VIC_SATURATION * static_cast<float>(cos((VicColors[i].angel + VIC_PHASE + Offs) * (static_cast<float>(MATH_PI) / 180.0f)))) * brightness;
                c64_yuv_palette1[i*3+2] = -(VIC_SATURATION * static_cast<float>(sin((VicColors[i].angel + VIC_PHASE + Offs) * (static_cast<float>(MATH_PI) / 180.0f)))) * brightness;

                if (VicColors[i].direction == 0.0f)
                {
                        c64_yuv_palette0[i*3+1] = 0.0f;
                        c64_yuv_palette0[i*3+2] = 0.0f;
                        c64_yuv_palette1[i*3+1] = 0.0f;
                        c64_yuv_palette1[i*3+2] = 0.0f;
                }
                if (VicColors[i].direction < 0)
                {
                        c64_yuv_palette0[i*3+1] = -c64_yuv_palette0[i*3+1];
                        c64_yuv_palette0[i*3+2] = -c64_yuv_palette0[i*3+2];
                        c64_yuv_palette1[i*3+1] = -c64_yuv_palette1[i*3+1];
                        c64_yuv_palette1[i*3+2] = -c64_yuv_palette1[i*3+2];
                }
        }

        int x[4];
        for(x[0]=0;x[0]<16;x[0]++)
                for(x[1]=0;x[1]<16;x[1]++)
                        for(x[2]=0;x[2]<16;x[2]++)
                                for(x[3]=0;x[3]<16;x[3]++)
                                        {
                                                /// Tabelle 0 für Gerade Zeile
                                                _y = c64_yuv_palette0[x[0]*3];
                                                _u = c64_yuv_palette0[x[0]*3+1];
                                                _v = c64_yuv_palette0[x[0]*3+2];

                                                // UV BLUR
                                                for(int i=1; i<hor_blur_wuv ; i++)
                                                {
                                                        _u += c64_yuv_palette0[x[i]*3+1];
                                                        _v += c64_yuv_palette0[x[i]*3+2];
                                                }
                                                _u /= hor_blur_wuv;
                                                _v /= hor_blur_wuv;

                                                // Y BLUR
                                                float HoBlurWYIntension = 0.75f;
                                                if(hor_blur_wy > 1)
                                                {
                                                    _y *= HoBlurWYIntension;
                                                    float intension_add = (1.0f - HoBlurWYIntension) / (hor_blur_wy-1);
                                                    for(int i=1; i<hor_blur_wy ; i++)
                                                    {
                                                         _y += c64_yuv_palette0[x[i]*3] * i * intension_add;
                                                    }
                                                }

                                                ConvertYUVToRGB();

                                                ColorIn = COLOR_STRUCT(r, g, b, 0.0f);
                                                ChangeSaturation(&ColorIn,&ColorOut,saturation);

                                                ColorIn = ColorOut;
                                                ChangeContrast(&ColorIn, &ColorOut,contrast);

                                                rgb =  static_cast<uint32_t>(ColorOut.r);       // Rot
                                                rgb |= static_cast<uint32_t>(ColorOut.g)<<8;    // Grün
                                                rgb |= static_cast<uint32_t>(ColorOut.b)<<16;   // Blau

                                                BlurTable0[x[0]][x[1]][x[2]][x[3]] = rgb | 0xFF000000;

                                                /// Tabelle 1 für Gerade Zeile
                                                _y *= scanline;

                                                ConvertYUVToRGB();

                                                ColorIn = COLOR_STRUCT(r, g, b, 0.0f);
                                                ChangeSaturation(&ColorIn,&ColorOut,saturation);

                                                ColorIn = ColorOut;
                                                ChangeContrast(&ColorIn, &ColorOut,contrast);


                                                rgb =  static_cast<uint32_t>(ColorOut.r);       // Rot
                                                rgb |= static_cast<uint32_t>(ColorOut.g)<<8;    // Grün
                                                rgb |= static_cast<uint32_t>(ColorOut.b)<<16;   // Blau

                                                BlurTable0S[x[0]][x[1]][x[2]][x[3]] = rgb | 0xFF000000;

                                                /// Tabelle 1 für Ungerade Zeilen
                                                _y = c64_yuv_palette1[x[0]*3];
                                                _u = c64_yuv_palette1[x[0]*3+1];
                                                _v = c64_yuv_palette1[x[0]*3+2];

                                                // UV BLUR
                                                for(int i=1; i<hor_blur_wuv ; i++)
                                                {
                                                        _u += c64_yuv_palette1[x[i]*3+1];
                                                        _v += c64_yuv_palette1[x[i]*3+2];
                                                }
                                                _u /= hor_blur_wuv;
                                                _v /= hor_blur_wuv;

                                                // Y BLUR
                                                if(hor_blur_wy > 1)
                                                {
                                                    _y *= HoBlurWYIntension;
                                                    float intension_add = (1.0f - HoBlurWYIntension) / (hor_blur_wy-1);
                                                    for(int i=1; i<hor_blur_wy ; i++)
                                                    {
                                                         _y += c64_yuv_palette0[x[i]*3] * i * intension_add;
                                                    }
                                                }

                                                ConvertYUVToRGB();

                                                ColorIn = COLOR_STRUCT(r, g, b, 0.0f);
                                                ChangeSaturation(&ColorIn,&ColorOut,saturation);

                                                ColorIn = ColorOut;
                                                ChangeContrast(&ColorIn, &ColorOut,contrast);

                                                rgb =  static_cast<uint32_t>(ColorOut.r);       // Rot
                                                rgb |= static_cast<uint32_t>(ColorOut.g)<<8;    // Grün
                                                rgb |= static_cast<uint32_t>(ColorOut.b)<<16;   // Blau

                                                BlurTable1[x[0]][x[1]][x[2]][x[3]] = rgb | 0xFF000000;

                                                _y *= scanline;

                                                ConvertYUVToRGB();

                                                ColorIn = COLOR_STRUCT(r, g, b, 0.0f);
                                                ChangeSaturation(&ColorIn,&ColorOut,saturation);

                                                ColorIn = ColorOut;
                                                ChangeContrast(&ColorIn, &ColorOut,contrast);

                                                rgb = static_cast<uint32_t>(ColorOut.r);       // Rot
                                                rgb |= static_cast<uint32_t>(ColorOut.g)<<8;    // Grün
                                                rgb |= static_cast<uint32_t>(ColorOut.b)<<16;   // Blau

                                                BlurTable1S[x[0]][x[1]][x[2]][x[3]] = rgb | 0xFF000000;

                                        }
}

void VideoCrtClass::ConvertVideo(void* Outpuffer,long Pitch,unsigned char* VICOutPuffer,int VICOutPufferOffset,int OutXW,int OutYW,int InXW,int,bool)
{
    static uint8_t w0,w1,w2,w3;
    video_source = VICOutPuffer;
    video_source += VICOutPufferOffset;

    if(is_crt_output)
    {
        if(is_double2x)
        {
            for(int y=0;y<(OutYW/2);y++)
            {
                out_buffer = (static_cast<uint32_t*>(Outpuffer) + ((y*2)*Pitch/4));
                out_buffer_scanline = (static_cast<uint32_t*>(Outpuffer) + (((y*2)+1)*(Pitch/4)));

                w0 = w1 = w2 = w3 = *video_source & 0x0F;

                switch(y&1)
                {
                case 0:
                    for(int x=0;x<(OutXW/2);x++)
                    {
                        *(out_buffer++) = BlurTable0[w0][w1][w2][w3];
                        *(out_buffer++) = BlurTable0[w0][w1][w2][w3];
                        *(out_buffer_scanline++) = BlurTable0S[w0][w1][w2][w3];
                        *(out_buffer_scanline++) = BlurTable0S[w0][w1][w2][w3];
                        w3 = w2;
                        w2 = w1;
                        w1 = w0;
                        w0 = *(video_source+x+1) & 0x0F;
                    }
                    break;
                case 1:
                    for(int x=0;x<(OutXW/2);x++)
                    {
                        *(out_buffer++) = BlurTable1[w0][w1][w2][w3];
                        *(out_buffer++) = BlurTable1[w0][w1][w2][w3];
                        *(out_buffer_scanline++) = BlurTable1S[w0][w1][w2][w3];
                        *(out_buffer_scanline++) = BlurTable1S[w0][w1][w2][w3];
                        w3 = w2;
                        w2 = w1;
                        w1 = w0;
                        w0 = *(video_source+x+1) & 0x0F;
                    }
                    break;
                }
                video_source = video_source+InXW;
            }
        }
        else
        { 
            for(int y=0;y<OutYW;y++)
            {
                out_buffer = (static_cast<uint32_t*>(Outpuffer) + ((y)*Pitch/4));
                out_buffer_scanline = (static_cast<uint32_t*>(Outpuffer) + (((y*2)+1)*(Pitch/4)));

                w0 = w1 = w2 = w3 = *video_source & 0x0F;

                switch(y&1)
                {
                case 0:
                    for(int x=0;x<(OutXW);x++)
                    {
                            *(out_buffer++) = BlurTable0[w0][w1][w2][w3];
                            w3 = w2;
                            w2 = w1;
                            w1 = w0;
                            w0 = *(video_source+x+1) & 0x0F;
                    }
                    break;
                case 1:
                    for(int x=0;x<(OutXW);x++)
                    {
                            *(out_buffer++) = BlurTable1[w0][w1][w2][w3];
                            w3 = w2;
                            w2 = w1;
                            w1 = w0;
                            w0 = *(video_source+x+1) & 0x0F;
                    }
                    break;
                }
                video_source = video_source+InXW;

            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    ///////////////////// AUSGABE ÜBER NORMALE FARBPALETTE ///////////////////
    else
    if(is_double2x)
    {
        for(int y=0;y<(OutYW/2);y++)
        {
            out_buffer = (static_cast<uint32_t*>(Outpuffer) + ((y*2)*Pitch/4));
            out_buffer_scanline = (static_cast<uint32_t*>(Outpuffer) + (((y*2)+1)*(Pitch/4)));
            for(int x=0;x<(OutXW/2);x++)
            {
                *(out_buffer++) = palette[video_source[x] & 0x0F];
                *(out_buffer++) = palette[video_source[x] & 0x0F];
                *(out_buffer_scanline++) = palette[video_source[x] & 0x0F];
                *(out_buffer_scanline++) = palette[video_source[x] & 0x0F];
            }
            video_source = video_source+InXW;
        }
    }
    else
    {
        for(int y=0;y<(OutYW);y++)
        {
            out_buffer = (static_cast<uint32_t*>(Outpuffer) + ((y)*Pitch/4));
            for(int x=0;x<(OutXW);x++)
            {
                *(out_buffer++) = palette[video_source[x] & 0x0F];
            }
            video_source = video_source+InXW;
        }
    }
}

void VideoCrtClass::ChangeSaturation(COLOR_STRUCT *col_in, COLOR_STRUCT *col_out, float value)
{
    float R = col_in->r;
    float G = col_in->g;
    float B = col_in->b;

    float grey = R * 0.2125f + G * 0.7154f + B * 0.0721f;
    col_out->r = grey + value * (R - grey);
    col_out->g = grey + value * (G - grey);
    col_out->b = grey + value * (B - grey);

    if(col_out->r > 255.0f) col_out->r = 255.0f;
    else if(col_out->r < 0.0f) col_out->r = 0.0f;

    if(col_out->g > 255.0f) col_out->g = 255.0f;
    else if(col_out->g < 0.0f) col_out->g = 0.0f;

    if(col_out->b > 255.0f) col_out->b = 255.0f;
    else if(col_out->b < 0.0f) col_out->b = 0.0f;
}

void VideoCrtClass::ChangeContrast(COLOR_STRUCT *col_in, COLOR_STRUCT *col_out, float value)
{
    float R = col_in->r;
    float G = col_in->g;
    float B = col_in->b;

    col_out->r = 0.5f + value * (R - 0.5f);
    col_out->g = 0.5f + value * (G - 0.5f);
    col_out->b = 0.5f + value * (B - 0.5f);

    if(col_out->r > 255.0f) col_out->r = 255.0f;
    else if(col_out->r < 0.0f) col_out->r = 0.0f;

    if(col_out->g > 255.0f) col_out->g = 255.0f;
    else if(col_out->g < 0.0f) col_out->g = 0.0f;

    if(col_out->b > 255.0f) col_out->b = 255.0f;
    else if(col_out->b < 0.0f) col_out->b = 0.0f;
}
