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
// Letzte Änderung am 16.04.2023                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "video_crt_class.h"
#include "c64_colors.h"
#include <math.h>
#include <algorithm>

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
    enable_user_palette_crt_mode = false;
    current_color_palette_mumber = 0;
    is_double2x = false;

    hor_blur_wy = 0;
    hor_blur_wuv = 0;

    sector = 360.0f/16.0f;
    origin = sector/2.0f;
    radian = static_cast<float>(MATH_PI)/180.0f;

    enable_pal_delay_line = false;
    pal_delay_line_u_only = false;

    CreateVicIIColors();
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
}

void VideoCrtClass::SetHorizontalBlurUV(int wblur)
{
    if(wblur > 5) wblur = 5;
    hor_blur_wuv = wblur;
}

void VideoCrtClass::SetScanline(int value)
{
    scanline = value/100.0f;
}

void VideoCrtClass::SetSaturation(float value)
{
    saturation = value * 100.0f;
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
    CreateVicIIColors();
}

void VideoCrtClass::EnableUserPalette(bool enabled)
{
	enable_user_palette = enabled;
	CreateVicIIColors();
}

void VideoCrtClass::EnableUserPaletteCrtMode(bool enabled)
{
	enable_user_palette_crt_mode = enabled;
	CreateVicIIColors();
}

void VideoCrtClass::EnablePalDelayLine(bool enabled)
{
    enable_pal_delay_line = enabled;
}

void VideoCrtClass::PalDelayLineUOnly(bool enabled)
{
    pal_delay_line_u_only = enabled;
}

void VideoCrtClass::SetUserPaletteColor(int color_number, uint8_t r, uint8_t g, uint8_t b)
{
	user_palette[color_number] = 0xFF000000 | static_cast<uint32_t>(b<<16 | g<<8 | r);
	CreateVicIIColors();
}

