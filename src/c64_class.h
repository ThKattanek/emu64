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
// Letzte Änderung am 22.08.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef C64CLASS_H
#define C64CLASS_H

#include "./video_crt_class.h"
#include "./mmu_class.h"
#include "./mos6510_class.h"
#include "./mos6569_class.h"
#include "./mos6581_8085_class.h"
#include "./mos6526_class.h"
#include "./cartridge_class.h"
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
#include <functional>

#define MAX_STRING_LENGTH 1024

#define MAX_FLOPPY_NUM 4
#define MAX_BREAK_GROUP_NUM 255
#define MAX_SDL_JOYSTICK_NUM 16
#define MAX_VJOY_NUM 16

#define SUBDIVS_SCREEN 20             // Für Screenverzerrungen (unterteilung des Bildschirms)

#define SCREEN_RATIO_4_3 1.34f        // Screenratio 4:3 (1,33333)
#define SCREEN_RATIO_5_4 1.25f        // Screenratio 5:4 (1,25)
#define SCREEN_RATIO_16_9 1.777f      // Screenratio 16:9 (1,777)

enum SCREENSHOT_FORMATS {SCREENSHOT_FORMAT_BMP, SCREENSHOT_FORMAT_PNG, SCREENSHOT_FORMATS_COUNT};

class C64Class
{

public:
    C64Class(int *ret_error, VideoCrtClass *video_crt_output, std::function<void(char*)> log_function, const char *data_path);
    ~C64Class();
    void StartEmulation();
    void EndEmulation();
    void SetLimitCycles(int nCycles);
    void SetEnableDebugCart(bool enable);
    void WarpModeLoop();
    void FillAudioBuffer(uint8_t *stream, int laenge); // Über diese Funktion wird der C64 Takt erzeugt !! //
    void KeyEvent(uint8_t  matrix_code, KeyStatus status, bool isAutoShift);
    bool LoadC64Roms(char *kernalrom, char *basicrom, char *charrom);
    bool LoadFloppyRom(uint8_t floppy_nr, char *dos1541rom);
    bool LoadDiskImage(uint8_t floppy_nr, char *filename);
    void LoadPRGFromD64(uint8_t floppy_nr, char *c64_filename, int command);
    void SetFloppyWriteProtect(uint8_t floppy_nr, bool status);
    void SetCommandLine(char *c64_command);
    void KillCommandLine();
    uint8_t ReadC64Byte(uint16_t address);
    void WriteC64Byte(uint16_t address, uint8_t value);
    uint8_t* GetRAMPointer(uint16_t address);
    void SetGrafikModi(bool enable_32bit_colors, bool enable_screen_doublesize, bool enable_screen_crt_output, bool filter_enable, uint16_t fullscreen_width = 0, uint16_t fullscreen_height = 0);
    void SetSDLWindowName(char *name);
    void SetFullscreen(bool is_fullscreen);
    void ToggleScreenMode();
    void InitGrafik();
    void ReleaseGrafik();
    void DrawC64Screen();
    void SetFocusToC64Window();
    void SetWindowAspectRatio(bool enable);
    void SetFullscreenAspectRatio(bool enable);
    void AnalyzeSDLEvent(SDL_Event *event);
    void SetC64Speed(int speed);
    void EnableWarpMode(bool enabled);
    void SetDistortion(float_t value);
    void SetMouseHiddenTime(int time);  // Time in ms // Bei 0 Wird der Cursor nicht mehr ausgeblendet
    void GetWindowPos(int *x, int *y);
    void SetWindowPos(int x, int y);
    void GetWindowSize(int *x, int *y);
    void SetWindowSize(int w, int h);

    bool LoadTapeImage(char *filename);
    bool RecordTapeImage(char* filename);
    uint8_t SetTapeKeys(uint8_t pressed_key);
    bool GetTapeMotorStatus();
    bool GetTapeRecordLedStatus();
    uint32_t GetTapeCounter();
    float_t GetTapeLenTime();
    uint32_t GetTapeLenCount();
    void SetTapeSoundVolume(float_t volume);

    void SoftReset();
    void HardReset();
    void SetReset(int status, int hard_reset);
    int LoadAutoRun(uint8_t floppy_nr, char *filename);
    int LoadPRG(char *filename, uint16_t *return_start_address);

    int LoadCRT(char *filename);
    void RemoveCRT();
    int CreateNewEasyFlashImage(char *filename, char *crt_name);

    void InsertREU();
    void RemoveREU();
    int LoadREUImage(char *filename);
    int SaveREUImage(char *filename);
    void ClearREURam();

    void InsertGEORAM();
    void RemoveGEORAM();
    int LoadGEORAMImage(char *filename);
    int SaveGEORAMImage(char *filename);
    void ClearGEORAMRam();

