/*
 * c64_class.h
 *
 *  Created on: 11.11.2013
 *      Author: thorsten
 */

#ifndef C64_CLASS_H_
#define C64_CLASS_H_

#include "../../../mos6581_8085_class.h"

class C64Class {
public:
	C64Class();
	virtual ~C64Class();
	void FillAudioBuffer(short *stream, int laenge); // Über diese Funktion wird der C64 Takt erzeugt !! //

	bool RESET;     // Reset Leitung -> Für Alle Module mit Reset Eingang

    MOS6581_8085    *sid1;
    MOS6581_8085    *sid2;
};

#endif /* C64_CLASS_H_ */
