//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: video_crt_class.h                     //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 12.09.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef VIDEOCRT_CLASS_H
#define VIDEOCRT_CLASS_H

#include "structs.h"
#include <stdint.h>

struct VIC_COLOR_STRUCT
{
    float luminace;
    float angel;
    float direction;
};

typedef struct COLOR_STRUCT
{
    COLOR_STRUCT(){r = 0;g = 0;b = 0;a = 0;}
    COLOR_STRUCT(float _r,float _g,float _b,float _a){r = _r;g = _g;b = _b;a = _a;}
    float r;
    float g;
    float b;
    float a;

} COLOR_STRUCT, *LPCOLOR_STRUCT;

class VideoCrtClass
{

public:
        VideoCrtClass();
        ~VideoCrtClass();
        void ConvertVideo(void* Outpuffer, long Pitch, unsigned char* VICOutPuffer, int VICOutPufferOffset, int OutXW, int OutYW, int InXW, int InYW, bool FlipTex);
        void UpdateParameter(void);
        void SetPhaseAlternatingLineOffset(int offset);
        void SetHorizontalBlurY(int wblur);
        void SetHorizontalBlurUV(int wblur);
        void SetScanline(int value);
        void SetSaturation(float value);
        void SetBrightness(float value);
        void SetContrast(float value);
        void SetC64Palette(uint8_t palnr);
        void EnableVideoDoubleSize(bool enabled);
        void EnableCrtOutput(bool enabled);
        float *GetC64YUVPalette();

        bool start_c64_is_doublesize;
        bool start_c64_is_palmode;

private:
        inline void ConvertYUVToRGB(COLOR_STRUCT *color_out);

        bool                is_double2x;
        bool                is_crt_output;
        uint32_t            *out_buffer;
        uint32_t            *out_buffer_scanline;
        uint8_t             *video_source;

        //////// Normaler Palettenmodus //////////
        uint32_t	palette[256];
        uint8_t		current_color_palette_mumber;
        //////////////////////////////////////////

        float		c64_yuv_palette0[16*3];
        float		c64_yuv_palette1[16*3];

        void CreateVicIIColors(void);

        float blur_y_mul;
        float blur_uv_mul;

        uint32_t rgb;
        float _y,_u,_v;

        float  _y1,_u1,_v1;
        float  _y2,_u2,_v2;
        float   _ut,_vt;
        float   _uo[1024],_vo[1024];
        int16_t r,g,b;
        float   blur_y_sum;
        float   blur_u_sum;
        float   blur_v_sum;

        float sector;
        float origin;
        float radian;
        float screen;

        //// Einstellbare Werte
        float	saturation;				// 0 - 2000
        float	brightness;				// 0 - 2
        float	contrast;				// 0 - 1
        int32_t hor_blur_wy;
        int32_t	hor_blur_wuv;
        int32_t phase_alternating_line;		// 0 - 2000
        float   scanline;

        // NEU //
        uint32_t BlurTable0[16][16][16][16];	// 16 ^ 4 (Für Maximal 4 Pixel Blur)
        uint32_t BlurTable1[16][16][16][16];	// 16 ^ 4 (Für Maximal 4 Pixel Blur)
        uint32_t BlurTable0S[16][16][16][16];	// 16 ^ 4 (Für Maximal 4 Pixel Blur)
        uint32_t BlurTable1S[16][16][16][16];	// 16 ^ 4 (Für Maximal 4 Pixel Blur)
};
#endif // VIDEOCRT_CLASS_H
