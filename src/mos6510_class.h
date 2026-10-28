//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: mos6510_class.h                       //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef MOS_6510_CLASS_H
#define MOS_6510_CLASS_H

#include "./structs.h"
#include <functional>

class MOS6510
{
public:

    /// Funktionen ///
    MOS6510(void);
    ~MOS6510(void);
    bool OneZyklus();
    void Phi1();
    void ClearJAMFlag(void);
    void GetRegister(REG_STRUCT *reg);
    unsigned short GetPC() const { return PC; }
    void SetRegister(REG_STRUCT *reg);
    bool GetInterrupts(int typ);
    void GetInterneRegister(IREG_STRUCT *ireg);
    //bool SaveFreez(FILE *File);
    //bool LoadFreez(FILE *File,unsigned short Version);

    void TriggerInterrupt(int typ);
    void ClearInterrupt(int typ);

    std::function<unsigned char(unsigned short)> *ReadProcTbl;
    std::function<void(unsigned short,unsigned char)> *WriteProcTbl;

    /// Variablen ///

    // Signale von Außen
    bool *RDY;
    bool *RESET;
    bool WRITE_FF00;

    bool EnableExtInterrupts;

    bool *ResetReady;	// Wird bei einem Reset False und beim erreichen einer
    // Adresse wird es True
    unsigned short ResetReadyAdr;
    bool JAMFlag;
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

    unsigned char  *MCT;		// Zeigt immer an die aktuelle stelle in derMicro Code Tabel
    unsigned char   Pointer;
    unsigned short  Adresse;
    unsigned short  BranchAdresse;
    unsigned short  AktOpcode;
    unsigned short  AktOpcodePC;
    bool            CpuWait;
    unsigned char   TMPByte;

    unsigned short  PC;
    unsigned char   AC;
    unsigned char   XR;
    unsigned char   YR;
    unsigned char   SP;
    unsigned char   SR;
    bool            Interrupts[IntQuellenC64];

    unsigned char   irq_state;          // if Größer 0 ist die Leitung low
    bool            irq_is_low_pegel;
    bool            irq_is_active;
    bool            irq_fetch_decision; // IRQ Entscheidung vom Ende des vorigen Befehls (wird beim Opcode Fetch ausgewertet)
    bool            nmi_hijack;         // NMI hat waehrend BRK/IRQ den Vektor auf $FFFA/B umgelenkt
    bool            i_flag_early;       // CLI/SEI hat das I-Flag schon im RDY Wartezyklus geaendert
    bool            branch_irq_early;   // Taken Branch ohne Seitenwechsel: Interrupt wird einen Takt frueher entschieden
    bool            branch_irq_sample;  // IRQ Pegel im zweiten Zyklus eines Branches
    bool            branch_nmi_sample;  // NMI vorhanden im zweiten Zyklus eines Branches

    bool            irq_delay;
    uint8_t         irq_delay_sr_value;

    bool            nmi_state;
    bool            nmi_edge_event;     // Die NMI Leitung wurde angehoben (die Flanke wird erst beim naechsten Takt ausgewertet)
    bool            nmi_fall_edge;
    bool            nmi_is_active;
    bool            nmi_fetch_decision; // NMI Entscheidung vom Ende des vorigen Befehls (wird beim Opcode Fetch ausgewertet)

    bool             shxy_dma;
    uint8_t			axa_byte;
};

#define SetAdresseLo(wert) Adresse = ((Adresse&0xFF00)|wert)
#define SetAdresseHi(wert) Adresse = ((Adresse&0x00FF)|(wert<<8))

#define SetPCLo(wert) PC=((PC&0xFF00)|wert)
#define SetPCHi(wert) PC=((PC&0x00FF)|(wert<<8))

#define GetPCLo ((unsigned char)PC)
#define GetPCHi ((unsigned char)(PC>>8))

class MOS6510_PORT
{
public:

    /// Funktionen ///

    MOS6510_PORT(void);
    ~MOS6510_PORT(void);
    void Reset(void);
    void ConfigChanged(int tape_sense, int caps_sense,unsigned char pullup);
    //bool SaveFreez(FILE               //IRQCounter = 0;* File);
    //bool LoadFreez(FILE* File,unsigned short Version);

    /// Variablen ///

    // Wert zum Prozessor Port schreiben
    unsigned char DIR;
    unsigned char DATA;

    // Wert vom Prozessor Port lesen
    unsigned char DIR_READ;
    unsigned char DATA_READ;

    // Status der Prozessor Port Pins
    unsigned char DATA_OUT;

    /* cycle that should invalidate the unused bits of the data port. */
    unsigned long DATA_SET_CLK_BIT6;
    unsigned long DATA_SET_CLK_BIT7;

    /* indicates if the unused bits of the data port are still
    valid or should be read as 0, 1 = unused bits valid,
    0 = unused bits should be 0 */
    unsigned char DATA_SET_BIT6;
    unsigned char DATA_SET_BIT7;

    /* indicated if the unused bits are in the process of falling off. */
    unsigned char DATA_FALLOFF_BIT6;
    unsigned char DATA_FALLOFF_BIT7;

    bool TAPE_SENSE;
    bool DATASETTE_MOTOR;
};
#endif // MOS_6510_CLASS_H
