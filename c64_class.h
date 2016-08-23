//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: c64_class.h                           //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 23.08.2016                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef C64CLASS_H
#define C64CLASS_H

#include "videopal_class.h"
#include "mmu_class.h"
#include "mos6510_class.h"
#include "mos6569_class.h"
#include "mos6581_8085_class.h"
#include "mos6526_class.h"
#include "crt_class.h"
#include "reu_class.h"
#include "georam_class.h"
#include "floppy1541_class.h"
#include "cpu_info.h"
#include "vcd_class.h"

#include <GL/glu.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>

#include "tr1/functional"
using namespace std::tr1;
using namespace std::tr1::placeholders;

#define MAX_C64_SCREEN_TITLE_LENGTH 100

#define FloppyAnzahl 4
#define MAX_BREAK_GROUPS 256
#define MAX_JOYSTICKS 16
#define MAX_VJOYS 16

#define SUBDIVS_SCREEN 20            // Für Screenverzerrungen (Kissen etc.)

#define SCREEN_RATIO_4_3 1.34        // Screenratio 4:3 (1,33333)
#define SCREEN_RATIO_5_4 1.25        // Screenratio 5:4 (1,25)
#define SCREEN_RATIO_16_9 1.777      // Screenratio 16:9 (1,777)

class C64Class
{

public:
    C64Class(int *ret_error,VideoPalClass *_pal, function<void(char*)> log_function, const char* gfx_path);
    ~C64Class();
    void StartEmulation(void);
    void EndEmulation(void);
    void FillAudioBuffer(unsigned char *stream, int laenge); // Über diese Funktion wird der C64 Takt erzeugt !! //
    void KeyEvent(unsigned char  matrix_code,KeyStatus status, bool isAutoShift);
    bool LoadC64Roms(char *kernalrom,char *basicrom,char *charrom);
    bool LoadFloppyRom(int floppy_nr,char *dos1541rom);
    bool LoadDiskImage(int floppy_nr,char *filename);
    void LoadPRGFromD64(int floppy_nr,char *c64_filename,int command);
    void SetFloppyWriteProtect(int floppy_nr, bool status);
    void SetCommandLine(char *c64_command);
    void KillCommandLine(void);
    unsigned char ReadC64Byte(unsigned short adresse);
    void WriteC64Byte(unsigned short adresse,unsigned char wert);
    unsigned char* GetRAMPointer(unsigned short adresse);
    void SetGrafikModi(bool colbits32, bool doublesize,bool enable_pal,bool filter_enable, int fullres_xw = 0, int fullres_yw = 0);
    void SetWindowTitle(char *title_name);
    void SetFullscreen(void);
    void InitGrafik(void);
    void ReleaseGrafik(void);
    void DrawC64Screen(void);
    void SetFocusToC64Window(void);
    void SetWindowAspectRatio(bool enabled);
    void SetFullscreenAspectRatio(bool enabled);
    void AnalyzeSDLEvent(SDL_Event *event);
    void SetC64Speed(int speed);
    void SetDistortion(float value);
    void SetMouseHiddenTime(int time);  // Time in ms // Bei 0 Wird der Cursor nicht mehr ausgeblendet
    void GetWindowPos(int *x, int *y);
    void SetWindowPos(int x, int y);
    void GetWindowSize(int *x, int *y);
    void SetWindowSize(int w, int h);

    void SoftReset(void);
    void HardReset(void);
    void SetReset(int status, int hard_reset);
    int LoadAutoRun(int floppy_nr, char *filename);
    int LoadPRG(char *filename, unsigned short* ret_startadresse);
    int LoadCRT(char *filename);
    void RemoveCRT(void);

    void InsertREU(void);
    void RemoveREU(void);
    int LoadREUImage(char* filename);
    int SaveREUImage(char* filename);
    void ClearREURam(void);

    void InsertGEORAM(void);
    void RemoveGEORAM(void);
    int LoadGEORAMImage(char* filename);
    int SaveGEORAMImage(char* filename);
    void ClearGEORAMRam(void);

    void SetMouse1351Port(unsigned char port);

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
    void GetIECStatus(IEC_STRUCT *iec);

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
    void JoystickNewScan(void);
    void StartRecJoystickMapping(int slot_nr);
    void StopRecJoystickMapping(void);
    void ClearJoystickMapping(int slot_nr);
    void IncMouseHiddenCounter(void);

    unsigned char GetMapReadSource(unsigned char page);
    unsigned char GetMapWriteDestination(unsigned char page);

    void SaveScreenshot(const char *filename);

    bool StartIECDump(const char *filename);
    void StopIECDump();

    void SetSIDVolume(float volume);    // Lautstärke der SID's (0.0f - 1.0f)

    int             AktWindowXW;
    int             AktWindowYW;
    int             AktWindowColorBits;
    unsigned short  AktC64ScreenXW;
    unsigned short  AktC64ScreenYW;
    bool            ColBits32;
    bool            DoubleSize;
    bool            PalEnable;
    bool            FilterEnable;
    int             FullResXW;
    int             FullResYW;
    bool            isFullscreen;
    bool            ChangeGrafikModi;
    bool            ChangeWindowPos;
    bool            ChangeWindowSize;
    bool            HoldVicRefresh;
    bool            VicRefreshIsHold;

    float           C64ScreenAspectRatio;
    bool            EnableWindowAspectRatio;
    bool            EnableFullscreenAspectRatio;

    SDL_Window      *C64Window;
    SDL_GLContext   GLContext;

    int             win_pos_x;
    int             win_pos_y;
    int             win_size_w;
    int             win_size_h;

