//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: wd1770_class.cpp                      //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 21.08.2021                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "wd1770_class.h"

WD1770::WD1770()
{

}

WD1770::~WD1770()
{

}

void WD1770::Reset()
{

}

void WD1770::OneZyklus()
{

}

uint8_t WD1770::ReadIO(uint16_t address)
{
	// Register 0 - 3
	cout << "Read from WD1770 IO-Address: " << std::hex << address << endl;
	return 0;
}

void WD1770::WriteIO(uint16_t address, uint8_t value)
{
	// Register 0 - 3
	cout << "Write to WD1770 IO-Address: " << std::hex << address << ", " << (uint16_t)value << endl;
}
