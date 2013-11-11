/*
 * text_box_class.h
 *
 *  Created on: 20.10.2013
 *      Author: thorsten
 */

#ifndef TEXT_BOX_CLASS_H_
#define TEXT_BOX_CLASS_H_

#include <GLES/gl.h>
#include <GLES/glext.h>
#include <stdlib.h>

class VERTEX{
	private: float x,y,z;
	public: void set(float x, float y, float z){this->x = x;this->y = y;this->z = z;}
};

class MAPCOORD{
	private: float x,y;
	public: void set(float x, float y){this->x = x;this->y = y;}
};

class TextBoxClass {
public:
	TextBoxClass(float xw, float yw, int text_xw, int text_yw);
	virtual ~TextBoxClass();

	void SetFont(unsigned int texture_id, int x_chars, int y_chars, char* ascii);
	void SetFont(unsigned int texture_id, int x_chars, int y_chars);
	void SetPos(float x, float y);
	void SetText(int x, int y, char* text);
	char* GetTextBuffer();
	void UpdateTextBuffer();
	void ClearTextBuffer();
	void Renderer();

private:
	//void CreateFontMapping(int x_chars, int y_chars, char* ascii);

private:
	float pos_x,pos_y;
	float size_xw,size_yw;

	int anz_vertex;
	int anz_mapcoord;
	int anz_indices;

	VERTEX *vertices;
	MAPCOORD *mapcoord;
	unsigned short *indices;

	MAPCOORD mapfont[256*4];

	unsigned int texture_id;

	int text_buffer_xw,text_buffer_yw;
	int text_buffer_size;
	char *text_buffer;
};

#endif /* TEXT_BOX_CLASS_H_ */
