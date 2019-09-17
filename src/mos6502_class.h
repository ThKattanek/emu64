//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: mos6502_class.h                       //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 07.08.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef MOS6502_CLASS_H
#define MOS6502_CLASS_H

#include <functional>

#include "./structs.h"

class MOS6502
{
public:

    /// Funktionen ///

    MOS6502(void);
    ~MOS6502(void);

    bool OneZyklus(void);
    void Phi1();
    void Reset(void);
    void ClearJAMFlag(void);
    void GetRegister(REG_STRUCT *reg);
    void SetRegister(REG_STRUCT *reg);
    void GetInterneRegister(IREG_STRUCT *ireg);
    void SET_SR_BIT6(void);

    void TriggerInterrupt(int typ);
    void ClearInterrupt(int typ);

    std::function<unsigned char(unsigned short)> *ReadProcTbl;
    std::function<void(unsigned short,unsigned char)> *WriteProcTbl;

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
    unsigned short  AktOpcode;
    unsigned short  AktOpcodePC;
    bool            JAMFlag;
    unsigned char   TMPByte;
    bool            LastOPC_CLI;
    unsigned short  PC;
    unsigned char   AC;
    unsigned char   XR;
    unsigned char   YR;
    unsigned char   SP;
    unsigned char   SR;

    unsigned char   irq_state;          // if Größer 0 ist die Leitung low
    bool            irq_is_low_pegel;
    bool            irq_is_active;

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
