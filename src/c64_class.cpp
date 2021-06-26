//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: c64_class.cpp                         //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 26.06.2021                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "./c64_class.h"
#include "./c64_keys.h"

#include <QDebug>

void AudioMix(void *not_used, Uint8 *stream, int laenge);
int SDLThread(void *userdat);
int SDLThreadLoad(void *userdat);
int SDLThreadWarp(void *userdat);

#define C64Takt 985248  // 50,124542Hz (Original C64 PAL)

#define RecPollingWaitStart 20

#define C64ScreenXW 384         //384

const char* C64Class::screenshot_format_name[] = {"BMP","PNG"};

C64Class::C64Class(int *ret_error, int soundbuffer_size, VideoCrtClass *video_crt_output, bool start_minimized, std::function<void(char*)> log_function, const char *data_path):
    mmu(nullptr),cpu(nullptr),vic(nullptr),sid1(nullptr),sid2(nullptr),cia1(nullptr),cia2(nullptr),crt(nullptr)
{
    *ret_error = 0;

    this->start_minimized = start_minimized;

    changed_graphic_modi = false;
    changed_window_pos = false;
    changed_window_size = false;

    sdl_joystick_update_is_stoped = true;
    rec_joy_mapping = false;
    sdl_joystick_stop_update = true;
    sdl_joystick_is_open = false;
    sdl_joystick_count = 0;

    return_key_is_down = false;

    key_map_is_rec = false;
    rec_matrix_code = false;

    virtual_port1 = 0;
    virtual_port2 = 1;

    warp_mode = false;

    LogText = log_function;

    enable_mouse_1351 = false;
    mouse_1351_x_rel = mouse_1351_y_rel = 0;

    mouse_port = 0;  // Port1 = 0 ... Port2 = 1
    poti_ax = poti_ay = poti_bx = poti_by = 0xFF;    // HighZ zum Beginn (Keine Paddles / Maus angeschlossen)
    poti_x = poti_y = 0xFF;

    mouse_is_hidden = false;
    mouse_hide_counter = 0;
    mouse_hide_time = 3000;

    sprintf(floppy_sound_path,"%s%s",data_path,"/floppy_sounds/");
    sprintf(gfx_path,"%s%s",data_path,"/gfx/");
    sprintf(rom_path,"%s%s",data_path,"/roms/");

    LogText(const_cast<char*>(">> C64 Klasse wurde gestartet...\n"));
    LogText(const_cast<char*>(">> GfxPath = "));
    LogText(const_cast<char*>(gfx_path));
    LogText(const_cast<char*>("\n"));

    this->video_crt_output = video_crt_output;
    breakgroup_count = 0;
    floppy_found_breakpoint = false;
    enable_ext_wires = false;

    iec_is_dumped = false;

    current_c64_screen_width = current_window_width = current_c64_screen_height = current_window_height = 0;

    SDL_ClearError();
    mutex1 = SDL_CreateMutex();
    if(!mutex1)
    {
        LogText(const_cast<char*>("<< ERROR: Fehler beim erstellen eines SDL_Mutex.\n"));
        LogText(const_cast<char*>("<< SDL_Error: "));
        LogText(const_cast<char*>(SDL_GetError()));
        LogText(const_cast<char*>("\n"));
        *ret_error = -1;
        return;
    }

    sdl_window = nullptr;
    c64_screen = nullptr;
    c64_screen_texture = 0;
    screen_aspect_ratio = SCREEN_RATIO_4_3;
    enable_window_aspect_ratio = true;
    enable_fullscreen_aspect_ratio = true;
    c64_screen_is_obselete = false;
    start_screenshot = false;
    enable_exit_screenshot = false;
    frame_skip_counter = 1;

    enable_distortion = true;
    SetDistortion(-0.05f);

    is_screenshot_enable = false;
    screenshot_number = 0;
    screenshot_dir = nullptr;
    screenshot_format = 0;

    video_capture = nullptr;

    /// SDL Installieren ///

    SDL_ClearError();
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
         LogText(const_cast<char*>("<< ERROR: Fehler beim installieren von SDL2\n"));
         LogText(const_cast<char*>("<< SDL_Error: "));
         LogText(const_cast<char*>(SDL_GetError()));
         LogText(const_cast<char*>("\n"));
         *ret_error = -2;
         return;
    }
    else
        LogText(const_cast<char*>(">> SDL2 wurde installiert\n"));

    char filename[FILENAME_MAX];
    sprintf(filename,"%spfeil0.png",gfx_path);
    img_joy_arrow0 = IMG_Load(filename);
    if(!img_joy_arrow0)
    {
        LogText(const_cast<char*>("<< ERROR: Folgendes Bild konnte nicht geladen werden --- "));
        LogText(filename);
        LogText(const_cast<char*>("\n"));
    }
    else
    {
        LogText(const_cast<char*>(">> Folgendes Bild wurde erfolgreich geladen --- "));
        LogText(filename);
        LogText(const_cast<char*>("\n"));

        if ( (img_joy_arrow0->w & (img_joy_arrow0->w - 1)) != 0 ) LogText(const_cast<char*>("<< WARNUNG: Die Breite ist keine Potenz von 2^n\n"));
        if ( (img_joy_arrow0->h & (img_joy_arrow0->h - 1)) != 0 ) LogText(const_cast<char*>("<< WARNUNG: Die Höhe ist keine Potenz von 2^n\n"));
    }

    sprintf(filename,"%spfeil1.png",gfx_path);
    img_joy_arrow1 = IMG_Load(filename);
    if(!img_joy_arrow1)
    {
        LogText(const_cast<char*>("<< ERROR: Folgendes Bild konnte nicht geladen werden --- "));
        LogText(filename);
        LogText(const_cast<char*>("\n"));
    }
    else
    {
        LogText(const_cast<char*>(">> Folgendes Bild wurde erfolgreich geladen --- "));
        LogText(filename);
        LogText(const_cast<char*>("\n"));

        if ( (img_joy_arrow1->w & (img_joy_arrow1->w - 1)) != 0 ) LogText(const_cast<char*>("<< WARNUNG: Die Breite ist keine Potenz von 2^n\n"));
        if ( (img_joy_arrow1->h & (img_joy_arrow1->h - 1)) != 0 ) LogText(const_cast<char*>("<< WARNUNG: Die Höhe ist keine Potenz von 2^n\n"));
    }

    sprintf(filename,"%skreis0.png",gfx_path);
    img_joy_button0 = IMG_Load(filename);
    if(!img_joy_button0)
    {
        LogText(const_cast<char*>("<< ERROR: Folgendes Bild konnte nicht geladen werden --- "));
        LogText(filename);
        LogText(const_cast<char*>("\n"));
    }
    else
    {
        LogText(const_cast<char*>(">> Folgendes Bild wurde erfolgreich geladen --- "));
        LogText(filename);
        LogText(const_cast<char*>("\n"));

        if ( (img_joy_button0->w & (img_joy_button0->w - 1)) != 0 ) LogText(const_cast<char*>("<< WARNUNG: Die Breite ist keine Potenz von 2^n\n"));
        if ( (img_joy_button0->h & (img_joy_button0->h - 1)) != 0 ) LogText(const_cast<char*>("<< WARNUNG: Die Höhe ist keine Potenz von 2^n\n"));
    }

    sprintf(filename,"%skreis1.png",gfx_path);
    img_joy_button1 = IMG_Load(filename);
    if(!img_joy_button1)
    {
        LogText(const_cast<char*>("<< ERROR: Folgendes Bild konnte nicht geladen werden --- "));
        LogText(filename);
        LogText(const_cast<char*>("\n"));
    }
    else
    {
        LogText(const_cast<char*>(">> Folgendes Bild wurde erfolgreich geladen --- "));
        LogText(filename);
        LogText(const_cast<char*>("\n"));

        if ( (img_joy_button1->w & (img_joy_button1->w - 1)) != 0 ) LogText(const_cast<char*>("<< WARNUNG: Die Breite ist keine Potenz von 2^n\n"));
        if ( (img_joy_button1->h & (img_joy_button1->h - 1)) != 0 ) LogText(const_cast<char*>("<< WARNUNG: Die Hoehe ist keine Potenz von 2^n\n"));
    }

    /// VideoCaptuer installieren ///

    video_capture = new VideoCaptureClass();

    /// SLD Audio Installieren (C64 Emulation) ///


    char out_text[1024];

    // Alle Audio Devices in Log auflisten
    sprintf(out_text, ">> Alle Audio Devices:\n");
    LogText(out_text);

    int i, count = SDL_GetNumAudioDevices(0);
    for (i = 0; i < count; ++i)
    {
        sprintf(out_text, "\t -Audio device %d: %s\n", i, SDL_GetAudioDeviceName(i, 0));
        LogText(out_text);
    }

    //  SDL Audio Format
    //  +----------------------sample is signed if set
    //  |
    //  |        +----------sample is bigendian if set
    //  |        |
    //  |        |           +--sample is float if set
    //  |        |           |
    //  |        |           |  +--sample bit size---+
    //  |        |           |  |                    |
    // 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0

    // Wunschformat
    // Muss aber nicht sein, das dieses auch verwendet wird
    // Wichtig ... Es muss alles aus audio_spec_have verwendetet werden

    SDL_memset(&audio_spec_want, 0, sizeof(audio_spec_want));
    audio_spec_want.freq = AudioSampleRate;
    audio_spec_want.format = AUDIO_S16SYS;
    audio_spec_want.channels = 2;
    audio_spec_want.samples = soundbuffer_size;
    audio_spec_want.callback = AudioMix;
    audio_spec_want.userdata = this;

    #ifdef _WIN32
        if(getenv("SDL_AUDIODRIVER") == NULL)
            putenv("SDL_AUDIODRIVER=directsound");

    #endif

    SDL_ClearError();
    audio_dev =  SDL_OpenAudioDevice(NULL, 0, &audio_spec_want, &audio_spec_have, SDL_AUDIO_ALLOW_ANY_CHANGE);
    if( audio_dev == 0 )
    {
        LogText(const_cast<char*>("<< ERROR: Fehler beim installieren von SDL_Audio\n"));
        LogText(const_cast<char*>("<< SDL_Error: "));
        LogText(const_cast<char*>(SDL_GetError()));
        LogText(const_cast<char*>("\n"));
        sprintf(out_text, "\t -Audio Buffersize: %d\n" ,audio_spec_have.samples);
        LogText(out_text);
        *ret_error = -3;
        return;
    }

    audio_frequency = audio_spec_have.freq;
    audio_channels = audio_spec_have.channels;
    audio_sample_bit_size = audio_spec_have.format & 0x00ff;
    is_audio_sample_little_endian = audio_spec_have.format & 0x8000;
    is_audio_sample_float = audio_spec_have.format & 0x0100;
    is_audio_sample_signed = audio_spec_have.format & 0x1000;
    audio_16bit_buffer_size = audio_spec_have.samples * 2;
    audio_16bit_buffer = new int16_t[audio_16bit_buffer_size];

    sprintf(out_text, "\t -Audio Driver: %s\n" , getenv("SDL_AUDIODRIVER"));
    LogText(out_text);
    sprintf(out_text, "\t -Audio Frequency: %d\n" , audio_frequency);
    LogText(out_text);
    sprintf(out_text, "\t -Audio Channels: %d\n" ,audio_channels);
    LogText(out_text);
    sprintf(out_text, "\t -Audio Buffersize: %d\n" ,audio_spec_have.samples);
    LogText(out_text);
    sprintf(out_text, "\t -Audio Sample Bit Size: %d\n",audio_sample_bit_size);
    LogText(out_text);
    sprintf(out_text, "\t -Audio Sample Is Float: %d\n",is_audio_sample_float);
    LogText(out_text);
    sprintf(out_text, "\t -Audio Sample Is Signed: %d\n",is_audio_sample_signed);
    LogText(out_text);
    sprintf(out_text, "\t -Audio Sample Is Little Endian: %d\n",is_audio_sample_little_endian);
    LogText(out_text);

    OpenSDLJoystick();

    SetGrafikModi(video_crt_output->start_c64_is_doublesize, video_crt_output->start_c64_is_palmode, false);

    sprintf(filename,"%ssdl_icon.png",gfx_path);
    sdl_window_icon = IMG_Load(filename);
    if(sdl_window_icon != nullptr)
    {
        SDL_ClearError();
        if(0 != SDL_SetColorKey(sdl_window_icon,SDL_TRUE,SDL_MapRGB(sdl_window_icon->format,0,0,0)))
        {
            LogText(const_cast<char*>("<< ERROR: Fehler beim festlegen des ColorKey im Window Icon.\n"));
            LogText(const_cast<char*>("<< SDL_Error: "));
            LogText(const_cast<char*>(SDL_GetError()));
            LogText(const_cast<char*>("\n"));
            *ret_error = -4;
            return;
        }

    }
    else
        LogText(const_cast<char*>("<< ERROR: Fehler beim laden des SLDFenster Icons\n"));

    game_port1 = 0;
    game_port2 = 0;

    /// Init Classes ///
    mmu = new MMU();
    cpu = new MOS6510();
    vic = new VICII();
    int sid_ret_error;
    sid1 = new MOS6581_8085(0,audio_frequency,audio_spec_have.samples,&sid_ret_error);
    sid2 = new MOS6581_8085(1,audio_frequency,audio_spec_have.samples,&sid_ret_error);
    cia1 = new MOS6526(0);
    cia2 = new MOS6526(1);
    crt = new CartridgeClass();
    reu = new REUClass();
    geo = new GEORAMClass();
    tape = new TAPE1530(audio_frequency,audio_spec_have.samples,C64Takt);

    vic_buffer = vic->video_buffer;

    cia2->floppy_iec_wire = &floppy_iec_wire;
    cia2->c64_iec_wire = &c64_iec_wire;

    iec_export_vdc.AddWire("c64_out_atn",false);
    iec_export_vdc.AddWire("c64_out_clk",false);
    iec_export_vdc.AddWire("c64_out_data",false);
    iec_export_vdc.AddWire("floppy_out_clk",false);
    iec_export_vdc.AddWire("floppy_out_data",false);

    /// Floppy mit C64 verbinden ///

    sprintf(filename,"%s1541.rom",rom_path);

    char motor_filename[FILENAME_MAX];
    char motor_on_filename[FILENAME_MAX];
    char motor_off_filename[FILENAME_MAX];
    char bumper_filename[FILENAME_MAX];
    char stepper_inc_filename[FILENAME_MAX];
    char stepper_dec_filename[FILENAME_MAX];

    sprintf(motor_filename,"%smotor.raw",floppy_sound_path);
    sprintf(motor_on_filename,"%smotor_on.raw",floppy_sound_path);
    sprintf(motor_off_filename,"%smotor_off.raw",floppy_sound_path);
    sprintf(bumper_filename,"%sanschlag.raw",floppy_sound_path);
    sprintf(stepper_inc_filename,"%sstepper_inc.raw",floppy_sound_path);
    sprintf(stepper_dec_filename,"%sstepper_dec.raw",floppy_sound_path);

    for(int i=0; i<MAX_FLOPPY_NUM; i++)
    {
        floppy[i] = new Floppy1541(&reset_wire,audio_frequency,audio_spec_have.samples,&floppy_found_breakpoint);
        floppy[i]->SetResetReady(&floppy_reset_ready[i],0xEBFF);
        floppy[i]->SetC64IEC(&c64_iec_wire);
        floppy[i]->SetDeviceNumber(static_cast<uint8_t>(8+i));
        floppy[i]->LoadDosRom(filename);
        //floppy[i]->LoadFloppySounds((char*)"floppy_sounds/motor.raw",(char*)"floppy_sounds/motor_on.raw",(char*)"floppy_sounds/motor_off.raw",(char*)"floppy_sounds/anschlag.raw",(char*)"floppy_sounds/stepper_inc.raw",(char*)"floppy_sounds/stepper_dec.raw");
        floppy[i]->LoadFloppySounds(motor_filename,motor_on_filename,motor_off_filename,bumper_filename,stepper_inc_filename,stepper_dec_filename);
        floppy[i]->SetEnableFloppy(false);
        floppy[i]->SetEnableFloppySound(true);
    }

    /// Init Vars ///
    c64_frequency = C64Takt;
    c64_speed = 100;
    cpu_pc_history_pos = 0;
    io_source = 0;
    c64_command_line_lenght = 0;
    c64_command_line_current_pos = 0;
    c64_command_line_status = false;
    c64_command_line_count_s = false;
    debug_mode = one_cycle = one_opcode = false;
    cycle_counter = 0;
    limit_cyles_counter = 0;
    debug_animation = false;
    animation_speed_add = audio_spec_have.samples/audio_frequency;
    animation_speed_counter = 0;

    for(int i=0;i<8;i++)
    {
        key_matrix_to_port_a_ext[i] = key_matrix_to_port_a[i] = 0;
        key_matrix_to_port_b_ext[i] = key_matrix_to_port_b[i] = 0;
    }

    /// Callbackroutinen setzen ///
    ReadProcTbl = mmu->CPUReadProcTbl;
    WriteProcTbl = mmu->CPUWriteProcTbl;
    cpu->ReadProcTbl = mmu->CPUReadProcTbl;
    cpu->WriteProcTbl = mmu->CPUWriteProcTbl;
    vic->ReadProcTbl = mmu->VICReadProcTbl;
    vic->RefreshProc = std::bind(&C64Class::VicRefresh,this,std::placeholders::_1);
    reu->ReadProcTbl = mmu->CPUReadProcTbl;
    reu->WriteProcTbl = mmu->CPUWriteProcTbl;

    mmu->VicIOWriteProc = std::bind(&VICII::WriteIO,vic,std::placeholders::_1,std::placeholders::_2);
    mmu->VicIOReadProc = std::bind(&VICII::ReadIO,vic,std::placeholders::_1);
    mmu->SidIOWriteProc = std::bind(&C64Class::WriteSidIO,this,std::placeholders::_1,std::placeholders::_2);
    mmu->SidIOReadProc = std::bind(&C64Class::ReadSidIO,this,std::placeholders::_1);
    mmu->Cia1IOWriteProc = std::bind(&MOS6526::WriteIO,cia1,std::placeholders::_1,std::placeholders::_2);
    mmu->Cia1IOReadProc = std::bind(&MOS6526::ReadIO,cia1,std::placeholders::_1);
    mmu->Cia2IOWriteProc = std::bind(&MOS6526::WriteIO,cia2,std::placeholders::_1,std::placeholders::_2);
    mmu->Cia2IOReadProc = std::bind(&MOS6526::ReadIO,cia2,std::placeholders::_1);

    mmu->CRTRom1WriteProc = std::bind(&CartridgeClass::WriteRom1,crt,std::placeholders::_1,std::placeholders::_2);
    mmu->CRTRom2WriteProc = std::bind(&CartridgeClass::WriteRom2,crt,std::placeholders::_1,std::placeholders::_2);
    mmu->CRTRom3WriteProc = std::bind(&CartridgeClass::WriteRom3,crt,std::placeholders::_1,std::placeholders::_2);
    mmu->CRTRom1ReadProc = std::bind(&CartridgeClass::ReadRom1,crt,std::placeholders::_1);
    mmu->CRTRom2ReadProc = std::bind(&CartridgeClass::ReadRom2,crt,std::placeholders::_1);
    mmu->CRTRom3ReadProc = std::bind(&CartridgeClass::ReadRom3,crt,std::placeholders::_1);
    mmu->IO1ReadProc = std::bind(&C64Class::ReadIO1,this,std::placeholders::_1);
    mmu->IO1WriteProc = std::bind(&C64Class::WriteIO1,this,std::placeholders::_1,std::placeholders::_2);
    mmu->IO2ReadProc = std::bind(&C64Class::ReadIO2,this,std::placeholders::_1);
    mmu->IO2WriteProc = std::bind(&C64Class::WriteIO2,this,std::placeholders::_1,std::placeholders::_2);

    crt->ChangeMemMapProc = std::bind(&MMU::ChangeMemMap,mmu);

    /// Module mit Virtuellen Leitungen verbinden
    mmu->GAME = &game_wire;
    mmu->EXROM = &exrom_wire;
    mmu->RAM_H = &hi_ram_wire;
    mmu->RAM_L = &lo_ram_wire;
    mmu->CPU_PORT = &cpu_port;
    crt->exrom = &exrom_wire;
    crt->game = &game_wire;
    crt->CpuTriggerInterrupt = std::bind(&MOS6510::TriggerInterrupt,cpu,std::placeholders::_1);
    crt->CpuClearInterrupt = std::bind(&MOS6510::ClearInterrupt,cpu,std::placeholders::_1);
    cpu->RDY = &rdy_ba_wire;
    cpu->RESET = &reset_wire;
    cpu->ResetReady = &c64_reset_ready;
    cpu->ResetReadyAdr = 0xE5CD;
    cpu->EnableExtInterrupts = enable_ext_wires;
    cia1->reset_wire = &reset_wire;
    cia1->CpuTriggerInterrupt = std::bind(&MOS6510::TriggerInterrupt,cpu,std::placeholders::_1);
    cia1->CpuClearInterrupt = std::bind(&MOS6510::ClearInterrupt,cpu,std::placeholders::_1);
    cia1->VicTriggerLP = std::bind(&VICII::TriggerLightpen,vic);
    cia1->ChangePOTSwitch = std::bind(&C64Class::ChangePOTSwitch,this);
    cia1->pa = &cia1_port_a;
    cia1->pb = &cia1_port_b;
    cia2->reset_wire = &reset_wire;
    cia2->CpuTriggerInterrupt = std::bind(&MOS6510::TriggerInterrupt,cpu,std::placeholders::_1);
    cia2->CpuClearInterrupt = std::bind(&MOS6510::ClearInterrupt,cpu,std::placeholders::_1);
    cia2->pa = &cia2_port_a;
    cia2->pb = &cia2_port_b;
    vic->ba = &rdy_ba_wire;
    vic->CpuTriggerInterrupt = std::bind(&MOS6510::TriggerInterrupt,cpu,std::placeholders::_1);
    vic->CpuClearInterrupt = std::bind(&MOS6510::ClearInterrupt,cpu,std::placeholders::_1);
    vic->color_ram = mmu->GetFarbramPointer();
    vic->cia2_port_a = cia2_port_a.GetOutputBitsPointer();
    sid1->RESET = &reset_wire;
    sid2->RESET = &reset_wire;
    reu->BA = &rdy_ba_wire;
    reu->CpuTriggerInterrupt = std::bind(&MOS6510::TriggerInterrupt,cpu,std::placeholders::_1);
    reu->CpuClearInterrupt = std::bind(&MOS6510::ClearInterrupt,cpu,std::placeholders::_1);
    reu->RESET = &reset_wire;
    reu->WRITE_FF00 = &cpu->WRITE_FF00;

    reu_is_insert = false;

    /// Tape mit C64 verbinden ///
    tape->CPU_PORT = &cpu_port;
    cia1->flag_pin = &tape->CassRead;

    /// CRT mit MMU verbinden ///
    crt->c64_ram = mmu->GetRAMPointer();
    mmu->EasyFlashDirty1 = crt->GetFlash040Dirty(0);
    mmu->EasyFlashDirty2 = crt->GetFlash040Dirty(1);
    mmu->EasyFlashByte1 = crt->GetFlash040Byte(0);
    mmu->EasyFlashByte2 = crt->GetFlash040Byte(1);

    mmu->EasyFlashDirty1 = &easy_flash_dirty;
    mmu->EasyFlashDirty2 = &easy_flash_dirty;
    mmu->EasyFlashByte1 = &easy_flash_byte;
    mmu->EasyFlashByte2 = &easy_flash_byte;

    rdy_ba_wire = true;
    game_wire = true;
    exrom_wire = true;

    wait_reset_ready = false;

    mmu->Reset();
    cia1->Reset();
    cia2->Reset();

    sid_volume = 1.0f;

    sid1->RESET = &reset_wire;
    sid1->SetC64Zyklen(c64_frequency);     // PAL 63*312*50 = 982800
    sid1->SetChipType(MOS_8580);
    sid1->SoundOutputEnable = true;
    sid1->CycleExact = true;
    sid1->FilterOn = true;
    sid1->Reset();
    sid1->SetPotXY(poti_x, poti_y);

    sid2->RESET = &reset_wire;
    sid2->SetC64Zyklen(c64_frequency);     // PAL 63*312*50 = 982800
    sid2->SetChipType(MOS_8580);
    sid2->SoundOutputEnable = true;
    sid2->CycleExact = true;
    sid2->FilterOn = true;
    sid2->Reset();

    enable_stereo_sid = false;
    enable_stereo_sid_6channel_mode = false;
    stereo_sid_address = 0xD420;

    vic->ba = &rdy_ba_wire;
    vic->cia2_port_a = cia2->pa->GetOutputBitsPointer();

    /// Breakpoints ///
    cpu->BreakStatus = &break_status;
    cpu->BreakWerte = break_values;
    cpu->Breakpoints = breakpoints;

    vic->break_status = &break_status;
    vic->break_values = break_values;
    vic->breakpoints = breakpoints;

    cpu->History = cpu_pc_history;
    cpu->HistoryPointer = &cpu_pc_history_pos;

    for(int i=0;i<0x10000;i++) breakpoints[i] = 0;
}