    void SetMouse1351Port(unsigned char port);

    void ResetC64CycleCounter();
    void SetDebugMode(bool status);
    void SetCpuExtLines(bool status);
    void SetExtRDY(bool status);
    void OneCycle();
    void OneOpcode(int source);
    void SetDebugAnimation(bool status);
    void SetDebugAnimationSpeed(int cycle_sek);
    void GetC64CpuReg(REG_STRUCT *reg,IREG_STRUCT *ireg);
    void GetVicReg(VIC_STRUCT *vic_reg);
    void GetIECStatus(IEC_STRUCT *iec);

    int AddBreakGroup();
    void DelBreakGroup(int index);
    BREAK_GROUP* GetBreakGroup(int index);
    void UpdateBreakGroup();
    void DeleteAllBreakGroups();
    int GetBreakGroupCount();
    int LoadBreakGroups(char *filename);
    bool SaveBreakGroups(char *filename);
    bool ExportPRG(char *filename, uint16_t start_adresse, uint16_t end_adresse, int source);
    bool ExportRAW(char *filename, uint16_t start_adresse, uint16_t end_adresse, int source);
    bool ExportASM(char* filename, uint16_t start_adresse, uint16_t end_adresse, int source);
    void JoystickNewScan();
    void StartRecJoystickMapping(int slot_nr);
    void StopRecJoystickMapping();
    void ClearJoystickMapping(int slot_nr);
    void IncMouseHiddenCounter();

    void StartRecKeyMap(uint8_t keymatrix_code);
    void StopRecKeyMap();
    bool GetRecKeyMapStatus();
    C64_KEYS* GetC64KeyTable();
    const char** GetC64KeyNameTable();
    int GetC64KeyTableSize();

    uint8_t GetMapReadSource(uint8_t page);
    uint8_t GetMapWriteDestination(uint8_t page);

    void SaveScreenshot(const char *filename, uint8_t format = SCREENSHOT_FORMAT_PNG);
    const char* GetScreenshotFormatName(uint8_t format);
    void SetExitScreenshot(const char *filename);

    const char* GetAVVersion();
    bool StartVideoRecord(const char *filename, int audio_bitrate=128000, int video_bitrate=4000000);
    void SetPauseVideoRecord(bool status);
    void StopVideoRecord();
    int GetRecordedFrameCount();

    bool StartIECDump(const char *filename);
    void StopIECDump();

    void SetSIDVolume(float_t volume);  // Lautstärke der SID's (0.0f - 1.0f)
    void SetFirstSidTyp(int sid_typ);   // SID Typ des 1. SID (MOS_6581 oder MOS_8580)
    void SetSecondSidTyp(int sid_typ);  // SID Typ des 2. SID (MOS_6581 oder MOS_8580)
    void EnableStereoSid(bool enable);  // 2. SID aktivieren
    void SetStereoSidAddress(uint16_t address);
    void SetStereoSid6ChannelMode(bool enable);
    void SetSidCycleExact(bool enable);
    void SetSidFilter(bool enable);

    bool StartSidDump(const char *filename);
    void StopSidDump();
    int GetSidDumpFrames();

    void SetVicConfig(int var, bool enable);    // var = VIC_BORDER_ON, VIC_SPRITES_ON, VIC_SPR_SPR_COLL_ON, VIC_SPR_BCK_COLL_ON
    bool GetVicConfig(int var);

    void SetVicDisplaySizePal(uint16_t first_line, uint16_t last_line);
    void SetVicDisplaySizeNtsc(uint16_t first_line, uint16_t last_line);

    int GetVicFirstDisplayLinePal();
    int GetVicLastDisplayLinePal();
    int GetVicFirstDisplayLineNtsc();
    int GetVicLastDisplayLineNtsc();

    static const char* screenshot_format_name[SCREENSHOT_FORMATS_COUNT];

    uint16_t        current_window_width;
    uint16_t        current_window_height;
    int             current_window_color_bits;
    uint16_t        current_c64_screen_width;
    uint16_t        current_c64_screen_height;
    bool            enable_screen_32bit_colors;
    bool            enable_screen_doublesize;
    bool            enable_screen_crt_output;
    bool            enable_screen_filter;
    uint16_t        fullscreen_width;
    uint16_t        fullscreen_height;
    bool            enable_fullscreen;
    bool            changed_graphic_modi;
    bool            changed_window_pos;
    bool            changed_window_size;
    bool            enable_hold_vic_refresh;
    bool            vic_refresh_is_holded;

    float_t         screen_aspect_ratio;
    bool            enable_window_aspect_ratio;
    bool            enable_fullscreen_aspect_ratio;

