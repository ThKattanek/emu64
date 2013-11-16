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

#define numVertices 4
#define numIndices 6
vertex vertices[numVertices];
map mapcoord[numVertices];
unsigned short indices[numIndices];
float screen_pos_x1,screen_pos_y1,screen_pos_x2,screen_pos_y2;

unsigned int texID;

int menue_case = 0;

/// C64 Klasse
C64Class *c64;

/// Video Pal Klasse
VideoPalClass *videopal;

void SoundCallback(short *buffer, int num_samples)
{
	c64->FillAudioBuffer(buffer,num_samples);
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

    texID = texture->GetTexturID(ret);

    screen_pos_x1=-1;
    screen_pos_y1=1;
    screen_pos_x2=1;
    screen_pos_y2=-1;

	// Koordinaten der Punkte
	vertices[0].set(screen_pos_x1,screen_pos_y1,0.1f);
	vertices[1].set(screen_pos_x2,screen_pos_y1,0.1f);
	vertices[2].set(screen_pos_x2,screen_pos_y2,0.1f);
	vertices[3].set(screen_pos_x1,screen_pos_y2,0.1f);

	// Mappingkoordinaten
	mapcoord[0].set(0,0);
	mapcoord[1].set(1,0);
	mapcoord[2].set(1,1);
	mapcoord[3].set(0,1);

	// Indexpuffer
	memcpy(indices,(unsigned short[]){0,1,3,1,2,3},sizeof indices);

    videopal = new VideoPalClass();

    int err;
    c64 = new C64Class(SOUND_BUFFER_SIZE/2,videopal);
    c64->sd_ext_path = sd_ext_path;

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
    glViewport(0,0,xw,yw);
    glMatrixMode(GL_PROJECTION);
    glOrthof(0,xw,yw,0,-1,1);
    glLoadIdentity();
}

JNIEXPORT void JNICALL Java_de_kattisoft_emu64_NativeClass_Renderer(JNIEnv*, jobject)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, texID);
	videopal->ConvertVideo((void*)c64->C64ScreenBuffer,c64->AktC64ScreenXW*2,c64->vic_puffer,c64->AktC64ScreenXW,c64->AktC64ScreenYW,504,312,false);
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
	glVertexPointer (3, GL_FLOAT, 0, vertices);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer (2, GL_FLOAT, 0, mapcoord);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, indices);
	glDisable(GL_TEXTURE_2D);

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

JNIEXPORT void JNICALL Java_de_kattisoft_emu64_NativeClass_Home(JNIEnv*, jobject)
{
	c64->HardReset();
}

#ifdef __cplusplus
}
#endif
