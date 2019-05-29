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
// Letzte Änderung am 29.05.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef C64CLASS_H
#define C64CLASS_H

#include "./videopal_class.h"
#include "./mmu_class.h"
#include "./mos6510_class.h"
#include "./mos6569_class.h"
#include "./mos6581_8085_class.h"
#include "./mos6526_class.h"
#include "./crt_class.h"
#include "./reu_class.h"
#include "./georam_class.h"
#include "./floppy1541_class.h"
#include "./tape1530_class.h"
#include "./cpu_info.h"
#include "./vcd_class.h"
#include "./savepng.h"
#include "./video_capture_class.h"

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

#define SCREEN_RATIO_4_3 1.34f        // Screenratio 4:3 (1,33333)
#define SCREEN_RATIO_5_4 1.25f        // Screenratio 5:4 (1,25)
#define SCREEN_RATIO_16_9 1.777f      // Screenratio 16:9 (1,777)

enum SCREENSHOT_FORMATS {SCREENSHOT_FORMAT_BMP, SCREENSHOT_FORMAT_PNG, SCREENSHOT_FORMATS_COUNT};

class C64Class
{

public:
    C64Class(int *ret_error, VideoPalClass *_pal, function<void(char*)> log_function, const char *data_path);
    ~C64Class();
    void StartEmulation(void);
    void EndEmulation(void);
    void SetLimitCycles(int nCycles);
    void SetEnableDebugCart(bool enabled);
    void WarpModeLoop(void);
    void FillAudioBuffer(uint8_t *stream, int laenge); // Über diese Funktion wird der C64 Takt erzeugt !! //
    void KeyEvent(uint8_t  matrix_code, KeyStatus status, bool isAutoShift);
    bool LoadC64Roms(char *kernalrom, char *basicrom, char *charrom);
    bool LoadFloppyRom(uint8_t floppy_nr, char *dos1541rom);
    bool LoadDiskImage(uint8_t floppy_nr, char *filename);
    void LoadPRGFromD64(uint8_t floppy_nr, char *c64_filename, int command);
    void SetFloppyWriteProtect(uint8_t floppy_nr, bool status);
    void SetCommandLine(char *c64_command);
    void KillCommandLine(void);
    uint8_t ReadC64Byte(uint16_t adresse);
    void WriteC64Byte(uint16_t adresse, uint8_t wert);
    uint8_t* GetRAMPointer(uint16_t adresse);
    void SetGrafikModi(bool colbits32, bool doublesize, bool enable_pal, bool filter_enable, int fullres_xw = 0, int fullres_yw = 0);
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
    void EnableWarpMode(bool enabled);
    void SetDistortion(float value);
    void SetMouseHiddenTime(int time);  // Time in ms // Bei 0 Wird der Cursor nicht mehr ausgeblendet
    void GetWindowPos(int *x, int *y);
    void SetWindowPos(int x, int y);
    void GetWindowSize(int *x, int *y);
    void SetWindowSize(int w, int h);

    bool LoadTapeImage(char *filename);
    bool RecordTapeImage(char* filename);
    uint8_t SetTapeKeys(uint8_t pressed_key);
    bool GetTapeMotorStatus(void);
    bool GetTapeRecordLedStatus(void);
    uint32_t GetTapeCounter(void);
    float GetTapeLenTime(void);
    uint32_t GetTapeLenCount(void);
    void SetTapeSoundVolume(float volume);

    void SoftReset(void);
    void HardReset(void);
    void SetReset(int status, int hard_reset);
    int LoadAutoRun(uint8_t floppy_nr, char *filename);
    int LoadPRG(char *filename, uint16_t *ret_startadresse);

    int LoadCRT(char *filename);
    void RemoveCRT(void);
    int CreateNewEasyFlashImage(char *filename, char *crt_name);

    void InsertREU(void);
    void RemoveREU(void);
    int LoadREUImage(char *filename);
    int SaveREUImage(char *filename);
    void ClearREURam(void);

