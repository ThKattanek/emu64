#include <jni.h>
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <android/log.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>

#define  LOG_TAG    "NATIVE"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define printf(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__);

#include "asset_texture_class.h"
#include "text_box_class.h"
#include "dsp.h"
#include "c64_class.h"
#include "../../../videopal_class.h"


////////////////////// C64 Tastatur /////////////////////////

static const int Y_MIN[7] = {12,60,107,153,153,177,199};
static const int Y_MAX[7] = {59,106,152,198,174,198,247};
static const int KEYS_Y[7] ={18,17,17,15,2,2,1};
static const int X_MIN[7][18] = {{18,67,114,161,208,255,303,350,397,444,492,539,585,633,680,727,797,834},
                                {18,90,138,184,232,279,326,374,420,469,515,561,609,656,704,797,834},
                                {9,55,103,150,197,244,291,339,387,433,480,527,575,622,669,797,834},
                                {9,55,127,174,221,268,316,363,410,457,505,552,600,797,834},
                                {669,718},
                                {669,718},
                                {137}};
static const int X_MAX[7][18] = {{66,113,160,207,254,302,349,396,443,491,538,584,632,679,726,774,833,868},
                                {89,137,183,231,278,325,373,419,468,514,560,608,655,703,774,833,868},
                                {54,102,149,196,243,290,338,386,432,479,526,574,621,668,765,833,868},
                                {54,126,173,220,267,315,362,409,456,504,551,599,668,833,868},
                                {717,765},
                                {717,765},
                                {561}};

/// TansTabelle von LeyLayout zu C64Matrix
static unsigned short VK_TO_C64[7][18] = {{0x71,0x70,0x73,0x10,0x13,0x20,0x23,0x30,0x33,0x40,0x43,0x50,0x53,0x60,0x63,0x00,0x04,0x84},
                                         {0x72,0x76,0x11,0x16,0x21,0x26,0x31,0x36,0x41,0x46,0x51,0x56,0x61,0x66,0x100,0x05,0x85},
                                         {0x77,0x17,0x12,0x15,0x22,0x25,0x32,0x35,0x42,0x45,0x52,0x55,0x62,0x65,0x01,0x06,0x86},
                                         {0x75,0x17,0x14,0x27,0x24,0x37,0x34,0x47,0x44,0x57,0x54,0x67,0x64,0x03,0x83},
                                         {0x87,0x82},
                                         {0x07,0x02},
                                         {0x74}};

class vertex{
	private: float x,y,z;
	public: void set(float x, float y, float z){this->x = x;this->y = y;this->z = z;}
};

class map{
	private: float x,y;
	public: void set(float x, float y){this->x = x;this->y = y;}
};

/// Globale Variablen
JavaVM *vm;
AssetTextureClass *texture = NULL;
TextBoxClass *textBox01 = NULL;

char* text_buffer;
const char* sd_ext_path;
char emu64_data_path[1024];
unsigned char start_wert = 0;

