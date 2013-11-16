//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: c64_class.h                           //
// ANDROID PORT                                 //
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
#include "../../../videopal_class.h"

#include "tr1/functional"
using namespace std::tr1;
using namespace std::tr1::placeholders;

#define FloppyAnzahl 1
#define MAX_BREAK_GROUPS 256
#define MAX_JOYSTICKS 16
#define MAX_VJOYS 16

class C64Class {
public:
	C64Class(int SoundBufferSize,VideoPalClass *pal);
	virtual ~C64Class();
	void FillAudioBuffer(short *stream, int laenge); // Über diese Funktion wird der C64 Takt erzeugt !! //
    bool LoadC64Roms(char *kernalrom,char *basicrom,char *charrom);
    bool LoadFloppyRom(int floppy_nr,char *dos1541rom);
    bool LoadDiskImage(int floppy_nr,char *filename);
    void SetCommandLine(char *c64_command);
    void KillCommandLine(void);
    unsigned char ReadC64Byte(unsigned short adresse);
    void WriteC64Byte(unsigned short adresse,unsigned char wert);

    void SoftReset(void);
    void HardReset(void);
    void SetReset(int status, int hard_reset);
    int LoadAutoRun(int floppy_nr, char *filename);
    int LoadPRG(char *filename, unsigned short* ret_startadresse);
    int LoadCRT(char *filename);
    void RemoveCRT(void);

	void GetC64CpuReg(REG_STRUCT *reg,IREG_STRUCT *ireg);

    const char* sd_ext_path;

    int				VPort1;
    int             VPort2;

    unsigned char   GamePort1;
    unsigned char   GamePort2;

    int             AktWindowColorBits;
    int             AktC64ScreenXW;
    int             AktC64ScreenYW;

    unsigned char   *vic_puffer;
    VideoPalClass   *pal;
    unsigned char   *C64ScreenBuffer;
    unsigned char 	*C64ScreenBackBuffer;

    MMU             *mmu;
    MOS6510         *cpu;
    VICII           *vic;
    MOS6581_8085    *sid1;
    MOS6526         *cia1;
    MOS6526         *cia2;
    CRTClass        *crt;
    Floppy1541      *floppy[FloppyAnzahl];

    bool RESET;     // Reset Leitung -> Für Alle Module mit Reset Eingang

    bool            StereoEnable;
    unsigned short  Sid2Adresse;

    unsigned char   KeyboardMatrixToPBExt[8];
    unsigned char   KeyboardMatrixToPAExt[8];

    unsigned short	C64History[256];
    unsigned char	C64HistoryPointer;

    int             IOSource;

    bool            WaitResetReady;
    int             AutoLoadMode;
    char            AutoLoadCommandLine[1024];
    char            AutoLoadFilename[1024];

    function<void(void)> AnimationRefreshProc;
    function<void(void)> BreakpointProc;

private:
    void VicRefresh(unsigned char *vic_puffer);
    void WriteSidIO(unsigned short adresse,unsigned char wert);
    unsigned char ReadSidIO(unsigned short adresse);
    void WriteIO1(unsigned short adresse,unsigned char wert);
    void WriteIO2(unsigned short adresse,unsigned char wert);
    unsigned char ReadIO1(unsigned short adresse);
    unsigned char ReadIO2(unsigned short adresse);

    function<unsigned char(unsigned short)> *ReadProcTbl;
    function<void(unsigned short,unsigned char)> *WriteProcTbl;

    bool RDY_BA;            // Leitung CPU <-- VIC
    bool HRESET;            // Zusatz Anzeige für MMU Reset
    bool GAME;              // Leitung Expansionsport --> MMU;
    bool EXROM;             // Leitung Expansionsport --> MMU;
    bool RAM_H;             // Leitung Expansionsport --> MMU;
    bool RAM_L;             // Leitung Expansionsport --> MMU;
    MOS6510_PORT CPU_PORT;  // Prozessor Port

    bool EnableExtLines;
    bool ExtRDY;

    unsigned char   C64IEC;     // Leitungen vom C64 zur Floppy Bit 0=ATN 1=CLK 2=DATA
    unsigned char   FloppyIEC;	// Leitungen von Floppy zur c64 Bit 0=ATN 1=CLK 2=DATA

    /// Temporär ///
    int             EasyFlashDirty;
    unsigned char   EasyFlashByte;

    PORT CIA1_PA;
    PORT CIA1_PB;
    PORT CIA2_PA;
    PORT CIA2_PB;

    unsigned char   KeyboardMatrixToPB[8];
    unsigned char   KeyboardMatrixToPA[8];

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

    unsigned short  Breakpoints[0x10000];
    unsigned short  BreakWerte[16];
    unsigned short  BreakStatus;
    bool            FloppyFoundBreakpoint;

    ////////////////////////////////////////////////////////////

    bool 	C64ResetReady;
    bool 	FloppyResetReady[FloppyAnzahl];

    char	ComandZeile[256];
    int		ComandZeileSize;
    int		ComandZeileCount;
    bool	ComandZeileStatus;
    bool	ComandZeileCountS;

    int     CycleCounter;
    bool    DebugMode;
    bool	DebugAnimation;
    double  AnimationSpeedAdd;
    double	AnimationSpeedCounter;
    bool	OneZyk;
    bool	OneOpc;
    int     OneOpcSource;
};

#endif /* C64_CLASS_H_ */