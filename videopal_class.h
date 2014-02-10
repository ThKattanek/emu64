//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: videopal_class.h                      //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 12.01.2014                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef VIDEOPAL_CLASS_H
#define VIDEOPAL_CLASS_H

#include "structs.h"
#include <stdint.h>

struct VIC_COLOR_STRUCT
{
    double Luminace;
    double Angel;
    double Direction;
};

typedef struct COLOR_STRUCT
{
    COLOR_STRUCT(){r = 0;g = 0;b = 0;a = 0;};
    COLOR_STRUCT(float _r,float _g,float _b,float _a){r = _r;g = _g;b = _b;a = _a;};
    float r;
    float g;
    float b;
    float a;

} COLOR_STRUCT, *LPCOLOR_STRUCT;

class VideoPalClass
{

public:
        VideoPalClass(void);
        ~VideoPalClass(void);
        //void SetPixelFormat(SDL_PixelFormat *format);
        void ConvertVideo(void* Outpuffer,long Pitch,unsigned char* VICOutPuffer,int OutXW,int OutYW,int InXW,int InYW,bool FlipTex);
        void SetDisplayMode(int DisplayMode);
        void UpdateParameter(void);
        void SetPhaseAltLineOffset(int offset);
        void SetHorizontalBlurY(int wblur);
        void SetHorizontalBlurUV(int wblur);
        void SetScanline(int wert);
        void SetSaturation(float wert);
        void SetHelligkeit(float wert);
        void SetKontrast(float wert);
        void SetC64Palette(int palnr);
        void EnableVideoDoubleSize(bool enabled);
        void EnablePALOutput(bool enabled);
        float *GetC64YUVPalette();
        unsigned long YHistogramm[256];

        bool StartC64isColorBit32;
        bool StartC64isDoublesize;
        bool StartC64isPalmode;

private:
        void ChangeSaturation(COLOR_STRUCT *col_in, COLOR_STRUCT *col_out, float wert);
        void ChangeContrast(COLOR_STRUCT *col_in, COLOR_STRUCT *col_out, float wert);
        //SDL_PixelFormat     *pixel_format;

        bool                Double2x;
        bool                PALOutput;
        uint32_t            DestDisplayMode;
        uint16_t            *Outpuffer16;
        uint16_t            *Outpuffer16Scanline;
        uint32_t            *Outpuffer32;
        uint32_t            *Outpuffer32Scanline;
        uint8_t             *VideoSource8;

        //////// Normaler Palettenmodus //////////
        uint16_t	Palette16Bit[256];
        uint32_t	Palette32Bit[256];
        int		AktFarbMode;
        //////////////////////////////////////////

        float		C64YUVPalette1[16*3];
        float		C64YUVPalette2[16*3];

        void RGB_To_YUV(float rgb[3], float yuv[3]);
        void YUV_To_RGB(float yuv[3], float rgb[3]);
        void CreateVicIIColors(void);

        float blur_y_mul;
        float blur_uv_mul;

        uint32_t RGB;
        uint32_t RGBScanline;
        uint32_t RGB_OLD[1024];
        double _y,_u,_v;

        double  _y1,_u1,_v1;
        double  _y2,_u2,_v2;
        float   _ut,_vt;
        float   _uo[1024],_vo[1024];
        short   r,g,b;
        float   blur_y_sum;
        float   blur_u_sum;
        float   blur_v_sum;
        uint8_t ContrastTransform[256];

        //// Einstellbare Werte
        float	Saturation;				// 0 - 2000
        float	Helligkeit;				// 0 - 2
        float	Kontrast;				// 0 - 1
        int32_t HoBlurWY;
        int32_t	HoBlurWUV;
        int32_t PhaseAlternatingLine;		// 0 - 2000
        float   Scanline;

        // NEU //
        uint32_t BlurTable0[16][16][16][16];	// 16 ^ 4 (Für Maximal 4 Pixel Blur)
        uint32_t BlurTable1[16][16][16][16];	// 16 ^ 4 (Für Maximal 4 Pixel Blur)
        uint32_t BlurTable0S[16][16][16][16];	// 16 ^ 4 (Für Maximal 4 Pixel Blur)
        uint32_t BlurTable1S[16][16][16][16];	// 16 ^ 4 (Für Maximal 4 Pixel Blur)
};
#endif // VIDEOPAL_CLASS_H