#ifdef __cplusplus
extern "C"
{
#endif

unsigned short sample = 0;

vertex screen_vert[4];
vertex keys_vert[4];
map mapcoord[4];
unsigned short indices[6];
float screen_pos_x1,screen_pos_y1,screen_pos_x2,screen_pos_y2;

bool KeysVisible = false;
float keys_pos_x1,keys_pos_y1,keys_pos_x2,keys_pos_y2;
float keys_pos_x1_px,keys_pos_y1_px,keys_pos_x2_px,keys_pos_y2_px;
float keys_xw,keys_yw;
float keys_scale_x,keys_scale_y;
bool idx_isKey[16];
int idx_X_KEY[16];
int idx_Y_KEY[16];

unsigned int ScreenTextureID;
unsigned int TastaturTextureID;

bool	isKeys = false;

int menue_case = 0;

/// C64 Klasse
C64Class *c64;

/// Video Pal Klasse
VideoPalClass *videopal;

unsigned char* KeyMatrixToPA;
unsigned char* KeyMatrixToPB;

void SoundCallback(short *buffer, int num_samples)
{
	c64->FillAudioBuffer(buffer,num_samples);
}

void KeyDown(int idx, int mx, int my)
{
    static int xmin,xmax,ymin,ymax;
    static int AKT_Y_KEY,AKT_X_KEY;

    for(int i=0;i<7;i++)
    {
        ymin = int(Y_MIN[i] * keys_scale_y);
        ymax = int(Y_MAX[i] * keys_scale_y);

        if(my>=ymin && my<=ymax)
        {
            AKT_Y_KEY = i;
            LOGD("Y_KEY: %d",AKT_Y_KEY);
            for(int i=0;i<KEYS_Y[AKT_Y_KEY];i++)
            {
                xmin = int(X_MIN[AKT_Y_KEY][i] * keys_scale_x);
                xmax = int(X_MAX[AKT_Y_KEY][i] * keys_scale_x);

                if(mx>=xmin && mx<=xmax)
                {
                    AKT_X_KEY = i;
                    LOGD("X_KEY: %d",AKT_X_KEY);
                    goto Lend;
                }
                else AKT_X_KEY = 0xFF;
            }
        }
        else AKT_Y_KEY = 0xFF;
    }

Lend:

	if((AKT_Y_KEY != 0xFF) && (AKT_X_KEY != 0xFF))
	{
		idx_isKey[idx] = true;
		idx_X_KEY[idx] = AKT_X_KEY;
		idx_Y_KEY[idx] = AKT_Y_KEY;

        unsigned short C64Key = VK_TO_C64[AKT_Y_KEY][AKT_X_KEY];

        if (KeyMatrixToPA != NULL) KeyMatrixToPA[C64Key&0xF] |= 1<<((C64Key>>4)&0xF);
        if (KeyMatrixToPB != NULL) KeyMatrixToPB[(C64Key>>4)&0xF] |= 1<<(C64Key&0xF);
	}
}

void KeyUp(int idx)
{
	static int AKT_Y_KEY,AKT_X_KEY;

	if(idx_isKey[idx])
	{
		AKT_X_KEY = idx_X_KEY[idx];
		AKT_Y_KEY = idx_Y_KEY[idx];

		unsigned short C64Key = VK_TO_C64[AKT_Y_KEY][AKT_X_KEY];

        if (KeyMatrixToPA != NULL) KeyMatrixToPA[C64Key&0xF] &= ~(1<<((C64Key>>4)&0xF));
        if (KeyMatrixToPB != NULL) KeyMatrixToPB[(C64Key>>4)&0xF] &= ~(1<<(C64Key&0xF));
	}
}

JNIEXPORT void JNICALL Java_de_kattisoft_emu64_NativeClass_Init(JNIEnv*, jobject)
{
	LOGD("Init...");

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    texture = new AssetTextureClass(vm);
    int ret = texture->AddTexture("images/ascii_font.png");
    if(ret == -1) LOGE("Fehler beim laden der Textur !");

    textBox01 = new TextBoxClass(2.0f,2.0f,80,40);
    textBox01->SetFont(texture->GetTexturID(ret),16,16);
    textBox01->SetPos(-1.0f,-1.0f);
    text_buffer = textBox01->GetTextBuffer();

    textBox01->SetText(0,0,"Emu64 von Thorsten Kattanek");
    textBox01->SetText(0,1,"Version: ");
    textBox01->SetText(9,1,str_emu64_version);

    ret = texture->AddTexture();
    if(ret == -1) LOGE("Fehler beim erstellen der C64 Textur !");

    ScreenTextureID = texture->GetTexturID(ret);

	// Mappingkoordinaten
	mapcoord[0].set(0,0);
	mapcoord[1].set(1,0);
	mapcoord[2].set(1,1);
	mapcoord[3].set(0,1);

	// Indexpuffer
	memcpy(indices,(unsigned short[]){0,1,3,1,2,3},sizeof indices);

	///////////////////////////
	ret = texture->AddTexture("images/c64_tastatur.png");
	if(ret == -1) LOGE("Fehler beim laden der C64 Tastatur Textur !");

	TastaturTextureID = texture->GetTexturID(ret);

    videopal = new VideoPalClass();

    int err;
    c64 = new C64Class(SOUND_BUFFER_SIZE/2,videopal);
    c64->sd_ext_path = sd_ext_path;

    KeyMatrixToPA = c64->KeyboardMatrixToPAExt;
    KeyMatrixToPB = c64->KeyboardMatrixToPBExt;

    for(int i=0;i<16;i++)
    {
    	idx_isKey[i] = false;
    	idx_X_KEY[i] = 0xff;
    	idx_Y_KEY[i] = 0xff;
    }

    /// ROMS laden

    char kernal_rom_path[1024];
    char basic_rom_path[1024];
    char char_rom_path[1024];
    char floppy_rom_path[1024];

    sprintf(kernal_rom_path,"%s/kernal.rom",sd_ext_path);
    sprintf(basic_rom_path,"%s/basic.rom",sd_ext_path);
    sprintf(char_rom_path,"%s/char.rom",sd_ext_path);
    sprintf(floppy_rom_path,"%s/1541.rom",sd_ext_path);

    if(!c64->LoadC64Roms(kernal_rom_path,basic_rom_path,char_rom_path))
    {
    	textBox01->SetText(17,30,"*** Fehler beim laden der C64 Roms !!! ***");
    }

    for(int i=0; i<FloppyAnzahl; i++)
    {
    	if(!c64->LoadFloppyRom(i,floppy_rom_path))
    		textBox01->SetText(17,30,"*** Fehler beim laden des Floppy Roms !!! ***");
    }

    /// Sound starten und somit auch die C64 Emulation
    OpenSLWrap_Init(SoundCallback);

    c64->HardReset();
}

JNIEXPORT void JNICALL Java_de_kattisoft_emu64_NativeClass_Destroy(JNIEnv*, jobject)
{
	LOGD("Destroy...");

	/// Sound stoppen und somit auch die C64 Emulation
	OpenSLWrap_Shutdown();

	if(videopal != NULL)
	{
		delete videopal;
		videopal = NULL;
	}

	if(c64 != NULL)
	{
		delete c64;
		c64 = NULL;
	}

	if(textBox01 != NULL) delete textBox01;
	if(texture != NULL) delete texture;
}

JNIEXPORT void JNICALL Java_de_kattisoft_emu64_NativeClass_Resize(JNIEnv*, jobject, jint xw, jint yw)
{
	if(xw > yw)
	{
		/// Landscape ///
		/// C64 Bildschirmpos ///
		screen_pos_x1=-0.75;
		screen_pos_y1=1.0;
		screen_pos_x2=0.75;
		screen_pos_y2=-1.0;

		// Koordinaten der Punkte
		screen_vert[0].set(screen_pos_x1,screen_pos_y1,0.1f);
		screen_vert[1].set(screen_pos_x2,screen_pos_y1,0.1f);
		screen_vert[2].set(screen_pos_x2,screen_pos_y2,0.1f);
		screen_vert[3].set(screen_pos_x1,screen_pos_y2,0.1f);

		isKeys = true;
		/// C64 Bildschirmpos ///
		keys_pos_x1=-1.0;
		//keys_pos_y1=0.15625;
		keys_pos_y1=0.0;
		keys_pos_x2=1.0;
		//keys_pos_y2=0.15625-0.5;
		keys_pos_y2=-1;

		keys_pos_x1_px = ((keys_pos_x1+1.0f)/2.0f)*(float)xw;
		keys_pos_y1_px = (((-1.0f*keys_pos_y1)+1.0f)/2.0f)*(float)yw;
		keys_pos_x2_px = ((keys_pos_x2+1.0f)/2.0f)*(float)xw;
		keys_pos_y2_px = (((-1.0f*keys_pos_y2)+1.0f)/2.0f)*(float)yw;

		keys_xw = keys_pos_x2_px - keys_pos_x1_px;
		keys_yw = keys_pos_y2_px - keys_pos_y1_px;

		keys_scale_x = keys_xw / 880.0f;
		keys_scale_y = keys_yw / 260.0f;

		LOGD("XW: %.3f, YW: %.3f",keys_xw,keys_yw);

		// Koordinaten der Punkte
		keys_vert[0].set(keys_pos_x1,keys_pos_y1,0.0f);
		keys_vert[1].set(keys_pos_x2,keys_pos_y1,0.0f);
		keys_vert[2].set(keys_pos_x2,keys_pos_y2,0.0f);
		keys_vert[3].set(keys_pos_x1,keys_pos_y2,0.0f);
	}
	else
	{
		/// Portrait ///
		/// C64 Bildschirmpos ///
		screen_pos_x1=-1.0;
		screen_pos_y1=1.0;
		screen_pos_x2=1.0;
		screen_pos_y2=0.15625;

		// Koordinaten der Punkte
		screen_vert[0].set(screen_pos_x1,screen_pos_y1,0.1f);
		screen_vert[1].set(screen_pos_x2,screen_pos_y1,0.1f);
		screen_vert[2].set(screen_pos_x2,screen_pos_y2,0.1f);
		screen_vert[3].set(screen_pos_x1,screen_pos_y2,0.1f);

		isKeys = true;
		/// C64 Bildschirmpos ///
		keys_pos_x1=-1.0;
		//keys_pos_y1=0.15625;
		keys_pos_y1=-0.5;
		keys_pos_x2=1.0;
		//keys_pos_y2=0.15625-0.5;
		keys_pos_y2=-1;

		keys_pos_x1_px = ((keys_pos_x1+1.0f)/2.0f)*(float)xw;
		keys_pos_y1_px = (((-1.0f*keys_pos_y1)+1.0f)/2.0f)*(float)yw;
		keys_pos_x2_px = ((keys_pos_x2+1.0f)/2.0f)*(float)xw;
		keys_pos_y2_px = (((-1.0f*keys_pos_y2)+1.0f)/2.0f)*(float)yw;

		keys_xw = keys_pos_x2_px - keys_pos_x1_px;
		keys_yw = keys_pos_y2_px - keys_pos_y1_px;

		keys_scale_x = keys_xw / 880.0f;
		keys_scale_y = keys_yw / 260.0f;

		LOGD("XW: %.3f, YW: %.3f",keys_xw,keys_yw);

		// Koordinaten der Punkte
		keys_vert[0].set(keys_pos_x1,keys_pos_y1,0.1f);
		keys_vert[1].set(keys_pos_x2,keys_pos_y1,0.1f);
		keys_vert[2].set(keys_pos_x2,keys_pos_y2,0.1f);
		keys_vert[3].set(keys_pos_x1,keys_pos_y2,0.1f);
	}

    glViewport(0,0,xw,yw);
    glMatrixMode(GL_PROJECTION);
    glOrthof(0,xw,yw,0,-1,1);
    glLoadIdentity();
}

JNIEXPORT void JNICALL Java_de_kattisoft_emu64_NativeClass_Renderer(JNIEnv*, jobject)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, ScreenTextureID);
	videopal->ConvertVideo((void*)c64->C64ScreenBuffer,c64->AktC64ScreenXW*2,c64->vic_puffer + 8*13,c64->AktC64ScreenXW,c64->AktC64ScreenYW,504,312,false);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, c64->AktC64ScreenXW, c64->AktC64ScreenYW, 0,GL_RGB, GL_UNSIGNED_SHORT_5_6_5, c64->C64ScreenBuffer);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor4f(1.0f,1.0f,1.0f,1.0f);

	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer (3, GL_FLOAT, 0, screen_vert);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer (2, GL_FLOAT, 0, mapcoord);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
	glDisable(GL_TEXTURE_2D);

	if(KeysVisible)
	{
		glBindTexture(GL_TEXTURE_2D, TastaturTextureID);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

		glColor4f(1.0f,1.0f,1.0f,0.3f);

		glEnable(GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_TEXTURE_2D);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer (3, GL_FLOAT, 0, keys_vert);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer (2, GL_FLOAT, 0, mapcoord);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
		glDisable(GL_TEXTURE_2D);
	}

	/// Debug INFOS anzeigen ///

	REG_STRUCT cpu_reg;
	IREG_STRUCT cpu_ireg;
	cpu_reg.REG_MASK = REG_MASK_ALL;
	c64->GetC64CpuReg(&cpu_reg,&cpu_ireg);

	char str01[81];
	sprintf(str01,"PC:$%4.4X AC:$%2.2X XR:$%2.2X YR:$%2.2X SR:$%2.2X SP:$%3.3X",cpu_reg.PC,(unsigned char)cpu_reg.AC & 0xff,(unsigned char)cpu_reg.XR & 0xff,(unsigned char)cpu_reg.YR & 0xff,(unsigned char)cpu_reg.SR & 0xff,(unsigned short)((cpu_reg.SP & 0xff)+0x100));
	textBox01->SetText(0,2,str01);
	textBox01->Renderer();

}

