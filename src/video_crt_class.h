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
// Letzte Änderung am 16.04.2023                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef VIDEOCRT_CLASS_H
#define VIDEOCRT_CLASS_H

#include <stdint.h>
#include <vector>

#define SKIP_LINE_SIZE 1032     // Max. Breite einer Quellzeile für den Frameskip (inkl. 3 Pixel Vorlauf der Filter)

typedef struct COLOR_STRUCT
{
    COLOR_STRUCT(){r = 0;g = 0;b = 0;a = 0;}
    COLOR_STRUCT(float _r,float _g,float _b,float _a){r = _r;g = _g;b = _b;a = _a;}
    float r;
    float g;
    float b;
    float a;

} COLOR_STRUCT, *LPCOLOR_STRUCT;

typedef struct YUV_COLOR
{
	float y,u,v;
} YUV_COLOR, *LPYUV_COLOR;

class VideoCrtClass
{
public:
        VideoCrtClass();
        ~VideoCrtClass();
        void ConvertVideo(void* Outpuffer, long Pitch, unsigned char* VICOutPuffer, int VICOutPufferOffset, int OutXW, int OutYW, int InXW, int InYW, bool FlipTex);
        void SkipVideo(unsigned char* VICOutPuffer, int VICOutPufferOffset, int OutXW, int OutYW, int InXW);
        int GetDelayLineSourceRow(int VICOutPufferOffset, int OutYW, int InXW) const;
        void ConvertSkippedVideo(void* Outpuffer, long Pitch, unsigned char* VICOutPuffer, int VICOutPufferOffset, int OutXW, int OutYW, int InXW, int InYW, bool FlipTex);
        void UpdateParameter(void);
        void SetFirstVicRevision(bool enabled);
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
		void EnableUserPalette(bool enabled);
		void EnableUserPaletteCrtMode(bool enabled);
		void EnablePalDelayLine(bool enabled);
        void PalDelayLineUOnly(bool enabled);
		void SetUserPaletteColor(int color_number, uint8_t r, uint8_t g, uint8_t b);
        float *GetC64YUVPalette();

        bool start_c64_is_doublesize;
        bool start_c64_is_palmode;

private:
        void CreateVicIIColors(void);
        void ConvertVideoIntern(void* Outpuffer, long Pitch, unsigned char* VICOutPuffer, int VICOutPufferOffset, int OutXW, int OutYW, int InXW);
        void PrimeDelayLine(int index, int OutXW);

        // Frameskip: Die PAL Delay Line überträgt die letzte Zeile eines Bildes in die erste des nächsten.
        // Für ausgelassene Bilder wird die letzte Quellzeile gemerkt, damit das nächste umgerechnete Bild identisch bleibt.
        // [0] = zuletzt ausgelassenes Bild, [1] = das Bild davor
        uint8_t     skip_line[2][SKIP_LINE_SIZE];
        bool        skip_line_valid[2];
        int         skip_line_parity[2];
        uint8_t     prime_source[2 * SKIP_LINE_SIZE];
        std::vector<uint32_t> prime_buffer;

        inline void ConvertYUVToRGB(COLOR_STRUCT *color_out);
		inline void ConvertRGBToYUV();

        bool                is_double2x;
        bool                is_crt_output;
        uint32_t            *out_buffer;
        uint32_t            *out_buffer_scanline;
        uint8_t             *video_source;

        //////// Normaler Palettenmodus //////////
        uint32_t	palette[256];
        uint8_t		current_color_palette_mumber;
		//////////////////////////////////////////

		////////// User Palettenmodus ////////////
		uint32_t	user_palette[256];
		bool enable_user_palette;					// im Palettenmodus nicht CRT Modus
		bool enable_user_palette_crt_mode;			// im CRT Modus
		//////////////////////////////////////////

        float   _y,_u,_v;
        float   _y1,_u1,_v1;
        float   _ut,_vt;
		float   _uo[1024],_vo[1024];
        int16_t r,g,b;

        YUV_COLOR c64_yuv_colors_0[16];
        YUV_COLOR c64_yuv_colors_1[16];

        float sector;
        float origin;
        float radian;

        //// Einstellbare Werte
        bool    is_first_pal_vic_revision;  // true = first VIC revision, false = all revisions > 1
		bool	enable_pal_delay_line;		// true = pal delay line is eanable (u+v)
        bool    pal_delay_line_u_only;      // true = pal delay liny is u only
        float	saturation;                 // 0 - 1
        float	brightness;                 // 0 - 1
        float	contrast;                   // 0 - 1
        int32_t hor_blur_wy;
        int32_t	hor_blur_wuv;
        int32_t phase_alternating_line;		// 0 - 2000
        float   scanline;
};
#endif // VIDEOCRT_CLASS_H