    SDL_Window      *sdl_window;
    SDL_Surface     *sdl_window_icon;
    SDL_GLContext   gl_context;

    int             sdl_window_pos_x;
    int             sdl_window_pos_y;
    int             sdl_window_size_width;
    int             sdl_window_size_height;

    SDL_mutex       *mutex1;  // Dient für das füllen des Soundbuffers

    SDL_AudioSpec   audio_spec_want;
    SDL_AudioSpec   audio_spec_have;

    uint16_t        audio_sample_bit_size;
    uint16_t        audio_channels;
    bool            is_audio_sample_little_endian;
    bool            is_audio_sample_float;
    bool            is_audio_sample_signed;

    int16_t         *audio_16bit_buffer;
    int             audio_16bit_buffer_size;

    SDL_Surface     *c64_screen;
    GLuint          c64_screen_texture;
    uint8_t         *c64_screen_buffer;
    bool            c64_screen_is_obselete;

    bool            enable_distortion;
    float_t         distortion_value;

    /// Distortion (Verzerrung) ///
    POINT_STRUCT    distortion_grid_points[(SUBDIVS_SCREEN+1)*(SUBDIVS_SCREEN+1)];
    POINT_STRUCT    distortion_grid[(SUBDIVS_SCREEN)*(SUBDIVS_SCREEN)*4];
    POINT_STRUCT    distortion_grid_texture_coordinates[(SUBDIVS_SCREEN)*(SUBDIVS_SCREEN)*4];

    int				frame_skip_counter;

    SDL_Surface     *img_joy_arrow0;
    SDL_Surface     *img_joy_arrow1;
    SDL_Surface     *img_joy_button0;
    SDL_Surface     *img_joy_button1;

    GLuint          texture_joy_arrow0;
    GLuint          texture_joy_arrow1;
    GLuint          texture_joy_button0;
    GLuint          texture_joy_button1;

    bool            rec_joy_mapping;
    int             rec_joy_mapping_pos;          // 0-4 // Hoch - Runter - Links - Rechts - Feuer
    int             rec_joy_slot_nr;              // 0 - (MAX_VJOYS-1)
    int             rec_polling_wait;
    int             rec_polling_wait_counter;

    VIRTUAL_JOY_STRUCT  virtual_joys[MAX_VJOY_NUM];
    int                 virtual_port1;
    int                 virtual_port2;

    uint8_t         game_port1;
    uint8_t         game_port2;

    SDL_Thread      *sdl_thread;
    bool            sdl_thread_pause;
    bool            sdl_thread_is_paused;

    SDL_Thread      *warp_thread;
    bool            warp_thread_end;

    uint8_t         *vic_buffer;
    VideoCrtClass   *video_crt_output;

    MMU             *mmu;
    MOS6510         *cpu;
    VICII           *vic;
    MOS6581_8085    *sid1;
    MOS6581_8085    *sid2;
    MOS6526         *cia1;
    MOS6526         *cia2;
    CartridgeClass  *crt;
    REUClass        *reu;
    GEORAMClass     *geo;
    Floppy1541      *floppy[MAX_FLOPPY_NUM];
    TAPE1530        *tape;

    bool            enable_stereo_sid;
    bool            enable_stereo_sid_6channel_mode;
    uint16_t        stereo_sid_address;

    uint8_t         key_matrix_to_port_a_ext[8];
    uint8_t         key_matrix_to_port_b_ext[8];

    uint8_t         io_source;

    bool            wait_reset_ready;
    uint8_t         auto_load_mode;
    char            auto_load_command_line[MAX_STRING_LENGTH];
    char            auto_load_filename[MAX_STRING_LENGTH];

    bool            loop_thread_end;
    bool            loop_thread_is_end;

    std::function<void(void)> AnimationRefreshProc;
    std::function<void(void)> BreakpointProc;
    std::function<void(char*)> LogText;
    std::function<void(void)> CloseEventC64Screen;
    std::function<void(void)> LimitCyclesEvent;
    std::function<void(unsigned char)> DebugCartEvent;

    uint16_t        cpu_pc_history[256];
    uint8_t         cpu_pc_history_pos;

    uint8_t         screenshot_format;

    bool            start_screenshot;
    char            screenshot_filename[MAX_STRING_LENGTH];

    bool            enable_exit_screenshot;
    char            exit_screenshot_filename[MAX_STRING_LENGTH];

    float_t         sid_volume;