JNIEXPORT void JNICALL Java_de_kattisoft_emu64_NativeClass_Pause(JNIEnv*, jobject)
{
	LOGD("PAUSE...");
}

JNIEXPORT void JNICALL Java_de_kattisoft_emu64_NativeClass_Resume(JNIEnv*, jobject)
{
	LOGD("RESUME...");
}

JNIEXPORT void JNICALL Java_de_kattisoft_emu64_NativeClass_SetSDExtPath(JNIEnv* env, jobject, jstring string)
{
	sd_ext_path = env->GetStringUTFChars(string, JNI_FALSE);
	LOGD("Emu64 Daten Pfad: %s",sd_ext_path);
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* _vm, void* reserved)
{
	vm = _vm;
	LOGD("OnLoad...");

	return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL Java_de_kattisoft_emu64_NativeClass_Menue(JNIEnv*, jobject)
{
	LOGD("Menü...");
	char str00[1024];

	switch(menue_case)
	{
	case 0:
		sprintf(str00,"%s/1_pixel_fld.prg",sd_ext_path);
	    c64->LoadAutoRun(0,str00);
		break;
	case 1:
		sprintf(str00,"%s/plexer_dycp.prg",sd_ext_path);
	    c64->LoadAutoRun(0,str00);
		break;
	case 2:
		sprintf(str00,"%s/flexgrid.prg",sd_ext_path);
	    c64->LoadAutoRun(0,str00);
		break;
	case 3:
		c64->HardReset();
		break;
	}

	menue_case++;
	if(menue_case == 4) menue_case = 0;
}

JNIEXPORT void JNICALL Java_de_kattisoft_emu64_NativeClass_TouchDown(JNIEnv*, jobject,int idx, float x, float y)
{
	bool isKeys = false;

	if(y >= keys_pos_y1_px && y <= keys_pos_y2_px )
	{
		if(x >= keys_pos_x1_px && x <= keys_pos_x2_px )
		{
			KeyDown(idx, x - keys_pos_x1_px,y - keys_pos_y1_px);
			isKeys = true;
		}
	}

	if(!KeysVisible)
	{
		if(KeysVisible) KeysVisible = false;
		else KeysVisible = true;
	}
	else if (!isKeys)
	{
		if(KeysVisible) KeysVisible = false;
		else KeysVisible = true;
	}
}

JNIEXPORT void JNICALL Java_de_kattisoft_emu64_NativeClass_TouchUp(JNIEnv*, jobject,int idx, float x, float y)
{
	KeyUp(idx);
}

JNIEXPORT void JNICALL Java_de_kattisoft_emu64_NativeClass_Home(JNIEnv*, jobject)
{
	c64->HardReset();
}

#ifdef __cplusplus
}
#endif