C64Class::~C64Class()
{
    EndEmulation();

    for(int i=0; i<MAX_FLOPPY_NUM; i++)
    {
        if(floppy[i] != nullptr) delete floppy[i];
    }

    if(mmu != nullptr) delete mmu;
    if(cpu != nullptr) delete cpu;
    if(vic != nullptr) delete vic;
    if(sid1 != nullptr)
    {
        StopSidDump();
        delete sid1;
    }
    if(sid2 != nullptr) delete sid2;
    if(cia1 != nullptr) delete cia1;
    if(cia2 != nullptr) delete cia2;
    if(crt != nullptr) delete crt;
    if(reu != nullptr) delete reu;
    if(geo != nullptr) delete geo;

    if(video_capture != nullptr) delete video_capture;

    if(audio_16bit_buffer != nullptr)
        delete [] audio_16bit_buffer;
}

void C64Class::StartEmulation()
{
    /// SLD Thread starten (ab hier startet auch die C64 Emulation ///
    SDL_ClearError();
    sdl_thread = SDL_CreateThread(SDLThread, "C64Thread", this);
    if(sdl_thread == nullptr)
    {
        LogText(const_cast<char*>("<< ERROR: SDL Thread (C64Thread) konnte nicht erstellt werden.\n"));
        LogText(const_cast<char*>("<< SDL_Error: "));
        LogText(const_cast<char*>(SDL_GetError()));
        LogText(const_cast<char*>("\n"));
    }
    else
    {
        LogText(const_cast<char*>(">> C64Thread wurde gestartet.\n"));

        SDL_PauseAudioDevice(audio_dev, 0);
        LogText(const_cast<char*>(">> SDL Audiostream wurde getartet.\n"));
    }
    return;
}

void C64Class::EndEmulation()
{
    EnableWarpMode(false);
    if(enable_exit_screenshot)
    {
        SwapRBSurface(c64_screen);
        SDL_SavePNG(c64_screen, exit_screenshot_filename);
    }

    /// Loop Thread beenden ///
    loop_thread_end = true;

    int time_out = 1000;    // 1000ms
    while (!loop_thread_is_end && time_out > 0)
    {
        SDL_Delay(1);
        time_out--;
    }

    SDL_PauseAudioDevice(audio_dev, 1);
    if(audio_dev > 0) SDL_CloseAudioDevice(audio_dev);

    CloseSDLJoystick();
    SDL_Quit();
}

void C64Class::SetLimitCycles(int nCycles)
{
    limit_cyles_counter = nCycles;
}

void C64Class::SetEnableDebugCart(bool enable)
{
    cpu->SetEnableDebugCart(enable);
}

void AudioMix(void *not_used, Uint8 *stream, int laenge)
{
    C64Class *c64 = static_cast<C64Class*>(not_used);
    if(c64->rec_polling_wait)
    {
        c64->rec_polling_wait_counter--;
        if(c64->rec_polling_wait_counter == 0) c64->rec_polling_wait = false;
    }

    SDL_LockMutex(c64->mutex1);
    c64->FillAudioBuffer(stream,laenge);
    SDL_UnlockMutex(c64->mutex1);
}

int SDLThreadWarp(void *userdat)
{
    C64Class *c64 = static_cast<C64Class*>(userdat);

    while(!c64->warp_thread_end)
    {
        c64->WarpModeLoop();
    }
    return 0;
}

void C64Class::SDLThreadPauseBegin()
{
    sdl_thread_is_paused = false;
    sdl_thread_pause = true;
    SDL_Delay(40);
}

void C64Class::SDLThreadPauseEnd()
{
    sdl_thread_pause = false;
}

int SDLThread(void *userdat)
{
    C64Class *c64 = static_cast<C64Class*>(userdat);
    c64->LogText(">> SDLThread Start.\n");

    SDL_Event event;
    c64->loop_thread_end = false;
    c64->loop_thread_is_end = false;
    c64->sdl_thread_pause = false;
    c64->c64_screen_buffer = nullptr;

    while (!c64->loop_thread_end)
    {
        /// Wird immer ausgeführt wenn die Funktion SetGrafikModi ausgefürt wurde ///
        if(c64->changed_graphic_modi)
        {
            c64->changed_graphic_modi = false;
            c64->LogText(">> SDLThread: InitGrafik Start.\n");
            c64->InitGrafik();
            c64->LogText(">> SDLThread: InitGrafik Ende.\n");
        }

        if(c64->changed_vsync)
        {
            c64->changed_vsync = false;
            c64->ChangeVSync();
        }

        if(c64->changed_window_pos)
        {
            c64->changed_window_pos = false;
            SDL_SetWindowPosition(c64->sdl_window, c64->sdl_window_pos_x, c64->sdl_window_pos_y);
        }

        if(c64->changed_window_size)
        {
            c64->changed_window_size = false;
            glViewport(0,0,c64->sdl_window_size_width, c64->sdl_window_size_height);
            glMatrixMode(GL_PROJECTION);
            glOrtho(0,c64->sdl_window_size_width, c64->sdl_window_size_height,0,-1,1);
            glLoadIdentity();
            SDL_SetWindowSize(c64->sdl_window, c64->sdl_window_size_width, c64->sdl_window_size_height);
        }

        /// Wird ausgeführt wenn Keine Thread Pause anliegt ///
        if(!c64->sdl_thread_pause)
        {
            while (SDL_PollEvent (&event))
            {
                /// Es ist ein neues SDL Event vorhanden ///
                /// Event auswerten und verarbeiten ///
                c64->AnalyzeSDLEvent(&event);
            }

            /// Wenn der Thread noch nicht beendet wurde ///
            if(!c64->loop_thread_end)
            {
                if(c64->c64_screen_is_obselete)
                {
                    c64->DrawC64Screen();
                    c64->c64_screen_is_obselete = false;
                }
                else SDL_Delay(1);
            }
        }
        else
        {
            SDL_Delay(1);
            if(!c64->sdl_thread_is_paused) c64->sdl_thread_is_paused = true;
        }

        c64->IncMouseHiddenCounter();
    }

    c64->ReleaseGrafik();
    c64->loop_thread_is_end = true;

    c64->LogText(">> SDLThread Ende\n");

    return 0;
}

void C64Class::CalcDistortionGrid()
{
    float_t div = 2.0f / SUBDIVS_SCREEN;
    for(int y=0; y<SUBDIVS_SCREEN+1; y++)
        for(int x=0; x<SUBDIVS_SCREEN+1; x++)
        {
            float_t X = x*div-1.0f;
            float_t Y = y*div-1.0f;
            distortion_grid_points[y*(SUBDIVS_SCREEN+1)+x].y = X*X*(distortion_value*Y)+Y;
            distortion_grid_points[y*(SUBDIVS_SCREEN+1)+x].x = Y*Y*(distortion_value*X)+X;
        }

    div = 1.0f / SUBDIVS_SCREEN;
    for(int y=0; y<SUBDIVS_SCREEN; y++)
        for(int x=0; x<SUBDIVS_SCREEN; x++)
        {
            distortion_grid[y*SUBDIVS_SCREEN*4+x*4+0] = distortion_grid_points[(y)*(SUBDIVS_SCREEN+1)+x];
            distortion_grid[y*SUBDIVS_SCREEN*4+x*4+1] = distortion_grid_points[(y)*(SUBDIVS_SCREEN+1)+x+1];
            distortion_grid[y*SUBDIVS_SCREEN*4+x*4+2] = distortion_grid_points[(y+1)*(SUBDIVS_SCREEN+1)+x+1];
            distortion_grid[y*SUBDIVS_SCREEN*4+x*4+3] = distortion_grid_points[(y+1)*(SUBDIVS_SCREEN+1)+x];

            distortion_grid_texture_coordinates[(SUBDIVS_SCREEN-y-1)*SUBDIVS_SCREEN*4+x*4+0].x = x*div;
            distortion_grid_texture_coordinates[(SUBDIVS_SCREEN-y-1)*SUBDIVS_SCREEN*4+x*4+0].y = (y+1)*div;

            distortion_grid_texture_coordinates[(SUBDIVS_SCREEN-y-1)*SUBDIVS_SCREEN*4+x*4+1].x = (x+1)*div;
            distortion_grid_texture_coordinates[(SUBDIVS_SCREEN-y-1)*SUBDIVS_SCREEN*4+x*4+1].y = (y+1)*div;

            distortion_grid_texture_coordinates[(SUBDIVS_SCREEN-y-1)*SUBDIVS_SCREEN*4+x*4+2].x = (x+1)*div;
            distortion_grid_texture_coordinates[(SUBDIVS_SCREEN-y-1)*SUBDIVS_SCREEN*4+x*4+2].y = (y)*div;

            distortion_grid_texture_coordinates[(SUBDIVS_SCREEN-y-1)*SUBDIVS_SCREEN*4+x*4+3].x = x*div;
            distortion_grid_texture_coordinates[(SUBDIVS_SCREEN-y-1)*SUBDIVS_SCREEN*4+x*4+3].y = y*div;
        }
}

void C64Class::VicRefresh(uint8_t *vic_puffer)
{
    if(enable_hold_vic_refresh)
    {
        vic_refresh_is_holded = true;
        return;
    }

    if((video_crt_output == nullptr) || (sdl_thread_pause == true)) return;

    if(SDL_LockSurface(c64_screen) != 0)
    {
        LogText(const_cast<char*>("<< ERROR: SDL Surface (VicRefresh) konnte nicht gelockt werden.\n"));
        LogText(const_cast<char*>("<< SDL_Error: "));
        LogText(const_cast<char*>(SDL_GetError()));
        LogText(const_cast<char*>("\n"));
        return;
    }

    video_crt_output->ConvertVideo(static_cast<void*>(c64_screen->pixels),c64_screen->pitch,vic_puffer,104,current_c64_screen_width,current_c64_screen_height,504,312,false);

    this->vic_buffer = vic_puffer;
    vic->SwitchVideoBuffer();
    SDL_UnlockSurface(c64_screen);

    ///////////////////////////////////

    video_capture->AddFrame(static_cast<uint8_t*>(c64_screen->pixels),c64_screen->pitch);

    ///////////////////////////////////

    ///////////////////////////////////
    /// Auf Screenshot Start prüfen ///
    if(start_screenshot)
    {
        SwapRBSurface(c64_screen);

        switch (screenshot_format) {
        case SCREENSHOT_FORMAT_BMP:
            SDL_ClearError();
            if(0 != SDL_SaveBMP(c64_screen, screenshot_filename))
            {
                LogText(const_cast<char*>("<< ERROR: Es konnte der Screenshot als BMP nicht gespeichert werden.\n"));
                LogText(const_cast<char*>("<< SDL_Error: "));
                LogText(const_cast<char*>(SDL_GetError()));
                LogText(const_cast<char*>("\n"));
            }
            else
            {
                LogText(const_cast<char*>(">> Screenshot wurde erfolgreich gespeichert ["));
                LogText(screenshot_filename);
                LogText(const_cast<char*>("\n"));
            }
            break;
        case SCREENSHOT_FORMAT_PNG:
            if(0 != SDL_SavePNG(c64_screen, screenshot_filename))
            {
                LogText(const_cast<char*>("<< ERROR: Es konnte der Screenshot als PNG nicht gespeichert werden.\n"));
                LogText(const_cast<char*>("<< SDL_Error: "));
                LogText(const_cast<char*>(SDL_GetError()));
                LogText(const_cast<char*>("\n"));
            }
            else
            {
                LogText(const_cast<char*>(">> Screenshot wurde erfolgreich gespeichert ["));
                LogText(screenshot_filename);
                LogText(const_cast<char*>("\n"));
            }
            break;
        default:

            break;
        }
        start_screenshot = false;
    }
    ////////////////////////////////////

    if(enable_mouse_1351) UpdateMouse();

    c64_screen_is_obselete = true;
}

