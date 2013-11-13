//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: c64_class.h                           //
// ANDROID PORT                                             //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 13.11.2013                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef C64_CLASS_H_
#define C64_CLASS_H_

#include "../../../version.h"
#include "../../../mmu_class.h"
#include "../../../mos6510_class.h"
#include "../../../mos6569_class.h"
#include "../../../mos6581_8085_class.h"
#include "../../../mos6526_class.h"
#include "../../../crt_class.h"
#include "../../../floppy1541_class.h"
#include "../../../cpu_info.h"
#include "../../../c64keys2usb.h"

#include "tr1/functional"
using namespace std::tr1;
using namespace std::tr1::placeholders;

#define FloppyAnzahl 1
#define MAX_BREAK_GROUPS 256
#define MAX_JOYSTICKS 16
#define MAX_VJOYS 16

class C64Class {
public:
	C64Class(int SoundBufferSize);
	virtual ~C64Class();
	void FillAudioBuffer(short *stream, int laenge); // Über diese Funktion wird der C64 Takt erzeugt !! //

	bool RESET;     // Reset Leitung -> Für Alle Module mit Reset Eingang

    MOS6581_8085    *sid1;
    MOS6581_8085    *sid2;

    const char* sd_ext_path;

    int counter;
};

#endif /* C64_CLASS_H_ */
