/*
 * text_box_class.cpp
 *
 *  Created on: 20.10.2013
 *      Author: thorsten
 */

#include <text_box_class.h>

TextBoxClass::TextBoxClass(float xw, float yw, int text_xw, int text_yw)
{
	int i;
	float x_div, y_div;

	size_xw = xw;
	size_yw = yw;

	pos_x = pos_y = 0.0f;

	text_buffer_xw = text_xw;
	text_buffer_yw = text_yw;
	text_buffer_size = text_xw * text_yw;

	text_buffer = new char[text_buffer_size];
	for(int i=0; i<text_buffer_size; i++)
		text_buffer[i] = 0x20; // Leerzeichen

	anz_vertex = (text_xw)*(text_yw)*4;
	anz_mapcoord = 0;

	vertices = NULL;
	mapcoord = NULL;
	indices = NULL;

	// Indexpuffer
	anz_indices = text_buffer_size * 6;
	indices = new unsigned short[anz_indices];

	i=0;
	for(int y=0; y<(text_buffer_yw);y++)
		for(int x=0; x<(text_buffer_xw);x++)
		{
			int nr = y*text_buffer_xw*4 + x*4;

			indices[i++] = nr;
			indices[i++] = nr+1;
			indices[i++] = nr+2;

			indices[i++] = nr;
			indices[i++] = nr+2;
			indices[i++] = nr+3;
		}

	// Koordinaten der Punkte
	vertices = new VERTEX[anz_vertex];

	i=0;
	x_div = size_xw / text_buffer_xw;
	y_div = size_yw / text_buffer_yw;

	for(int y=0; y<(text_buffer_yw);y++)
		for(int x=0; x<(text_buffer_xw);x++)
		{
			vertices[i++].set(x * x_div, (text_buffer_yw-y-1) * y_div ,0.0f);
			vertices[i++].set(x * x_div, (text_buffer_yw-y) * y_div ,0.0f);
			vertices[i++].set((x+1) * x_div, (text_buffer_yw-y) * y_div ,0.0f);
			vertices[i++].set((x+1) * x_div, (text_buffer_yw-y-1) * y_div ,0.0f);
		}

	anz_mapcoord = (text_xw)*(text_yw)*4;
	mapcoord = new MAPCOORD[anz_mapcoord];

	i=0;
	for(int y=0; y<(text_buffer_yw);y++)
		for(int x=0; x<(text_buffer_xw);x++)
		{
			mapcoord[i++].set(0.0f,0.0f);
			mapcoord[i++].set(0.0f,0.0f);
			mapcoord[i++].set(0.0f,0.0f);
			mapcoord[i++].set(0.0f,0.0f);
		}
}

TextBoxClass::~TextBoxClass()
{
	if(indices != NULL) delete indices;
	if(vertices != NULL) delete vertices;
	if(mapcoord != NULL) delete mapcoord;
}

void TextBoxClass::Renderer()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Position
	glTranslatef(pos_x,pos_y,0);

	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	glColor4f(1.0f,1.0f,1.0f,1.0f);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer (3, GL_FLOAT, 0, vertices);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer (2, GL_FLOAT, 0, mapcoord);
	glDrawElements(GL_TRIANGLES, anz_indices, GL_UNSIGNED_SHORT, indices);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void TextBoxClass::SetPos(float x, float y)
{
	pos_x = x;
	pos_y = y;
}

void TextBoxClass::SetFont(unsigned int texture_id, int x_chars, int y_chars, char* ascii)
{
	this->texture_id = texture_id;

	for(int i=0; i<256*4; i++)
		mapfont[i].set(0,0);

	float xw		= 1.0f/x_chars;
	float yw 		= 1.0f/y_chars;
	float start_x;
	float start_y;

	int x,y;
	x=y=0;

	while(*ascii != 0)
	{
		start_x = x*xw;
		start_y = y*yw;

		mapfont[*ascii*4].set(start_x,start_y+yw);
		mapfont[*ascii*4+1].set(start_x,start_y);
		mapfont[*ascii*4+2].set(start_x+xw,start_y);
		mapfont[*ascii*4+3].set(start_x+xw,start_y+yw);

		x++;
		if(x == x_chars)
		{
			x = 0;
			y++;
			if(y == y_chars) break;
		}
		ascii++;
	}
}

void TextBoxClass::SetFont(unsigned int texture_id, int x_chars, int y_chars)
{
	this->texture_id = texture_id;

	float xw		= 1.0f/x_chars;
	float yw 		= 1.0f/y_chars;
	float start_x;
	float start_y;

	int x,y;
	x=y=0;

	int ascii = 0;
	while(ascii < 256)
	{
		start_x = x*xw;
		start_y = y*yw;

		mapfont[ascii*4].set(start_x,start_y+yw);
		mapfont[ascii*4+1].set(start_x,start_y);
		mapfont[ascii*4+2].set(start_x+xw,start_y);
		mapfont[ascii*4+3].set(start_x+xw,start_y+yw);

		x++;
		if(x == x_chars)
		{
			x = 0;
			y++;
			if(y == y_chars) break;
		}
		ascii++;
	}
}

void TextBoxClass::SetText(int x, int y, char* text)
{
	if(x >= text_buffer_xw || y >= text_buffer_yw || text == NULL) return;

	int buffer_pos = y*text_buffer_xw+x;
	int map_pos = buffer_pos*4;
	int _char;

	while((*text != 0) && (map_pos < text_buffer_size * 4))
	{
		_char = *text*4;
		mapcoord[map_pos++]=mapfont[_char++];
		mapcoord[map_pos++]=mapfont[_char++];
		mapcoord[map_pos++]=mapfont[_char++];
		mapcoord[map_pos++]=mapfont[_char++];
		text_buffer[buffer_pos++] = *text;
		text++;
	}
}

char* TextBoxClass::GetTextBuffer()
{
	return text_buffer;
}

void TextBoxClass::UpdateTextBuffer()
{
	int i=0;
	for(int y=0; y<(text_buffer_yw);y++)
		for(int x=0; x<(text_buffer_xw);x++)
		{
			int txt = text_buffer[y*text_buffer_xw+x]*4;
			mapcoord[i++] = mapfont[txt++];
			mapcoord[i++] = mapfont[txt++];
			mapcoord[i++] = mapfont[txt++];
			mapcoord[i++] = mapfont[txt++];
		}
}

void TextBoxClass::ClearTextBuffer()
{
	for(int i=0; i<text_buffer_size;i++)
	{
		text_buffer[i] = ' ';
	}
	UpdateTextBuffer();
}