inline void VideoCrtClass::ConvertYUVToRGB(COLOR_STRUCT *color_out)
{
    float source = 2.8f;    // PAL
    float target = 2.2f;    // sRGB

    float _r, _g, _b;

    // from yuv to rgb
	// R = Y + 1.140V
	// G = Y - 0.395U - 0.581V
	// B = Y + 2.032U

	r = static_cast<int16_t>(_y + 1.140f *_v);
	g = static_cast<int16_t>(_y - 0.395f * _u - 0.581f * _v);
	b = static_cast<int16_t>(_y + 2.032f * _u);

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

void VideoCrtClass::ConvertRGBToYUV()
{
	/*
	Y = 0.299R + 0.587G + 0.114B
	U = 0.492 (B-Y)
	V = 0.877 (R-Y)

	It can also be represented as:

	Y =  0.299R + 0.587G + 0.114B
	U = -0.147R - 0.289G + 0.436B
	V =  0.615R - 0.515G - 0.100B
	*/
}

inline void VideoCrtClass::CreateVicIIColors(void)
{
	float offset =((static_cast<float>(phase_alternating_line) / 1000.0f ) - 1.0f) * 2.0f;

	for(int i=0; i<16; i++)
    {
        if(!enable_user_palette_crt_mode)
        {
            c64_yuv_colors_0[i].u = 0;

            float color_angle = COLOR_ANGLES[i];

            if(color_angle != 0.0f)
            {
                float angle = ( origin + color_angle * sector ) * radian;
                c64_yuv_colors_0[i].u = cosf( angle );
                c64_yuv_colors_0[i].v = sinf( angle );
            }
            else
            {
                c64_yuv_colors_0[i].u = c64_yuv_colors_0[i].v = 0.0f;
            }

            if(color_angle != 0.0f)
            {
                float angle = ( origin + color_angle * sector ) * radian;
                c64_yuv_colors_1[i].u = cosf( angle + offset);
                c64_yuv_colors_1[i].v = sinf( angle + offset);
            }
            else
            {
                c64_yuv_colors_1[i].u = c64_yuv_colors_1[i].v = 0.0f;
            }

            if(is_first_pal_vic_revision)
                c64_yuv_colors_0[i].y = c64_yuv_colors_1[i].y = 8 * LUMA_TABLE[0][i];
            else
                c64_yuv_colors_0[i].y = c64_yuv_colors_1[i].y = 8 * LUMA_TABLE[1][i];
        }
        else
        {
            // Y = 0.299R + 0.587G + 0.114B
            // U = 0.492 (B-Y)
            // V = 0.877 (R-Y)
            //  	It can also be represented as:
            //  Y =  0.299R + 0.587G + 0.114B
            //  U = -0.147R - 0.289G + 0.436B
            //  V =  0.615R - 0.515G - 0.100B

            uint8_t r = (user_palette[i] & 0x000000ff);
            uint8_t g = ((user_palette[i] >> 8) & 0x000000ff);
            uint8_t b = ((user_palette[i] >> 16) & 0x000000ff);

            float y = 0.299f * r + 0.587f * g + 0.114 * b;
            float u = 0.492 * (b - y);
            float v = 0.877 * (r - y);

            c64_yuv_colors_0[i].y = y;
            c64_yuv_colors_0[i].u = u ;
            c64_yuv_colors_0[i].v = v ;

            c64_yuv_colors_1[i].y = y;
            c64_yuv_colors_1[i].u = u ;
            c64_yuv_colors_1[i].v = v ;
        }
	}
}

void VideoCrtClass::ConvertVideo(void* Outpuffer,long Pitch,unsigned char* VICOutPuffer,int VICOutPufferOffset,int OutXW,int OutYW,int InXW,int,bool)
{
	// Outbuffer -> uint32 (ABGR)

	float _y,_u,_v;
	float r,g,b;
	uint32_t rgb;

    float _y0,_y1,_y2,_y3;
    float _u0,_u1,_u2,_u3;
    float _v0,_v1,_v2,_v3;

    float _uf1,_uf2,_uf3;
    float _vf1,_vf2,_vf3;

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

                _y0 = _y1 = _y2 = _y3 = c64_yuv_colors_0[video_source[0] & 0x0f].y;
                _u0 = _u1 = _u2 = _u3 = c64_yuv_colors_0[video_source[0] & 0x0f].u;
                _v0 = _v1 = _v2 = _v3 = c64_yuv_colors_0[video_source[0] & 0x0f].v;

                for(int x=0;x<(OutXW);x++)
				{
					// from yuv to rgb
					// R = Y + 1.140V
					// G = Y - 0.395U - 0.581V
					// B = Y + 2.032U

                    int _x = x/2;

					// Pixel
                    if(y & 1)
					{
                        _y0 = c64_yuv_colors_0[video_source[_x] & 0x0f].y;
                        _u0 = c64_yuv_colors_0[video_source[_x] & 0x0f].u;
                        _v0 = c64_yuv_colors_0[video_source[_x] & 0x0f].v;

                        _uf1 = c64_yuv_colors_0[video_source[_x+1] & 0x0f].u;
                        _uf2 = c64_yuv_colors_0[video_source[_x+2] & 0x0f].u;
                        _uf3 = c64_yuv_colors_0[video_source[_x+3] & 0x0f].u;

                        _vf1 = c64_yuv_colors_0[video_source[_x+1] & 0x0f].v;
                        _vf2 = c64_yuv_colors_0[video_source[_x+2] & 0x0f].v;
                        _vf3 = c64_yuv_colors_0[video_source[_x+3] & 0x0f].v;
					}
					else
                    {
                        _y0 = c64_yuv_colors_1[video_source[_x] & 0x0f].y;
                        _u0 = c64_yuv_colors_1[video_source[_x] & 0x0f].u;
                        _v0 = c64_yuv_colors_1[video_source[_x] & 0x0f].v;

                        _uf1 = c64_yuv_colors_1[video_source[_x+1] & 0x0f].u;
                        _uf2 = c64_yuv_colors_1[video_source[_x+2] & 0x0f].u;
                        _uf3 = c64_yuv_colors_1[video_source[_x+3] & 0x0f].u;

                        _vf1 = c64_yuv_colors_1[video_source[_x+1] & 0x0f].v;
                        _vf2 = c64_yuv_colors_1[video_source[_x+2] & 0x0f].v;
                        _vf3 = c64_yuv_colors_1[video_source[_x+3] & 0x0f].v;
                    }

                    // y blur
                    switch(hor_blur_wy)
                    {
                    case 1:
                        _y = _y0;
                        break;
                    case 2:
                        _y = (_y0 + _y1) / 2;
                        _y1 = _y;
                        break;
                    case 3:
                        _y = (_y0 + _y1 + _y2) / 3;
                        _y2 = _y1;
                        _y1 = _y;
                        break;
                    case 4:
                        _y = (_y0 + _y1 + _y2 + _y3) / 4;
                        _y3 = _y2;
                        _y2 = _y1;
                        _y1 = _y;
                        break;
                    default:
                        _y = _y0;
                        break;
                    }

                    // uv blur
                    switch(hor_blur_wuv)
                    {
                    case 1:
                        _u = _u0;
                        _v = _v0;
                        break;
                    case 2:
                        _u = (_u0 + _u1 + _uf1) / 3;
                        _u1 = _u;
                        _v = (_v0 + _v1 + _vf1) / 3;
                        _v1 = _v;
                        break;
                    case 3:
                        _u = (_u0 + _u1 + _u2 + _uf1 + _uf2) / 5;
                        _u2 = _u1;
                        _u1 = _u;
                        _v = (_v0 + _v1 + _v2 + _vf1 + _vf2) / 5;
                        _v2 = _v1;
                        _v1 = _v;
                        break;
                    case 4:
                        _u = (_u0 + _u1 + _u2 + _u3 + _uf1 + _uf2 + _uf3) / 7;
                        _u3 = _u2;
                        _u2 = _u1;
                        _u1 = _u;
                        _v = (_v0 + _v1 + _v2 + _v3 + _vf1 + _vf2 + _vf3) / 7;
                        _v3 = _v2;
                        _v2 = _v1;
                        _v1 = _v;
                        break;
                    default:
                        _u = _u0;
                        _v = _v0;
                        break;
                    }

                    /// PAL DELAY LINE
                    if(enable_pal_delay_line)
                    {
                        if(!pal_delay_line_u_only)
                        {
                            _ut = (_u + _uo[x]) / 2.0f;
                            _uo[x] = _u;
                            _u = _ut;

                            _vt = (_v + _vo[x]) / 2.0f;
                            _vo[x] = _v;
                            _v = _vt;
                        }
                        else
                        {
                            _ut = (_u + _uo[x]) / 2.0f;
                            _uo[x] = _u;
                            _u = _ut;
                        }
                    }

                    if(!enable_user_palette_crt_mode)
                    {
                        _y += brightness;
                        _u *= saturation;
                        _v *= saturation;
                        _y *= contrast;
                        _u *= contrast;
                        _v *= contrast;
                    }

					r = _y + 1.140f * _v;
					g = _y - 0.395f * _u - 0.581f * _v;
					b = _y + 2.032f * _u;

                    r = std::max(r, 0.0f);
                    g = std::max(g, 0.0f);
                    b = std::max(b, 0.0f);

                    r = std::min(r, 255.0f);
                    g = std::min(g, 255.0f);
                    b = std::min(b, 255.0f);

					rgb =  static_cast<uint32_t>(r);       // Rot
					rgb |= static_cast<uint32_t>(g)<<8;    // Grün
					rgb |= static_cast<uint32_t>(b)<<16;   // Blau

					rgb |= 0xFF000000;

					*(out_buffer++) = rgb;

					// Scanline
					////////////////////////////////////////////////
					_y *= scanline;

					r = _y + 1.140f * _v;
					g = _y - 0.395f * _u - 0.581f * _v;
					b = _y + 2.032f * _u;

                    r = std::max(r, 0.0f);
                    g = std::max(g, 0.0f);
                    b = std::max(b, 0.0f);

                    r = std::min(r, 255.0f);
                    g = std::min(g, 255.0f);
                    b = std::min(b, 255.0f);

					rgb =  static_cast<uint32_t>(r);       // Rot
					rgb |= static_cast<uint32_t>(g)<<8;    // Grün
					rgb |= static_cast<uint32_t>(b)<<16;   // Blau

					rgb |= 0xFF000000;

					*(out_buffer_scanline++) = rgb;
				}
				video_source = video_source+InXW;
			}
		}
		else
		{
			for(int y=0;y<OutYW;y++)
			{
				out_buffer = (static_cast<uint32_t*>(Outpuffer) + ((y)*Pitch/4));

                _y0 = _y1 = _y2 = _y3 = c64_yuv_colors_0[video_source[0] & 0x0f].y;
                _u0 = _u1 = _u2 = _u3 = c64_yuv_colors_0[video_source[0] & 0x0f].u;
                _v0 = _v1 = _v2 = _v3 = c64_yuv_colors_0[video_source[0] & 0x0f].v;

				for(int x=0;x<(OutXW);x++)
				{
					// from yuv to rgb
					// R = Y + 1.140V
					// G = Y - 0.395U - 0.581V
					// B = Y + 2.032U

					// Pixel
					if(y & 1)
					{
                        _y0 = c64_yuv_colors_0[video_source[x] & 0x0f].y;
                        _u0 = c64_yuv_colors_0[video_source[x] & 0x0f].u;
                        _v0 = c64_yuv_colors_0[video_source[x] & 0x0f].v;

                        _uf1 = c64_yuv_colors_0[video_source[x+1] & 0x0f].u;
                        _uf2 = c64_yuv_colors_0[video_source[x+2] & 0x0f].u;
                        _uf3 = c64_yuv_colors_0[video_source[x+3] & 0x0f].u;

                        _vf1 = c64_yuv_colors_0[video_source[x+1] & 0x0f].v;
                        _vf2 = c64_yuv_colors_0[video_source[x+2] & 0x0f].v;
                        _vf3 = c64_yuv_colors_0[video_source[x+3] & 0x0f].v;
					}
					else
					{
                        _y0 = c64_yuv_colors_1[video_source[x] & 0x0f].y;
                        _u0 = c64_yuv_colors_1[video_source[x] & 0x0f].u;
                        _v0 = c64_yuv_colors_1[video_source[x] & 0x0f].v;

                        _uf1 = c64_yuv_colors_1[video_source[x+1] & 0x0f].u;
                        _uf2 = c64_yuv_colors_1[video_source[x+2] & 0x0f].u;
                        _uf3 = c64_yuv_colors_1[video_source[x+3] & 0x0f].u;

                        _vf1 = c64_yuv_colors_1[video_source[x+1] & 0x0f].v;
                        _vf2 = c64_yuv_colors_1[video_source[x+2] & 0x0f].v;
                        _vf3 = c64_yuv_colors_1[video_source[x+3] & 0x0f].v;
					}

                    // y blur
                    switch(hor_blur_wy)
                    {
                    case 1:
                        _y = _y0;
                        break;
                    case 2:
                        _y = (_y0 + _y1) / 2;
                        _y1 = _y;
                        break;
                    case 3:
                        _y = (_y0 + _y1 + _y2) / 3;
                        _y2 = _y1;
                        _y1 = _y;
                        break;
                    case 4:
                        _y = (_y0 + _y1 + _y2 + _y3) / 4;
                        _y3 = _y2;
                        _y2 = _y1;
                        _y1 = _y;
                        break;
                    default:
                        _y = _y0;
                        break;
                    }

                    // uv blur
                    switch(hor_blur_wuv)
                    {
                    case 1:
                        _u = _u0;
                        _v = _v0;
                        break;
                    case 2:
                        _u = (_u0 + _u1 + _uf1) / 3;
                        _u1 = _u;
                        _v = (_v0 + _v1 + _vf1) / 3;
                        _v1 = _v;
                        break;
                    case 3:
                        _u = (_u0 + _u1 + _u2 + _uf1 + _uf2) / 5;
                        _u2 = _u1;
                        _u1 = _u;
                        _v = (_v0 + _v1 + _v2 + _vf1 + _vf2) / 5;
                        _v2 = _v1;
                        _v1 = _v;
                        break;
                    case 4:
                        _u = (_u0 + _u1 + _u2 + _u3 + _uf1 + _uf2 + _uf3) / 7;
                        _u3 = _u2;
                        _u2 = _u1;
                        _u1 = _u;
                        _v = (_v0 + _v1 + _v2 + _v3 + _vf1 + _vf2 + _vf3) / 7;
                        _v3 = _v2;
                        _v2 = _v1;
                        _v1 = _v;
                        break;
                    default:
                        _u = _u0;
                        _v = _v0;
                        break;
                    }

					/// PAL DELAY LINE
					if(enable_pal_delay_line)
					{
                        if(!pal_delay_line_u_only)
                        {
                            _ut = (_u + _uo[x]) / 2.0f;
                            _uo[x] = _u;
                            _u = _ut;

                            _vt = (_v + _vo[x]) / 2.0f;
                            _vo[x] = _v;
                            _v = _vt;
                        }
                        else
                        {
                            _ut = (_u + _uo[x]) / 2.0f;
                            _uo[x] = _u;
                            _u = _ut;
                        }
					}

                    if(!enable_user_palette_crt_mode)
                    {
                        _y += brightness;
                        _u *= saturation;
                        _v *= saturation;
                        _y *= contrast;
                        _u *= contrast;
                        _v *= contrast;
                    }

					r = _y + 1.140f * _v;
					g = _y - 0.395f * _u - 0.581f * _v;
					b = _y + 2.032f * _u;

                    r = std::max(r, 0.0f);
                    g = std::max(g, 0.0f);
                    b = std::max(b, 0.0f);

                    r = std::min(r, 255.0f);
                    g = std::min(g, 255.0f);
                    b = std::min(b, 255.0f);

					rgb =  static_cast<uint32_t>(r);       // Rot
					rgb |= static_cast<uint32_t>(g)<<8;    // Grün
					rgb |= static_cast<uint32_t>(b)<<16;   // Blau

					rgb |= 0xFF000000;

					*(out_buffer++) = rgb;
				}
				video_source = video_source+InXW;
			}
		}
    }
	else
    //////////////////////////////////////////////////////////////////////////
    ///////////////////// AUSGABE ÜBER NORMALE FARBPALETTE ///////////////////
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
