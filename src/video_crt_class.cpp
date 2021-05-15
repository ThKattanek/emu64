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
// Letzte Änderung am 13.09.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "video_crt_class.h"
#include "c64_colors.h"
#include <math.h>

#include <iostream>

static uint8_t LUMA_TABLE[2][16] =
{
    {0,32,8,24,16,16,8,24,16,8,16,8,16,24,16,24},       // First VIC II PAL Revision -> 5 different luma levels
    {0,32,10,20,12,16,8,24,12,8,16,10,15,24,15,20}      // All PAL Revisions > 1     -> 9 different luma levels
};

/*
angles[ 0x4 ]                 = 2;                // Purple
angles[ 0x2 ] = angles[ 0xa ] = 4;                // Red
angles[ 0x8 ]                 = 5;                // Orange
angles[ 0x9 ]                 = 6;                // Brown
angles[ 0x7 ]                 = 7;                // Yellow
angles[ 0x5 ] = angles[ 0xd ] = 2 + 8;            // Green
angles[ 0x3 ]                 = 4 + 8;            // Cyan
angles[ 0x6 ] = angles[ 0xe ] = 7 + 8;            // Blue
*/

static uint8_t COLOR_ANGLES[16] =
{
    0, 0, 4, 4+8, 2, 2+8, 7+8, 7, 5, 6, 4, 0, 0, 2+8, 7+8, 0
};

#define MATH_PI	3.141592653589793238462643383279

VideoCrtClass::VideoCrtClass()
{
    is_first_pal_vic_revision = false;

	enable_user_palette = false;
    current_color_palette_mumber = 0;
    is_double2x = false;
    contrast = 0.8f;

    hor_blur_wy = 0;
    hor_blur_wuv = 0;

    sector = 360.0f/16.0f;
    origin = sector/2.0f;
    radian = static_cast<float>(MATH_PI)/180.0f;
    screen = 1.0f/5.0f;
}

VideoCrtClass::~VideoCrtClass(void)
{
}

void VideoCrtClass::UpdateParameter(void)
{
    CreateVicIIColors();
}

void VideoCrtClass::SetFirstVicRevision(bool enabled)
{
    is_first_pal_vic_revision = enabled;
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
    saturation = value * 100.0f;
    saturation *=   1.0f - screen;
}

void VideoCrtClass::SetBrightness(float value)
{
    brightness = (value * 100.f) - 50.f;
    brightness *= 2.0f;
}

