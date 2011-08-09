//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: videopal_class.h                      //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 25.04.2011		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#ifndef VIDEOPAL_CLASS_H
#define VIDEOPAL_CLASS_H

#include "structs.h"
#include "SDL/SDL.h"

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
        void SetPixelFormat(SDL_PixelFormat *format);
        void ConvertVideo(void* DXOutpuffer,long Pitch,unsigned char* VICOutPuffer,int OutXW,int OutYW,int InXW,int InYW,bool FlipTex);
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
        unsigned long YHistogramm[256];

private:
        SDL_PixelFormat *pixel_format;

        bool		Double2x;
        bool		PALOutput;
        unsigned long	DestDisplayMode;
        unsigned short	*DXOutpuffer16;
        unsigned short	*DXOutpuffer16Scanline;
        unsigned long	*DXOutpuffer32;
        unsigned long	*DXOutpuffer32Scanline;
        unsigned char	*VideoSource8;

        //////// Normaler Palettenmodus //////////
        unsigned short	Palette16Bit[256];
        unsigned long	Palette32Bit[256];
        int		AktFarbMode;
        //////////////////////////////////////////

        float		C64YUVPalette1[16*3];
        float		C64YUVPalette2[16*3];

        void RGB_To_YUV(float rgb[3], float yuv[3]);
        void YUV_To_RGB(float yuv[3], float rgb[3]);
        void CreateVicIIColors(void);

        float blur_y_mul;
        float blur_uv_mul;

        unsigned long RGB;
        unsigned long RGBScanline;
        unsigned long RGB_OLD[1024];
        double _y,_u,_v;

        double  _y1,_u1,_v1;
        double  _y2,_u2,_v2;
        float   _ut,_vt;
        float   _uo[1024],_vo[1024];
        short   r,g,b;
        float   blur_y_sum;
        float   blur_u_sum;
        float   blur_v_sum;
        unsigned char ContrastTransform[256];

        //// Einstellbare Werte
        float	Saturation;				// 0 - 2000
        float	Helligkeit;				// 0 - 2
        float	Kontrast;				// 0 - 1
        int     HoBlurWY;
        int	HoBlurWUV;
        int     PhaseAlternatingLine;		// 0 - 2000
        float   Scanline;

        // NEU //
        unsigned long BlurTable0[16][16][16][16];	// 16 ^ 4 (Für Maximal 4 Pixel Blur)
        unsigned long BlurTable1[16][16][16][16];	// 16 ^ 4 (Für Maximal 4 Pixel Blur)
        unsigned long BlurTable0S[16][16][16][16];	// 16 ^ 4 (Für Maximal 4 Pixel Blur)
        unsigned long BlurTable1S[16][16][16][16];	// 16 ^ 4 (Für Maximal 4 Pixel Blur)
};
#endif // VIDEOPAL_CLASS_H