void C64Class::WarpModeLoop()
{
    if(limit_cyles_counter > 0)
    {
        limit_cyles_counter--;
        if(limit_cyles_counter == 0)
        {
            // Event auslösen
            if(LimitCyclesEvent != nullptr) LimitCyclesEvent();
        }
    }

    if(cpu->WRITE_DEBUG_CART)
    {
        // Event auslösen
        cpu->WRITE_DEBUG_CART = false;
        if(DebugCartEvent != nullptr) DebugCartEvent(cpu->GetDebugCartValue());
    }

    NextSystemCycle();

    ////////////////////////// Testweise //////////////////////////

    static int zyklen_counter = 0;
    if(++zyklen_counter == 19656)
    {
        zyklen_counter = 0;
        if(wait_reset_ready)
        {
            if(!floppy[0]->GetEnableFloppy())
            {
                if(c64_reset_ready)
                {
                    SDL_ClearError();
                    if(SDL_CreateThread(SDLThreadLoad ,"C64ThreadLoad",this) == nullptr)
                    {
                        LogText(const_cast<char*>("<< ERROR: SDL Thread (C64ThreadLoad) konnte nicht erstellt werden.\n"));
                        LogText(const_cast<char*>("<< SDL_Error: "));
                        LogText(const_cast<char*>(SDL_GetError()));
                        LogText(const_cast<char*>("\n"));
                    }
                    wait_reset_ready = false;
                }
            }
            else
            {
                if((c64_reset_ready == true) && (floppy_reset_ready[0] == true))
                {
                    SDL_ClearError();
                    if(SDL_CreateThread(SDLThreadLoad ,"C64ThreadLoad",this) == nullptr)
                    {
                        LogText(const_cast<char*>("<< ERROR: SDL Thread (C64ThreadLoad) konnte nicht erstellt werden.\n"));
                        LogText(const_cast<char*>("<< SDL_Error: "));
                        LogText(const_cast<char*>(SDL_GetError()));
                        LogText(const_cast<char*>("\n"));
                    }
                    wait_reset_ready = false;
                }
            }
        }

        if(c64_command_line_count_s)
        {
            c64_command_line_count_s=false;
            if(c64_command_line_status)
            {
                if(c64_command_line_current_pos==c64_command_line_lenght)
                {
                    c64_command_line_status=false;
                }
                else
                {
                    WriteC64Byte(0xC6,1);
                    WriteC64Byte(0x277,static_cast<uint8_t>(c64_command_line[c64_command_line_current_pos]));
                    c64_command_line_current_pos++;
                }
            }
        }
        if(ReadC64Byte(0xC6)==0)
        {
            c64_command_line_count_s=true;
        }
    }
    //////////////////////////////////////////////////////////////////////////////

    if(iec_is_dumped)
    {
        iec_export_vdc.NextStep();
        iec_export_vdc.SetWire(0,c64_iec_wire & 16);
        iec_export_vdc.SetWire(1,c64_iec_wire & 64);
        iec_export_vdc.SetWire(2,c64_iec_wire & 128);
        iec_export_vdc.SetWire(3,floppy_iec_wire & 64);
        iec_export_vdc.SetWire(4,floppy_iec_wire & 128);
    }
}

int C64Class::GetAudioSampleRate()
{
    return  audio_frequency;
}

void C64Class::FillAudioBuffer(uint8_t *stream, int laenge)
{
    sid1->ZeroSoundBufferPos();
    sid2->ZeroSoundBufferPos();

    for(int i=0; i<MAX_FLOPPY_NUM; i++)
        floppy[i]->ZeroSoundBufferPos();

    tape->ZeroSoundBufferPos();

    int sample_buffer_size_mono = laenge / ((audio_sample_bit_size/8) * audio_channels);

    if(!debug_mode)
    {
        while((sid1->SoundBufferPos < sample_buffer_size_mono) && (debug_mode == false))
        {
            if(limit_cyles_counter > 0)
            {
                limit_cyles_counter--;
                if(limit_cyles_counter == 0)
                {
                    // Event auslösen
                    if(LimitCyclesEvent != nullptr) LimitCyclesEvent();
                }
            }

            if(cpu->WRITE_DEBUG_CART)
            {
                // Event auslösen
                cpu->WRITE_DEBUG_CART = false;
                if(DebugCartEvent != nullptr) DebugCartEvent(cpu->GetDebugCartValue());
            }

            NextSystemCycle();

            if((break_status != 0) || (floppy_found_breakpoint == true )) if(CheckBreakpoints()) break;


            ////////////////////////// Testweise //////////////////////////

            static int zyklen_counter = 0;
            if(++zyklen_counter == 19656)
            {
                zyklen_counter = 0;
                if(wait_reset_ready)
                {
                    if(!floppy[0]->GetEnableFloppy())
                    {
                        if(c64_reset_ready)
                        {
                            SDL_ClearError();
                            if(SDL_CreateThread(SDLThreadLoad ,"C64ThreadLoad",this) == nullptr)
                            {
                                LogText(const_cast<char*>("<< ERROR: SDL Thread (C64ThreadLoad) konnte nicht erstellt werden.\n"));
                                LogText(const_cast<char*>("<< SDL_Error: "));
                                LogText(const_cast<char*>(SDL_GetError()));
                                LogText(const_cast<char*>("\n"));
                            }
                            wait_reset_ready = false;
                        }
                    }
                    else
                    {
                        if((c64_reset_ready == true) && (floppy_reset_ready[0] == true))
                        {
                            SDL_ClearError();
                            if(SDL_CreateThread(SDLThreadLoad ,"C64ThreadLoad",this) == nullptr)
                            {
                                LogText(const_cast<char*>("<< ERROR: SDL Thread (C64ThreadLoad) konnte nicht erstellt werden.\n"));
                                LogText(const_cast<char*>("<< SDL_Error: "));
                                LogText(const_cast<char*>(SDL_GetError()));
                                LogText(const_cast<char*>("\n"));
                            }
                            wait_reset_ready = false;
                        }
                    }
                }

                if(c64_command_line_count_s)
                {
                    c64_command_line_count_s=false;
                    if(c64_command_line_status)
                    {
                        if(c64_command_line_current_pos==c64_command_line_lenght)
                        {
                            c64_command_line_status=false;
                        }
                        else
                        {
                            WriteC64Byte(0xC6,1);
                            WriteC64Byte(0x277,static_cast<uint8_t>(c64_command_line[c64_command_line_current_pos]));
                            c64_command_line_current_pos++;
                        }
                    }
                }
                if(ReadC64Byte(0xC6)==0)
                {
                    c64_command_line_count_s=true;
                }
            }
            //////////////////////////////////////////////////////////////////////////////


            if(iec_is_dumped)
            {
                iec_export_vdc.NextStep();
                iec_export_vdc.SetWire(0,c64_iec_wire & 16);
                iec_export_vdc.SetWire(1,c64_iec_wire & 64);
                iec_export_vdc.SetWire(2,c64_iec_wire & 128);
                iec_export_vdc.SetWire(3,floppy_iec_wire & 64);
                iec_export_vdc.SetWire(4,floppy_iec_wire & 128);
            }

        }

        int sample_buffer_size = ((laenge / audio_channels) * 2) / (audio_sample_bit_size/8);

        if(enable_stereo_sid)
        {
            if(!enable_stereo_sid_6channel_mode)
            {
                int j=0;
                for(int i=0; i<(sample_buffer_size); i+=2)
                {
                    audio_16bit_buffer[i] = static_cast<int16_t>(sid1->SoundBuffer[j] * sid_volume);
                    audio_16bit_buffer[i+1] = static_cast<int16_t>(sid2->SoundBuffer[j] * sid_volume);
                    j++;
                }
            }
            else
            {
                int j=0;
                for(int i=0; i<(sample_buffer_size); i+=2)
                {
                    audio_16bit_buffer[i] = audio_16bit_buffer[i+1] = static_cast<int16_t>(static_cast<float_t>(sid1->SoundBuffer[j] + sid2->SoundBuffer[j]) * sid_volume * 0.75f);
                    j++;
                }
            }
        }
        else
        {
            int j=0;
            for(int i=0; i<(sample_buffer_size); i+=2)
            {
                audio_16bit_buffer[i] = audio_16bit_buffer[i+1] = static_cast<int16_t>(sid1->SoundBuffer[j] * sid_volume);
                j++;
            }
        }

        /// Capture Audio
        video_capture->FillSourceAudioBuffer(audio_16bit_buffer, sample_buffer_size);

        /// Floppysound dazu mixen ///

        for(int i=0; i<MAX_FLOPPY_NUM; i++)
        {
            if(floppy[i]->GetEnableFloppySound()) SDL_MixAudioFormat(reinterpret_cast<uint8_t*>(audio_16bit_buffer),reinterpret_cast<uint8_t*>(floppy[i]->GetSoundBuffer()), AUDIO_S16,static_cast<uint32_t>(sample_buffer_size*2),SDL_MIX_MAXVOLUME);
        }

        /// Tapesound dazu mixen ///
        SDL_MixAudioFormat(reinterpret_cast<uint8_t*>(audio_16bit_buffer),reinterpret_cast<uint8_t*>(tape->GetSoundBuffer()), AUDIO_S16,static_cast<uint32_t>(sample_buffer_size*2),SDL_MIX_MAXVOLUME);

        /// Audio auf Output Stream ausgeben
        if(!is_audio_sample_float && audio_sample_bit_size == 16)
        {
            if(audio_channels == 2)
            {
                memcpy(stream,audio_16bit_buffer,static_cast<size_t>(laenge));
            }
            else if(audio_channels > 2)
            {
                int i_src = 0;
                int i_dst = 0;

                int16_t *int16_stream = reinterpret_cast<int16_t*>(stream);
                for(int i=0; i<sample_buffer_size_mono; i++)
                {
                    int16_stream[i_dst++] = audio_16bit_buffer[i_src++];
                    int16_stream[i_dst++] = audio_16bit_buffer[i_src++];
                    for(int j=2; j<audio_channels; j++)
                        int16_stream[i_dst++] = 0x7fff;
                }
            }
        }

        else if(is_audio_sample_float && audio_sample_bit_size == 32)
        {
            if(audio_channels == 2)
            {
                float *float_stream = reinterpret_cast<float*>(stream);
                for(int i=0; i<sample_buffer_size; i++)
                    float_stream[i] = static_cast<float>(audio_16bit_buffer[i]) / static_cast<float>(0x7fff);
            }
            else if(audio_channels > 2)
            {
                int i_src = 0;
                int i_dst = 0;

                float *float_stream = reinterpret_cast<float*>(stream);
                for(int i=0; i<sample_buffer_size_mono; i++)
                {
                    float_stream[i_dst++] = static_cast<float>(audio_16bit_buffer[i_src++]) / static_cast<float>(0x7fff);
                    float_stream[i_dst++] = static_cast<float>(audio_16bit_buffer[i_src++]) / static_cast<float>(0x7fff);
                    for(int j=2; j<audio_channels; j++)
                        float_stream[i_dst++] = 0.0f;
                }
            }
        }
    }
    else
    {
        if(debug_animation)
        {
            animation_speed_counter += animation_speed_add;
            if(animation_speed_counter >= 1)
            {
                for(int i=0;i<static_cast<int>(animation_speed_counter);i++)
                {
                    NextSystemCycle();

                    if((break_status != 0) || (floppy_found_breakpoint == true )) if(CheckBreakpoints()) break;

                    if(iec_is_dumped)
                    {
                        iec_export_vdc.NextStep();
                        iec_export_vdc.SetWire(0,c64_iec_wire & 16);
                        iec_export_vdc.SetWire(1,c64_iec_wire & 64);
                        iec_export_vdc.SetWire(2,c64_iec_wire & 128);
                        iec_export_vdc.SetWire(3,floppy_iec_wire & 64);
                        iec_export_vdc.SetWire(4,floppy_iec_wire & 128);
                    }
                }
                animation_speed_counter = 0;
            }
            if(AnimationRefreshProc != nullptr) AnimationRefreshProc();
        }
        else
        {
            if(one_cycle)
            {
                one_cycle = false;

                NextSystemCycle();

                if((break_status != 0) || (floppy_found_breakpoint == true )) CheckBreakpoints();

                if(AnimationRefreshProc != nullptr) AnimationRefreshProc();

                if(iec_is_dumped)
                {
                    iec_export_vdc.NextStep();
                    iec_export_vdc.SetWire(0,c64_iec_wire & 16);
                    iec_export_vdc.SetWire(1,c64_iec_wire & 64);
                    iec_export_vdc.SetWire(2,c64_iec_wire & 128);
                    iec_export_vdc.SetWire(3,floppy_iec_wire & 64);
                    iec_export_vdc.SetWire(4,floppy_iec_wire & 128);
                }
            }

            if(one_opcode)
            {
                one_opcode = false;

                do
                {
                    NextSystemCycle();
                }
                while (!cpu_states[one_opcode_source]);

                if((break_status != 0) || (floppy_found_breakpoint == true )) CheckBreakpoints();

                if(AnimationRefreshProc != nullptr) AnimationRefreshProc();

                if(iec_is_dumped)
                {
                    iec_export_vdc.NextStep();
                    iec_export_vdc.SetWire(0,c64_iec_wire & 16);
                    iec_export_vdc.SetWire(1,c64_iec_wire & 64);
                    iec_export_vdc.SetWire(2,c64_iec_wire & 128);
                    iec_export_vdc.SetWire(3,floppy_iec_wire & 64);
                    iec_export_vdc.SetWire(4,floppy_iec_wire & 128);
                }
            }
        }

        // Audiopuffer mit 0 füllen ();
        memset(stream,0,static_cast<size_t>(laenge));
    }

    // Für Ozi Ausgabe
    if(AudioOutProc != nullptr) AudioOutProc(reinterpret_cast<uint8_t*>(sid1->SoundBufferV0), reinterpret_cast<uint8_t*>(sid1->SoundBufferV1), reinterpret_cast<uint8_t*>(sid1->SoundBufferV2), sample_buffer_size_mono);
}

void C64Class::KeyEvent(uint8_t matrix_code, KeyStatus key_status, bool isAutoShift)
{
    const uint8_t AutoShift = 0x17; // C64 Shift Links

    switch(key_status)
    {
    case KEY_DOWN:
        key_matrix_to_port_b[matrix_code>>4]|=1<<(matrix_code&0x07);
        if(isAutoShift) key_matrix_to_port_b[AutoShift>>4]|=1<<(AutoShift&0x07);

        key_matrix_to_port_a[matrix_code&0x07]|=(1<<(matrix_code>>4));
        if(isAutoShift) key_matrix_to_port_a[AutoShift&0x07]|=(1<<(AutoShift>>4));
        break;
    case KEY_UP:
        key_matrix_to_port_b[matrix_code>>4]&=~(1<<(matrix_code&0x07));
        if(isAutoShift) key_matrix_to_port_b[AutoShift>>4]&=~(1<<(AutoShift&0x07));

        key_matrix_to_port_a[matrix_code&0x07]&=~(1<<(matrix_code>>4));
        if(isAutoShift) key_matrix_to_port_a[AutoShift&0x07]&=~(1<<(AutoShift>>4));
        break;
    }
}

void C64Class::SoftReset()
{
    wait_reset_ready = false;
    SetReset(false,true);
    SDL_Delay(40);
    SetReset(true,true);
}

void C64Class::HardReset()
{
    wait_reset_ready = false;
    SetReset(false,false);
    SDL_Delay(40);
    SetReset(true,true);
}

inline void C64Class::SetReset(int status, int hard_reset)
{
    reset_wire = status;
    if(!status)
    {
        crt->Reset();
        rdy_ba_wire = true;
        if(!hard_reset) mmu->Reset();
        KillCommandLine();
    }
}

inline void C64Class::CheckKeys()
{
    uint8_t out_pa, out_pb;
    uint8_t in_pa, in_pb;

    out_pa = ~cia1_port_a.GetOutput();
    out_pb = ~cia1_port_b.GetOutput();

    in_pa = in_pb = 0;

    uint8_t cbit = 1;
    for(int i=0;i<8;i++)
    {
        if(out_pa & cbit) in_pb |= (key_matrix_to_port_b[i]|key_matrix_to_port_b_ext[i]);
        if(out_pb & cbit) in_pa |= (key_matrix_to_port_a[i]|key_matrix_to_port_a_ext[i]);
        cbit <<= 1;
    }
    cia1_port_a.SetInput(~(in_pa|game_port2));
    cia1_port_b.SetInput(~(in_pb|game_port1));
}

void C64Class::ResetC64CycleCounter()
{
    cycle_counter = 0;
}

bool C64Class::LoadC64Roms(const char *kernalrom, const char *basicrom, const char *charrom)
{
    if(!mmu->LoadKernalRom(kernalrom)) return false;
    if(!mmu->LoadBasicRom(basicrom)) return false;
    if(!mmu->LoadCharRom(charrom)) return false;
    return true;
}

bool C64Class::LoadFloppyRom(uint8_t floppy_nr, const char *dos1541rom)
{
    if(floppy_nr < MAX_FLOPPY_NUM)
    {
        if(!floppy[floppy_nr]->LoadDosRom(dos1541rom)) return false;
        return true;
    }
    return false;
}

bool C64Class::LoadDiskImage(uint8_t floppy_nr, FILE *file, int typ)
{
    if(floppy_nr < MAX_FLOPPY_NUM)
    {
		return floppy[floppy_nr]->LoadDiskImage(file, typ);
    }
    return false;
}

void C64Class::LoadPRGFromD64(uint8_t floppy_nr, char *c64_filename, int command)
{
    char str00[1024];
    KillCommandLine();

    switch (command)
    {
    case 0:
        HardReset();
        SDL_Delay(100);
        sprintf(str00,"LOAD\"%s\",%d,1%cRUN%c",c64_filename,floppy_nr+8,13,13);
        SetCommandLine(str00);
        break;
    case 1:
        sprintf(str00,"LOAD\"%s\",%d,1%cRUN%c",c64_filename,floppy_nr+8,13,13);
        SetCommandLine(str00);
        break;
    case 2:
        sprintf(str00,"LOAD\"%s\",%d,1%c",c64_filename,floppy_nr+8,13);
        SetCommandLine(str00);
        break;
    }
}

void C64Class::SetFloppyWriteProtect(uint8_t floppy_nr, bool status)
{
    if(floppy_nr < 4)
        floppy[floppy_nr]->SetWriteProtect(status);
}

void C64Class::SetCommandLine(char *c64_command)
{
    strcpy(c64_command_line,c64_command);
    c64_command_line_lenght = static_cast<uint16_t>(strlen(c64_command_line));
    c64_command_line_current_pos = 0;
    c64_command_line_status = true;
    c64_command_line_count_s = true;
}

void C64Class::KillCommandLine()
{
    c64_command_line_lenght=0;
    c64_command_line_status=false;
    c64_command_line_count_s=false;
}

uint8_t C64Class::ReadC64Byte(uint16_t address)
{
    return ReadProcTbl[(address)>>8](address);
}

void C64Class::WriteC64Byte(uint16_t address, uint8_t value)
{
    WriteProcTbl[(address)>>8](address,value);
}

uint8_t* C64Class::GetRAMPointer(uint16_t address)
{
    return mmu->GetRAMPointer() + address;
}

void C64Class::SetGrafikModi(bool enable_screen_doublesize, bool enable_screen_crt_output, bool enable_screen_filter, uint16_t fullscreen_width, uint16_t fullscreen_height)
{
    this->enable_screen_doublesize = enable_screen_doublesize;
    this->enable_screen_crt_output =  enable_screen_crt_output;
    this->enable_screen_filter = enable_screen_filter;
    this->start_minimized = start_minimized;
    this->fullscreen_width = fullscreen_width;
    this->fullscreen_height = fullscreen_height;

    changed_graphic_modi = true;

    /*
    char str00[255];
    sprintf(str00,">>   Doublesize = %d\n>>   PAL = %d\n>>   Filter = %d\n>>   FullResXW = %d\n>>   FullResrYW = %d\n", enable_screen_doublesize, enable_screen_crt_output, enable_screen_filter, fullscreen_width, fullscreen_height);

    LogText(const_cast<char*>(">> Grafikmodus wurde gesetzt:\n"));
    LogText(str00);
    */
}