    VideoCaptureClass *video_capture;

private:
    void CalcDistortionGrid();
    void VicRefresh(uint8_t *vic_puffer);
    void CheckKeys();
    uint16_t DisAss(FILE *file, uint16_t pc, bool line_draw, int source);
    bool CheckBreakpoints();
    void WriteSidIO(uint16_t address, uint8_t value);
    uint8_t ReadSidIO(uint16_t address);
    void WriteIO1(uint16_t address, uint8_t value);
    void WriteIO2(uint16_t address, uint8_t value);
    uint8_t ReadIO1(uint16_t address);
    uint8_t ReadIO2(uint16_t address);
    void SDLThreadPauseBegin();
    void SDLThreadPauseEnd();
    void OpenSDLJoystick();
    void CloseSDLJoystick();
    void ChangePOTSwitch();
    void UpdateMouse();
    int InitVideoCaptureSystem();
    void CloseVideoCaptureSystem();
    void SwapRBSurface(SDL_Surface *surface); // swaps the color red with blue in sdl surface

    std::function<uint8_t(uint16_t)> *ReadProcTbl;
    std::function<void(uint16_t, uint8_t)> *WriteProcTbl;

    char  gfx_path[MAX_STRING_LENGTH];
    char  floppy_sound_path[MAX_STRING_LENGTH];
    char  rom_path[MAX_STRING_LENGTH];

    char sdl_window_name[MAX_STRING_LENGTH];

    bool sdl_joystick_is_open;
    int  sdl_joystick_count;
    SDL_Joystick *sdl_joystick[MAX_SDL_JOYSTICK_NUM];
    const char *sdl_joystick_name[MAX_SDL_JOYSTICK_NUM];
    bool sdl_joystick_stop_update;
    bool sdl_joystick_update_is_stoped;

    bool reset_wire;        // Reset Leitung -> Für Alle Module mit Reset Eingang
    bool rdy_ba_wire;       // Leitung CPU <-- VIC
    bool game_wire;         // Leitung Expansionsport --> MMU;
    bool exrom_wire;        // Leitung Expansionsport --> MMU;
    bool hi_ram_wire;       // Leitung Expansionsport --> MMU;
    bool lo_ram_wire;       // Leitung Expansionsport --> MMU;

    MOS6510_PORT cpu_port;  // Prozessor Port

    bool enable_ext_wires;
    bool ext_rdy_wire;

    uint8_t c64_iec_wire;       // Leitungen vom C64 zur Floppy Bit 4=ATN 6=CLK 7=DATA
    uint8_t floppy_iec_wire;    // Leitungen von Floppy zur c64 Bit 6=CLK 7=DATA

    VCDClass iec_export_vdc;      // Klasse zum Exportieren der IEC Signale
    bool iec_is_dumped;

    /// Temporär ///
    bool        easy_flash_dirty;
    uint8_t     easy_flash_byte;

    PORT cia1_port_a;
    PORT cia1_port_b;
    PORT cia2_port_a;
    PORT cia2_port_b;

    bool        enable_mouse_1351;
    uint16_t    mouse_1351_x_rel;
    uint16_t    mouse_1351_y_rel;

    uint8_t     mouse_port;
    uint8_t     poti_ax;
    uint8_t     poti_ay;
    uint8_t     poti_bx;
    uint8_t     poti_by;

    uint8_t     poti_x;
    uint8_t     poti_y;

    uint8_t     key_matrix_to_port_a[8];
    uint8_t     key_matrix_to_port_b[8];

    bool return_key_is_down;

    bool reu_is_insert;

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

    uint16_t        breakpoints[0x10000];
    uint16_t        break_values[16];
    uint16_t        break_status;
    bool            floppy_found_breakpoint;

    int             breakgroup_count;
    BREAK_GROUP     *breakgroup[MAX_BREAK_GROUP_NUM];

    ////////////////////////////////////////////////////////////

    bool        c64_reset_ready;
    bool        floppy_reset_ready[MAX_FLOPPY_NUM];

    char        c64_command_line[MAX_STRING_LENGTH];
    uint16_t    c64_command_line_lenght;
    uint16_t    c64_command_line_current_pos;
    bool        c64_command_line_status;
    bool        c64_command_line_count_s;

    uint32_t    cycle_counter;
    int         limit_cyles_counter;        // Dieser Counter wird wenn er > 0 ist bei jeden Zyklus um 1 runtergezählt
                                            // Wechselt er von 1 auf 0 wird die Emulation angehalten und ein Ergeignis ausgelöst
    bool        debug_mode;
    bool        debug_animation;
    float_t     animation_speed_add;
    float_t     animation_speed_counter;
    bool        one_cycle;
    bool        one_opcode;
    int         one_opcode_source;

    bool        warp_mode;

    bool        mouse_is_hidden;
    int         mouse_hide_counter;
    int         mouse_hide_time;

    bool        key_map_is_rec;
    uint8_t     rec_matrix_code;
};

#endif // C64CLASS_H
