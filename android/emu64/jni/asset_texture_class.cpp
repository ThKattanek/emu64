/*
 * asset_texture_class.cpp
 *
 *  Created on: 20.10.2013
 *      Author: thorsten
 */

#include <asset_texture_class.h>


AssetTextureClass::AssetTextureClass(JavaVM *vm) {
	this->vm = vm;

	anz_texturen = 0;
	glGenTextures(MAX_TEXTUREN, TexturIDs);
}

AssetTextureClass::~AssetTextureClass()
{
	glDeleteTextures(MAX_TEXTUREN, TexturIDs);
}

int AssetTextureClass::AddTexture(const char* filename)
{
	if(anz_texturen == MAX_TEXTUREN) return -1;

	glBindTexture(GL_TEXTURE_2D, TexturIDs[anz_texturen]);

	int ret = LoadTexture(filename,anz_texturen);
	if(ret < 0) return -1;

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	int size = TexturXW[anz_texturen] * TexturYW[anz_texturen]*4;
	unsigned char* buffer =  (unsigned char*)TexturPixel[anz_texturen];

	/// Rot mit Blau tauchen (BGRA -> RGBA)
	for(int i=0;i<size;i+=4)
	{
		// Standardtrick tauschen ohne TempVariable mittels 3x XOR //
		buffer[i] ^= buffer[i+2];
		buffer[i+2] ^= buffer[i];
		buffer[i] ^= buffer[i+2];
	}

	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, TexturXW[anz_texturen], TexturYW[anz_texturen], 0,GL_RGBA, GL_UNSIGNED_BYTE, TexturPixel[anz_texturen]);

	anz_texturen++;
	return (anz_texturen-1);
}

int AssetTextureClass::AddTexture()
{
	if(anz_texturen == MAX_TEXTUREN) return -1;

	anz_texturen++;
	return (anz_texturen-1);
}

int AssetTextureClass::LoadTexture(const char* filename, int number)// void* pixels, int* xw, int* yw)
{
	JNIEnv *env = NULL;
	jclass cls;
	jmethodID constr;

	if (vm == NULL) return -1;

	vm->GetEnv((void**)&env, JNI_VERSION_1_6);
	if (env == NULL) return -1;

	cls = env->FindClass(IMAGE_MANAGER_KLASSE);
	if(!cls) return -1;
	else
	{
	    constr = env->GetMethodID(cls, "<init>", "()V");
	    if(!constr) return -1;
	}

	jmethodID mid;

	/* Ask the PNG manager for a bitmap */
	mid = env->GetMethodID(cls, "open","(Ljava/lang/String;)Landroid/graphics/Bitmap;");
	jstring name = env->NewStringUTF(filename);
	jobject obj  = env->NewObject(cls, constr);
	jobject png = env->CallObjectMethod(obj, mid, name);
	env->DeleteLocalRef(name);
	env->NewGlobalRef(png);

	if(png == 0)
	{
		return -1;
	}

	/* Get image dimensions */
	mid = env->GetMethodID(cls, "getWidth", "(Landroid/graphics/Bitmap;)I");
	int width = env->CallIntMethod(obj, mid, png);
	mid = env->GetMethodID(cls, "getHeight", "(Landroid/graphics/Bitmap;)I");
	int height = env->CallIntMethod(obj, mid, png);

	/* Get pixels */
	jintArray array = env->NewIntArray(width * height);
	env->NewGlobalRef(array);
	mid = env->GetMethodID(cls, "getPixels", "(Landroid/graphics/Bitmap;[I)V");
	env->CallVoidMethod(obj, mid, png, array);

	TexturPixel[number] = env->GetIntArrayElements(array, 0);
	TexturXW[number] = width;
	TexturYW[number] = height;

	env->ReleaseIntArrayElements(array, (int*)TexturPixel[number], 0);
	env->DeleteGlobalRef(array);

	/* Free image */
	mid = env->GetMethodID(cls, "close", "(Landroid/graphics/Bitmap;)V");
	env->CallVoidMethod(obj, mid, png);
	env->DeleteGlobalRef(png);

	return 0;
}

unsigned int AssetTextureClass::GetTexturID(int number)
{
	return TexturIDs[number];
}
