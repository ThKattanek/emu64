#include <jni.h>
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <android/log.h>
#include <stdio.h>

#define  LOG_TAG    "NATIVE"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define printf(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__);

#include "asset_texture_class.h"
#include "text_box_class.h"
#include "dsp.h"
#include "c64_class.h"

/// Globale Variablen
JavaVM *vm;
AssetTextureClass *texture = NULL;
TextBoxClass *textBox01 = NULL;

char* text_buffer;
const char* sd_ext_path;
unsigned char start_wert = 0;

#ifdef __cplusplus
extern "C"
{
#endif

unsigned short sample = 0;

/// C64 Klasse
C64Class *c64;

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

    int err;
    c64 = new C64Class(SOUND_BUFFER_SIZE/2);
    c64->sd_ext_path = sd_ext_path;

    /// Sound starten und somit auch die C64 Emulation
    OpenSLWrap_Init(SoundCallback);
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

JNIEXPORT void JNICALL Java_de_kattisoft_emu64_NativeClass_Destroy(JNIEnv*, jobject)
{
	LOGD("Destroy...");

	OpenSLWrap_Shutdown();

	if(c64 != NULL)
	{
		delete c64;
		c64 = NULL;
	}

	if(textBox01 != NULL) delete textBox01;
	if(texture != NULL) delete texture;
}

JNIEXPORT void JNICALL Java_de_kattisoft_emu64_NativeClass_SetSDExtPath(JNIEnv* env, jobject, jstring string)
{
	sd_ext_path = env->GetStringUTFChars(string, JNI_FALSE);
	LOGD("SD Karte Extern: %s",sd_ext_path);
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* _vm, void* reserved)
{
	vm = _vm;
	LOGD("OnLoad...");

	return JNI_VERSION_1_6;
}

#ifdef __cplusplus
}
#endif