    SDL_AudioSpec want,have;

    SDL_Surface     *C64Screen;
    SDL_Surface     *C64ScreenIcon;
    GLuint          C64ScreenTexture;
    unsigned char   *C64ScreenBuffer;
    bool            IsC64ScreenObsolete;
    bool            DistortionEnable;
    float           Distortion;

    /// Distortion (Verzzerung) ///
    POINT_STRUCT    DistortionGridPoints[(SUBDIVS_SCREEN+1)*(SUBDIVS_SCREEN+1)];
    POINT_STRUCT    DistortionGrid[(SUBDIVS_SCREEN)*(SUBDIVS_SCREEN)*4];
    POINT_STRUCT    DistortionGridTex[(SUBDIVS_SCREEN)*(SUBDIVS_SCREEN)*4];

    int				FrameSkip;
    int				FrameSkipCounter;

    SDL_Surface     *Pfeil0;
    SDL_Surface     *Pfeil1;
    SDL_Surface     *Kreis0;
    SDL_Surface     *Kreis1;

    GLuint          Pfeil0Texture;
    GLuint          Pfeil1Texture;
    GLuint          Kreis0Texture;
    GLuint          Kreis1Texture;

    bool            RecJoyMapping;
    int             RecJoyMappingPos;          // 0-4 // Hoch - Runter - Links - Rechts - Feuer
    int             RecJoySlotNr;              // 0 - (MAX_VJOYS-1)
    int             RecPollingWait;
    int             RecPollingWaitCounter;

    VIRTUAL_JOY_STRUCT  VJoys[MAX_VJOYS];
    int                 VPort1;
    int                 VPort2;

    unsigned char   GamePort1;
    unsigned char   GamePort2;

    SDL_Thread      *sdl_thread;
    bool            sdl_thread_pause;
    bool            sdl_thread_is_paused;

    unsigned char   *vic_puffer;
    VideoPalClass   *pal;

    MMU             *mmu;
    MOS6510         *cpu;
    VICII           *vic;
    MOS6581_8085    *sid1;
    MOS6581_8085    *sid2;
    MOS6526         *cia1;
    MOS6526         *cia2;
    CRTClass        *crt;
    REUClass        *reu;
    GEORAMClass     *geo;
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
    bool            LoopThreadIsEnd;

    function<void(void)> AnimationRefreshProc;
    function<void(void)> BreakpointProc;
    function<void(char*)> LogText;

    unsigned short	C64History[256];
    unsigned char	C64HistoryPointer;

    bool StartScreenshot;
    char ScreenshotFilename[1024];

    float SIDVolume;

private:
    void CalcDistortionGrid();
    void VicRefresh(unsigned char *vic_puffer);
    void CheckKeys(void);
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
    void OpenSDLJoystick(void);
    void CloseSDLJoystick(void);
    void ChangePOTSwitch(void);
    void UpdateMouse(void);
    int InitVideoCaptureSystem(void);
    void CloseVideoCaptureSystem(void);

    function<unsigned char(unsigned short)> *ReadProcTbl;
    function<void(unsigned short,unsigned char)> *WriteProcTbl;

    const char* GfxPath;
    const char* RomPath;
    const char* FloppySoundPath;

    char window_title[MAX_C64_SCREEN_TITLE_LENGTH];

    bool SDLJoystickIsOpen;
    int  JoystickAnzahl;
    SDL_Joystick *Joystick[MAX_JOYSTICKS];
    const char *JoystickNamen[MAX_JOYSTICKS];
    bool StopJoystickUpdate;
    bool JoyStickUdateIsStop;

    bool RDY_BA;            // Leitung CPU <-- VIC
    bool HRESET;            // Zusatz Anzeige für MMU Reset
    bool GAME;              // Leitung Expansionsport --> MMU;
    bool EXROM;             // Leitung Expansionsport --> MMU;
    bool RAM_H;             // Leitung Expansionsport --> MMU;
    bool RAM_L;             // Leitung Expansionsport --> MMU;
    MOS6510_PORT CPU_PORT;  // Prozessor Port

    bool EnableExtLines;
    bool ExtRDY;

    unsigned char   C64IEC;     // Leitungen vom C64 zur Floppy Bit 4=ATN 6=CLK 7=DATA
    unsigned char   FloppyIEC;	// Leitungen von Floppy zur c64 Bit 6=CLK 7=DATA

    VCDClass IecVcdExport;      // Klasse zum Exportieren der IEC Signale
    bool IecIsDumped;

    /// Temporär ///
    int             EasyFlashDirty;
    unsigned char   EasyFlashByte;

    PORT CIA1_PA;
    PORT CIA1_PB;
    PORT CIA2_PA;
    PORT CIA2_PB;

    bool Mouse1351Enable;
    int MouseXRel;
    int MouseYRel;

    int MousePort;
    unsigned char POT_AX;
    unsigned char POT_AY;
    unsigned char POT_BX;
    unsigned char POT_BY;

    unsigned char POT_X;
    unsigned char POT_Y;

    unsigned char   KeyboardMatrixToPB[8];
    unsigned char   KeyboardMatrixToPA[8];

    bool ReuIsInsert;

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

    unsigned short   BreakGroupAnz;
    BREAK_GROUP     *BreakGroup[MAX_BREAK_GROUPS];

    ////////////////////////////////////////////////////////////

    bool C64ResetReady;
    bool FloppyResetReady[FloppyAnzahl];

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

    bool    MouseIsHidden;
    int     MouseHiddenCounter;
    int     MouseHiddenTime;
};

#endif // C64CLASS_H