void VideoCrtClass::SetContrast(float value)
{
    contrast = value * 2.0f;
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

void VideoCrtClass::EnableUserPalette(bool enabled)
{
	enable_user_palette = enabled;
}

void VideoCrtClass::SetUserPaletteColor(int color_number, uint8_t r, uint8_t g, uint8_t b)
{
	user_palette[color_number] = 0xFF000000 | static_cast<uint32_t>(b<<16 | g<<8 | r);
}

float *VideoCrtClass::GetC64YUVPalette()
{
    return c64_yuv_palette0;
}

inline void VideoCrtClass::ConvertYUVToRGB(COLOR_STRUCT *color_out)
{
    float source = 2.8f;    // PAL
    float target = 2.2f;    // sRGB

    float _r, _g, _b;

    // from yuv to rgb
    r = static_cast<int16_t>(_y + 0.0f *_u + 1.140f *_v);
    g = static_cast<int16_t>(_y - 0.396f * _u - 0.581f * _v);
    b = static_cast<int16_t>(_y + 2.029f * _u);

    // gamma correction

    // MIN = 0
    if(r<0) r=0;
    if(g<0) g=0;
    if(b<0) b=0;

    // MAX = 255
    if(r>255) r=255;
    if(g>255) g=255;
    if(b>255) b=255;

    _r = powf(255.0f, 1.0f - source) * powf(r, source);
    _g = powf(255.0f, 1.0f - source) * powf(g, source);
    _b = powf(255.0f, 1.0f - source) * powf(b, source);

    _r = powf(255.0f, 1.0f - 1.0f/target) * powf(_r, 1.0f/target);
    _g = powf(255.0f, 1.0f - 1.0f/target) * powf(_g, 1.0f/target);
    _b = powf(255.0f, 1.0f - 1.0f/target) * powf(_b, 1.0f/target);

    r = static_cast<int16_t>(roundf(_r));
    g = static_cast<int16_t>(roundf(_g));
    b = static_cast<int16_t>(roundf(_b));

    color_out->r = r;
    color_out->g = g;
    color_out->b = b;
    color_out->a = 0.0f;
}

inline void VideoCrtClass::CreateVicIIColors(void)
{
    COLOR_STRUCT color_out;

    // Für Phase Alternating Line
    // --> pahphase_alternating_line 0 - 2000 ==== Offs Umrechnen nach -2.0f - 2.0f
    float Offs =((static_cast<float>(phase_alternating_line) / 1000.0f ) - 1.0f) * 2.0f;

    for(int i=0;i<16;i++)
    {
        c64_yuv_palette0[i*3+1] = 0;
        c64_yuv_palette0[i*3+2] = 0;

        float color_angle = COLOR_ANGLES[i];

        if(color_angle != 0.0f)
        {
            float angle = ( origin + color_angle * sector ) * radian;
            c64_yuv_palette0[i*3+1] = cosf( angle ) * saturation;
            c64_yuv_palette0[i*3+2] = sinf( angle ) * saturation;

            c64_yuv_palette1[i*3+1] = cosf( angle + Offs) * saturation;
            c64_yuv_palette1[i*3+2] = sinf( angle + Offs) * saturation;
        }

        if(is_first_pal_vic_revision)
            c64_yuv_palette0[i*3+0] = c64_yuv_palette1[i*3+0] = 8 * LUMA_TABLE[0][i] + brightness;
        else
            c64_yuv_palette0[i*3+0] = c64_yuv_palette1[i*3+0] = 8 * LUMA_TABLE[1][i] + brightness;

        c64_yuv_palette0[i*3+0] *= contrast + screen;
        c64_yuv_palette0[i*3+1] *= contrast + screen;
        c64_yuv_palette0[i*3+2] *= contrast + screen;

        c64_yuv_palette1[i*3+0] *= contrast + screen;
        c64_yuv_palette1[i*3+1] *= contrast + screen;
        c64_yuv_palette1[i*3+2] *= contrast + screen;
    }

    int x[4];
    for(x[0]=0;x[0]<16;x[0]++)
    {
        for(x[1]=0;x[1]<16;x[1]++)
        {
            for(x[2]=0;x[2]<16;x[2]++)
            {
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

                    ConvertYUVToRGB(&color_out);

                    rgb =  static_cast<uint32_t>(color_out.r);       // Rot
                    rgb |= static_cast<uint32_t>(color_out.g)<<8;    // Grün
                    rgb |= static_cast<uint32_t>(color_out.b)<<16;   // Blau

                    BlurTable0[x[0]][x[1]][x[2]][x[3]] = rgb | 0xFF000000;

                    /// Tabelle 1 für Gerade Zeile
                    _y *= scanline;

                    ConvertYUVToRGB(&color_out);

                    rgb =  static_cast<uint32_t>(color_out.r);       // Rot
                    rgb |= static_cast<uint32_t>(color_out.g)<<8;    // Grün
                    rgb |= static_cast<uint32_t>(color_out.b)<<16;   // Blau

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

                    ConvertYUVToRGB(&color_out);

                    rgb =  static_cast<uint32_t>(color_out.r);       // Rot
                    rgb |= static_cast<uint32_t>(color_out.g)<<8;    // Grün
                    rgb |= static_cast<uint32_t>(color_out.b)<<16;   // Blau

                    BlurTable1[x[0]][x[1]][x[2]][x[3]] = rgb | 0xFF000000;

                    _y *= scanline;

                    ConvertYUVToRGB(&color_out);

                    rgb = static_cast<uint32_t>(color_out.r);       // Rot
                    rgb |= static_cast<uint32_t>(color_out.g)<<8;    // Grün
                    rgb |= static_cast<uint32_t>(color_out.b)<<16;   // Blau

                    BlurTable1S[x[0]][x[1]][x[2]][x[3]] = rgb | 0xFF000000;
                }
            }
        }
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
				if(!enable_user_palette)
				{
					*(out_buffer++) = palette[video_source[x] & 0x0F];
					*(out_buffer++) = palette[video_source[x] & 0x0F];
					*(out_buffer_scanline++) = palette[video_source[x] & 0x0F];
					*(out_buffer_scanline++) = palette[video_source[x] & 0x0F];
				}
				else
				{
					*(out_buffer++) = user_palette[video_source[x] & 0x0F];
					*(out_buffer++) = user_palette[video_source[x] & 0x0F];
					*(out_buffer_scanline++) = user_palette[video_source[x] & 0x0F];
					*(out_buffer_scanline++) = user_palette[video_source[x] & 0x0F];
				}
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
				if(!enable_user_palette)
				{
					*(out_buffer++) = palette[video_source[x] & 0x0F];
				}
				else
				{
					*(out_buffer++) = user_palette[video_source[x] & 0x0F];
				}
            }
            video_source = video_source+InXW;
        }
    }
}