void C64Class::SetSDLWindowName(const char *name)
{
    strcpy(sdl_window_name, name);
    SDL_SetWindowTitle(sdl_window, name);
}

void C64Class::SetFullscreen(bool is_fullscreen)
{
    static int size_w, size_h, pos_x, pos_y;
	static SDL_DisplayMode window_display_mode;

	if(is_fullscreen)
	{
		if(0 == SDL_GetCurrentDisplayMode(0, &window_display_mode))
		{
			enable_fullscreen = is_fullscreen;
		}
		else return;
	}

    if(is_fullscreen)
    {
        SDL_GetWindowSize(sdl_window, &size_w, &size_h);
        SDL_GetWindowPosition(sdl_window, &pos_x, &pos_y);
		SDL_ShowCursor(false);

		int current_video_display = SDL_GetWindowDisplayIndex(sdl_window);
		if(current_video_display >= MAX_VIDEO_DISPLAYS)
			current_video_display = 0;

		SDL_SetWindowDisplayMode(sdl_window, &fullscreen_display_mode[current_video_display]);
		SDL_SetWindowFullscreen(sdl_window,SDL_WINDOW_FULLSCREEN);
    }
    else
    {
        SDL_ShowCursor(true);
		SDL_SetWindowFullscreen(sdl_window,0);

        sdl_window_pos_x = pos_x;
        sdl_window_pos_y = pos_y;
        sdl_window_size_width = size_w;
        sdl_window_size_height = size_h;
        changed_window_pos = true;
        changed_window_size = true;
    }

    SetFocusToC64Window();
}

void C64Class::ToggleScreenMode()
{
     enable_fullscreen = ! enable_fullscreen;
     SetFullscreen(enable_fullscreen);
}

