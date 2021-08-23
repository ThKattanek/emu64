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
// Letzte Änderung am 23.08.2021                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef MOS8520_H
#define MOS8520_H

#include <inttypes.h>
#include <functional>
#include <iostream>

#include "./cia_port.h"

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

	PORT        pa;
	PORT        pb;
	uint8_t     pa_latch;
	uint8_t     pb_latch;

private:

	// Funktionen

	// Variablen

	uint8_t     io[16];
	uint8_t     ddr_a;
	uint8_t     ddr_b;
};

#endif // MOS8520_H
