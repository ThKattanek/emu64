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

