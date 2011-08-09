//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: mos6526_port.cpp                      //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 05.04.2011		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#include "mos6526_port.h"

PORT::PORT()
{
	InputBits = 0;
	OutputBits = 0;
}

PORT::~PORT()
{
}

unsigned char* PORT::GetOutputBitsPointer(void)
{
	return &OutputBits;
}

unsigned char* PORT::GetInputBitsPointer(void)
{
	return &InputBits;
}

unsigned char PORT::GetInput(void)
{
	return ~InputBits;
}

void PORT::SetInput(unsigned char wert)
{
	InputBits = ~wert;
}

unsigned char PORT::GetOutput(void)
{
	return ~OutputBits;
}

void PORT::SetOutput(unsigned char wert)
{
	OutputBits = ~wert;
}

/*
bool PORT::SaveFreez(FILE* File)
{
	fwrite(&InputBits,1,sizeof(InputBits),File);
	fwrite(&OutputBits,1,sizeof(OutputBits),File);
	return true;
}

bool PORT::LoadFreez(FILE* File,unsigned short Version)
{
	switch (Version)
	{
	case 0x0100:
	case 0x0101:
		fread(&InputBits,1,sizeof(InputBits),File);
		fread(&OutputBits,1,sizeof(OutputBits),File);
		break;
	}
	return true;
}
*/