void C64Class::InitGrafik()
{
    char out_string[1024];
    /// VicRefresh stoppen und solange warten bis wirklich Stop ist ///
    enable_hold_vic_refresh = true;
    vic_refresh_is_holded = false;


    LogText("\tInitGrafik: Warten bis Vic-Refresh angehalten wurde.\n");

    int timeout = 1000;
    while(!vic_refresh_is_holded && timeout > 0)
    {
        SDL_Delay(1);
        timeout--;
    }

    LogText("\tInitGrafik: Vic-Refresh wurde angehalten.\n");

    /// Allegmeine Einstellungen ///

    if((fullscreen_width != 0) && (fullscreen_height != 0))
        enable_fullscreen  = true;
    else enable_fullscreen = false;

    sprintf(out_string,"\tInitGrafik: EnableFullscreen: %d\n",enable_fullscreen);
    LogText(out_string);

    current_window_width = current_c64_screen_width = C64ScreenXW;
    current_window_height = current_c64_screen_height = vic->GetAktVicDisplayLastLine() - vic->GetAktVicDisplayFirstLine();

    if(enable_screen_doublesize)
    {
        current_window_width *=2;
        current_window_height *=2;

        current_c64_screen_width *=2;
        current_c64_screen_height *=2;
    }

    sprintf(out_string,"\tInitGrafik: WindowWidth: %d, WindowHeight: %d\n",current_window_width, current_window_height);
    LogText(out_string);
    sprintf(out_string,"\tInitGrafik: C64ScreenWidth: %d, C64ScreenHeight: %d\n",current_c64_screen_width, current_c64_screen_height);
    LogText(out_string);

    current_window_color_bits = 32;
    if(c64_screen_buffer != nullptr)
    {
        delete[] c64_screen_buffer;
    }
    c64_screen_buffer = new uint8_t[current_c64_screen_width*current_c64_screen_height*4];

    sprintf(out_string,"\tInitGrafik: WindowColorBits: %d\n", current_window_color_bits);
    LogText(out_string);

    video_crt_output->EnableCrtOutput(enable_screen_crt_output);
    video_crt_output->EnableVideoDoubleSize(enable_screen_doublesize);

    sprintf(out_string,"\tInitGrafik: EnableVideoCrtOutput: %d\n", enable_screen_crt_output);
    LogText(out_string);
    sprintf(out_string,"\tInitGrafik: EnableVideoDoubleSize: %d\n", enable_screen_doublesize);
    LogText(out_string);

    if(sdl_window == nullptr)
    {
        LogText("\tInitGrafik: SDL_Window noch nicht vorhanden.\n");

        // Wenn no-gui command
        // Flag SDL_WINDOW_HIDDEN   -> Window wird nicht dargestellt auch nicht in der Taskleiste

        sdl_window = SDL_CreateWindow(sdl_window_name, SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,current_window_width,current_window_height,SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS |SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
        if(sdl_window == nullptr)
            LogText("\tInitGrafik: Fehler beim erstellen des SDL_Window.\n");
        else
        {
            sprintf(out_string,"\tInitGrafik: SDL_Window wurde erstellt (%d,%d).\n",current_window_width, current_window_height);
            LogText(out_string);
        }
        SDL_SetWindowIcon(sdl_window,sdl_window_icon);
        LogText("\tInitGrafik: SDL_Window Icon wurde gesetzt.\n");
    }
    else
    {
        LogText("\tInitGrafik: SDL_Window ist schon vorhanden.\n");

        SDL_SetWindowSize(sdl_window,current_window_width,current_window_height);

        sprintf(out_string,"\tInitGrafik: SDL_Window Groesse wurde gesetzt (%d,%d).\n",current_window_width, current_window_height);
        LogText(out_string);
    }

    // Wenn Minimized Comandline
    if(start_minimized)
        SDL_MinimizeWindow(sdl_window);

    gl_context = SDL_GL_CreateContext(sdl_window);
    if(gl_context == nullptr)
        LogText("\tInitGrafik: Fehler beim erstellen des GLContext.\n");
    else
        LogText("\tInitGrafik: GLContext wurde erstellt.\n");

    // OpenGL Initialisieren //
    // SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0,0,current_window_width,current_window_height);
    LogText("\tInitGrafik: GLVieport wurde gesetzt.\n");
    glMatrixMode(GL_PROJECTION);
    glOrtho(0,current_window_width,current_window_height,0,-1,1);
    LogText("\tInitGrafik: GLOrtho wurde gesetzt.\n");
    glLoadIdentity();

    glGenTextures(1,&c64_screen_texture);
    LogText("\tInitGrafik: C64 Screen Textur wurde generiert.\n");

    glBindTexture( GL_TEXTURE_2D, c64_screen_texture);
    LogText("\tInitGrafik: C64 Screen Textur wurde gebunden.\n");

    // Textur Stretching Parameter setzen
    if(enable_screen_filter)
    {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        LogText("\tInitGrafik: Screenfilter ist aktiviert.\n");
    }
    else
    {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        LogText("\tInitGrafik: Screenfilter ist deaktiviert.\n");
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    if(c64_screen != nullptr)
    {
       SDL_FreeSurface(c64_screen);
        LogText("\tInitGrafik: SDL Surface C64Screen wurde wieder freigegeben.\n");
    }

    c64_screen = SDL_CreateRGBSurface(0, current_c64_screen_width, current_c64_screen_height, current_window_color_bits, 0, 0, 0, 0);
    if(c64_screen != nullptr)
        LogText("\tInitGrafik: C64Screen - SDL RGB Surface wurde erstellt.\n");
    else
        LogText("\tInitGrafik: C64Screen - SDL RGB Surface konnte nicht erstellt werden.\n");

    glTexImage2D( GL_TEXTURE_2D, 0, 4, current_c64_screen_width, current_c64_screen_height, 0,GL_RGBA, GL_UNSIGNED_BYTE, c64_screen_buffer);

    if(glGetError() == GL_NO_ERROR)
        LogText("\tInitGrafik: 2DTexturImage wurde aus C64ScreenBuffer erstellt.\n");
    else
        LogText("\tInitGrafik: 2DTexturImage konnte nicht aus C64ScreenBuffer erstellt werden.\n");

    GLenum  TextureFormat = 0;
    GLint   NofColors = 0;

    if(img_joy_arrow0 != nullptr)
    {
        NofColors = img_joy_arrow0->format->BytesPerPixel;

        if(NofColors == 4)
        {
            TextureFormat=GL_RGBA;
        }
        else if(NofColors == 3) //no alpha channel
        {
            TextureFormat=GL_RGB;
        }
    }

    // Ich setze mal vorraus das alle 4 Images das selbe Format haben !! //

    if(img_joy_arrow0 != nullptr)
    {
        LogText("\tInitGrafik: Textur ImgJoyArrow0 wird erstellt.\n");
        glGenTextures(1,&texture_joy_arrow0);
        glBindTexture( GL_TEXTURE_2D, texture_joy_arrow0 );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        //glTexImage2D( GL_TEXTURE_2D, 0, NofColors, Pfeil0->w, Pfeil0->h, 0,TextureFormat, GL_UNSIGNED_BYTE, Pfeil0->pixels );
        gluBuild2DMipmaps(GL_TEXTURE_2D, NofColors, img_joy_arrow0->w, img_joy_arrow0->h,TextureFormat, GL_UNSIGNED_BYTE, img_joy_arrow0->pixels );

        if(glGetError() == GL_NO_ERROR)
            LogText("\tInitGrafik: Textur konnte erstellt werden.\n");
        else
            LogText("\tInitGrafik: Textur konnte nicht erstellt werden.\n");
    }

    if(img_joy_arrow1 != nullptr)
    {
        LogText("\tInitGrafik: Textur ImgJoyArrow1 wird erstellt.\n");
        glGenTextures(1,&texture_joy_arrow1);
        glBindTexture( GL_TEXTURE_2D, texture_joy_arrow1 );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        //glTexImage2D( GL_TEXTURE_2D, 0, NofColors, Pfeil1->w, Pfeil1->h, 0,TextureFormat, GL_UNSIGNED_BYTE, Pfeil1->pixels );
        gluBuild2DMipmaps(GL_TEXTURE_2D, NofColors, img_joy_arrow1->w, img_joy_arrow1->h,TextureFormat, GL_UNSIGNED_BYTE, img_joy_arrow1->pixels );

        if(glGetError() == GL_NO_ERROR)
            LogText("\tInitGrafik: Textur konnte erstellt werden.\n");
        else
            LogText("\tInitGrafik: Textur konnte nicht erstellt werden.\n");
    }

    if(img_joy_button0 != nullptr)
    {
        LogText("\tInitGrafik: Textur ImgJoyButton0 wird erstellt.\n");
        glGenTextures(1,&texture_joy_button0);
        glBindTexture( GL_TEXTURE_2D, texture_joy_button0 );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        //glTexImage2D( GL_TEXTURE_2D, 0, NofColors, Kreis0->w, Kreis0->h, 0,TextureFormat, GL_UNSIGNED_BYTE, Kreis0->pixels );
        gluBuild2DMipmaps(GL_TEXTURE_2D, NofColors, img_joy_button0->w, img_joy_button0->h,TextureFormat, GL_UNSIGNED_BYTE, img_joy_button0->pixels );

        if(glGetError() == GL_NO_ERROR)
            LogText("\tInitGrafik: Textur konnte erstellt werden.\n");
        else
            LogText("\tInitGrafik: Textur konnte nicht erstellt werden.\n");
    }

    if(img_joy_button1 != nullptr)
    {
        LogText("\tInitGrafik: Textur ImgJoyButton1 wird erstellt.\n");
        glGenTextures(1,&texture_joy_button1);
        glBindTexture( GL_TEXTURE_2D, texture_joy_button1 );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        //glTexImage2D( GL_TEXTURE_2D, 0, NofColors, Kreis1->w, Kreis1->h, 0,TextureFormat, GL_UNSIGNED_BYTE, Kreis1->pixels );
        gluBuild2DMipmaps(GL_TEXTURE_2D, NofColors, img_joy_button1->w, img_joy_button1->h,TextureFormat, GL_UNSIGNED_BYTE, img_joy_button1->pixels );

        if(glGetError() == GL_NO_ERROR)
            LogText("\tInitGrafik: Textur konnte erstellt werden.\n");
        else
            LogText("\tInitGrafik: Textur konnte nicht erstellt werden.\n");
    }

    // VSYNC
    if(enable_vsync)
        SDL_GL_SetSwapInterval(1);
    else
        SDL_GL_SetSwapInterval(0);

    /// VicRefresh wieder zulassen ///
    enable_hold_vic_refresh = false;

    LogText("\tInitGrafik: Vic-Refresh wurde wieder freigegeben.\n");
}

void C64Class::ChangeVSync()
{
    // VSYNC
    if(enable_vsync)
        SDL_GL_SetSwapInterval(1);
    else
        SDL_GL_SetSwapInterval(0);
}

void C64Class::ReleaseGrafik()
{
    changed_graphic_modi = false;

    /// VicRefresh stoppen und solange warten bis wirklich Stop ist ///
    enable_hold_vic_refresh = true;
    vic_refresh_is_holded = false;

    int timeout = 1000;
    while(!vic_refresh_is_holded && timeout > 0)
    {
          SDL_Delay(1);
          timeout--;
    }

    if(c64_screen != nullptr)
    {
       SDL_FreeSurface(c64_screen);
       c64_screen = nullptr;
    }

    if(sdl_window != nullptr)
    {
        SDL_DestroyWindow(sdl_window);
        sdl_window = nullptr;
    }

    if(c64_screen_buffer != nullptr)
    {
        delete[] c64_screen_buffer;
        c64_screen_buffer = nullptr;
    }
}

void C64Class::DrawC64Screen()
{
    /// Fensterinhalt löschen
    glClearColor(0.0f, 0.0f, 0.0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /// Texture auswählen und mit aktuellen C64 Bilddaten füllen
    glBindTexture(GL_TEXTURE_2D,c64_screen_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, current_c64_screen_width, current_c64_screen_height,GL_RGBA, GL_UNSIGNED_BYTE, c64_screen->pixels);

    /// Aspect Ratio berechnen
    float_t scale_x;
    float_t scale_y;

    if((enable_window_aspect_ratio == false && enable_fullscreen == false) || (enable_fullscreen_aspect_ratio == false && enable_fullscreen == true))
    {
        scale_x = 1.0f;
        scale_y = 1.0f;
    }
    else
    {
        if((static_cast<float_t>(current_window_width) / static_cast<float_t>(current_window_height)) >= screen_aspect_ratio)
        {
            scale_y = 1.0f;
            int xw = int(current_window_height * screen_aspect_ratio);
            scale_x = (1.0f * xw) / current_window_width;
        }
        else
        {
            scale_x = 1.0f;
            int yw = int(current_window_width / screen_aspect_ratio);
            scale_y = (1.0f * yw) / current_window_height;
        }
    }

    /// C64 Screen darstellen
    glBegin(GL_QUADS);
    for(int i=0; i < SUBDIVS_SCREEN*SUBDIVS_SCREEN*4; i+=4)
    {
        glTexCoord2f(distortion_grid_texture_coordinates[i+0].x,distortion_grid_texture_coordinates[i+0].y);
        glVertex3f(distortion_grid[i+0].x*scale_x,distortion_grid[i+0].y*scale_y,0.0);

        glTexCoord2f(distortion_grid_texture_coordinates[i+1].x,distortion_grid_texture_coordinates[i+1].y);
        glVertex3f(distortion_grid[i+1].x*scale_x,distortion_grid[i+1].y*scale_y,0.0);

        glTexCoord2f(distortion_grid_texture_coordinates[i+2].x,distortion_grid_texture_coordinates[i+2].y);
        glVertex3f(distortion_grid[i+2].x*scale_x,distortion_grid[i+2].y*scale_y,0.0);

        glTexCoord2f(distortion_grid_texture_coordinates[i+3].x,distortion_grid_texture_coordinates[i+3].y);
        glVertex3f(distortion_grid[i+3].x*scale_x,distortion_grid[i+3].y*scale_y,0.0);
    }
    glEnd();

    /// Pfeile für Joymapping darstenn
    if(rec_joy_mapping)
    {
        /// Nach Oben ///
        if(rec_joy_mapping_pos == 0) glBindTexture(GL_TEXTURE_2D,texture_joy_arrow1);
        else glBindTexture(GL_TEXTURE_2D,texture_joy_arrow0);
        glBegin(GL_QUADS);
        glTexCoord2i(1,0);
        glVertex3f(0.3f,1.0f,0.0f);
        glTexCoord2i(1,1);
        glVertex3f(0.3f,0.1f,0.0f);
        glTexCoord2i(0,1);
        glVertex3f(-0.3f,0.1f,0.0f);
        glTexCoord2i(0,0);
        glVertex3f(-0.3f,1.0f,0.0f);
        glEnd();

        /// Nach Unten ///
        if(rec_joy_mapping_pos == 1) glBindTexture(GL_TEXTURE_2D,texture_joy_arrow1);
        else glBindTexture(GL_TEXTURE_2D,texture_joy_arrow0);
        glBegin(GL_QUADS);
        glTexCoord2i(1,0);
        glVertex3f(0.3f,-1.0f,0.0f);
        glTexCoord2i(1,1);
        glVertex3f(0.3f,-0.1f,0.0f);
        glTexCoord2i(0,1);
        glVertex3f(-0.3f,-0.1f,0.0f);
        glTexCoord2i(0,0);
        glVertex3f(-0.3f,-1.0f,0.0f);
        glEnd();

        /// Nach Links ///
        if(rec_joy_mapping_pos == 2) glBindTexture(GL_TEXTURE_2D,texture_joy_arrow1);
        else glBindTexture(GL_TEXTURE_2D,texture_joy_arrow0);
        glBegin(GL_QUADS);
        glTexCoord2i(1,0);
        glVertex3f(-0.75f,-0.4f,0.0f);
        glTexCoord2i(1,1);
        glVertex3f(-0.08f,-0.4f,0.0f);
        glTexCoord2i(0,1);
        glVertex3f(-0.08f,0.4f,0.0f);
        glTexCoord2i(0,0);
        glVertex3f(-0.75f,0.4f,0.0f);
        glEnd();

        /// Nach Rechts ///
        if(rec_joy_mapping_pos == 3) glBindTexture(GL_TEXTURE_2D,texture_joy_arrow1);
        else glBindTexture(GL_TEXTURE_2D,texture_joy_arrow0);
        glBegin(GL_QUADS);
        glTexCoord2i(1,0);
        glVertex3f(0.75f,-0.4f,0.0f);
        glTexCoord2i(1,1);
        glVertex3f(0.08f,-0.4f,0.0f);
        glTexCoord2i(0,1);
        glVertex3f(0.08f,0.4f,0.0f);
        glTexCoord2i(0,0);
        glVertex3f(0.75f,0.4f,0.0f);
        glEnd();

        /// Fire ///
        if(rec_joy_mapping_pos == 4) glBindTexture(GL_TEXTURE_2D,texture_joy_button1);
        else glBindTexture(GL_TEXTURE_2D,texture_joy_button0);
        glBegin(GL_QUADS);
        glTexCoord2i(1,0);
        glVertex3f(0.11f,-0.15f,0.0f);
        glTexCoord2i(1,1);
        glVertex3f(0.11f,0.15f,0.0f);
        glTexCoord2i(0,1);
        glVertex3f(-0.11f,0.15f,0.0f);
        glTexCoord2i(0,0);
        glVertex3f(-0.11f,-0.15f,0.0f);
        glEnd();
    }

    SDL_GL_SwapWindow(sdl_window);
}

void C64Class::SetFocusToC64Window()
{
    SDL_RaiseWindow(sdl_window);
}

void C64Class::SetWindowAspectRatio(bool enable)
{
    enable_window_aspect_ratio = enable;
}

void C64Class::SetVSync(bool enable)
{
    enable_vsync = enable;
    changed_vsync = true;
}

void C64Class::SetFullscreenAspectRatio(bool enable)
{
    enable_fullscreen_aspect_ratio = enable;
}

void C64Class::AnalyzeSDLEvent(SDL_Event *event)
{
    static bool joy_center_flag = true;
    int8_t joy_axis_tbl[5] = {1,1,0,0,-1};

    SDL_Keymod keymod;

	bool is_hotkey = false;

    switch (event->type)
    {
        case SDL_WINDOWEVENT:

        switch (event->window.event)
        {
            case SDL_WINDOWEVENT_SIZE_CHANGED:
            case SDL_WINDOWEVENT_RESIZED:
                current_window_width = static_cast<uint16_t>(event->window.data1);
                current_window_height = static_cast<uint16_t>(event->window.data2);

                glViewport(0,0,current_window_width,current_window_height);
                glMatrixMode(GL_PROJECTION);
                glOrtho(0,current_window_width,current_window_height,0,-1,1);
                glLoadIdentity();
                break;

            default:
                break;
        }
        break;

        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
        {
            if((rec_joy_mapping == true) && (event->jbutton.type == SDL_JOYBUTTONDOWN))
            {
                virtual_joys[rec_joy_slot_nr].Type[rec_joy_mapping_pos] = VJOY_TYPE_BUTTON;
                virtual_joys[rec_joy_slot_nr].JoyIndex[rec_joy_mapping_pos] = static_cast<uint8_t>(event->jbutton.which);
                virtual_joys[rec_joy_slot_nr].ButtonNr[rec_joy_mapping_pos] = event->jbutton.button;
            }
            else if((rec_joy_mapping == true) && (event->jbutton.type == SDL_JOYBUTTONUP) && (rec_polling_wait == false))
            {
                rec_joy_mapping_pos++;
                if(rec_joy_mapping_pos == 5)
                {
                    /// Rec Mapping ist fertig ///
                    rec_joy_mapping = false;
                }
                else
                {
                    rec_polling_wait = true;
                    rec_polling_wait_counter = RecPollingWaitStart;
                }
            }
            else
            {
                /// VJoyStick abfrage ///
                /// Port1
                if((virtual_port1 >= 0) && (virtual_port1 < MAX_VJOY_NUM))
                {
                    for(int i=0;i<5;i++)
                    {
                        if((virtual_joys[virtual_port1].ButtonNr[i] == event->jbutton.button) &&
                                (virtual_joys[virtual_port1].Type[i] == VJOY_TYPE_BUTTON) &&
                                (virtual_joys[virtual_port1].JoyIndex[i] == event->jbutton.which))
                        {
                            if(event->jbutton.state == 1) game_port1 |= 1<<i;
                            else game_port1 &= ~(1<<i);
                        }
                    }
                }

                /// Port2
                if((virtual_port2 >= 0) && (virtual_port2 < MAX_VJOY_NUM))
                {
                    for(int i=0;i<5;i++)
                    {
                        if((virtual_joys[virtual_port2].ButtonNr[i] == event->jbutton.button) &&
                                (virtual_joys[virtual_port2].Type[i] == VJOY_TYPE_BUTTON) &&
                                (virtual_joys[virtual_port2].JoyIndex[i] == event->jbutton.which))
                        {
                            if(event->jbutton.state == 1) game_port2 |= 1<<i;
                            else game_port2 &= ~(1<<i);
                        }
                    }
                }
            }
            break;
        }

        case SDL_JOYHATMOTION:
        {
            if((rec_joy_mapping == true) && (rec_polling_wait == false))
            {
                if(event->jhat.value > 0)
                {
                    virtual_joys[rec_joy_slot_nr].Type[rec_joy_mapping_pos] = VJOY_TYPE_HAT;
                    virtual_joys[rec_joy_slot_nr].JoyIndex[rec_joy_mapping_pos] = static_cast<uint8_t>(event->jhat.which);
                    virtual_joys[rec_joy_slot_nr].HatNr[rec_joy_mapping_pos] = event->jhat.hat;
                    virtual_joys[rec_joy_slot_nr].HatValue[rec_joy_mapping_pos] = event->jhat.value;
                }
                else
                {
                    rec_joy_mapping_pos++;
                    if(rec_joy_mapping_pos == 5)
                    {
                        /// Rec Mapping ist fertig ///
                        rec_joy_mapping = false;
                    }
                    else
                    {
                        rec_polling_wait = true;
                        rec_polling_wait_counter = RecPollingWaitStart;
                    }
                }
            }
            else
            {
                /// VJoyStick abfrage ///
                /// Port1
                if((virtual_port1 >= 0) && (virtual_port1 < MAX_VJOY_NUM))
                {
                    for(int i=0;i<5;i++)
                    {
                        if((virtual_joys[virtual_port1].HatNr[i] == event->jhat.hat) &&
                                (virtual_joys[virtual_port1].Type[i] == VJOY_TYPE_HAT) &&
                                (virtual_joys[virtual_port1].JoyIndex[i] == event->jhat.which))
                        {
                            if((event->jhat.value & virtual_joys[virtual_port1].HatValue[i]) == virtual_joys[virtual_port1].HatValue[i]) game_port1 |= 1<<i;
                            else game_port1 &= ~(1<<i);
                        }
                    }
                }

                /// VJoyStick abfrage ///
                /// Port2
                if((virtual_port2 >= 0) && (virtual_port2 < MAX_VJOY_NUM))
                {
                    for(int i=0;i<5;i++)
                    {
                        if((virtual_joys[virtual_port2].HatNr[i] == event->jhat.hat) &&
                                (virtual_joys[virtual_port2].Type[i] == VJOY_TYPE_HAT) &&
                                (virtual_joys[virtual_port2].JoyIndex[i] == event->jhat.which))
                        {
                            if((event->jhat.value & virtual_joys[virtual_port2].HatValue[i]) == virtual_joys[virtual_port2].HatValue[i]) game_port2 |= 1<<i;
                            else game_port2 &= ~(1<<i);
                        }
                    }
                }

            }
          break;
        }

        case SDL_JOYAXISMOTION:
        {
            if((rec_joy_mapping == true) && (rec_polling_wait == false))
            {
                if(((event->jaxis.value < -16000) || (event->jaxis.value > 16000)) && (joy_center_flag == true) && ((event->jaxis.axis & 1) == joy_axis_tbl[rec_joy_mapping_pos]))
                {
                    joy_center_flag = false;

                    virtual_joys[rec_joy_slot_nr].Type[rec_joy_mapping_pos] = VJOY_TYPE_AXIS;
                    virtual_joys[rec_joy_slot_nr].JoyIndex[rec_joy_mapping_pos] = static_cast<uint8_t>(event->jaxis.which);
                    virtual_joys[rec_joy_slot_nr].AxisNr[rec_joy_mapping_pos] = event->jaxis.axis;
                    if(event->jaxis.value > 0) virtual_joys[rec_joy_slot_nr].AxisValue[rec_joy_mapping_pos] = 0;
                    else virtual_joys[rec_joy_slot_nr].AxisValue[rec_joy_mapping_pos] = 1;

                    rec_joy_mapping_pos++;
                    if(rec_joy_mapping_pos == 5)
                    {
                        /// Rec Mapping ist fertig ///
                        rec_joy_mapping = false;
                    }
                    else
                    {
                        rec_polling_wait = true;
                        rec_polling_wait_counter = RecPollingWaitStart;
                    }
                }

                if((event->jaxis.value > -14000) && (event->jaxis.value < 14000))
                {
                    joy_center_flag = true;
                }
            }
            else
            {
                /// VJoyStick abfrage ///
                /// Port1
                if((virtual_port1 >= 0) && (virtual_port1 < MAX_VJOY_NUM))
                {
                    for(int i=0;i<5;i++)
                    {
                        if((virtual_joys[virtual_port1].AxisNr[i] == event->jaxis.axis) &&
                                (virtual_joys[virtual_port1].Type[i] == VJOY_TYPE_AXIS) &&
                                (virtual_joys[virtual_port1].JoyIndex[i] == event->jaxis.which))
                        {
                            if(!((event->jaxis.value >= -14000) && (event->jaxis.value <= 14000)))
                            {
                                if(event->jaxis.value > 16000)
                                {
                                    if(virtual_joys[virtual_port1].AxisValue[i] == 0) game_port1 |= 1<<i;
                                }
                                else if(event->jaxis.value < -16000)
                                {
                                    if(virtual_joys[virtual_port1].AxisValue[i] == 1) game_port1 |= 1<<i;
                                }
                            }
                            else game_port1 &= ~(1<<i);
                        }
                    }
                }

                /// VJoyStick abfrage ///
                /// Port2
                if((virtual_port2 >= 0) && (virtual_port2 < MAX_VJOY_NUM))
                {
                    for(int i=0;i<5;i++)
                    {
                        if((virtual_joys[virtual_port2].AxisNr[i] == event->jaxis.axis) &&
                                (virtual_joys[virtual_port2].Type[i] == VJOY_TYPE_AXIS) &&
                                (virtual_joys[virtual_port2].JoyIndex[i] == event->jaxis.which))
                        {
                            if(!((event->jaxis.value >= -14000) && (event->jaxis.value <= 14000)))
                            {
                                if(event->jaxis.value > 16000)
                                {
                                    if(virtual_joys[virtual_port2].AxisValue[i] == 0) game_port2 |= 1<<i;
                                }
                                else if(event->jaxis.value < -16000)
                                {
                                    if(virtual_joys[virtual_port2].AxisValue[i] == 1) game_port2 |= 1<<i;
                                }
                            }
                            else game_port2 &= ~(1<<i);
                        }
                    }
                }
            }
          break;
        }

        case SDL_KEYDOWN:
        {
            switch(event->key.keysym.sym)
            {
            case SDLK_F11:
                SaveScreenshot();
                break;

            case SDLK_F12:
                if(rec_joy_mapping)
                {
                    rec_joy_mapping = false;
                }
                else
                {
                    if((KMOD_LSHIFT == (event->key.keysym.mod & KMOD_LSHIFT)) || (KMOD_RSHIFT == (event->key.keysym.mod & KMOD_RSHIFT)))
                    {
                        HardReset();
                        reset_wire = false;
                    }
                    else
                    {
                        wait_reset_ready = false;
                        reset_wire = false;
                    }
                }

                break;

			case SDLK_RETURN:

                keymod = SDL_GetModState();

                if((KMOD_MODE == (keymod & KMOD_MODE) || KMOD_LALT == (keymod & KMOD_LALT) || KMOD_RALT == (keymod & KMOD_RALT)) && (return_key_is_down == false))
                {
                    ToggleScreenMode();     // switching between fullscreen an window mode
                }

                return_key_is_down = true;
                break;

			case SDLK_j:				// HotKey [ALT+J] swap joyports
				// if ALT pressed down
				if(KMOD_LALT == (SDL_GetModState() & KMOD_LALT))
				{
					is_hotkey = true;
					SwapJoyPorts();
				}
				break;

			case SDLK_w:				// HotKey [ALT+W] toggle warpmode
				// if ALT pressed down
				if(KMOD_LALT == (SDL_GetModState() & KMOD_LALT))
				{
					is_hotkey = true;
					ToggleWarpMode();
				}
				break;

            default:
                break;
            }

            if(rec_joy_mapping == true)
            {
                virtual_joys[rec_joy_slot_nr].Type[rec_joy_mapping_pos] = VJOY_TYPE_KEY;
                virtual_joys[rec_joy_slot_nr].KeyDown[rec_joy_mapping_pos] = static_cast<uint8_t>(event->key.keysym.scancode);
            }
            else
            {
                if(key_map_is_rec)
                {
                    // Mappen der gedrückten Taste
                    for(int i=0;i<C64_KEY_COUNT;i++)
                    {
                        if(c64_key_table[i].SDLKeyCode == event->key.keysym.sym)
                        {
                            c64_key_table[i].SDLKeyCode = 0xFFFF;
                        }
                    }

                    c64_key_table[((rec_matrix_code & 0xF0)>>1) + (rec_matrix_code & 0x07)].SDLKeyCode = event->key.keysym.sym;
                    key_map_is_rec = false;
                }
				else if (!is_hotkey)	// nur wenn es kein HotKey war
                {
                    /// Auf RESTORE KEY TESTEN
                    if(c64_key_table[64].SDLKeyCode == event->key.keysym.sym)
                        cpu->TriggerInterrupt(RESTORE_NMI);
                    else
                    {
                        for(int i=0;i<C64_KEY_COUNT;i++)
                        {
                            if(c64_key_table[i].SDLKeyCode == event->key.keysym.sym && (event->key.keysym.mod != KMOD_LALT) && (event->key.keysym.mod != KMOD_RALT))
                            {
                                KeyEvent(c64_key_table[i].MatrixCode,KEY_DOWN,c64_key_table[i].Shift);
                            }
                        }
                    }
                }
            }

            /// VJoyStick abfrage ///
            /// Port1
            if((virtual_port1 >= 0) && (virtual_port1 < MAX_VJOY_NUM))
            {
                for(int i=0;i<5;i++)
                {
                    if((virtual_joys[virtual_port1].KeyDown[i] == event->key.keysym.scancode) &&
                            (virtual_joys[virtual_port1].Type[i] == VJOY_TYPE_KEY))
                        game_port1 |= 1<<i;
                }
            }

            /// Port2
            if((virtual_port2 >= 0) && (virtual_port2 < MAX_VJOY_NUM))
            {
                for(int i=0;i<5;i++)
                {
                    if((virtual_joys[virtual_port2].KeyDown[i] == event->key.keysym.scancode) &&
                            (virtual_joys[virtual_port2].Type[i] == VJOY_TYPE_KEY))
                        game_port2 |= 1<<i;
                }
            }
            break;
        }

        case SDL_KEYUP:
        {
            switch(event->key.keysym.sym)
            {

            case SDLK_RETURN:
                return_key_is_down = false;
                break;

            case SDLK_F12:
                reset_wire = true;
                break;

            default:
                break;
            }

            if(rec_joy_mapping == true)
            {
                virtual_joys[rec_joy_slot_nr].Type[rec_joy_mapping_pos] = VJOY_TYPE_KEY;
                virtual_joys[rec_joy_slot_nr].KeyUp[rec_joy_mapping_pos] = static_cast<uint8_t>(event->key.keysym.scancode);

                rec_joy_mapping_pos++;
                if(rec_joy_mapping_pos == 5)
                {
                    /// Rec Mapping ist fertig ///
                    rec_joy_mapping = false;
                }
            }
            else
            {
                /// Auf RESTORE KEY TESTEN
                if(c64_key_table[64].SDLKeyCode == event->key.keysym.sym)
                    cpu->ClearInterrupt(RESTORE_NMI);
                else
                {
                    for(int i=0;i<C64_KEY_COUNT;i++)
                    {
                        if(c64_key_table[i].SDLKeyCode == event->key.keysym.sym)
                        {
                            KeyEvent(c64_key_table[i].MatrixCode,KEY_UP,c64_key_table[i].Shift);
                        }
                    }
                }
            }

            /// VJoyStick abfrage ///
            /// Port1
            if((virtual_port1 >= 0) && (virtual_port1 < MAX_VJOY_NUM))
            {
                for(int i=0;i<5;i++)
                {
                    if((virtual_joys[virtual_port1].KeyUp[i] == event->key.keysym.scancode) &&
                            (virtual_joys[virtual_port1].Type[i] == VJOY_TYPE_KEY))
                        game_port1 &= ~(1<<i);
                }
            }

            /// Port2
            if((virtual_port2 >= 0) && (virtual_port2 < MAX_VJOY_NUM))
            {
                for(int i=0;i<5;i++)
                {
                    if((virtual_joys[virtual_port2].KeyUp[i] == event->key.keysym.scancode) &&
                            (virtual_joys[virtual_port2].Type[i] == VJOY_TYPE_KEY))
                        game_port2 &= ~(1<<i);
                }
            }
            break;
        }

        case SDL_MOUSEBUTTONDOWN:
            switch(event->button.button)
            {
            case SDL_BUTTON_LEFT:

                // Wenn Linke STRG Taste gedrück dann Mouse 1351 umschalten
				if(KMOD_LCTRL == (SDL_GetModState() & KMOD_LCTRL))
                {
                    enable_mouse_1351 = !enable_mouse_1351;
                    SDL_SetRelativeMouseMode(SDL_bool(enable_mouse_1351));
                    mouse_1351_x_rel = mouse_1351_y_rel = 0;
                }

                if(enable_mouse_1351)
                {
                    if(mouse_port == 0)
                        game_port1 |= 0x10;
                    else
                        game_port2 |= 0x10;
                }
                break;

            case SDL_BUTTON_RIGHT:

                if(enable_mouse_1351)
                {
                    if(mouse_port == 0)
                        game_port1 |= 0x01;
                    else
                        game_port2 |= 0x01;
                }
                break;

            default:
                break;
            }
            break;

    case SDL_MOUSEBUTTONUP:
        switch(event->button.button)
        {
        case SDL_BUTTON_LEFT:

            if(enable_mouse_1351)
            {
                if(mouse_port == 0)
                    game_port1 &= ~0x10;
                else game_port2 &= ~0x10;
            }
            break;

        case SDL_BUTTON_RIGHT:

            if(enable_mouse_1351)
            {
                if(mouse_port == 0)
                    game_port1 &= ~0x01;
                else game_port2 &= ~0x01;
            }
            break;

        default:
            break;
        }
        break;

        case SDL_MOUSEMOTION:

        if(!enable_fullscreen)
        {
            mouse_hide_counter = 0;
            mouse_is_hidden = false;
            SDL_ShowCursor(true);
        }

        if(enable_mouse_1351)
        {
            mouse_1351_x_rel += event->motion.xrel;
            mouse_1351_y_rel += event->motion.yrel;
        }

        break;

        case SDL_QUIT:
            if(CloseEventC64Screen != nullptr) CloseEventC64Screen();

#ifdef _WIN32
        //LoopThreadEnd = true;
#endif
        break;

        default:
            break;
    }
}

void C64Class::SetDistortion(float_t value)
{
    distortion_value = value;
    CalcDistortionGrid();
}

void C64Class::SetMouseHiddenTime(int time)
{
    mouse_hide_time = time;
}

void C64Class::GetWindowPos(int *x, int *y)
{
    SDL_GetWindowPosition(sdl_window, x, y);
}

void C64Class::SetWindowPos(int x, int y)
{
    sdl_window_pos_x = x;
    sdl_window_pos_y = y;
    changed_window_pos = true;
}

void C64Class::GetWindowSize(int *w, int *h)
{
    SDL_GetWindowSize(sdl_window, w, h);
}

void C64Class::SetWindowSize(int w, int h)
{
    sdl_window_size_width = w;
    sdl_window_size_height = h;
	changed_window_size = true;
}

int C64Class::GetNumDisplays()
{
	return SDL_GetNumVideoDisplays();
}

const char *C64Class::GetDisplayName(int display_index)
{
	return SDL_GetDisplayName(display_index);
}

int C64Class::GetNumDisplayModes(int display_index)
{
	return SDL_GetNumDisplayModes(display_index);
}

int C64Class::GetDisplayMode(int display_index, int mode_index, int &w, int &h, int &refresh_rate, uint32_t &format)
{
	SDL_DisplayMode display_mode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0 };

	if(0 == SDL_GetDisplayMode(display_index, mode_index, &display_mode))
	{
		w = display_mode.w;
		h = display_mode.h;
		refresh_rate = display_mode.refresh_rate;
		format = display_mode.format;
	}
	else
	{
		w = 0;
		h = 0;
		refresh_rate = 0;
		format = 0;
	}
	return 0;
}

void C64Class::SetFullscreenDisplayMode(int display_index, int mode_index)
{
	if(display_index >= MAX_VIDEO_DISPLAYS || mode_index < 0)
		return;

	SDL_DisplayMode display_mode;

	if(0 == SDL_GetDisplayMode(display_index, mode_index, &display_mode))
	{
		fullscreen_display_mode[display_index] = display_mode;
	}

	//cout << "SetDisplayMode: " << display_index << ";" << mode_index << endl;
}

bool C64Class::LoadTapeImage(const char *filename)
{
    return tape->LoadTapeImage(filename);
}

bool C64Class::RecordTapeImage(const char *filename)
{
    return tape->RecordTapeImage(filename);
}

uint8_t C64Class::SetTapeKeys(uint8_t pressed_key)
{
    return tape->SetTapeKeys(pressed_key);
}

bool C64Class::GetTapeMotorStatus()
{
    return !(cpu_port.DATA_READ & 32);
}

bool C64Class::GetTapeRecordLedStatus()
{
    if((!(cpu_port.DATA_READ & 32)) && tape->IsPressedRecord()) return true;
    else return false;
}

uint32_t C64Class::GetTapeCounter()
{
    return tape->GetCounter();
}

float_t C64Class::GetTapeLenTime()
{
    return tape->GetTapeLenTime();
}

uint32_t C64Class::GetTapeLenCount()
{
    return tape->GetTapeLenCount();
}

void C64Class::SetTapeSoundVolume(float_t volume)
{
    tape->SetTapeSoundVolume(volume);
}

void C64Class::SetC64Frequency(int c64_frequency)
{
    this->c64_frequency = c64_frequency;
    SetC64Speed(c64_speed);
}

void C64Class::SetC64Speed(int speed)
{
    c64_speed = speed;
    sid1->SetC64Zyklen(c64_frequency*(speed/100.f));
    sid2->SetC64Zyklen(c64_frequency*(speed/100.f));
    tape->SetC64Zyklen(c64_frequency*(speed/100.f));
}

void C64Class::EnableWarpMode(bool enabled)
{
    if(enabled == warp_mode) return;

    // WarpMode setzen
    warp_mode = enabled;

    if(warp_mode)
    {
        // WarpMode aktivieren
        SDL_PauseAudioDevice(audio_dev, 1);     // Audiostream pausieren
        SDL_LockMutex(mutex1);      // Warten auf Mutex1 und sperren
        warp_thread_end = false;
        warp_thread = SDL_CreateThread(SDLThreadWarp,"WarpThread",this);
    }
    else
    {
        // WarpMode deaktivieren
		warp_thread_end = true;
        SDL_UnlockMutex(mutex1);    // Mutex1 wieder freigeben
        SDL_PauseAudioDevice(audio_dev, 0);     // Audiostream wieder starten
	}
}

void C64Class::ToggleWarpMode()
{
	if(warp_mode == true)
		EnableWarpMode(false);
	else
		EnableWarpMode(true);
}

bool C64Class::IsWarpMode()
{
	return warp_mode;
}

int SDLThreadLoad(void *userdat)
{
    uint16_t PRGStartAdresse;
    C64Class *c64 = static_cast<C64Class*>(userdat);

    switch(c64->auto_load_mode)
    {
    case 0:
        c64->SetCommandLine(c64->auto_load_command_line);
        break;
    case 1:
        c64->LoadPRG(c64->auto_load_filename,&PRGStartAdresse);
        if(PRGStartAdresse <= 0x0801) sprintf(c64->auto_load_command_line,"RUN%c",13);
        else sprintf(c64->auto_load_command_line,"SYS %d%c",PRGStartAdresse,13);
        c64->SetCommandLine(c64->auto_load_command_line);
        break;
    case 2:
        c64->LoadPRG(c64->auto_load_filename,&PRGStartAdresse);
        //if(LoadPRG(c64->AutoLoadFilename,&PRGStartAdresse) == 5) return 4; Behandlung wenn mehr als 1 File in T64
        if(PRGStartAdresse <= 0x0801) sprintf(c64->auto_load_command_line,"RUN%c",13);
        else sprintf(c64->auto_load_command_line,"SYS %d%c",PRGStartAdresse,13);
        c64->SetCommandLine(c64->auto_load_command_line);
        break;
    }
    return 0;
}

// ret 0=OK 1=nicht unterstütztes Format 2=D64 n.IO 3=G64 n.IO 4=OK nur es war ein CRT
int C64Class::LoadAutoRun(uint8_t floppy_nr, const char *filename)
{
    char EXT[4];

    size_t len = strlen(filename);
    strcpy(EXT,filename+len-3);

    EXT[0]=static_cast<char>(toupper(EXT[0]));
    EXT[1]=static_cast<char>(toupper(EXT[1]));
    EXT[2]=static_cast<char>(toupper(EXT[2]));

    if(0==strcmp("D64",EXT))
    {
		/*
		if(!LoadDiskImage(floppy_nr,filename)) return 2;

        KillCommandLine();
        auto_load_mode = 0;
        sprintf(auto_load_command_line,"LOAD\"*\",%d,1%cRUN%c",floppy_nr+8,13,13);
        HardReset();
        wait_reset_ready = true;
        c64_reset_ready = false;
        floppy_reset_ready[0] = false;
		*/
        return 0;
    }

    if(0==strcmp("G64",EXT))
    {
		/*
        if(!LoadDiskImage(floppy_nr,filename)) return 3;

        KillCommandLine();
        auto_load_mode = 0;
        sprintf(auto_load_command_line,"LOAD\"*\",%d,1%cRUN%c",floppy_nr+8,13,13);
        HardReset();
        wait_reset_ready = true;
        c64_reset_ready = false;
        floppy_reset_ready[0] = false;
		*/
        return 0;
    }

    if(0==strcmp("PRG",EXT) || 0==strcmp("C64",EXT))
    {
        KillCommandLine();
        auto_load_mode = 1;
        strcpy(auto_load_filename,filename);
        HardReset();
        wait_reset_ready = true;
        c64_reset_ready = false;
        floppy_reset_ready[0] = false;
        return 0;
    }

    if(0==strcmp("T64",EXT))
    {
        KillCommandLine();
        auto_load_mode = 2;
        strcpy(auto_load_filename,filename);
        HardReset();
        wait_reset_ready = true;
        c64_reset_ready = false;
        floppy_reset_ready[0] = false;
        return 0;
    }

    if(0==strcmp("P00",EXT))
    {
        KillCommandLine();
        auto_load_mode = 1;
        strcpy(auto_load_filename,filename);
        HardReset();
        wait_reset_ready = true;
        c64_reset_ready = false;
        floppy_reset_ready[0] = false;
        return 0;
    }

    if(0==strcmp("FRZ",EXT))
    {
        KillCommandLine();

        //LoadFreez(filename,FreezReturn);
        return 0;
    }

    if(0==strcmp("CRT",EXT))
    {
        KillCommandLine();

		// LoadCRT(filename);
        return 4;
    }
    return 1;
}

int C64Class::LoadPRG(const char *filename, uint16_t *return_start_address)
{
    uint8_t *RAM = mmu->GetRAMPointer();
    FILE *file;
    char EXT[4];

    size_t len = strlen(filename);
    strcpy(EXT,filename+len-3);

    EXT[0]=static_cast<char>(toupper(EXT[0]));
    EXT[1]=static_cast<char>(toupper(EXT[1]));
    EXT[2]=static_cast<char>(toupper(EXT[2]));

    char str00[256];

    if(0==strcmp("PRG",EXT) || 0==strcmp("C64",EXT))
    {
        LogText(const_cast<char*>(">> PRG laden: "));
        LogText(const_cast<char*>(filename));
        LogText(const_cast<char*>("\n"));

        uint16_t start_address;
        size_t reading_bytes;
        uint8_t temp[2];
        file = fopen (filename, "rb");
        if (file == nullptr)
        {
            LogText(const_cast<char*>("<< ERROR: Datei konnte nicht geöffnet werden\n"));
            return 0x01;
        }
        reading_bytes = fread (&temp,1,2,file);
        start_address = static_cast<uint16_t>(temp[0]|(temp[1]<<8));
        if(return_start_address != nullptr) *return_start_address = start_address;

        reading_bytes=fread (RAM+start_address,1,0xFFFF-start_address,file);

        RAM[0x2B] = 0x01;
        RAM[0x2C] = 0x08;

        sprintf(str00,">>   SartAdresse: $%4.4X(%d)",start_address,start_address);
        LogText(str00);

        start_address += static_cast<uint16_t>(reading_bytes);
        RAM[0x2D] = static_cast<uint8_t>(start_address);
        RAM[0x2E] = static_cast<uint8_t>(start_address>>8);
        RAM[0xAE] = static_cast<uint8_t>(start_address);
        RAM[0xAF] = static_cast<uint8_t>(start_address>>8);

        fclose(file);

        sprintf(str00,"EndAdresse: $%4.4X(%d)\n", start_address, start_address);
        LogText(str00);

        return 0x00;
    }

    if(0==strcmp("T64",EXT))
    {
        LogText(const_cast<char*>(">> T64 laden: "));
        LogText(const_cast<char*>(filename));
        LogText(const_cast<char*>("\n"));

        char signature[32];
        size_t reading_bytes;
        uint16_t T64Entries;
        uint16_t start_address;
        uint16_t end_address;
        int FileStartOffset;

        file = fopen (filename, "rb");
        if (file == nullptr)
        {
            LogText(const_cast<char*>("<< ERROR: Datei konnte nicht geöffnet werden"));
                return 0x01;
        }

        reading_bytes = fread(signature,1,32,file);
        if(reading_bytes != 32)
        {
            cout << "Error T64 0x02" << endl;
            fclose(file);
            return 0x02;
        }

        // Es gibt irgendwie zu viele verschiedene Kennungen :(
        /*
        if((strcmp(Kennung, "C64 tape image file") != 0) && (strcmp(Kennung, "C64S tape image file") != 0))
        {
            cout << "Error T64 0x03" << endl;
            fclose(file);
            return 0x03;
        }
        */

        fseek(file,4,SEEK_CUR);
        reading_bytes = fread(&T64Entries,1,2,file);

        if(T64Entries==0)
        {
                fclose(file);
                return 0x04;
        }

        /*
        if(T64Entries>1)
        {
                fclose(file);
                return 0x05;
        }
        */

        fseek(file,0x42,SEEK_SET);
        reading_bytes = fread(&start_address,1,2,file);
        if(return_start_address != nullptr) *return_start_address = start_address;
        reading_bytes = fread(&end_address,1,2,file);
        fseek(file,2,SEEK_CUR);
        reading_bytes = fread(&FileStartOffset,1,4,file);

        fseek(file,FileStartOffset,SEEK_SET);
        reading_bytes = fread(RAM + start_address,1,end_address - start_address,file);
        fclose(file);

        RAM[0x2B] = 0x01;
        RAM[0x2C] = 0x08;

        RAM[0x2D] = static_cast<uint8_t>(end_address);
        RAM[0x2E] = static_cast<uint8_t>(end_address>>8);
        RAM[0xAE] = static_cast<uint8_t>(end_address);
        RAM[0xAF] = static_cast<uint8_t>(end_address>>8);

        sprintf(str00,">>   SartAdresse: $%4.4X(%d)", start_address, start_address);
        LogText(str00);
        sprintf(str00,"EndAdresse: $%4.4X(%d)\n", end_address, end_address);
        LogText(str00);

        return 0x00;
    }

    if(0==strcmp("P00",EXT))
    {
        LogText(const_cast<char*>(">> P00 laden: "));
        LogText(const_cast<char*>(filename));
        LogText(const_cast<char*>("\n"));

        char signature[8];
        uint16_t start_address;
        size_t reading_bytes;
        uint8_t temp[2];
        file = fopen (filename, "rb");
        if (file == nullptr)
        {
            LogText(const_cast<char*>("<< ERROR: Datei konnte nicht geöffnet werden"));
                return 0x01;
        }

        reading_bytes = fread(signature,1,7,file);
        signature[7]=0;
        if(0!=strcmp("C64File",signature))
        {
                fclose(file);
                return 0x06;
        }

        fseek(file,0x1A,SEEK_SET);

        reading_bytes = fread (&temp,1,2,file);
        start_address = static_cast<uint16_t>(temp[0]|(temp[1]<<8));
        if(return_start_address != nullptr) *return_start_address = start_address;

        reading_bytes=fread (RAM+start_address,1,0xFFFF,file);

        RAM[0x2B] = 0x01;
        RAM[0x2C] = 0x08;

        sprintf(str00,">>   SartAdresse: $%4.4X(%d)",start_address,start_address);
        LogText(str00);

        start_address += static_cast<uint16_t>(reading_bytes);
        RAM[0x2D] = static_cast<uint8_t>(start_address);
        RAM[0x2E] = static_cast<uint8_t>(start_address>>8);
        RAM[0xAE] = static_cast<uint8_t>(start_address);
        RAM[0xAF] = static_cast<uint8_t>(start_address>>8);

        fclose(file);

        sprintf(str00,"EndAdresse: $%4.4X(%d)\n",start_address,start_address);
        LogText(str00);

        return 0x00;
    }
    return 0x02;
}

int C64Class::LoadCRT(FILE *file)
{
    reu->Remove();
    geo->Remove();

	int ret = crt->LoadCartridgeImage(file);
    if(ret == 0)
    {
        io_source = 1;

        KillCommandLine();
        HardReset();
    }
    return ret;
}

void C64Class::RemoveCRT()
{
    crt->RemoveCartridgeImage();
    io_source = 0;
    KillCommandLine();
    HardReset();
}

int C64Class::CreateNewEasyFlashImage(FILE *file, const char *crt_name)
{
	return crt->CreateNewEasyFlashImage(file, crt_name);
}

void C64Class::InsertREU()
{
    io_source = 2;

    crt->RemoveCartridgeImage();
    geo->Remove();

    reu->Insert();
    reu_is_insert = true;

    KillCommandLine();
    HardReset();
}

void C64Class::RemoveREU()
{
    reu->Remove();
    io_source = 0;

    reu_is_insert = false;

    KillCommandLine();
    HardReset();
}

int C64Class::LoadREUImage(const char *filename)
{
    return reu->LoadRAM(filename);
}

int C64Class::SaveREUImage(const char *filename)
{
    return reu->SaveRAM(filename);
}

void C64Class::ClearREURam()
{
    reu->ClearRAM();
}

void C64Class::InsertGEORAM()
{
    io_source = 3;

    crt->RemoveCartridgeImage();
    reu->Remove();
    reu_is_insert = false;

    geo->Insert();

    KillCommandLine();
    HardReset();
}

void C64Class::RemoveGEORAM()
{
    geo->Remove();
    io_source = 0;

    KillCommandLine();
    HardReset();
}

int C64Class::LoadGEORAMImage(const char *filename)
{
    return geo->LoadRAM(filename);
}

int C64Class::SaveGEORAMImage(const char *filename)
{
    return geo->SaveRAM(filename);
}

void C64Class::ClearGEORAMRam()
{
    geo->ClearRAM();
}

void C64Class::SetMouse1351Port(uint8_t port)
{
    mouse_port = port;
}

void C64Class::SetDebugMode(bool status)
{
    debug_mode = status;
    if(debug_mode)
    {
        one_cycle = false;
        one_opcode = false;
        sid1->SoundOutputEnable = false;
        sid2->SoundOutputEnable = false;
        for(int i=0; i<MAX_FLOPPY_NUM; i++) floppy[i]->SetEnableFloppySound(false);
    }
    else
    {
        one_cycle = false;
        one_opcode = false;
        sid1->SoundOutputEnable = true;
        sid2->SoundOutputEnable = true;
        for(int i=0; i<MAX_FLOPPY_NUM; i++) floppy[i]->SetEnableFloppySound(true);
    }
}

void C64Class::SetCpuExtLines(bool status)
{
    enable_ext_wires = status;
    if(status)
    {
        cpu->ClearInterrupt(CIA_IRQ);
        cpu->ClearInterrupt(CIA_NMI);
        cpu->ClearInterrupt(REU_IRQ);
        cpu->ClearInterrupt(VIC_IRQ);
        cpu->ClearInterrupt(CRT_NMI);
    }
    cpu->EnableExtInterrupts = status;
}

void C64Class::SetExtRDY(bool status)
{
    ext_rdy_wire = status;
}

void C64Class::OneCycle()
{
    debug_animation = false;
    one_cycle = true;
}

void C64Class::OneOpcode(int source)
{
    debug_animation = false;
    one_opcode_source = source;
    one_opcode = true;
}

void C64Class::SetDebugAnimation(bool status)
{
    debug_animation = status;
}

void C64Class::SetDebugAnimationSpeed(int cycle_sek)
{
    animation_speed_counter = 0;
    animation_speed_add = static_cast<float_t>(audio_spec_have.samples) / static_cast<float_t>(audio_frequency) * static_cast<float_t>(cycle_sek);
}

void C64Class::GetC64CpuReg(REG_STRUCT *reg, IREG_STRUCT *ireg)
{
    cpu->GetInterneRegister(ireg);
    ireg->cycle_counter = cycle_counter;
    ireg->game = game_wire;
    ireg->exrom = exrom_wire;
    cpu->GetRegister(reg);
}

void C64Class::GetVicReg(VIC_STRUCT *vic_reg)
{
    vic->GetRegister(vic_reg);
    vic_reg->irq = cpu->GetInterrupts(VIC_IRQ);
}

void C64Class::GetIECStatus(IEC_STRUCT *iec)
{
    iec->ATN_OUT = !!(c64_iec_wire & 16);
    iec->CLOCK_OUT = !!(c64_iec_wire & 64);
    iec->DATA_OUT = !!(c64_iec_wire & 128);
    iec->CLOCK_IN = !!(floppy_iec_wire & 64);
    iec->DATA_IN = !!(floppy_iec_wire & 128);
}

int C64Class::AddBreakGroup()
{
    if(breakgroup_count == MAX_BREAK_GROUP_NUM) return -1;

    breakgroup[breakgroup_count] = new BREAK_GROUP;
    memset(breakgroup[breakgroup_count],0,sizeof(BREAK_GROUP));
    breakgroup[breakgroup_count]->iRZZyklus = 1;
    breakgroup_count ++;
    return breakgroup_count - 1;
}

void C64Class::DelBreakGroup(int index)
{
    if(index >= breakgroup_count) return;
    delete breakgroup[index];
    breakgroup_count--;
    for(int i = index; i < breakgroup_count; i++) breakgroup[i] = breakgroup[i+1];
    UpdateBreakGroup();
}

BREAK_GROUP* C64Class::GetBreakGroup(int index)
{
    if(index >= breakgroup_count) return nullptr;
    return breakgroup[index];
}

void C64Class::UpdateBreakGroup()
{
    for(int i=0; i<0x10000;i++) breakpoints[i] = 0;
    for(int i=0;i<breakgroup_count;i++)
    {
        BREAK_GROUP* bg = breakgroup[i];
        if(bg->Enable)
        {
            if(bg->bPC) breakpoints[bg->iPC] |= 1;
            if(bg->bAC) breakpoints[bg->iAC] |= 2;
            if(bg->bXR) breakpoints[bg->iXR] |= 4;
            if(bg->bYR) breakpoints[bg->iYR] |= 8;
            if(bg->bRAdresse) breakpoints[bg->iRAdresse] |= 16;
            if(bg->bWAdresse) breakpoints[bg->iWAdresse] |= 32;
            if(bg->bRWert) breakpoints[bg->iRWert] |= 64;
            if(bg->bWWert) breakpoints[bg->iWWert] |= 128;
            if(bg->bRZ) breakpoints[bg->iRZ] |= 256;
            if(bg->bRZZyklus) breakpoints[bg->iRZZyklus] |= 512;
        }
    }

    for(int i=0;i<MAX_FLOPPY_NUM;i++)
    {
        if(floppy[i]->GetEnableFloppy())
        {
            floppy[i]->UpdateBreakGroup();
        }
    }
}

void C64Class::DeleteAllBreakGroups()
{
    for(int i=0;i<breakgroup_count;i++)
    {
        delete breakgroup[i];
    }
    breakgroup_count = 0;
    UpdateBreakGroup();
}

int C64Class::GetBreakGroupCount()
{
    return breakgroup_count;
}

int C64Class::LoadBreakGroups(const char *filename)
{
    FILE *file;
    char Kennung[10];
    uint8_t Version;
    uint8_t Groupanzahl;
    size_t reading_elements;

    DeleteAllBreakGroups();

    file = fopen (filename, "rb");
    if (file == nullptr)
    {
            /// Datei konnte nicht geöffnet werden ///
            return -1;
    }

    /// Kennung ///
    reading_elements = fread(Kennung,sizeof(Kennung),1,file);

    if(0 != strcmp("EMU64_BPT",Kennung))
    {
        /// Kein Emu64 Format ///
        fclose(file);
        return -2;
    }

    /// Version ///
    reading_elements = fread(&Version,sizeof(Version),1,file);

    if(Version > 1) return -3;

    /// Groupanzahl ///
    reading_elements = fread(&Groupanzahl,sizeof(Groupanzahl),1,file);
    if(reading_elements != 1)
        return -5;

    if(Groupanzahl == 0) return -4;

    /// Groups ///
    for(int ii=0;ii<Groupanzahl;ii++)
    {
        int i = AddBreakGroup();
        reading_elements = fread(breakgroup[i]->Name,sizeof(breakgroup[i]->Name),1,file);
        reading_elements = fread(&breakgroup[i]->Enable,sizeof(breakgroup[i]->Enable),1,file);
        reading_elements = fread(&breakgroup[i]->bPC,sizeof(breakgroup[i]->bPC),1,file);
        reading_elements = fread(&breakgroup[i]->iPC,sizeof(breakgroup[i]->iPC),1,file);
        reading_elements = fread(&breakgroup[i]->bAC,sizeof(breakgroup[i]->bAC),1,file);
        reading_elements = fread(&breakgroup[i]->iAC,sizeof(breakgroup[i]->iAC),1,file);
        reading_elements = fread(&breakgroup[i]->bXR,sizeof(breakgroup[i]->bXR),1,file);
        reading_elements = fread(&breakgroup[i]->iXR,sizeof(breakgroup[i]->iXR),1,file);
        reading_elements = fread(&breakgroup[i]->bYR,sizeof(breakgroup[i]->bYR),1,file);
        reading_elements = fread(&breakgroup[i]->iYR,sizeof(breakgroup[i]->iYR),1,file);
        reading_elements = fread(&breakgroup[i]->bRAdresse,sizeof(breakgroup[i]->bRAdresse),1,file);
        reading_elements = fread(&breakgroup[i]->iRAdresse,sizeof(breakgroup[i]->iRAdresse),1,file);
        reading_elements = fread(&breakgroup[i]->bWAdresse,sizeof(breakgroup[i]->bWAdresse),1,file);
        reading_elements = fread(&breakgroup[i]->iWAdresse,sizeof(breakgroup[i]->iWAdresse),1,file);
        reading_elements = fread(&breakgroup[i]->bRWert,sizeof(breakgroup[i]->bRWert),1,file);
        reading_elements = fread(&breakgroup[i]->iRWert,sizeof(breakgroup[i]->iRWert),1,file);
        reading_elements = fread(&breakgroup[i]->bWWert,sizeof(breakgroup[i]->bWWert),1,file);
        reading_elements = fread(&breakgroup[i]->iWWert,sizeof(breakgroup[i]->iWWert),1,file);
        reading_elements = fread(&breakgroup[i]->bRZ,sizeof(breakgroup[i]->bRZ),1,file);
        reading_elements = fread(&breakgroup[i]->iRZ,sizeof(breakgroup[i]->iRZ),1,file);
        reading_elements = fread(&breakgroup[i]->bRZZyklus,sizeof(breakgroup[i]->bRZZyklus),1,file);
        reading_elements = fread(&breakgroup[i]->iRZZyklus,sizeof(breakgroup[i]->iRZZyklus),1,file);
    }
    return 0;
}

bool C64Class::SaveBreakGroups(const char *filename)
{
    FILE *file;
    char Kennung[]  = "EMU64_BPT";
    uint8_t Version    = 1;

    file = fopen (filename, "wb");
    if (file == nullptr)
    {
            return false;
    }

    /// Kennung ///
    fwrite(Kennung,sizeof(Kennung),1,file);

    /// Version ///
    fwrite(&Version,sizeof(Version),1,file);

    /// Groupanzahl ///
    fwrite(&breakgroup_count,sizeof(breakgroup_count),1,file);

    /// Groups ///
    for(int i=0;i<breakgroup_count;i++)
    {
        fwrite(breakgroup[i]->Name,sizeof(breakgroup[i]->Name),1,file);
        fwrite(&breakgroup[i]->Enable,sizeof(breakgroup[i]->Enable),1,file);
        fwrite(&breakgroup[i]->bPC,sizeof(breakgroup[i]->bPC),1,file);
        fwrite(&breakgroup[i]->iPC,sizeof(breakgroup[i]->iPC),1,file);
        fwrite(&breakgroup[i]->bAC,sizeof(breakgroup[i]->bAC),1,file);
        fwrite(&breakgroup[i]->iAC,sizeof(breakgroup[i]->iAC),1,file);
        fwrite(&breakgroup[i]->bXR,sizeof(breakgroup[i]->bXR),1,file);
        fwrite(&breakgroup[i]->iXR,sizeof(breakgroup[i]->iXR),1,file);
        fwrite(&breakgroup[i]->bYR,sizeof(breakgroup[i]->bYR),1,file);
        fwrite(&breakgroup[i]->iYR,sizeof(breakgroup[i]->iYR),1,file);
        fwrite(&breakgroup[i]->bRAdresse,sizeof(breakgroup[i]->bRAdresse),1,file);
        fwrite(&breakgroup[i]->iRAdresse,sizeof(breakgroup[i]->iRAdresse),1,file);
        fwrite(&breakgroup[i]->bWAdresse,sizeof(breakgroup[i]->bWAdresse),1,file);
        fwrite(&breakgroup[i]->iWAdresse,sizeof(breakgroup[i]->iWAdresse),1,file);
        fwrite(&breakgroup[i]->bRWert,sizeof(breakgroup[i]->bRWert),1,file);
        fwrite(&breakgroup[i]->iRWert,sizeof(breakgroup[i]->iRWert),1,file);
        fwrite(&breakgroup[i]->bWWert,sizeof(breakgroup[i]->bWWert),1,file);
        fwrite(&breakgroup[i]->iWWert,sizeof(breakgroup[i]->iWWert),1,file);
        fwrite(&breakgroup[i]->bRZ,sizeof(breakgroup[i]->bRZ),1,file);
        fwrite(&breakgroup[i]->iRZ,sizeof(breakgroup[i]->iRZ),1,file);
        fwrite(&breakgroup[i]->bRZZyklus,sizeof(breakgroup[i]->bRZZyklus),1,file);
        fwrite(&breakgroup[i]->iRZZyklus,sizeof(breakgroup[i]->iRZZyklus),1,file);
    }

    fclose(file);
    return true;
}

bool C64Class::ExportPRG(const char *filename, uint16_t start_adresse, uint16_t end_adresse, int source)
{
    FILE *file;
    uint8_t *RAM;

    if(source > 0)
    {
        RAM = floppy[source-1]->GetRamPointer();
    }
    else RAM = mmu->GetRAMPointer();

    if(RAM == nullptr) return false;

    file = fopen (filename, "wb");
    if (file == nullptr)
    {
        return false;
    }
    fwrite(&start_adresse,sizeof(start_adresse),1,file);
    for(int i=start_adresse;i<end_adresse;i++) fwrite(&RAM[i],1,1,file);
    fclose(file);

    return true;
}

bool C64Class::ExportRAW(char *filename, uint16_t start_adresse, uint16_t end_adresse, int source)
{
    FILE *file;
    uint8_t *RAM;

    if(source > 0)
    {
        RAM = floppy[source-1]->GetRamPointer();
    }
    else RAM = mmu->GetRAMPointer();

    if(RAM == nullptr) return false;

    file = fopen (filename, "wb");
    if (file == nullptr)
    {
        return false;
    }
    for(int i=start_adresse;i<end_adresse;i++) fwrite(&RAM[i],1,1,file);
    fclose(file);

    return true;
}

bool C64Class::ExportASM(const char *filename, uint16_t start_adresse, uint16_t end_adresse, int source)
{
    FILE *file;

    file = fopen (filename, "w");
    if (file == nullptr)
    {
        return false;
    }

    char *source_str = const_cast<char*>("");

    switch(source)
    {
    case 0:
        source_str = const_cast<char*>("C64");
        break;
    case 1:
        source_str = const_cast<char*>("Floppy #08");
        break;
    case 2:
        source_str = const_cast<char*>("Floppy #09");
        break;
    case 3:
        source_str = const_cast<char*>("Floppy #10");
        break;
    case 4:
        source_str = const_cast<char*>("Floppy #11");
        break;
    }

    fprintf(file,"Disassembler Listing wurde mit Emu64 %s erzeugt\n",VERSION_STRING);
    fprintf(file,"-------------------------------------------------\n");
    fprintf(file,"Von: $%4.4X Bis: $%4.4X Quelle: %s",start_adresse,end_adresse,source_str);
    fprintf(file,"\n");
    fprintf(file,"\n");

    uint16_t pc = start_adresse;
L10:
    pc = DisAss(file,pc,true,source);
    if(pc < end_adresse) goto L10;

    fclose(file);

    return true;
}

uint8_t C64Class::GetMapReadSource(uint8_t page)
{
    return mmu->GetReadSource(page);
}

uint8_t C64Class::GetMapWriteDestination(uint8_t page)
{
    return mmu->GetWriteDestination(page);
}

void C64Class::SetScreenshotNumber(uint32_t number)
{
    screenshot_number = number;
}

uint32_t C64Class::GetScreenshotNumber()
{
    return screenshot_number;
}

void C64Class::SetScreenshotFormat(uint8_t format)
{
    screenshot_format = format;
}

void C64Class::SetScreenshotDir(const char *screenshot_dir)
{
    if(this->screenshot_dir != nullptr)
    {
        delete [] this->screenshot_dir;
        this->screenshot_dir = nullptr;
    }

    this->screenshot_dir = new char[strlen(screenshot_dir)+1];

#ifdef __STDC_LIB_EXT1__
    strcpy_s(this->screenshot_dir, static_cast<size_t>(strlen(screenshot_dir))+1, screenshot_dir);
#else
    strcpy(this->screenshot_dir, screenshot_dir);
#endif
}

void C64Class::EnableScreenshots(bool is_enable)
{
    is_screenshot_enable = is_enable;
}

void C64Class::SaveScreenshot()
{
    if(!is_screenshot_enable) return;

    cout << "Screenshot Nummer: " << screenshot_number << endl;

    switch (screenshot_format)
    {
    case SCREENSHOT_FORMAT_BMP: // .bmp
        sprintf(screenshot_filename,"%s/emu64_%d.bmp",screenshot_dir,screenshot_number);
        break;
    case SCREENSHOT_FORMAT_PNG: // .png
        sprintf(screenshot_filename,"%s/emu64_%d.png",screenshot_dir,screenshot_number);
        break;
    default:
        sprintf(screenshot_filename,"%s/emu64_%d.png",screenshot_dir,screenshot_number);
        break;
    }

    screenshot_number++;
    start_screenshot = true;
}

const char *C64Class::GetScreenshotFormatName(uint8_t format)
{
    if(format >= SCREENSHOT_FORMATS_COUNT)
        return nullptr;
    return screenshot_format_name[format];
}

void C64Class::SetExitScreenshot(const char *filename)
{
    strcpy(exit_screenshot_filename,filename);
    enable_exit_screenshot = true;
}

const char* C64Class::GetAVVersion()
{
    return video_capture->GetAVVersion();
}

bool C64Class::StartVideoRecord(const char *filename, int audio_bitrate, int video_bitrate)
{
    if(video_capture != nullptr)
    {
		c64_frequency_temp = c64_frequency;
		c64_frequency = 982800;	// Auf genau 50 Hz stellen !!
		SetC64Speed(c64_speed);

        video_capture->SetAudioBitrate(audio_bitrate);
        video_capture->SetVideoBitrate(video_bitrate);

        return video_capture->StartCapture(filename, "mp4", current_c64_screen_width, current_c64_screen_height);
    }
    return false;
}

void C64Class::SetPauseVideoRecord(bool status)
{
    video_capture->SetCapturePause(status);
}

void C64Class::StopVideoRecord()
{
    if(video_capture != nullptr)
    {
        video_capture->StopCapture();

		c64_frequency = c64_frequency_temp;
		SetC64Speed(c64_speed);
    }
}

int C64Class::GetRecordedFrameCount()
{
    return video_capture->GetRecordedFrameCount();
}

bool C64Class::StartIECDump(const char *filename)
{
    char VersionString[256];

    sprintf(VersionString,"Emu64 Version %s",VERSION_STRING);

    iec_export_vdc.SetVersionString(VersionString);
    iec_export_vdc.SetTimeScaleString("989 ns");
    iec_export_vdc.SetModuleString("IEC-BUS");

    if(!iec_export_vdc.Open(filename)) return false;

    iec_is_dumped = true;

    return true;
}

void C64Class::StopIECDump()
{
    iec_is_dumped = false;
    iec_export_vdc.Close();
}

void C64Class::SetSIDVolume(float_t volume)
{
    sid_volume = volume;
}

void C64Class::SetFirstSidTyp(int sid_typ)
{
    sid1->SetChipType(sid_typ);
}

void C64Class::SetSecondSidTyp(int sid_typ)
{
    sid2->SetChipType(sid_typ);
}

void C64Class::EnableStereoSid(bool enable)
{
    enable_stereo_sid = enable;
}

void C64Class::SetStereoSidAddress(uint16_t address)
{
    stereo_sid_address = address;
    if(stereo_sid_address == 0xD400)
        sid2->SetIODelayEnable(true);
    else
        sid2->SetIODelayEnable(false);
}

void C64Class::SetStereoSid6ChannelMode(bool enable)
{
    enable_stereo_sid_6channel_mode = enable;
}

void C64Class::SetSidCycleExact(bool enable)
{
    sid1->CycleExact = enable;
    sid2->CycleExact = enable;
}

void C64Class::SetSidFilter(bool enable)
{
    sid1->FilterOn = enable;
    sid2->FilterOn = enable;
}

bool C64Class::StartSidDump(const char *filename)
{
    return sid1->IoDump->StartCapture(filename);
}

void C64Class::StopSidDump()
{
    sid1->IoDump->StopCapture();
}

int C64Class::GetSidDumpFrames()
{
    //This is PAL Only
    return sid1->IoDump->GetCycleCounts() / (63*312); // a Frame in PAL is 63 Cycles with 312 rasterlines
}

void C64Class::SetVicConfig(int var, bool enable)
{
    if(var >= VIC_CONFIG_NUM) return;
    vic->vic_config[var] = enable;
}

bool C64Class::GetVicConfig(int var)
{
    if(var >= VIC_CONFIG_NUM) return false;
    return vic->vic_config[var];
}

void C64Class::SetVicDisplaySizePal(uint16_t first_line, uint16_t last_line)
{
    vic->SetVicVDisplayPalSize(first_line, last_line);
}

void C64Class::SetVicDisplaySizeNtsc(uint16_t first_line, uint16_t last_line)
{
    vic->SetVicVDisplayNtscSize(first_line, last_line);
}

int C64Class::GetVicFirstDisplayLinePal()
{
    return vic->GetVicFirstDisplayLinePal();
}

int C64Class::GetVicLastDisplayLinePal()
{
    return vic->GetVicLastDisplayLinePal();
}

int C64Class::GetVicFirstDisplayLineNtsc()
{
    return vic->GetVicFirstDisplayLineNtsc();
}

int C64Class::GetVicLastDisplayLineNtsc()
{
    return vic->GetVicLastDisplayLineNtsc();
}

void C64Class::NextSystemCycle()
{
    CheckKeys();
    cycle_counter++;

    /// Für Externe Erweiterungen ///
    //if(ExtZyklus) ZyklusProcExt();

    floppy_iec_wire = 0;
    for(int i=0; i<MAX_FLOPPY_NUM; i++)
    {
        cpu_states[i+1] = floppy[i]->OneCycle();
        floppy_iec_wire |= ~floppy[i]->FloppyIECLocal;
    }
    floppy_iec_wire = ~floppy_iec_wire;

    // PHI0
    if(enable_ext_wires) rdy_ba_wire = ext_rdy_wire;
    cpu_states[0] = cpu->OneZyklus();

    // PHI1
    vic->OneCycle();
    cia1->OneZyklus();
    cia2->OneZyklus();
    sid1->OneZyklus();
    if(enable_stereo_sid) sid2->OneZyklus();
    reu->OneZyklus();
    tape->OneCycle();
    cpu->Phi1();
}

uint16_t C64Class::DisAss(FILE *file, uint16_t PC, bool line_draw, int source)
{
    char Ausgabe[36];Ausgabe[0]=0;
    char Adresse[7];Adresse[0]=0;
    char Speicher[14];Speicher[0]=0;
    char Opcode[5];Opcode[0]=0;
    char Anhang[10];Anhang[0]=0;

    int TMP;
    int OPC;

    uint16_t word;
    uint16_t A;
    char B;

    uint8_t ram0;
    uint8_t ram1;
    uint8_t ram2;

    if(source > 0)
    {
        ram0 = floppy[source-1]->ReadByte(PC+0);
        ram1 = floppy[source-1]->ReadByte(PC+1);
        ram2 = floppy[source-1]->ReadByte(PC+2);
    }
    else
    {
        ram0 = ReadC64Byte(PC+0);
        ram1 = ReadC64Byte(PC+1);
        ram2 = ReadC64Byte(PC+2);
    }

    sprintf(Adresse,"$%4.4X ",PC);			// Adresse Ausgeben

    OPC=ram0*3;					// Opcodes Ausgeben //
    sprintf(Opcode,"%c%c%c ",CpuOPC[OPC+0],CpuOPC[OPC+1],CpuOPC[OPC+2]);

    TMP=CpuOPCInfo[ram0];		// Memory und Adressierung Ausgeben //
    TMP=TMP>>4;
    TMP=TMP&15;

    switch (TMP)
    {
    case 0:									// Implizit //
        sprintf(Speicher,"$%2.2X          ",ram0);
        Anhang[0] = 0;
        PC++;
        break;
    case 1:									// Unmittelbar //
        sprintf(Speicher,"$%2.2X $%2.2X      ",ram0,ram1);
        sprintf(Anhang,"#$%2.2X",ram1);
        PC+=2;
        break;
    case 2:									// Absolut //
        sprintf(Speicher,"$%2.2X $%2.2X $%2.2X  ",ram0,ram1,ram2);
        word=ram1;
        word|=ram2<<8;
        sprintf(Anhang,"$%4.4X",word);
        PC+=3;
        break;
    case 3:									// Zerropage //
        sprintf(Speicher,"$%2.2X $%2.2X      ",ram0,ram1);
        sprintf(Anhang,"$%2.2X",ram1);
        PC+=2;
        break;
    case 4:									// Absolut X Indexziert //
        sprintf(Speicher,"$%2.2X $%2.2X $%2.2X  ",ram0,ram1,ram2);
        word=ram1;
        word|=ram2<<8;
        sprintf(Anhang,"$%4.4X,X",word);
        PC+=3;
        break;
    case 5:									// Absolut Y Indexziert //
        sprintf(Speicher,"$%2.2X $%2.2X $%2.2X  ",ram0,ram1,ram2);
        word=ram1;
        word|=ram2<<8;
        sprintf(Anhang,"$%4.4X,Y",word);
        PC+=3;
        break;
    case 6:									// Zerropage X Indexziert //
        sprintf(Speicher,"$%2.2X $%2.2X      ",ram0,ram1);
        sprintf(Anhang,"$%2.2X,X",ram1);
        PC+=2;
        break;
    case 7:									// Indirekt X Indiziert //
        sprintf(Speicher,"$%2.2X $%2.2X      ",ram0,ram1);
        sprintf(Anhang,"($%2.2X,X)",ram1);
        PC+=2;
        break;
    case 8:									// Indirekt Y Indiziert //
        sprintf(Speicher,"$%2.2X $%2.2X      ",ram0,ram1);
        sprintf(Anhang,"($%2.2X),Y",ram1);
        PC+=2;
        break;
    case 9:                                 // Branch
        sprintf(Speicher,"$%2.2X $%2.2X      ",ram0,ram1);
        B = static_cast<char>(ram1);
        A = static_cast<uint16_t>(PC + 2 + B);

        sprintf(Anhang,"$%4.4X",A);
        PC+=2;
        break;
    case 10:									// Indirekt //
        sprintf(Speicher,"$%2.2X $%2.2X $%2.2X  ",ram0,ram1,ram2);
        word=ram1;
        word|=ram2<<8;
        sprintf(Anhang,"($%4.4X)",word);
        PC+=3;
        break;
    case 11:									// Zerropage Y Indexziert //
        sprintf(Speicher,"$%2.2X $%2.2X      ",ram0,ram1);
        sprintf(Anhang,"$%2.2X,Y",ram1);
        PC+=2;
        break;
    }

    sprintf(Ausgabe,"%s%s%s%s",Adresse,Speicher,Opcode,Anhang);
    fprintf(file,"%s\n",Ausgabe);

    OPC/=3;
    if(((OPC==0x40)||(OPC==0x60)||(OPC==0x4C)) && (line_draw == true))
    {
        fprintf(file,"------------------------------\n");
    }

    return PC;
}

bool C64Class::CheckBreakpoints()
{
    int BreaksIO = 0;

    for (int i=0;i<breakgroup_count;i++)
    {
        BREAK_GROUP* bg = breakgroup[i];
        int count1 = 0;
        int count2 = 0;

        if(bg->Enable)
        {
            if(bg->bPC)
            {
                count1++;
                if((break_status&1) && (break_values[0] == bg->iPC)) count2++;
            }
            if(bg->bAC)
            {
                count1++;
                if((break_status&2) && (break_values[1] == bg->iAC)) count2++;
            }
            if(bg->bXR)
            {
                count1++;
                if((break_status&4) && (break_values[2] == bg->iXR)) count2++;
            }
            if(bg->bYR)
            {
                count1++;
                if((break_status&8) && (break_values[3] == bg->iYR)) count2++;
            }
            if(bg->bRAdresse)
            {
                count1++;
                if((break_status&16) && (break_values[4] == bg->iRAdresse)) count2++;
            }
            if(bg->bWAdresse)
            {
                count1++;
                if((break_status&32) && (break_values[5] == bg->iWAdresse)) count2++;
            }
            if(bg->bRWert)
            {
                count1++;
                if((break_status&64) && (break_values[6] == bg->iRWert)) count2++;
            }
            if(bg->bWWert)
            {
                count1++;
                if((break_status&128) && (break_values[7] == bg->iWWert)) count2++;
            }
            if(bg->bRZ)
            {
                count1++;
                if((break_status&256) && (break_values[8] == bg->iRZ)) count2++;
            }
            if(bg->bRZZyklus)
            {
                count1++;
                if((break_status&512) && (break_values[9] == bg->iRZZyklus)) count2++;
            }
        }
        if((count1 == count2) && (count1 > 0))
        {
            breakgroup[i]->bTrue = true;
            BreaksIO++;
        }
        else breakgroup[i]->bTrue = false;
    }
    break_status = 0;

    floppy_found_breakpoint = false;
    int floppy_break = 0;
    for(int i=0; i<MAX_FLOPPY_NUM; i++)
    {
        if(floppy[i]->GetEnableFloppy())
        {
            if(floppy[i]->CheckBreakpoints()) floppy_break |= 1;
        }
    }

    if((BreaksIO > 0) || (floppy_break == 1))
    {
        if(!debug_mode)
        {
            debug_mode = true;
            debug_animation = false;
            one_cycle = false;
            one_opcode = false;
            sid1->SoundOutputEnable = false;
            sid2->SoundOutputEnable = false;
            for(int i=0; i<MAX_FLOPPY_NUM; i++) floppy[i]->SetEnableFloppySound(false);
            if(BreakpointProc != nullptr) BreakpointProc();
            return true;
        }
        else
        {
            debug_animation = false;
            one_opcode = false;
            one_cycle = false;
            if(BreakpointProc != nullptr) BreakpointProc();
            return true;
        }
    }
    return false;
}

void C64Class::WriteSidIO(uint16_t address, uint8_t value)
{
    if(enable_stereo_sid)
    {
        if((address & 0xFFE0) == 0xD400) sid1->WriteIO(address,value);
        if((address & 0xFFE0) == stereo_sid_address) sid2->WriteIO(address,value);
    }
    else
    {
        sid1->WriteIO(address,value);
    }
}

uint8_t C64Class::ReadSidIO(uint16_t address)
{
    /*
    if(StereoEnable)
    {
        if(Sid2Adresse == 0xD400)
        {
            return sid1_ReadIO(adresse);
        }
        else
        {
            if((adresse && 0x001F) == 0x00) return sid1_ReadIO(adresse);
            else return sid2_ReadIO(adresse);
        }
    }
    else
    {
        return sid1_ReadIO(adresse);
    }
    */

    return sid1->ReadIO(address);
}

/// $DE00
void C64Class::WriteIO1(uint16_t address, uint8_t value)
{
    switch(io_source)
    {
    case 0: // NO_MODUL
        break;
    case 1:	// CRT
        crt->WriteIO1(address,value);
        break;
    case 2:	// REU
        reu->WriteIO1(address,value);
        break;
    case 3:	// GEORAM
        geo->WriteIO1(address,value);
        break;
    default:
        break;
    }
}

/// $DF00
void C64Class::WriteIO2(uint16_t address, uint8_t value)
{
    switch(io_source)
    {
    case 0: // NO_MODUL
        break;
    case 1: // CRT
        crt->WriteIO2(address,value);
        break;
    case 2: // REU
        reu->WriteIO2(address,value);
        break;
    case 3: // GEORAM
        geo->WriteIO2(address,value);
        break;
    default:
        break;
    }
}

/// $DE00
uint8_t C64Class::ReadIO1(uint16_t address)
{
    switch(io_source)
    {
    case 0: // NO_MODUL
        // return 0;
        return vic->last_read_gp_access;
    case 1: // CRT
        return crt->ReadIO1(address);
    case 2: // REU
        return reu->ReadIO1(address);
    case 3: // GEORAM
        return geo->ReadIO1(address);
    default:
        return 0;
    }
}

/// $DF00

uint8_t C64Class::ReadIO2(uint16_t address)
{
    switch(io_source)
    {
    case 0: // NO_MODUL
        //return 0;
        return vic->last_read_gp_access;
    case 1: // CRT
        return crt->ReadIO2(address);
    case 2: // REU
        return reu->ReadIO2(address);
    case 3: // GEORAM
        return geo->ReadIO2(address);
    default:
        return 0;
    }
}

void C64Class::JoystickNewScan()
{
    OpenSDLJoystick();
}

void C64Class::StartRecJoystickMapping(int slot_nr)
{
    rec_polling_wait = false;
    rec_joy_slot_nr = slot_nr;
    rec_joy_mapping_pos = 0;
    rec_joy_mapping = true;
}

void C64Class::StopRecJoystickMapping()
{
    if(rec_joy_mapping)
        rec_joy_mapping = false;
}

void C64Class::ClearJoystickMapping(int slot_nr)
{
    char str00[32];
    sprintf(str00,"Slot %d",slot_nr+1);
    strcpy(virtual_joys[slot_nr].Name, str00);

    for(int i=0;i<5;i++)
    {
        virtual_joys[slot_nr].Type[i] = 0;
        virtual_joys[slot_nr].JoyIndex[i] = 0;
        virtual_joys[slot_nr].KeyDown[i] = 0;
        virtual_joys[slot_nr].KeyUp[i] = 0;
        virtual_joys[slot_nr].ButtonNr[i] = 0;
        virtual_joys[slot_nr].HatNr[i] = 0;
        virtual_joys[slot_nr].HatValue[i] = 0;
        virtual_joys[slot_nr].AxisNr[i] = 0;
        virtual_joys[slot_nr].AxisValue[i] = 0;
	}
}

void C64Class::SwapJoyPorts()
{
	int temp = virtual_port1;
	virtual_port1 = virtual_port2;
	virtual_port2 = temp;
	LogText(">> JoyPorts vertauscht\n");
}

void C64Class::IncMouseHiddenCounter()
{
    if(!mouse_is_hidden && (mouse_hide_time > 0))
    {
        mouse_hide_counter++;
        if(mouse_hide_counter >= mouse_hide_time)
        {
            mouse_is_hidden = true;
            SDL_ShowCursor(false);
        }
    }
}

void C64Class::StartRecKeyMap(uint8_t keymatrix_code)
{
    rec_matrix_code = keymatrix_code;
    key_map_is_rec = true;
}

void C64Class::StopRecKeyMap()
{
    key_map_is_rec = false;
}

bool C64Class::GetRecKeyMapStatus()
{
    return key_map_is_rec;
}

C64_KEYS* C64Class::GetC64KeyTable()
{
    return c64_key_table;
}

const char** C64Class::GetC64KeyNameTable()
{
    return c64_key_names;
}

int C64Class::GetC64KeyTableSize()
{
    return C64_KEY_COUNT;
}

void C64Class::OpenSDLJoystick()
{
    sdl_joystick_update_is_stoped = false;
    sdl_joystick_stop_update = true;
    int AbbruchCounter = 1000;
    while(!sdl_joystick_update_is_stoped && (AbbruchCounter > 0))
    {
        AbbruchCounter--;
    }

    if(sdl_joystick_is_open)
    {
        sdl_joystick_is_open = false;

        SDL_ClearError();
        SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
        LogText(const_cast<char*>(">> SDL Subsystem Joystick wurde geschlossen\n"));
        LogText(const_cast<char*>("<< SDL_Error: "));
        LogText(const_cast<char*>(SDL_GetError()));
        LogText(const_cast<char*>("\n"));
    }

    SDL_ClearError();
    if(SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0)
    {
        sdl_joystick_is_open = false;
        LogText(const_cast<char*>("<< ERROR: SDL Subsystem Joystick konnte nicht geöffnet werden\n"));
        LogText(const_cast<char*>("<< SDL_Error: "));
        LogText(const_cast<char*>(SDL_GetError()));
        LogText(const_cast<char*>("\n"));
    }
    else
    {
        sdl_joystick_is_open = true;
        LogText(const_cast<char*>(">> SDL Subsytem Joystick wurde erfolgreich geoeffnet\n"));
        sdl_joystick_count = SDL_NumJoysticks();
        if(sdl_joystick_count > MAX_SDL_JOYSTICK_NUM) sdl_joystick_count = MAX_SDL_JOYSTICK_NUM;

        char str00[256];
        switch(sdl_joystick_count)
        {
        case 0:
            LogText(const_cast<char*>("<< SDL konnte keinen Joystick/Gamepad erkennen\n"));
            break;
        case 1:
            LogText(const_cast<char*>(">> SDL konnte 1 Joystick/Gamepad erkennen\n"));
            break;
        default:
            sprintf(str00,">> SDL konnte %d Joysticks/Gamepads erkennen\n",sdl_joystick_count);
            LogText(str00);
            break;
        }

        if(sdl_joystick_count > 0)
        {
            for(int i=0; i<sdl_joystick_count; i++)
            {
                sdl_joystick[i] = SDL_JoystickOpen(i);
                sdl_joystick_name[i] = SDL_JoystickName(sdl_joystick[i]);

                sprintf(str00,">>   [%2.2d] %s\n",i,sdl_joystick_name[i]);
                LogText(str00);
            }
        }
    }
    sdl_joystick_stop_update = true;
}

void C64Class::CloseSDLJoystick()
{
    sdl_joystick_update_is_stoped = false;
    sdl_joystick_stop_update = true;
    int AbbruchCounter = 1000;
    while(!sdl_joystick_update_is_stoped && (AbbruchCounter > 0))
    {
        AbbruchCounter--;
    }
    if(sdl_joystick_is_open)
    {
        sdl_joystick_is_open = false;
        SDL_ClearError();
        SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
        LogText(const_cast<char*>(">> SDL Subsystem Joystick wurde geschlossen\n"));
        LogText(const_cast<char*>("<< SDL_Error: "));
        LogText(const_cast<char*>(SDL_GetError()));
        LogText(const_cast<char*>("\n"));
    }
    sdl_joystick_stop_update = true;
}

///
/// \brief  Wird aufgerufen wenn der CIA1 Ausgang PA6 und PA7 sich ändert
///         Somit Schaltet IC 4066 POTX und POTY
///
void C64Class::ChangePOTSwitch()
{
    if(!enable_mouse_1351)
    {
        poti_x = 0xFF;
        poti_y = 0xFF;
    }
    else
    {
        switch(cia1_port_a.GetOutput() >> 6)
        {
        case 0:
            poti_x = poti_y = 0xFF;
            break;
        case 1:
            poti_x = poti_ax;
            poti_y = poti_ay;
            break;
        case 2:
            poti_x = poti_bx;
            poti_y = poti_by;
            break;
        case 3:
            poti_x = poti_ax | poti_bx;
            poti_y = poti_ay | poti_by;
            break;
        }
    }

    sid1->SetPotXY(poti_x, poti_y);
}

void C64Class::UpdateMouse(void)
{
    switch(mouse_port)
    {
    case 0: // Maus an Port 1 angeschlossen
        poti_ax = (mouse_1351_x_rel>>2) & 0x7F;
        poti_ay = (~mouse_1351_y_rel>>2) & 0x7F;
        poti_bx = 0xFF;
        poti_by = 0xFF;
        break;

    case 1: // Maus an Port 2 angeschlossen
        poti_ax = 0xFF;
        poti_ay = 0xFF;
        poti_bx = (mouse_1351_x_rel>>2) & 0x7F;
        poti_by = (~mouse_1351_y_rel>>2) & 0x7F;
        break;
    }

    ChangePOTSwitch();
}

void C64Class::SwapRBSurface(SDL_Surface *surface)
{
    int xw=surface->w;
    int yw=surface->h;

    SDL_LockSurface(surface);
    for(int y=0; y<yw; y++)
    {
        uint32_t* data = static_cast<uint32_t*>(surface->pixels) + (surface->pitch/4)*y;
        for(int x=0; x<xw; x++)
        {
            // R <--> B
            data[x] = (data[x] & 0xff00ff00) | ((data[x]>>16) & 0xff) | ((data[x]<<16) & 0xff0000);
        }
    }
    SDL_UnlockSurface(surface);
}
