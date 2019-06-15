//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: mos6526_port.cpp                      //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 15.06.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "mos6526_port.h"

PORT::PORT()
{
    input_bits = 0;
    output_bits = 0;
}

PORT::~PORT()
{
}

uint8_t *PORT::GetOutputBitsPointer()
{
    return &output_bits;
}

uint8_t *PORT::GetInputBitsPointer()
{
    return &input_bits;
}

unsigned char PORT::GetInput()
{
    return ~input_bits;
}

void PORT::SetInput(uint8_t value)
{
    input_bits = ~value;
}

uint8_t PORT::GetOutput()
{
    return ~output_bits;
}

void PORT::SetOutput(uint8_t value)
{
    output_bits = ~value;
}

/*
bool PORT::SaveFreez(FILE* File)
{
    fwrite(&input_bits,1,sizeof(input_bits),File);
    fwrite(&output_bits,1,sizeof(output_bits),File);
	return true;
}

bool PORT::LoadFreez(FILE* File,unsigned short Version)
{
	switch (Version)
	{
	case 0x0100:
	case 0x0101:
        fread(&input_bits,1,sizeof(input_bits),File);
        fread(&output_bits,1,sizeof(output_bits),File);
		break;
	}
	return true;
}
*/
