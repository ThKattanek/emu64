//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: wd1770_class.h                        //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 21.08.2021                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef WD1770_H
#define WD1770_H

#include <inttypes.h>
#include <functional>
#include <iostream>

using namespace std;

class WD1770
{
public:

	// Funktionen

	WD1770();
	~WD1770();

	void Reset();
	void OneZyklus();
	uint8_t ReadIO(uint16_t address);
	void WriteIO(uint16_t address, uint8_t value);

	// Variablen

	bool	*reset;
};

#endif // WD1770_H
