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
#include "./mos6502_class.h"

#define MAX_BREAK_GROUP_NUM 255

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
	bool OneCycle();

	/// Wird mit den einzelnen Chips verbunden via "bind" ///

	void WriteNoMem(uint16_t address, uint8_t value);
	uint8_t ReadNoMem(uint16_t address);
	void WriteRam(uint16_t address, uint8_t value);
	uint8_t ReadRam(uint16_t address);
	uint8_t ReadRom(uint16_t address);

	/// Variablen ///

	bool            *RESET;
	uint8_t         FloppyIECLocal;
	uint8_t         Jumper;

	uint16_t        History[256];
	uint8_t         HistoryPointer;

private:

	/// Funktionen ///

	bool    FloppyEnabled;

	/// Variablen ///

	std::function<uint8_t(uint16_t)>  ReadProcTbl[256];
	std::function<void(uint16_t, uint8_t)> WriteProcTbl[256];

	uint8_t RAM[0x2000];	// 8KB
	uint8_t ROM[0x8000];	// 32KB

	bool CIA_IRQ;
	bool IRQ;

	int CycleCounter;

	MOS6502 *cpu;

	/////////////////////// BREAKPOINTS ////////////////////////

	// Bit 0 = PC Adresse
	// Bit 1 = AC
	// Bit 2 = XR
	// Bit 3 = YR
	// Bit 4 = Lesen von einer Adresse
	// Bit 5 = Schreiben in einer Adresse
	// Bit 6 = Lesen eines Wertes
	// Bit 7 = Schreiben eines Wertes
	// Bit 8 = Beim erreichen einer bestommten Raster Zeile
	// Bit 9 = Beim erreichen eines Zyklus in einer Rasterzeile

	uint16_t        Breakpoints[0x10000];
	uint16_t        BreakWerte[16];
	uint16_t        BreakStatus;
	bool            *FoundBreakpoint;

	uint8_t         breakgroup_count;
	BREAK_GROUP     *BreakGroup[MAX_BREAK_GROUP_NUM];

	////////////////////////////////////////////////////////////
};

#endif // FLOPPY1581_H
