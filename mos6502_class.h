//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: mos6502_class.h                       //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 02.08.2011		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#ifndef MOS6502_CLASS_H
#define MOS6502_CLASS_H

#include "structs.h"

#include "tr1/functional"
using namespace std::tr1;
using namespace std::tr1::placeholders;

class MOS6502
{
public:

    /// Funktionen ///

    MOS6502(void);
    ~MOS6502(void);

    bool OneZyklus(void);
    void Reset(void);
    void ClearJAMFlag(void);
    void GetRegister(REG_STRUCT *reg);
    void SetRegister(REG_STRUCT *reg);
    void GetInterneRegister(IREG_STRUCT *ireg);
    void SET_SR_BIT6(void);

    void TriggerInterrupt(int typ);
    void ClearInterrupt(int typ);

    function<unsigned char(unsigned short)> *ReadProcTbl;
    function<void(unsigned short,unsigned char)> *WriteProcTbl;

    /// Variablen ///

    // Signale von Außen

    bool *RESET;
    bool *ResetReady;               // Wird bei einem Reset False und beim erreichen einer
                                    // Adresse wird es True
    unsigned short  ResetReadyAdr;
    unsigned short  *BreakStatus;
    unsigned short  *Breakpoints;
    unsigned short  *BreakWerte;
    unsigned short  *History;
    unsigned char   *HistoryPointer;

private:

    /// Funktionen ///

    inline void SET_SR_NZ(unsigned char wert);
    inline void SET_SIGN(unsigned char wert);
    inline void SET_ZERO(unsigned char wert);
    inline void SET_CARRY(unsigned char status);
    inline bool IF_CARRY(void);
    inline bool IF_DECIMAL(void);
    inline void SET_OVERFLOW(bool status);
    inline unsigned char Read(unsigned short adresse);
    inline void Write(unsigned short adresse, unsigned char wert);

    /// Variablen ///

    unsigned char  *MCT;	// Zeigt immer an die aktuelle stelle in derMicro Code Tabel
    unsigned char   Pointer;
    unsigned short  Adresse;
    unsigned short  BranchAdresse;
    unsigned short  AktOpcodePC;
    bool            JAMFlag;
    unsigned char   TMPByte;
    unsigned short  PC;
    unsigned char   AC;
    unsigned char   XR;
    unsigned char   YR;
    unsigned char   SP;
    unsigned char   SR;
    int             IRQCounter;
    bool            Interrupts[IntQuellenFloppy];

    /// Für OneZyklus ///

    unsigned int tmp,tmp1;
    unsigned short tmp2;
    unsigned short src;
    bool carry_tmp;
    unsigned char idxReg;
};

#define SetAdresseLo(wert) Adresse = ((Adresse&0xFF00)|wert)
#define SetAdresseHi(wert) Adresse = ((Adresse&0x00FF)|(wert<<8))

#define SetPCLo(wert) PC=((PC&0xFF00)|wert)
#define SetPCHi(wert) PC=((PC&0x00FF)|(wert<<8))

#define GetPCLo ((unsigned char)PC)
#define GetPCHi ((unsigned char)(PC>>8))

#endif // MOS6502_CLASS_H