    void InsertGEORAM(void);
    void RemoveGEORAM(void);
    int LoadGEORAMImage(char *filename);
    int SaveGEORAMImage(char *filename);
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
    bool ExportPRG(char *filename, uint16_t start_adresse, uint16_t end_adresse, int source);
    bool ExportRAW(char *filename, uint16_t start_adresse, uint16_t end_adresse, int source);
    bool ExportASM(char* filename, uint16_t start_adresse, uint16_t end_adresse, int source);
    void JoystickNewScan(void);
    void StartRecJoystickMapping(int slot_nr);
    void StopRecJoystickMapping(void);
    void ClearJoystickMapping(int slot_nr);
    void IncMouseHiddenCounter(void);

    void StartRecKeyMap(uint8_t keymatrix_code);
    void StopRecKeyMap(void);
    bool GetRecKeyMapStatus(void);
    C64_KEYS* GetC64KeyTable(void);
    const char** GetC64KeyNameTable(void);
    int GetC64KeyTableSize(void);

    uint8_t GetMapReadSource(uint8_t page);
    uint8_t GetMapWriteDestination(uint8_t page);

    void SaveScreenshot(const char *filename, int format = SCREENSHOT_FORMAT_PNG);
    const char* GetScreenshotFormatName(int format);
    void SetExitScreenshot(const char *filename);

    const char* GetAVVersion(void);
    bool StartVideoRecord(const char *filename, int audio_bitrate=128000, int video_bitrate=4000000);
    void SetPauseVideoRecord(bool status);
    void StopVideoRecord(void);
    int GetRecordedFrameCount(void);

    bool StartIECDump(const char *filename);
    void StopIECDump(void);

    void SetSIDVolume(float volume);    // Lautstärke der SID's (0.0f - 1.0f)
    void SetFirstSidTyp(int sid_typ);   // SID Typ des 1. SID (MOS_6581 oder MOS_8580)
    void SetSecondSidTyp(int sid_typ);  // SID Typ des 2. SID (MOS_6581 oder MOS_8580)
    void EnableSecondSid(bool enable);  // 2. SID aktivieren
    void SetSecondSidAddress(uint16_t address);
    void SetSid6ChannelMode(bool enable);
    void SetSidCycleExact(bool enable);
    void SetSidFilter(bool enable);

    bool StartSidDump(const char *filename);
    void StopSidDump(void);
    int GetSidDumpFrames(void);

    void SetVicConfig(int var, bool enable);    // var = VIC_BORDER_ON, VIC_SPRITES_ON, VIC_SPR_SPR_COLL_ON, VIC_SPR_BCK_COLL_ON
    bool GetVicConfig(int var);

    void SetVicDisplaySizePal(int first_line, int last_line);
    void SetVicDisplaySizeNtsc(int first_line, int last_line);

    int GetVicFirstDisplayLinePal(void);
    int GetVicLastDisplayLinePal(void);
    int GetVicFirstDisplayLineNtsc(void);
    int GetVicLastDisplayLineNtsc(void);

    int             AktWindowXW;
    int             AktWindowYW;
    int             AktWindowColorBits;
    uint16_t        AktC64ScreenXW;
    uint16_t        AktC64ScreenYW;
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

    SDL_mutex       *mutex1;  // Dient für das füllen des Soundbuffers

    int             win_pos_x;
    int             win_pos_y;
    int             win_size_w;
    int             win_size_h;

    SDL_AudioSpec want,have;

    SDL_Surface     *C64Screen;
    SDL_Surface     *C64ScreenIcon;
    GLuint          C64ScreenTexture;
    uint8_t         *C64ScreenBuffer;
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

    uint8_t         GamePort1;
    uint8_t         GamePort2;

    SDL_Thread      *sdl_thread;
    bool            sdl_thread_pause;
    bool            sdl_thread_is_paused;

    SDL_Thread      *warp_thread;
    bool            warp_thread_end;

    uint8_t         *vic_puffer;
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
    TAPE1530        *tape;

    bool RESET;     // Reset Leitung -> Für Alle Module mit Reset Eingang

