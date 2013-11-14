/*
 * asset_texture_class.h
 *
 *  Created on: 20.10.2013
 *      Author: thorsten
 */

#define IMAGE_MANAGER_KLASSE "de/kattisoft/emu64/ImageManager"

#define MAX_TEXTUREN 100

#ifndef ASSET_TEXTURE_CLASS_H_
#define ASSET_TEXTURE_CLASS_H_

#include <GLES/gl.h>
#include <GLES/glext.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

class AssetTextureClass {
public:
	AssetTextureClass(JavaVM *vm);
	virtual ~AssetTextureClass();
	int AddTexture(const char* filename);
	int AddTexture();
	unsigned int GetTexturID(int number);

private:
	int LoadTexture(const char* filename, int number);

	JavaVM *vm;
	int anz_texturen;
	unsigned int TexturIDs[MAX_TEXTUREN];
	int TexturXW[MAX_TEXTUREN];
	int TexturYW[MAX_TEXTUREN];
	void* TexturPixel[MAX_TEXTUREN];
};

#endif /* ASSET_TEXTURE_CLASS_H_ */
