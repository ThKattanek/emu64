//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: c64_class.h                           //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 27.07.2012		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#ifndef C64CLASS_H
#define C64CLASS_H

#include "version.h"
#include "videopal_class.h"
#include "mmu_class.h"
#include "mos6510_class.h"
#include "mos6569_class.h"
#include "mos6581_8085_class.h"
#include "mos6526_class.h"
#include "crt_class.h"
#include "floppy1541_class.h"
#include "cpu_info.h"

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_framerate.h>

#include "tr1/functional"
using namespace std::tr1;
using namespace std::tr1::placeholders;

#define FloppyAnzahl 4
#define MAX_BREAK_GROUPS 255

class C64Class
{

public:
    C64Class(int *ret_error,VideoPalClass *_pal,bool OpenGLOn, function<void(char*)> log_function);
    ~C64Class();
    void FillAudioBuffer(unsigned char *stream, int laenge); // Über diese Funktion wird der C64 Takt erzeugt !! //
    void KeyEvent(unsigned char  matrix_code,KeyStatus status, bool isAutoShift);
    bool LoadC64Roms(char *kernalrom,char *basicrom,char *charrom);
    bool LoadFloppyRom(int floppy_nr,char *dos1541rom);
    bool LoadDiskImage(int floppy_nr,char *filename);
    void SetCommandLine(char *c64_command);
    void KillCommandLine(void);
    unsigned char ReadC64Byte(unsigned short adresse);
    void WriteC64Byte(unsigned short adresse,unsigned char wert);
    unsigned char* GetRAMPointer(unsigned short adresse);
    void SetGrafikModi(bool colbits32, bool doublesize,bool enable_pal, int fullres_xw = 0, int fullres_yw = 0);

    void SoftReset(void);
    void HardReset(void);
    void SetReset(int status, int hard_reset);
    int LoadAutoRun(int floppy_nr, char *filename);
    int LoadPRG(char *filename, unsigned short* ret_startadresse);
    int LoadCRT(char *filename);
    void RemoveCRT(void);

    void ResetC64CycleCounter(void);
    void SetDebugMode(bool status);
    void SetCpuExtLines(bool status);
    void SetExtRDY(bool status);
    void OneZyklus(void);
    void OneOpcode(int source);
    void SetDebugAnimation(bool status);
    void SetDebugAnimationSpeed(int cycle_sek);
    void GetC64CpuReg(REG_STRUCT *reg,IREG_STRUCT *ireg);
    void GetVicReg(VIC_STRUCT *vic_reg);

    int AddBreakGroup(void);
    void DelBreakGroup(int index);
    BREAK_GROUP* GetBreakGroup(int index);
    void UpdateBreakGroup(void);
    void DeleteAllBreakGroups(void);
    int GetBreakGroupAnz(void);
    int LoadBreakGroups(char *filename);
    bool SaveBreakGroups(char *filename);
    bool ExportPRG(char *filename, unsigned short start_adresse, unsigned short end_adresse, int source);
    bool ExportRAW(char *filename, unsigned short start_adresse, unsigned short end_adresse, int source);
    bool ExportASM(char* filename, unsigned short start_adresse, unsigned short end_adresse, int source);

    unsigned char GetMapReadSource(unsigned char page);
    unsigned char GetMapWriteDestination(unsigned char page);

    int             AktWindowXW;
    int             AktWindowYW;
    int             AktWindowColorBits;
    int             AktC64ScreenXW;
    int             AktC64ScreenYW;
    bool            ColBits32;
    bool            DoubleSize;
    bool            PalEnable;
    int             FullResXW;
    int             FullResYW;
    bool            isFullscreen;
    bool            ChangeGrafikModi;
    bool            HoldVicRefresh;
    bool            VicRefreshIsHold;

    FPSmanager      fps_manager;
    SDL_Surface     *C64Screen;
    SDL_Surface     *C64ScreenBack;
    SDL_Surface     *C64ScreenIcon;
    unsigned char   *C64ScreenBuffer;
    bool            DrawScreenBack;
    bool            OpenGLEnable;

    SDL_Thread      *sdl_thread;
    bool            sdl_thread_pause;
    bool            sdl_thread_is_paused;

    VideoPalClass   *pal;

    MMU             *mmu;
    MOS6510         *cpu;
    VICII           *vic;
    MOS6581_8085    *sid1;
    MOS6581_8085    *sid2;
    MOS6526         *cia1;
    MOS6526         *cia2;
    CRTClass        *crt;
    Floppy1541      *floppy[FloppyAnzahl];

    bool RESET;     // Reset Leitung -> Für Alle Module mit Reset Eingang

    bool            StereoEnable;
    unsigned short  Sid2Adresse;

    unsigned char   KeyboardMatrixToPBExt[8];
    unsigned char   KeyboardMatrixToPAExt[8];

    int             IOSource;

    bool            WaitResetReady;
    int             AutoLoadMode;
    char            AutoLoadCommandLine[1024];
    char            AutoLoadFilename[1024];

    bool            LoopThreadEnd;

    function<void(void)> AnimationRefreshProc;
    function<void(void)> BreakpointProc;
    function<void(char*)> LogText;

    unsigned short	C64History[256];
    unsigned char	C64HistoryPointer;

private:
    void VicRefresh(unsigned char *vic_puffer);
    void CheckKeys(void);
    void UpdateJoyPorts(void);
    int DisAss(FILE *file, int pc, bool line_draw, int source);
    bool CheckBreakpoints(void);
    void WriteSidIO(unsigned short adresse,unsigned char wert);
    unsigned char ReadSidIO(unsigned short adresse);
    void WriteIO1(unsigned short adresse,unsigned char wert);
    void WriteIO2(unsigned short adresse,unsigned char wert);
    unsigned char ReadIO1(unsigned short adresse);
    unsigned char ReadIO2(unsigned short adresse);
    void SDLThreadPauseBegin(void);
    void SDLThreadPauseEnd(void);

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

    unsigned char   GamePort1;
    unsigned char   GamePort2;

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

    unsigned char   BreakGroupAnz;
    BREAK_GROUP     *BreakGroup[MAX_BREAK_GROUPS];

    ////////////////////////////////////////////////////////////

    bool C64ResetReady;
    bool FloppyResetReady[FloppyAnzahl];

    char	ComandZeile[256];
    int		ComandZeileSize;
    int		ComandZeileCount;
    bool	ComandZeileStatus;
    bool	ComandZeileCountS;

    int         CycleCounter;
    bool        DebugMode;
    bool	DebugAnimation;
    double      AnimationSpeedAdd;
    double	AnimationSpeedCounter;
    bool	OneZyk;
    bool	OneOpc;
    int         OneOpcSource;
};

#endif // C64CLASS_H