    bool            StereoEnable;
    uint16_t        Sid2Adresse;
    bool            Sid6ChannelMode;

    uint8_t         KeyboardMatrixToPBExt[8];
    uint8_t         KeyboardMatrixToPAExt[8];

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
    function<void(void)> CloseEventC64Screen;
    function<void(void)> LimitCyclesEvent;
    function<void(unsigned char)> DebugCartEvent;

    uint16_t        C64History[256];
    uint8_t         C64HistoryPointer;

    bool StartScreenshot;
    char ScreenshotFilename[1024];
    int  ScreenshotFormat;

    bool ExitScreenshotEnable;
    char ExitScreenshotFilename[1024];

    float SIDVolume;

    VideoCaptureClass *VideoCapture;

private:
    void CalcDistortionGrid(void);
    void VicRefresh(uint8_t *vic_puffer);
    void CheckKeys(void);
    int DisAss(FILE *file, int pc, bool line_draw, int source);
    bool CheckBreakpoints(void);
    void WriteSidIO(uint16_t adresse, uint8_t wert);
    uint8_t ReadSidIO(uint16_t adresse);
    void WriteIO1(uint16_t adresse, uint8_t wert);
    void WriteIO2(uint16_t adresse, uint8_t wert);
    uint8_t ReadIO1(uint16_t adresse);
    uint8_t ReadIO2(uint16_t adresse);
    void SDLThreadPauseBegin(void);
    void SDLThreadPauseEnd(void);
    void OpenSDLJoystick(void);
    void CloseSDLJoystick(void);
    void ChangePOTSwitch(void);
    void UpdateMouse(void);
    int InitVideoCaptureSystem(void);
    void CloseVideoCaptureSystem(void);
    void SwapRBSurface(SDL_Surface *surface); // swaps the color red with blue in sdl surface
    function<uint8_t(uint16_t)> *ReadProcTbl;
    function<void(uint16_t, uint8_t)> *WriteProcTbl;

    char* GfxPath;
    char* FloppySoundPath;
    char* RomPath;

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

    uint8_t         C64IEC;     // Leitungen vom C64 zur Floppy Bit 4=ATN 6=CLK 7=DATA
    uint8_t         FloppyIEC;	// Leitungen von Floppy zur c64 Bit 6=CLK 7=DATA

    VCDClass IecVcdExport;      // Klasse zum Exportieren der IEC Signale
    bool IecIsDumped;

    /// Temporär ///
    int             EasyFlashDirty;
    uint8_t         EasyFlashByte;

    PORT CIA1_PA;
    PORT CIA1_PB;
    PORT CIA2_PA;
    PORT CIA2_PB;

    bool Mouse1351Enable;
    int MouseXRel;
    int MouseYRel;

    int MousePort;
    uint8_t     POT_AX;
    uint8_t     POT_AY;
    uint8_t     POT_BX;
    uint8_t     POT_BY;

    uint8_t     POT_X;
    uint8_t     POT_Y;

    uint8_t     KeyboardMatrixToPB[8];
    uint8_t     KeyboardMatrixToPA[8];

    bool isReturnKeyDown;

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

    uint16_t        Breakpoints[0x10000];
    uint16_t        BreakWerte[16];
    uint16_t        BreakStatus;
    bool            FloppyFoundBreakpoint;

    uint8_t         BreakGroupAnz;
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
    int     LimitCylesCounter;      // Dieser Counter wird wenn er > 0 ist bei jeden Zyklus um 1 runtergezählt
                                    // Wechselt er von 1 auf 0 wird die Emulation angehalten und ein Ergeignis ausgelöst
    bool    DebugMode;
    bool	DebugAnimation;
    float_t AnimationSpeedAdd;
    float_t	AnimationSpeedCounter;
    bool	OneZyk;
    bool	OneOpc;
    int     OneOpcSource;

    bool    WarpMode;

    bool    MouseIsHidden;
    int     MouseHiddenCounter;
    int     MouseHiddenTime;

    bool    IsKeyMapRec;
    uint8_t RecMatrixCode;
};

#endif // C64CLASS_H
