//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: mos8520_class.cpp                     //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 21.08.2021                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "mos8520_class.h"

MOS8520::MOS8520()
{

}

MOS8520::~MOS8520()
{
}

void MOS8520::Reset()
{

}

void MOS8520::OneZyklus()
{

}

uint8_t MOS8520::ReadIO(uint16_t address)
{
	// Register 0 - 15
	cout << "Read from MOS8520 IO-Address: " << std::hex << address << endl;
	return 0;
}

void MOS8520::WriteIO(uint16_t address, uint8_t value)
{
	// Register 0 - 15
	cout << "Write to MOS8520 IO-Address: " << std::hex << address << ", " << (uint16_t)value << endl;
}
