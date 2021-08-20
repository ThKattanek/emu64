//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: floppy1581_class.h                    //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 20.08.2021	       		//
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef FLOPPY1581_H
#define FLOPPY1581_H

#include <math.h>
#include <cstring>
#include <fstream>
#include <functional>

#include "./structs.h"

class Floppy1581
{
public:
	/// Funktionen ///

	Floppy1581(bool *reset);
	void SetC64IEC(uint8_t *iec);
	void SetResetReady(bool* ResetReady, uint16_t ResetReadyAdr);
	void SetDeviceNumber(uint8_t number);
	bool LoadDosRom(const char* filename);
	void SetEnableFloppy(bool status);

	/// Variablen ///

	bool            *RESET;
	uint8_t         FloppyIECLocal;
	uint8_t         Jumper;

private:
	bool    FloppyEnabled;

	uint8_t RAM[0x0800];	// 2KB
	uint8_t ROM[0x8000];	// 32KB
};

#endif // FLOPPY1581_H
