//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: mos8520_class.h                       //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 21.08.2021                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef MOS8520_H
#define MOS8520_H

#include <inttypes.h>
#include <functional>
#include <iostream>

using namespace std;

class MOS8520
{
public:

	// Funktionen

	MOS8520();
	~MOS8520();

	void Reset();
	void OneZyklus();
	uint8_t ReadIO(uint16_t address);
	void WriteIO(uint16_t address, uint8_t value);

	// Variablen

	bool	*reset;
};

#endif // MOS8520_H
