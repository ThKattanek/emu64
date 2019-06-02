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
// Letzte Änderung am 02.06.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "c64_class.h"
#include "c64_keys.h"

#include <QDebug>

#define floppy_asyncron                  // Schaltet die Floppy Asyncron
#define more_one_floppy_cylce_count 66   // alle "more_one_floppy_cycle_counts" wird 1 FloppyZyklus doppelt ausgeführt

void AudioMix(void *nichtVerwendet, Uint8 *stream, int laenge);
int SDLThread(void *userdat);
int SDLThreadLoad(void *userdat);
int SDLThreadWarp(void *userdat);

#define AudioSampleRate 44100

//#define C64Takt 982800  // Genau 50Hz
#define C64Takt 985248  // 50,124542Hz (Original C64 PAL)

#ifdef _WIN32
    #define AudioPufferSize (882)    // 882 bei 44.100 Khz
#else
    #define AudioPufferSize (880*2)    // 882 bei 44.100 Khz
#endif

#define RecPollingWaitStart 20

#define C64ScreenXW 384         //384

static const char *ScreenschotFormatName[SCREENSHOT_FORMATS_COUNT]{"BMP","PNG"};

C64Class::C64Class(int *ret_error, VideoPalClass *_pal, function<void(char*)> log_function, const char *data_path):
    mmu(nullptr),cpu(nullptr),vic(nullptr),sid1(nullptr),sid2(nullptr),cia1(nullptr),cia2(nullptr),crt(nullptr)
{
    ChangeGrafikModi = false;
    ChangeWindowPos = false;
    ChangeWindowSize = false;

    JoyStickUdateIsStop = true;
    RecJoyMapping = false;
    StopJoystickUpdate = true;
    SDLJoystickIsOpen = false;
    JoystickAnzahl = 0;

    isReturnKeyDown = false;

    IsKeyMapRec = false;
    RecMatrixCode = false;

    VPort1 = 0;
    VPort2 = 1;

    WarpMode = false;

    LogText = log_function;

    Mouse1351Enable = false;
    MouseXRel = MouseYRel = 0;

    MousePort = 0;  // Port1 = 0 ... Port2 = 1
    POT_AX = POT_AY = POT_BX = POT_BY = 0xFF;    // HighZ zum Beginn (Keine Paddles / Maus angeschlossen)
    POT_X = POT_Y = 0xFF;

    MouseIsHidden = false;
    MouseHiddenCounter = 0;
    MouseHiddenTime = 3000;

    FloppySoundPath = new char[1024];
    GfxPath = new char [1024];
    RomPath = new char[1024];

    sprintf(FloppySoundPath,"%s%s",data_path,"/floppy_sounds/");
    sprintf(GfxPath,"%s%s",data_path,"/gfx/");
    sprintf(RomPath,"%s%s",data_path,"/roms/");

    LogText(const_cast<char*>(">> C64 Klasse wurde gestartet...\n"));
    LogText(const_cast<char*>(">> GfxPath = "));
    LogText(const_cast<char*>(GfxPath));
    LogText(const_cast<char*>("\n"));

    pal = _pal;
    BreakGroupAnz = 0;
    FloppyFoundBreakpoint = false;
    EnableExtLines = false;

    IecIsDumped = false;

    AktC64ScreenXW = AktWindowXW = AktC64ScreenYW = AktWindowYW = 0;

    mutex1 = SDL_CreateMutex();

    C64Window = nullptr;
    C64Screen = nullptr;
    C64ScreenTexture = 0;
    C64ScreenAspectRatio = SCREEN_RATIO_4_3;
    EnableWindowAspectRatio = true;
    EnableFullscreenAspectRatio = true;
    IsC64ScreenObsolete = false;
    StartScreenshot = false;
    ExitScreenshotEnable = false;
    FrameSkipCounter=1;

    DistortionEnable = true;
    SetDistortion(-0.05f);

    VideoCapture = nullptr;

    /// SDL Installieren ///

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
         LogText(const_cast<char*>("<< ERROR: Fehler beim installieren von SDL2\n"));
        *ret_error = -1;
    }
    else
        LogText(const_cast<char*>(">> SDL2 wurde installiert\n"));

    char filename[FILENAME_MAX];
    sprintf(filename,"%spfeil0.png",GfxPath);
    Pfeil0 = IMG_Load(filename);
    if(!Pfeil0)
    {
        LogText(const_cast<char*>("<< ERROR: Folgendes Bild konnte nicht geladen werden --- "));
        LogText(filename);
        LogText(const_cast<char*>("\n"));

        //*ret_error = -2;
    }
    else
    {
        LogText(const_cast<char*>(">> Folgendes Bild wurde erfolgreich geladen --- "));
        LogText(filename);
        LogText(const_cast<char*>("\n"));

        if ( (Pfeil0->w & (Pfeil0->w - 1)) != 0 ) LogText(const_cast<char*>("<< WARNUNG: Die Breite ist keine Potenz von 2^n\n"));
        if ( (Pfeil0->h & (Pfeil0->h - 1)) != 0 ) LogText(const_cast<char*>("<< WARNUNG: Die Höhe ist keine Potenz von 2^n\n"));
    }

    sprintf(filename,"%spfeil1.png",GfxPath);
    Pfeil1 = IMG_Load(filename);
    if(!Pfeil1)
    {
        LogText(const_cast<char*>("<< ERROR: Folgendes Bild konnte nicht geladen werden --- "));
        LogText(filename);
        LogText(const_cast<char*>("\n"));

        //*ret_error = -3;
    }
    else
    {
        LogText(const_cast<char*>(">> Folgendes Bild wurde erfolgreich geladen --- "));
        LogText(filename);
        LogText(const_cast<char*>("\n"));

        if ( (Pfeil1->w & (Pfeil1->w - 1)) != 0 ) LogText(const_cast<char*>("<< WARNUNG: Die Breite ist keine Potenz von 2^n\n"));
        if ( (Pfeil1->h & (Pfeil1->h - 1)) != 0 ) LogText(const_cast<char*>("<< WARNUNG: Die Höhe ist keine Potenz von 2^n\n"));
    }

    sprintf(filename,"%skreis0.png",GfxPath);
    Kreis0 = IMG_Load(filename);
    if(!Kreis0)
    {
        LogText(const_cast<char*>("<< ERROR: Folgendes Bild konnte nicht geladen werden --- "));
        LogText(filename);
        LogText(const_cast<char*>("\n"));

        //*ret_error = -4;
    }
    else
    {
        LogText(const_cast<char*>(">> Folgendes Bild wurde erfolgreich geladen --- "));
        LogText(filename);
        LogText(const_cast<char*>("\n"));

        if ( (Kreis0->w & (Kreis0->w - 1)) != 0 ) LogText(const_cast<char*>("<< WARNUNG: Die Breite ist keine Potenz von 2^n\n"));
        if ( (Kreis0->h & (Kreis0->h - 1)) != 0 ) LogText(const_cast<char*>("<< WARNUNG: Die Höhe ist keine Potenz von 2^n\n"));
    }

    sprintf(filename,"%skreis1.png",GfxPath);
    Kreis1 = IMG_Load(filename);
    if(!Kreis1)
    {
        LogText(const_cast<char*>("<< ERROR: Folgendes Bild konnte nicht geladen werden --- "));
        LogText(filename);
        LogText(const_cast<char*>("\n"));

       // *ret_error = -5;
    }
    else
    {
        LogText(const_cast<char*>(">> Folgendes Bild wurde erfolgreich geladen --- "));
        LogText(filename);
        LogText(const_cast<char*>("\n"));

        if ( (Kreis1->w & (Kreis1->w - 1)) != 0 ) LogText(const_cast<char*>("<< WARNUNG: Die Breite ist keine Potenz von 2^n\n"));
        if ( (Kreis1->h & (Kreis1->h - 1)) != 0 ) LogText(const_cast<char*>("<< WARNUNG: Die Hoehe ist keine Potenz von 2^n\n"));
    }

    /// VideoCaptuer installieren ///

    VideoCapture = new VideoCaptureClass();

    /// SLD Audio Installieren (C64 Emulation) ///

    SDL_memset(&want, 0, sizeof(want));
    want.freq = AudioSampleRate;
    want.format = AUDIO_S16;
    want.channels = 2;
    want.samples = AudioPufferSize;
    want.callback = AudioMix;
    want.userdata = this;

    if( SDL_OpenAudio(&want,&have) > 0 )
    {
        LogText(const_cast<char*>("<< ERROR: Fehler beim installieren von SDL_Audio\n"));
        *ret_error = -6;
    }
    LogText(const_cast<char*>(">> SDL_Audio wurde installiert\n"));

    if (want.format != have.format)
    {
        printf("Audio Format \"AUDIO_S16 wird nicht unterstuetzt.\n");
        *ret_error = -6;
    }

    OpenSDLJoystick();

    SetGrafikModi(pal->StartC64isColorBit32,pal->StartC64isDoublesize,pal->StartC64isPalmode,false);

    sprintf(filename,"%ssdl_icon.png",GfxPath);
    C64ScreenIcon = IMG_Load(filename);
    if(C64ScreenIcon != nullptr)
    {
        SDL_SetColorKey(C64ScreenIcon,SDL_TRUE,SDL_MapRGB(C64ScreenIcon->format,0,0,0));
    }
    else
        LogText(const_cast<char*>("<< ERROR: Fehler beim laden des SLDFenster Icons\n"));

    GamePort1 = 0;
    GamePort2 = 0;

    /// Init Classes ///
    mmu = new MMU();
    cpu = new MOS6510();
    vic = new VICII();
    sid1 = new MOS6581_8085(0,have.freq,have.samples,ret_error);
    sid2 = new MOS6581_8085(1,have.freq,have.samples,ret_error);
    cia1 = new MOS6526(0);
    cia2 = new MOS6526(1);
    crt = new CRTClass();
    reu = new REUClass();
    geo = new GEORAMClass();
    tape = new TAPE1530(have.freq,have.samples,C64Takt);

    vic_puffer = vic->VideoPuffer;

    cia2->FloppyIEC = &FloppyIEC;
    cia2->C64IEC = &C64IEC;

    IecVcdExport.AddWire("c64_out_atn",false);
    IecVcdExport.AddWire("c64_out_clk",false);
    IecVcdExport.AddWire("c64_out_data",false);
    IecVcdExport.AddWire("floppy_out_clk",false);
    IecVcdExport.AddWire("floppy_out_data",false);

    /// Floppy mit C64 verbinden ///

    sprintf(filename,"%s1541.rom",RomPath);

    char motor_filename[FILENAME_MAX];
    char motor_on_filename[FILENAME_MAX];
    char motor_off_filename[FILENAME_MAX];
    char anschlag_filename[FILENAME_MAX];
    char stepper_inc_filename[FILENAME_MAX];
    char stepper_dec_filename[FILENAME_MAX];

    sprintf(motor_filename,"%smotor.raw",FloppySoundPath);
    sprintf(motor_on_filename,"%smotor_on.raw",FloppySoundPath);
    sprintf(motor_off_filename,"%smotor_off.raw",FloppySoundPath);
    sprintf(anschlag_filename,"%sanschlag.raw",FloppySoundPath);
    sprintf(stepper_inc_filename,"%sstepper_inc.raw",FloppySoundPath);
    sprintf(stepper_dec_filename,"%sstepper_dec.raw",FloppySoundPath);

    for(int i=0; i<FLOPPY_ANZAHL; i++)
    {
        floppy[i] = new Floppy1541(&RESET,have.freq,have.samples,&FloppyFoundBreakpoint);
        floppy[i]->SetResetReady(&FloppyResetReady[i],0xEBFF);
        floppy[i]->SetC64IEC(&C64IEC);
        floppy[i]->SetDeviceNummer(static_cast<uint8_t>(8+i));
        floppy[i]->LoadDosRom(filename);
        //floppy[i]->LoadFloppySounds((char*)"floppy_sounds/motor.raw",(char*)"floppy_sounds/motor_on.raw",(char*)"floppy_sounds/motor_off.raw",(char*)"floppy_sounds/anschlag.raw",(char*)"floppy_sounds/stepper_inc.raw",(char*)"floppy_sounds/stepper_dec.raw");
        floppy[i]->LoadFloppySounds(motor_filename,motor_on_filename,motor_off_filename,anschlag_filename,stepper_inc_filename,stepper_dec_filename);
        floppy[i]->SetEnableFloppy(false);
        floppy[i]->SetEnableFloppySound(true);
    }

    /// Init Vars ///
    C64HistoryPointer = 0;
    IOSource = 0;
    ComandZeileSize = 0;
    ComandZeileCount = 0;
    ComandZeileStatus = false;
    ComandZeileCountS = false;
    DebugMode = OneZyk = OneOpc = false;
    CycleCounter = 0;
    LimitCylesCounter = 0;
    DebugAnimation = false;
    AnimationSpeedAdd = AudioPufferSize/AudioSampleRate;
    AnimationSpeedCounter = 0;

    for(int i=0;i<8;i++)
    {
        KeyboardMatrixToPAExt[i] = KeyboardMatrixToPA[i] = 0;
        KeyboardMatrixToPBExt[i] = KeyboardMatrixToPB[i] = 0;
    }

    /// Callbackroutinen setzen ///
    ReadProcTbl = mmu->CPUReadProcTbl;
    WriteProcTbl = mmu->CPUWriteProcTbl;
    cpu->ReadProcTbl = mmu->CPUReadProcTbl;
    cpu->WriteProcTbl = mmu->CPUWriteProcTbl;
    vic->ReadProcTbl = mmu->VICReadProcTbl;
    vic->RefreshProc = bind(&C64Class::VicRefresh,this,_1);
    reu->ReadProcTbl = mmu->CPUReadProcTbl;
    reu->WriteProcTbl = mmu->CPUWriteProcTbl;

    mmu->VicIOWriteProc = bind(&VICII::WriteIO,vic,_1,_2);
    mmu->VicIOReadProc = bind(&VICII::ReadIO,vic,_1);
    mmu->SidIOWriteProc = bind(&C64Class::WriteSidIO,this,_1,_2);
    mmu->SidIOReadProc = bind(&C64Class::ReadSidIO,this,_1);
    mmu->Cia1IOWriteProc = bind(&MOS6526::WriteIO,cia1,_1,_2);
    mmu->Cia1IOReadProc = bind(&MOS6526::ReadIO,cia1,_1);
    mmu->Cia2IOWriteProc = bind(&MOS6526::WriteIO,cia2,_1,_2);
    mmu->Cia2IOReadProc = bind(&MOS6526::ReadIO,cia2,_1);

    mmu->CRTRom1WriteProc = bind(&CRTClass::WriteRom1,crt,_1,_2);
    mmu->CRTRom2WriteProc = bind(&CRTClass::WriteRom2,crt,_1,_2);
    mmu->CRTRom3WriteProc = bind(&CRTClass::WriteRom3,crt,_1,_2);
    mmu->CRTRom1ReadProc = bind(&CRTClass::ReadRom1,crt,_1);
    mmu->CRTRom2ReadProc = bind(&CRTClass::ReadRom2,crt,_1);
    mmu->CRTRom3ReadProc = bind(&CRTClass::ReadRom3,crt,_1);
    mmu->IO1ReadProc = bind(&C64Class::ReadIO1,this,_1);
    mmu->IO1WriteProc = bind(&C64Class::WriteIO1,this,_1,_2);
    mmu->IO2ReadProc = bind(&C64Class::ReadIO2,this,_1);
    mmu->IO2WriteProc = bind(&C64Class::WriteIO2,this,_1,_2);

    crt->ChangeMemMapProc = bind(&MMU::ChangeMemMap,mmu);

    /// Module mit Virtuellen Leitungen verbinden
    mmu->GAME = &GAME;
    mmu->EXROM = &EXROM;
    mmu->RAM_H = &RAM_H;
    mmu->RAM_L = &RAM_L;
    mmu->CPU_PORT = &CPU_PORT;
    crt->EXROM = &EXROM;
    crt->GAME = &GAME;
    crt->CpuTriggerInterrupt = bind(&MOS6510::TriggerInterrupt,cpu,_1);
    crt->CpuClearInterrupt = bind(&MOS6510::ClearInterrupt,cpu,_1);
    cpu->RDY = &RDY_BA;
    cpu->RESET = &RESET;
    cpu->ResetReady = &C64ResetReady;
    cpu->ResetReadyAdr = 0xE5CD;
    cpu->EnableExtInterrupts = EnableExtLines;
    cia1->RESET = &RESET;
    cia1->CpuTriggerInterrupt = bind(&MOS6510::TriggerInterrupt,cpu,_1);
    cia1->CpuClearInterrupt = bind(&MOS6510::ClearInterrupt,cpu,_1);
    cia1->VicTriggerLP = bind(&VICII::TriggerLightpen,vic);
    cia1->ChangePOTSwitch = bind(&C64Class::ChangePOTSwitch,this);
    cia1->PA = &CIA1_PA;
    cia1->PB = &CIA1_PB;
    cia2->RESET = &RESET;
    cia2->CpuTriggerInterrupt = bind(&MOS6510::TriggerInterrupt,cpu,_1);
    cia2->CpuClearInterrupt = bind(&MOS6510::ClearInterrupt,cpu,_1);
    cia2->PA = &CIA2_PA;
    cia2->PB = &CIA2_PB;
    vic->BA = &RDY_BA;
    vic->CpuTriggerInterrupt = bind(&MOS6510::TriggerInterrupt,cpu,_1);
    vic->CpuClearInterrupt = bind(&MOS6510::ClearInterrupt,cpu,_1);
    vic->FarbRam = mmu->GetFarbramPointer();
    vic->CIA2_PA = CIA2_PA.GetOutputBitsPointer();
    sid1->RESET = &RESET;
    sid2->RESET = &RESET;
    reu->BA = &RDY_BA;
    reu->CpuTriggerInterrupt = bind(&MOS6510::TriggerInterrupt,cpu,_1);
    reu->CpuClearInterrupt = bind(&MOS6510::ClearInterrupt,cpu,_1);
    reu->RESET = &RESET;
    reu->WRITE_FF00 = &cpu->WRITE_FF00;

    ReuIsInsert = false;

    /// Tape mit C64 verbinden ///
    tape->CPU_PORT = &CPU_PORT;
    cia1->FLAG_PIN = &tape->CassRead;

    /// CRT mit MMU verbinden ///
    crt->RAM_C64 = mmu->GetRAMPointer();
    mmu->EasyFlashDirty1 = crt->GetFlash040Dirty(0);
    mmu->EasyFlashDirty2 = crt->GetFlash040Dirty(1);
    mmu->EasyFlashByte1 = crt->GetFlash040Byte(0);
    mmu->EasyFlashByte2 = crt->GetFlash040Byte(1);

    mmu->EasyFlashDirty1 = &EasyFlashDirty;
    mmu->EasyFlashDirty2 = &EasyFlashDirty;
    mmu->EasyFlashByte1 = &EasyFlashByte;
    mmu->EasyFlashByte2 = &EasyFlashByte;

    RDY_BA = true;
    GAME = true;
    EXROM = true;

    WaitResetReady = false;

    mmu->Reset();
    cia1->Reset();
    cia2->Reset();

    SIDVolume = 1.0;

    sid1->RESET = &RESET;
    sid1->SetC64Zyklen(C64Takt);     // PAL 63*312*50 = 982800
    sid1->SetChipType(MOS_8580);
    sid1->SoundOutputEnable = true;
    sid1->CycleExact = true;
    sid1->FilterOn = true;
    sid1->Reset();
    sid1->SetPotXY(POT_X, POT_Y);

    sid2->RESET = &RESET;
    sid2->SetC64Zyklen(C64Takt);     // PAL 63*312*50 = 982800
    sid2->SetChipType(MOS_8580);
    sid2->SoundOutputEnable = true;
    sid2->CycleExact = true;
    sid2->FilterOn = true;
    sid2->Reset();

    StereoEnable = false;
    Sid2Adresse = 0xD420;
    Sid6ChannelMode = false;

    vic->BA = &RDY_BA;
    vic->CIA2_PA = cia2->PA->GetOutputBitsPointer();

    /// Breakpoints ///
    cpu->BreakStatus = &BreakStatus;
    cpu->BreakWerte = BreakWerte;
    cpu->Breakpoints = Breakpoints;

    vic->BreakStatus = &BreakStatus;
    vic->BreakWerte = BreakWerte;
    vic->Breakpoints = Breakpoints;

    cpu->History = C64History;
    cpu->HistoryPointer = &C64HistoryPointer;

    for(int i=0;i<0x10000;i++) Breakpoints[i] = 0;
}

C64Class::~C64Class()
{
    EndEmulation();

    for(int i=0; i<FLOPPY_ANZAHL; i++)
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

    if(VideoCapture != nullptr) delete VideoCapture;

    if(FloppySoundPath != nullptr) delete[] FloppySoundPath;
    if(RomPath != nullptr) delete[] RomPath;
}

void C64Class::StartEmulation()
{
    /// SLD Thread starten (ab hier startet auch die C64 Emulation ///
    sdl_thread = SDL_CreateThread(SDLThread, "C64Thread", this);
    SDL_PauseAudio(0);
}

void C64Class::EndEmulation()
{
    EnableWarpMode(false);
    if(ExitScreenshotEnable)
    {
        SwapRBSurface(C64Screen);
        SDL_SavePNG(C64Screen, ExitScreenshotFilename);
    }

    /// Loop Thread beenden ///
    LoopThreadEnd = true;
    while (!LoopThreadIsEnd)
        SDL_Delay(1);

    SDL_PauseAudio(1);
    SDL_CloseAudio();

    CloseSDLJoystick();
    SDL_Quit();
}

void C64Class::SetLimitCycles(int nCycles)
{
    LimitCylesCounter = nCycles;
}

void C64Class::SetEnableDebugCart(bool enabled)
{
    cpu->SetEnableDebugCart(enabled);
}

void AudioMix(void *userdat, Uint8 *stream, int laenge)
{
    C64Class *c64 = static_cast<C64Class*>(userdat);
    if(c64->RecPollingWait)
    {
        c64->RecPollingWaitCounter--;
        if(c64->RecPollingWaitCounter == 0) c64->RecPollingWait = false;
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
    SDL_Event event;
    c64->LoopThreadEnd = false;
    c64->LoopThreadIsEnd = false;
    c64->sdl_thread_pause = false;

    c64->C64ScreenBuffer = nullptr;

    while (!c64->LoopThreadEnd)
    {
        /// Wird immer ausgeführt wenn die Funktion SetGrafikModi ausgefürt wurde ///
        if(c64->ChangeGrafikModi)
        {
            c64->ChangeGrafikModi = false;
            c64->InitGrafik();
        }

        if(c64->ChangeWindowPos)
        {
            c64->ChangeWindowPos = false;
            SDL_SetWindowPosition(c64->C64Window, c64->win_pos_x, c64->win_pos_y);
        }

        if(c64->ChangeWindowSize)
        {
            c64->ChangeWindowSize = false;
            SDL_SetWindowSize(c64->C64Window, c64->win_size_w, c64->win_size_h);

#ifdef _WIN32
            c64->AktWindowXW = c64->win_size_w;
            c64->AktWindowYW = c64->win_size_h;
            glViewport(0,0,c64->AktWindowXW, c64->AktWindowYW);
            glMatrixMode(GL_PROJECTION);
            glOrtho(0,c64->AktWindowXW, c64->AktWindowYW,0,-1,1);
            glLoadIdentity();
#endif
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
            if(!c64->LoopThreadEnd)
            {
                if(c64->IsC64ScreenObsolete)
                {
                    c64->DrawC64Screen();
                    c64->IsC64ScreenObsolete = false;
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
    c64->LoopThreadIsEnd = true;

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
            DistortionGridPoints[y*(SUBDIVS_SCREEN+1)+x].y = X*X*(Distortion*Y)+Y;
            DistortionGridPoints[y*(SUBDIVS_SCREEN+1)+x].x = Y*Y*(Distortion*X)+X;
        }

    div = 1.0f / SUBDIVS_SCREEN;
    for(int y=0; y<SUBDIVS_SCREEN; y++)
        for(int x=0; x<SUBDIVS_SCREEN; x++)
        {
            DistortionGrid[y*SUBDIVS_SCREEN*4+x*4+0] = DistortionGridPoints[(y)*(SUBDIVS_SCREEN+1)+x];
            DistortionGrid[y*SUBDIVS_SCREEN*4+x*4+1] = DistortionGridPoints[(y)*(SUBDIVS_SCREEN+1)+x+1];
            DistortionGrid[y*SUBDIVS_SCREEN*4+x*4+2] = DistortionGridPoints[(y+1)*(SUBDIVS_SCREEN+1)+x+1];
            DistortionGrid[y*SUBDIVS_SCREEN*4+x*4+3] = DistortionGridPoints[(y+1)*(SUBDIVS_SCREEN+1)+x];

            DistortionGridTex[(SUBDIVS_SCREEN-y-1)*SUBDIVS_SCREEN*4+x*4+0].x = x*div;
            DistortionGridTex[(SUBDIVS_SCREEN-y-1)*SUBDIVS_SCREEN*4+x*4+0].y = (y+1)*div;

            DistortionGridTex[(SUBDIVS_SCREEN-y-1)*SUBDIVS_SCREEN*4+x*4+1].x = (x+1)*div;
            DistortionGridTex[(SUBDIVS_SCREEN-y-1)*SUBDIVS_SCREEN*4+x*4+1].y = (y+1)*div;

            DistortionGridTex[(SUBDIVS_SCREEN-y-1)*SUBDIVS_SCREEN*4+x*4+2].x = (x+1)*div;
            DistortionGridTex[(SUBDIVS_SCREEN-y-1)*SUBDIVS_SCREEN*4+x*4+2].y = (y)*div;

            DistortionGridTex[(SUBDIVS_SCREEN-y-1)*SUBDIVS_SCREEN*4+x*4+3].x = x*div;
            DistortionGridTex[(SUBDIVS_SCREEN-y-1)*SUBDIVS_SCREEN*4+x*4+3].y = y*div;
        }
}

void C64Class::VicRefresh(uint8_t *vic_puffer)
{
    if(HoldVicRefresh)
    {
        VicRefreshIsHold = true;
        return;
    }

    if((pal == nullptr) || (sdl_thread_pause == true)) return;

    SDL_LockSurface(C64Screen);
    pal->ConvertVideo(static_cast<void*>(C64Screen->pixels),C64Screen->pitch,vic_puffer,104,AktC64ScreenXW,AktC64ScreenYW,504,312,false);

    this->vic_puffer = vic_puffer;
    vic->SwitchVideoPuffer();
    SDL_UnlockSurface(C64Screen);

    ///////////////////////////////////

    VideoCapture->AddFrame(static_cast<uint8_t*>(C64Screen->pixels),C64Screen->pitch);

    ///////////////////////////////////

    ///////////////////////////////////
    /// Auf Screenshot Start prüfen ///
    if(StartScreenshot)
    {
        SwapRBSurface(C64Screen);

        switch (ScreenshotFormat) {
        case SCREENSHOT_FORMAT_BMP:
            SDL_SaveBMP(C64Screen,ScreenshotFilename);
            break;
        case SCREENSHOT_FORMAT_PNG:
            SDL_SavePNG(C64Screen,ScreenshotFilename);
            break;
        default:

            break;
        }
        StartScreenshot = false;
    }
    ////////////////////////////////////

    if(Mouse1351Enable) UpdateMouse();

    IsC64ScreenObsolete = true;
}

void C64Class::WarpModeLoop()
{
    static unsigned int counter_plus=0;

    CheckKeys();
    CycleCounter++;

    if(LimitCylesCounter > 0)
    {
        LimitCylesCounter--;
        if(LimitCylesCounter == 0)
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

    /// Für Externe Erweiterungen ///
    //if(ExtZyklus) ZyklusProcExt();

    FloppyIEC = 0;
    for(int i=0; i<FLOPPY_ANZAHL; i++)
    {
        floppy[i]->OneZyklus();

        #ifdef floppy_asyncron
        counter_plus++;
        if(counter_plus == more_one_floppy_cylce_count)
        {
            counter_plus = 0;
            floppy[i]->OneZyklus();
        }
        #endif

        FloppyIEC |= ~floppy[i]->FloppyIECLocal;
    }
    FloppyIEC = ~FloppyIEC;

    vic->OneZyklus();
    cia1->OneZyklus();
    cia2->OneZyklus();

    sid1->OneZyklus();
    if(StereoEnable) sid2->OneZyklus();

    reu->OneZyklus();
    tape->OneCycle();

    if(EnableExtLines) RDY_BA = ExtRDY;
    cpu->OneZyklus();

    ////////////////////////// Testweise //////////////////////////

    static int zyklen_counter = 0;
    if(++zyklen_counter == 19656)
    {
        zyklen_counter = 0;
        if(WaitResetReady)
        {
            if(!floppy[0]->GetEnableFloppy())
            {
                if(C64ResetReady)
                {
                    SDL_CreateThread(SDLThreadLoad ,"C64Thread",this);
                    WaitResetReady = false;
                }
            }
            else
            {
                if((C64ResetReady == true) && (FloppyResetReady[0] == true))
                {
                    SDL_CreateThread(SDLThreadLoad ,"C64Thread" ,this);
                    WaitResetReady = false;
                }
            }
        }

        if(ComandZeileCountS)
        {
            ComandZeileCountS=false;
            if(ComandZeileStatus)
            {
                if(ComandZeileCount==ComandZeileSize)
                {
                    ComandZeileStatus=false;
                }
                else
                {
                    WriteC64Byte(0xC6,1);
                    WriteC64Byte(0x277,static_cast<uint8_t>(ComandZeile[ComandZeileCount]));
                    ComandZeileCount++;
                }
            }
        }
        if(ReadC64Byte(0xC6)==0)
        {
            ComandZeileCountS=true;
        }
    }
    //////////////////////////////////////////////////////////////////////////////

}

void C64Class::FillAudioBuffer(uint8_t *stream, int laenge)
{
    int16_t* puffer = reinterpret_cast<int16_t*>(stream);
    static uint32_t counter_plus=0;

    sid1->SoundBufferPos = 0;
    sid2->SoundBufferPos = 0;
    for(int i=0; i<FLOPPY_ANZAHL; i++)
        floppy[i]->ZeroSoundBufferPos();
    tape->ZeroSoundBufferPos();

    if(!DebugMode)
    {
        while((sid1->SoundBufferPos < (laenge/4) && (DebugMode == false)))
        {
            CheckKeys();
            CycleCounter++;

            if(LimitCylesCounter > 0)
            {
                LimitCylesCounter--;
                if(LimitCylesCounter == 0)
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

            /// Für Externe Erweiterungen ///
            //if(ExtZyklus) ZyklusProcExt();

            FloppyIEC = 0;
            for(int i=0; i<FLOPPY_ANZAHL; i++)
            {
                floppy[i]->OneZyklus();

                #ifdef floppy_asyncron
                counter_plus++;
                if(counter_plus == more_one_floppy_cylce_count)
                {
                    counter_plus = 0;
                    floppy[i]->OneZyklus();
                }
                #endif

                FloppyIEC |= ~floppy[i]->FloppyIECLocal;
            }
            FloppyIEC = ~FloppyIEC;

            vic->OneZyklus();
            cia1->OneZyklus();
            cia2->OneZyklus();
            sid1->OneZyklus();
            if(StereoEnable) sid2->OneZyklus();
            reu->OneZyklus();
            tape->OneCycle();

            if(EnableExtLines) RDY_BA = ExtRDY;
            cpu->OneZyklus();

            if((BreakStatus != 0) || (FloppyFoundBreakpoint == true )) if(CheckBreakpoints()) break;


            ////////////////////////// Testweise //////////////////////////

            static int zyklen_counter = 0;
            if(++zyklen_counter == 19656)
            {
                zyklen_counter = 0;
                if(WaitResetReady)
                {
                    if(!floppy[0]->GetEnableFloppy())
                    {
                        if(C64ResetReady)
                        {
                            SDL_CreateThread(SDLThreadLoad ,"C64Thread",this);
                            WaitResetReady = false;
                        }
                    }
                    else
                    {
                        if((C64ResetReady == true) && (FloppyResetReady[0] == true))
                        {
                            SDL_CreateThread(SDLThreadLoad ,"C64Thread" ,this);
                            WaitResetReady = false;
                        }
                    }
                }

                if(ComandZeileCountS)
                {
                    ComandZeileCountS=false;
                    if(ComandZeileStatus)
                    {
                        if(ComandZeileCount==ComandZeileSize)
                        {
                            ComandZeileStatus=false;
                        }
                        else
                        {
                            WriteC64Byte(0xC6,1);
                            WriteC64Byte(0x277,static_cast<uint8_t>(ComandZeile[ComandZeileCount]));
                            ComandZeileCount++;
                        }
                    }
                }
                if(ReadC64Byte(0xC6)==0)
                {
                    ComandZeileCountS=true;
                }
            }
            //////////////////////////////////////////////////////////////////////////////


            if(IecIsDumped)
            {
                IecVcdExport.NextStep();
                IecVcdExport.SetWire(0,C64IEC & 16);
                IecVcdExport.SetWire(1,C64IEC & 64);
                IecVcdExport.SetWire(2,C64IEC & 128);
                IecVcdExport.SetWire(3,FloppyIEC & 64);
                IecVcdExport.SetWire(4,FloppyIEC & 128);
            }

        }



        if(StereoEnable)
        {
            if(!Sid6ChannelMode)
            {
                int j=0;
                for(int i=0; i<(laenge/2); i+=2)
                {
                    puffer[i] = static_cast<int16_t>(sid1->SoundBuffer[j] * SIDVolume);
                    puffer[i+1] = static_cast<int16_t>(sid2->SoundBuffer[j] * SIDVolume);
                    j++;
                }
            }
            else
            {
                int j=0;
                for(int i=0; i<(laenge/2); i+=2)
                {
                    puffer[i] = puffer[i+1] = static_cast<int16_t>(static_cast<float_t>(sid1->SoundBuffer[j] + sid2->SoundBuffer[j]) * SIDVolume * 0.75f);
                    j++;
                }
            }
        }
        else
        {
            int j=0;
            for(int i=0; i<(laenge/2); i+=2)
            {
                puffer[i] = puffer[i+1] = static_cast<int16_t>(sid1->SoundBuffer[j] * SIDVolume);
                j++;
            }
        }

        /// Capture Audio
        VideoCapture->FillSourceAudioBuffer(puffer, laenge/2);

        /// Floppysound dazu mixen ///
        for(int i=0; i<FLOPPY_ANZAHL; i++)
        {
            if(floppy[i]->GetEnableFloppySound()) SDL_MixAudio(reinterpret_cast<uint8_t*>(puffer),reinterpret_cast<uint8_t*>(floppy[i]->GetSoundBuffer()),static_cast<uint32_t>(laenge),255);
        }

        /// Tapesound dazu mixen ///
        SDL_MixAudio(reinterpret_cast<uint8_t*>(puffer),reinterpret_cast<uint8_t*>(tape->GetSoundBuffer()),static_cast<uint32_t>(laenge),255);
    }
    else
    {
        if(DebugAnimation)
        {
            AnimationSpeedCounter += AnimationSpeedAdd;
            if(AnimationSpeedCounter >= 1)
            {
                for(int i=0;i<static_cast<int>(AnimationSpeedCounter);i++)
                {
                    CheckKeys();
                    CycleCounter++;

                    /// Für Externe Erweiterungen ///
                    //if(ExtZyklus) ZyklusProcExt();

                    FloppyIEC = 0;
                    for(int i=0; i<FLOPPY_ANZAHL; i++)
                    {
                        floppy[i]->OneZyklus();

                        #ifdef floppy_asyncron
                        counter_plus++;
                        if(counter_plus == more_one_floppy_cylce_count)
                        {
                            counter_plus = 0;
                            floppy[i]->OneZyklus();
                        }
                        #endif

                        FloppyIEC |= ~floppy[i]->FloppyIECLocal;
                    }
                    FloppyIEC = ~FloppyIEC;

                    vic->OneZyklus();

                    cia1->OneZyklus();
                    cia2->OneZyklus();
                    sid1->OneZyklus();
                    if(StereoEnable) sid2->OneZyklus();
                    reu->OneZyklus();
                    tape->OneCycle();

                    if(EnableExtLines) RDY_BA = ExtRDY;
                    cpu->OneZyklus();

                    if((BreakStatus != 0) || (FloppyFoundBreakpoint == true )) if(CheckBreakpoints()) break;

                    if(IecIsDumped)
                    {
                        IecVcdExport.NextStep();
                        IecVcdExport.SetWire(0,C64IEC & 16);
                        IecVcdExport.SetWire(1,C64IEC & 64);
                        IecVcdExport.SetWire(2,C64IEC & 128);
                        IecVcdExport.SetWire(3,FloppyIEC & 64);
                        IecVcdExport.SetWire(4,FloppyIEC & 128);
                    }
                }
                AnimationSpeedCounter = 0;
            }
            if(AnimationRefreshProc != nullptr) AnimationRefreshProc();
        }
        else
        {
            if(OneZyk)
            {
                OneZyk = false;

                CheckKeys();
                CycleCounter++;

                /// Für Externe Erweiterungen ///
                //if(ExtZyklus) ZyklusProcExt();

                FloppyIEC = 0;
                for(int i=0; i<FLOPPY_ANZAHL; i++)
                {
                    floppy[i]->OneZyklus();

                    #ifdef floppy_asyncron
                    counter_plus++;
                    if(counter_plus == more_one_floppy_cylce_count)
                    {
                        counter_plus = 0;
                        floppy[i]->OneZyklus();
                    }
                    #endif

                    FloppyIEC |= ~floppy[i]->FloppyIECLocal;
                }
                FloppyIEC = ~FloppyIEC;

                vic->OneZyklus();
                cia1->OneZyklus();
                cia2->OneZyklus();
                sid1->OneZyklus();
                if(StereoEnable) sid2->OneZyklus();
                reu->OneZyklus();
                tape->OneCycle();

                if(EnableExtLines) RDY_BA = ExtRDY;
                cpu->OneZyklus();

                if((BreakStatus != 0) || (FloppyFoundBreakpoint == true )) CheckBreakpoints();

                if(AnimationRefreshProc != nullptr) AnimationRefreshProc();

                if(IecIsDumped)
                {
                    IecVcdExport.NextStep();
                    IecVcdExport.SetWire(0,C64IEC & 16);
                    IecVcdExport.SetWire(1,C64IEC & 64);
                    IecVcdExport.SetWire(2,C64IEC & 128);
                    IecVcdExport.SetWire(3,FloppyIEC & 64);
                    IecVcdExport.SetWire(4,FloppyIEC & 128);
                }
            }

            if(OneOpc)
            {
                OneOpc = false;

loop_wait_next_opc:

                CheckKeys();
                CycleCounter++;

                /// Für Externe Erweiterungen ///
                //if(ExtZyklus) ZyklusProcExt();

                vic->OneZyklus();
                cia1->OneZyklus();
                cia2->OneZyklus();
                sid1->OneZyklus();
                if(StereoEnable) sid2->OneZyklus();
                reu->OneZyklus();
                tape->OneCycle();

                // Prüfen welches die aktuelle CPU ist (CPU,Floppy0,..1,..2,..3)
                // Diese wird genau um ein Opcode ausgeführt
                if(OneOpcSource > 0)
                {
                    int FloppyNr = OneOpcSource - 1;
                    FloppyIEC = 0;
                    for(int i=0; i<FLOPPY_ANZAHL; i++)
                    {
                        if(i != FloppyNr)
                        {
                            floppy[i]->OneZyklus();

                            #ifdef floppy_asyncron
                            counter_plus++;
                            if(counter_plus == more_one_floppy_cylce_count)
                            {
                                counter_plus = 0;
                                floppy[i]->OneZyklus();
                            }
                            #endif

                            FloppyIEC |= ~floppy[i]->FloppyIECLocal;
                        }
                    }
                    bool ret = floppy[FloppyNr]->OneZyklus();
                    FloppyIEC |= ~floppy[FloppyNr]->FloppyIECLocal;
                    FloppyIEC = ~FloppyIEC;
                    if(!ret)  goto loop_wait_next_opc;
                }
                else
                {
                    if(EnableExtLines) RDY_BA = ExtRDY;
                    if(!cpu->OneZyklus()) goto loop_wait_next_opc;
                }

                if((BreakStatus != 0) || (FloppyFoundBreakpoint == true )) CheckBreakpoints();

                if(AnimationRefreshProc != nullptr) AnimationRefreshProc();

                if(IecIsDumped)
                {
                    IecVcdExport.NextStep();
                    IecVcdExport.SetWire(0,C64IEC & 16);
                    IecVcdExport.SetWire(1,C64IEC & 64);
                    IecVcdExport.SetWire(2,C64IEC & 128);
                    IecVcdExport.SetWire(3,FloppyIEC & 64);
                    IecVcdExport.SetWire(4,FloppyIEC & 128);
                }
            }
        }

        // Audiopuffer mit 0 füllen ();
        memset(puffer,0,static_cast<size_t>(laenge));
    }
}

void C64Class::KeyEvent(uint8_t matrix_code, KeyStatus key_status, bool isAutoShift)
{
    const uint8_t AutoShift = 0x17; // C64 Shift Links

    switch(key_status)
    {
    case KEY_DOWN:
        KeyboardMatrixToPB[matrix_code>>4]|=1<<(matrix_code&0x07);
        if(isAutoShift) KeyboardMatrixToPB[AutoShift>>4]|=1<<(AutoShift&0x07);

        KeyboardMatrixToPA[matrix_code&0x07]|=(1<<(matrix_code>>4));
        if(isAutoShift) KeyboardMatrixToPA[AutoShift&0x07]|=(1<<(AutoShift>>4));
        break;
    case KEY_UP:
        KeyboardMatrixToPB[matrix_code>>4]&=~(1<<(matrix_code&0x07));
        if(isAutoShift) KeyboardMatrixToPB[AutoShift>>4]&=~(1<<(AutoShift&0x07));

        KeyboardMatrixToPA[matrix_code&0x07]&=~(1<<(matrix_code>>4));
        if(isAutoShift) KeyboardMatrixToPA[AutoShift&0x07]&=~(1<<(AutoShift>>4));
        break;
    }
}

void C64Class::SoftReset()
{
    WaitResetReady = false;
    SetReset(false,true);
    SDL_Delay(40);
    SetReset(true,true);
}

void C64Class::HardReset()
{
    WaitResetReady = false;
    SetReset(false,false);
    SDL_Delay(40);
    SetReset(true,true);
}

inline void C64Class::SetReset(int status, int hard_reset)
{
    RESET = status;
    if(!status)
    {
        crt->Reset();
        RDY_BA = true;
        if(!hard_reset) mmu->Reset();
        KillCommandLine();
    }
}

inline void C64Class::CheckKeys()
{
    uint8_t OUT_PA, OUT_PB;
    uint8_t IN_PA, IN_PB;

    OUT_PA = ~CIA1_PA.GetOutput();
    OUT_PB = ~CIA1_PB.GetOutput();

    IN_PA = IN_PB = 0;

    uint8_t cbit = 1;
    for(int i=0;i<8;i++)
    {
        if(OUT_PA & cbit) IN_PB |= (KeyboardMatrixToPB[i]|KeyboardMatrixToPBExt[i]);
        if(OUT_PB & cbit) IN_PA |= (KeyboardMatrixToPA[i]|KeyboardMatrixToPAExt[i]);
        cbit <<= 1;
    }
    CIA1_PA.SetInput(~(IN_PA|GamePort2));
    CIA1_PB.SetInput(~(IN_PB|GamePort1));
}

void C64Class::ResetC64CycleCounter()
{
    CycleCounter = 0;
}

bool C64Class::LoadC64Roms(char *kernalrom, char *basicrom, char *charrom)
{
    if(!mmu->LoadKernalRom(kernalrom)) return false;
    if(!mmu->LoadBasicRom(basicrom)) return false;
    if(!mmu->LoadCharRom(charrom)) return false;
    return true;
}

bool C64Class::LoadFloppyRom(uint8_t floppy_nr, char *dos1541rom)
{
    if(floppy_nr < FLOPPY_ANZAHL)
    {
        if(!floppy[floppy_nr]->LoadDosRom(dos1541rom)) return false;
        return true;
    }
    return false;
}

bool C64Class::LoadDiskImage(uint8_t floppy_nr, char *filename)
{
    if(floppy_nr < FLOPPY_ANZAHL)
    {
        return floppy[floppy_nr]->LoadDiskImage(filename);
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
    strcpy(ComandZeile,c64_command);
    ComandZeileSize = static_cast<int>(strlen(ComandZeile));
    ComandZeileCount = 0;
    ComandZeileStatus = true;
    ComandZeileCountS = true;
}

void C64Class::KillCommandLine()
{
    ComandZeileSize=0;
    ComandZeileStatus=false;
    ComandZeileCountS=false;
}

uint8_t C64Class::ReadC64Byte(uint16_t adresse)
{
    return ReadProcTbl[(adresse)>>8](adresse);
}

void C64Class::WriteC64Byte(uint16_t adresse, uint8_t wert)
{
    WriteProcTbl[(adresse)>>8](adresse,wert);
}

uint8_t* C64Class::GetRAMPointer(uint16_t adresse)
{
    return mmu->GetRAMPointer() + adresse;
}

void C64Class::SetGrafikModi(bool colbits32, bool doublesize, bool pal_enable, bool filter_enable, int fullres_xw, int fullres_yw)
{
    ColBits32 = colbits32;
    DoubleSize = doublesize;
    PalEnable =  pal_enable;
    FilterEnable = filter_enable;
    FullResXW = fullres_xw;
    FullResYW = fullres_yw;

    ChangeGrafikModi = true;

    char str00[255];
    sprintf(str00,">>   32Bit = %d\n>>   Doublesize = %d\n>>   PAL = %d\n>>   Filter = %d\n>>   FullResXW = %d\n>>   FullResrYW = %d\n",ColBits32,DoubleSize,PalEnable,FilterEnable,FullResXW,FullResYW);

    LogText(const_cast<char*>(">> Grafikmodus wurde gesetzt:\n"));
    LogText(str00);
}

void C64Class::SetWindowTitle(char *title_name)
{
    strcpy(window_title, title_name);
    SDL_SetWindowTitle(C64Window, title_name);
}

void C64Class::SetFullscreen()
{
    isFullscreen = true;
    SDL_ShowCursor(false);
    SDL_SetWindowFullscreen(C64Window,SDL_WINDOW_FULLSCREEN_DESKTOP);
    SetFocusToC64Window();
}

void C64Class::InitGrafik()
{
    /// VicRefresh stoppen und solange warten bis wirklich Stop ist ///
    HoldVicRefresh = true;
    VicRefreshIsHold = false;
    while(!VicRefreshIsHold)
    {
          SDL_Delay(1);
    }

    /// Allegmeine Einstellungen ///

    if((FullResXW != 0) && (FullResYW != 0))
        isFullscreen  = true;
    else isFullscreen = false;


    AktWindowXW = AktC64ScreenXW = C64ScreenXW;
    AktWindowYW = AktC64ScreenYW = vic->GetAktVicDisplayLastLine() - vic->GetAktVicDisplayFirstLine();

    if(DoubleSize)
    {
        AktWindowXW *=2;
        AktWindowYW *=2;

        AktC64ScreenXW *=2;
        AktC64ScreenYW *=2;
    }

    if(ColBits32)
    {
        AktWindowColorBits = 32;
        if(C64ScreenBuffer != nullptr)
        {
            delete[] C64ScreenBuffer;
        }
        C64ScreenBuffer = new uint8_t[AktC64ScreenXW*AktC64ScreenYW*4];
    }
    else
    {
        AktWindowColorBits = 16;
        if(C64ScreenBuffer != nullptr)
        {
            delete[] C64ScreenBuffer;
        }
        C64ScreenBuffer = new uint8_t[AktC64ScreenXW*AktC64ScreenYW*2];
    }

    pal->SetDisplayMode(AktWindowColorBits);
    pal->EnablePALOutput(PalEnable);
    pal->EnableVideoDoubleSize(DoubleSize);

    if(C64Window == nullptr)
    {
        C64Window = SDL_CreateWindow(window_title,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,AktWindowXW,AktWindowYW,SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS |SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

        SDL_SetWindowIcon(C64Window,C64ScreenIcon);
    }
    else SDL_SetWindowSize(C64Window,AktWindowXW,AktWindowYW);

    GLContext = SDL_GL_CreateContext(C64Window);

    // OpenGL Initialisieren //
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0,0,AktWindowXW,AktWindowYW);
    glMatrixMode(GL_PROJECTION);
    glOrtho(0,AktWindowXW,AktWindowYW,0,-1,1);
    glLoadIdentity();

    glGenTextures(1,&C64ScreenTexture);
    glBindTexture( GL_TEXTURE_2D, C64ScreenTexture);

    // Textur Stretching Parameter setzen
    if(FilterEnable)
    {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    }
    else
    {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    if(C64Screen != nullptr)
       SDL_FreeSurface(C64Screen);
    C64Screen = SDL_CreateRGBSurface(0,AktC64ScreenXW,AktC64ScreenYW,AktWindowColorBits,0,0,0,0);

    glTexImage2D( GL_TEXTURE_2D, 0, 4, AktC64ScreenXW,AktC64ScreenYW, 0,GL_RGBA, GL_UNSIGNED_BYTE, C64ScreenBuffer );

    GLenum  TextureFormat = 0;
    GLint   NofColors = 0;

    if(Pfeil0 != nullptr)
    {
        NofColors = Pfeil0->format->BytesPerPixel;

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

    if(Pfeil0 != nullptr)
    {
        glGenTextures(1,&Pfeil0Texture);
        glBindTexture( GL_TEXTURE_2D, Pfeil0Texture );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        //glTexImage2D( GL_TEXTURE_2D, 0, NofColors, Pfeil0->w, Pfeil0->h, 0,TextureFormat, GL_UNSIGNED_BYTE, Pfeil0->pixels );
        gluBuild2DMipmaps(GL_TEXTURE_2D, NofColors, Pfeil0->w, Pfeil0->h,TextureFormat, GL_UNSIGNED_BYTE, Pfeil0->pixels );
    }

    if(Pfeil1 != nullptr)
    {
        glGenTextures(1,&Pfeil1Texture);
        glBindTexture( GL_TEXTURE_2D, Pfeil1Texture );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        //glTexImage2D( GL_TEXTURE_2D, 0, NofColors, Pfeil1->w, Pfeil1->h, 0,TextureFormat, GL_UNSIGNED_BYTE, Pfeil1->pixels );
        gluBuild2DMipmaps(GL_TEXTURE_2D, NofColors, Pfeil1->w, Pfeil1->h,TextureFormat, GL_UNSIGNED_BYTE, Pfeil1->pixels );
    }

    if(Kreis0 != nullptr)
    {
        glGenTextures(1,&Kreis0Texture);
        glBindTexture( GL_TEXTURE_2D, Kreis0Texture );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        //glTexImage2D( GL_TEXTURE_2D, 0, NofColors, Kreis0->w, Kreis0->h, 0,TextureFormat, GL_UNSIGNED_BYTE, Kreis0->pixels );
        gluBuild2DMipmaps(GL_TEXTURE_2D, NofColors, Kreis0->w, Kreis0->h,TextureFormat, GL_UNSIGNED_BYTE, Kreis0->pixels );
    }

    if(Kreis1 != nullptr)
    {
        glGenTextures(1,&Kreis1Texture);
        glBindTexture( GL_TEXTURE_2D, Kreis1Texture );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        //glTexImage2D( GL_TEXTURE_2D, 0, NofColors, Kreis1->w, Kreis1->h, 0,TextureFormat, GL_UNSIGNED_BYTE, Kreis1->pixels );
        gluBuild2DMipmaps(GL_TEXTURE_2D, NofColors, Kreis1->w, Kreis1->h,TextureFormat, GL_UNSIGNED_BYTE, Kreis1->pixels );
    }

    /// VicRefresh wieder zulassen ///
    HoldVicRefresh = false;
}

void C64Class::ReleaseGrafik()
{
    ChangeGrafikModi = false;

    /// VicRefresh stoppen und solange warten bis wirklich Stop ist ///
    HoldVicRefresh = true;
    VicRefreshIsHold = false;
    while(!VicRefreshIsHold)
    {
          SDL_Delay(1);
    }

    if(C64Screen != nullptr)
    {
       SDL_FreeSurface(C64Screen);
       C64Screen = nullptr;
    }

    if(C64Window != nullptr)
    {
        SDL_DestroyWindow(C64Window);
        C64Window = nullptr;
    }

    if(C64ScreenBuffer != nullptr)
    {
        delete[] C64ScreenBuffer;
        C64ScreenBuffer = nullptr;
    }
}

void C64Class::DrawC64Screen()
{
    /// Fensterinhalt löschen
    glClearColor(0.0f, 0.0f, 0.0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /// Texture auswählen und mit aktuellen C64 Bilddaten füllen
    glBindTexture(GL_TEXTURE_2D,C64ScreenTexture);
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0, AktC64ScreenXW, AktC64ScreenYW,GL_RGBA, GL_UNSIGNED_BYTE, C64Screen->pixels);

    /// Aspect Ratio berechnen
    float_t scale_x;
    float_t scale_y;

    if((EnableWindowAspectRatio == false && isFullscreen == false) || (EnableFullscreenAspectRatio == false && isFullscreen == true))
    {
        scale_x = 1.0f;
        scale_y = 1.0f;
    }
    else
    {
        if((static_cast<float_t>(AktWindowXW) / static_cast<float_t>(AktWindowYW)) >= C64ScreenAspectRatio)
        {
            scale_y = 1.0f;
            int xw = int(AktWindowYW * C64ScreenAspectRatio);
            scale_x = (1.0f * xw) / AktWindowXW;
        }
        else
        {
            scale_x = 1.0f;
            int yw = int(AktWindowXW / C64ScreenAspectRatio);
            scale_y = (1.0f * yw) / AktWindowYW;
        }
    }

    /// C64 Screen darstellen
    glBegin(GL_QUADS);
    for(int i=0; i < SUBDIVS_SCREEN*SUBDIVS_SCREEN*4; i+=4)
    {
        glTexCoord2f(DistortionGridTex[i+0].x,DistortionGridTex[i+0].y);
        glVertex3f(DistortionGrid[i+0].x*scale_x,DistortionGrid[i+0].y*scale_y,0.0);

        glTexCoord2f(DistortionGridTex[i+1].x,DistortionGridTex[i+1].y);
        glVertex3f(DistortionGrid[i+1].x*scale_x,DistortionGrid[i+1].y*scale_y,0.0);

        glTexCoord2f(DistortionGridTex[i+2].x,DistortionGridTex[i+2].y);
        glVertex3f(DistortionGrid[i+2].x*scale_x,DistortionGrid[i+2].y*scale_y,0.0);

        glTexCoord2f(DistortionGridTex[i+3].x,DistortionGridTex[i+3].y);
        glVertex3f(DistortionGrid[i+3].x*scale_x,DistortionGrid[i+3].y*scale_y,0.0);
    }
    glEnd();

    /// Pfeile für Joymapping darstenn
    if(RecJoyMapping)
    {
        /// Nach Oben ///
        if(RecJoyMappingPos == 0) glBindTexture(GL_TEXTURE_2D,Pfeil1Texture);
        else glBindTexture(GL_TEXTURE_2D,Pfeil0Texture);
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
        if(RecJoyMappingPos == 1) glBindTexture(GL_TEXTURE_2D,Pfeil1Texture);
        else glBindTexture(GL_TEXTURE_2D,Pfeil0Texture);
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
        if(RecJoyMappingPos == 2) glBindTexture(GL_TEXTURE_2D,Pfeil1Texture);
        else glBindTexture(GL_TEXTURE_2D,Pfeil0Texture);
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
        if(RecJoyMappingPos == 3) glBindTexture(GL_TEXTURE_2D,Pfeil1Texture);
        else glBindTexture(GL_TEXTURE_2D,Pfeil0Texture);
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
        if(RecJoyMappingPos == 4) glBindTexture(GL_TEXTURE_2D,Kreis1Texture);
        else glBindTexture(GL_TEXTURE_2D,Kreis0Texture);
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

    SDL_GL_SwapWindow(C64Window);
}

void C64Class::SetFocusToC64Window()
{
    SDL_RaiseWindow(C64Window);
}

void C64Class::SetWindowAspectRatio(bool enabled)
{
    EnableWindowAspectRatio = enabled;
}

void C64Class::SetFullscreenAspectRatio(bool enabled)
{
    EnableFullscreenAspectRatio = enabled;
}

void C64Class::AnalyzeSDLEvent(SDL_Event *event)
{
    static bool joy_center_flag = true;
    static char joy_axis_tbl[5] = {1,1,0,0,-1};

    SDL_Keymod keymod;

    switch (event->type)
    {
        case SDL_WINDOWEVENT:

        switch (event->window.event)
        {
            case SDL_WINDOWEVENT_RESIZED:

                AktWindowXW = static_cast<uint16_t>(event->window.data1);
                AktWindowYW = static_cast<uint16_t>(event->window.data2);
                glViewport(0,0,AktWindowXW,AktWindowYW);
                glMatrixMode(GL_PROJECTION);
                glOrtho(0,AktWindowXW,AktWindowYW,0,-1,1);
                glLoadIdentity();
                break;

            default:
                break;
        }
        break;

        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
        {
            if((RecJoyMapping == true) && (event->jbutton.type == SDL_JOYBUTTONDOWN))
            {
                VJoys[RecJoySlotNr].Type[RecJoyMappingPos] = VJOY_TYPE_BUTTON;
                VJoys[RecJoySlotNr].JoyIndex[RecJoyMappingPos] = static_cast<uint8_t>(event->jbutton.which);
                VJoys[RecJoySlotNr].ButtonNr[RecJoyMappingPos] = event->jbutton.button;
            }
            else if((RecJoyMapping == true) && (event->jbutton.type == SDL_JOYBUTTONUP) && (RecPollingWait == false))
            {
                RecJoyMappingPos++;
                if(RecJoyMappingPos == 5)
                {
                    /// Rec Mapping ist fertig ///
                    RecJoyMapping = false;
                }
                else
                {
                    RecPollingWait = true;
                    RecPollingWaitCounter = RecPollingWaitStart;
                }
            }
            else
            {
                /// VJoyStick abfrage ///
                /// Port1
                if((VPort1 >= 0) && (VPort1 < MAX_VJOYS))
                {
                    for(int i=0;i<5;i++)
                    {
                        if((VJoys[VPort1].ButtonNr[i] == event->jbutton.button) &&
                                (VJoys[VPort1].Type[i] == VJOY_TYPE_BUTTON) &&
                                (VJoys[VPort1].JoyIndex[i] == event->jbutton.which))
                        {
                            if(event->jbutton.state == 1) GamePort1 |= 1<<i;
                            else GamePort1 &= ~(1<<i);
                        }
                    }
                }

                /// Port2
                if((VPort2 >= 0) && (VPort2 < MAX_VJOYS))
                {
                    for(int i=0;i<5;i++)
                    {
                        if((VJoys[VPort2].ButtonNr[i] == event->jbutton.button) &&
                                (VJoys[VPort2].Type[i] == VJOY_TYPE_BUTTON) &&
                                (VJoys[VPort2].JoyIndex[i] == event->jbutton.which))
                        {
                            if(event->jbutton.state == 1) GamePort2 |= 1<<i;
                            else GamePort2 &= ~(1<<i);
                        }
                    }
                }
            }
            break;
        }

        case SDL_JOYHATMOTION:
        {
            if((RecJoyMapping == true) && (RecPollingWait == false))
            {
                if(event->jhat.value > 0)
                {
                    VJoys[RecJoySlotNr].Type[RecJoyMappingPos] = VJOY_TYPE_HAT;
                    VJoys[RecJoySlotNr].JoyIndex[RecJoyMappingPos] = static_cast<uint8_t>(event->jhat.which);
                    VJoys[RecJoySlotNr].HatNr[RecJoyMappingPos] = event->jhat.hat;
                    VJoys[RecJoySlotNr].HatValue[RecJoyMappingPos] = event->jhat.value;
                }
                else
                {
                    RecJoyMappingPos++;
                    if(RecJoyMappingPos == 5)
                    {
                        /// Rec Mapping ist fertig ///
                        RecJoyMapping = false;
                    }
                    else
                    {
                        RecPollingWait = true;
                        RecPollingWaitCounter = RecPollingWaitStart;
                    }
                }
            }
            else
            {
                /// VJoyStick abfrage ///
                /// Port1
                if((VPort1 >= 0) && (VPort1 < MAX_VJOYS))
                {
                    for(int i=0;i<5;i++)
                    {
                        if((VJoys[VPort1].HatNr[i] == event->jhat.hat) &&
                                (VJoys[VPort1].Type[i] == VJOY_TYPE_HAT) &&
                                (VJoys[VPort1].JoyIndex[i] == event->jhat.which))
                        {
                            if((event->jhat.value & VJoys[VPort1].HatValue[i]) == VJoys[VPort1].HatValue[i]) GamePort1 |= 1<<i;
                            else GamePort1 &= ~(1<<i);
                        }
                    }
                }

                /// VJoyStick abfrage ///
                /// Port2
                if((VPort2 >= 0) && (VPort2 < MAX_VJOYS))
                {
                    for(int i=0;i<5;i++)
                    {
                        if((VJoys[VPort2].HatNr[i] == event->jhat.hat) &&
                                (VJoys[VPort2].Type[i] == VJOY_TYPE_HAT) &&
                                (VJoys[VPort2].JoyIndex[i] == event->jhat.which))
                        {
                            if((event->jhat.value & VJoys[VPort2].HatValue[i]) == VJoys[VPort2].HatValue[i]) GamePort2 |= 1<<i;
                            else GamePort2 &= ~(1<<i);
                        }
                    }
                }

            }
          break;
        }

        case SDL_JOYAXISMOTION:
        {
            if((RecJoyMapping == true) && (RecPollingWait == false))
            {
                if(((event->jaxis.value < -16000) || (event->jaxis.value > 16000)) && (joy_center_flag == true) && ((event->jaxis.axis & 1) == joy_axis_tbl[RecJoyMappingPos]))
                {
                    joy_center_flag = false;

                    VJoys[RecJoySlotNr].Type[RecJoyMappingPos] = VJOY_TYPE_AXIS;
                    VJoys[RecJoySlotNr].JoyIndex[RecJoyMappingPos] = static_cast<uint8_t>(event->jaxis.which);
                    VJoys[RecJoySlotNr].AxisNr[RecJoyMappingPos] = event->jaxis.axis;
                    if(event->jaxis.value > 0) VJoys[RecJoySlotNr].AxisValue[RecJoyMappingPos] = 0;
                    else VJoys[RecJoySlotNr].AxisValue[RecJoyMappingPos] = 1;

                    RecJoyMappingPos++;
                    if(RecJoyMappingPos == 5)
                    {
                        /// Rec Mapping ist fertig ///
                        RecJoyMapping = false;
                    }
                    else
                    {
                        RecPollingWait = true;
                        RecPollingWaitCounter = RecPollingWaitStart;
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
                if((VPort1 >= 0) && (VPort1 < MAX_VJOYS))
                {
                    for(int i=0;i<5;i++)
                    {
                        if((VJoys[VPort1].AxisNr[i] == event->jaxis.axis) &&
                                (VJoys[VPort1].Type[i] == VJOY_TYPE_AXIS) &&
                                (VJoys[VPort1].JoyIndex[i] == event->jaxis.which))
                        {
                            if(!((event->jaxis.value >= -14000) && (event->jaxis.value <= 14000)))
                            {
                                if(event->jaxis.value > 16000)
                                {
                                    if(VJoys[VPort1].AxisValue[i] == 0) GamePort1 |= 1<<i;
                                }
                                else if(event->jaxis.value < -16000)
                                {
                                    if(VJoys[VPort1].AxisValue[i] == 1) GamePort1 |= 1<<i;
                                }
                            }
                            else GamePort1 &= ~(1<<i);
                        }
                    }
                }

                /// VJoyStick abfrage ///
                /// Port2
                if((VPort2 >= 0) && (VPort2 < MAX_VJOYS))
                {
                    for(int i=0;i<5;i++)
                    {
                        if((VJoys[VPort2].AxisNr[i] == event->jaxis.axis) &&
                                (VJoys[VPort2].Type[i] == VJOY_TYPE_AXIS) &&
                                (VJoys[VPort2].JoyIndex[i] == event->jaxis.which))
                        {
                            if(!((event->jaxis.value >= -14000) && (event->jaxis.value <= 14000)))
                            {
                                if(event->jaxis.value > 16000)
                                {
                                    if(VJoys[VPort2].AxisValue[i] == 0) GamePort2 |= 1<<i;
                                }
                                else if(event->jaxis.value < -16000)
                                {
                                    if(VJoys[VPort2].AxisValue[i] == 1) GamePort2 |= 1<<i;
                                }
                            }
                            else GamePort2 &= ~(1<<i);
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
            case SDLK_F12:
                if(RecJoyMapping)
                {
                    RecJoyMapping = false;
                }
                else
                {
                    if(event->key.keysym.mod == KMOD_LSHIFT)
                    {
                        HardReset();
                        RESET = false;
                    }
                    else
                    {
                        WaitResetReady = false;
                        RESET = false;
                    }
                }

                break;

              case SDLK_RETURN:

                keymod = SDL_GetModState();

                if((KMOD_MODE == (keymod & KMOD_MODE) || KMOD_LALT == (keymod & KMOD_LALT) || KMOD_RALT == (keymod & KMOD_RALT)) && (isReturnKeyDown == false))
                {
                    isFullscreen = !isFullscreen;
                    if(isFullscreen)
                    {
                        SDL_ShowCursor(false);
                        SDL_SetWindowFullscreen(C64Window,SDL_WINDOW_FULLSCREEN_DESKTOP);
                    }
                    else
                    {
                        SDL_ShowCursor(true);
                        SDL_SetWindowFullscreen(C64Window,0);
                    }

                }

                isReturnKeyDown = true;
                break;

            default:
                break;
            }

            if(RecJoyMapping == true)
            {
                VJoys[RecJoySlotNr].Type[RecJoyMappingPos] = VJOY_TYPE_KEY;
                VJoys[RecJoySlotNr].KeyDown[RecJoyMappingPos] = static_cast<uint8_t>(event->key.keysym.scancode);
            }
            else
            {
                if(IsKeyMapRec)
                {
                    // Mappen der gedrückten Taste
                    for(int i=0;i<C64KeyNum;i++)
                    {
                        if(C64KeyTable[i].SDLKeyCode == event->key.keysym.sym)
                        {
                            C64KeyTable[i].SDLKeyCode = 0xFFFF;
                        }
                    }

                    C64KeyTable[((RecMatrixCode & 0xF0)>>1) + (RecMatrixCode & 0x07)].SDLKeyCode = event->key.keysym.sym;
                    IsKeyMapRec = false;
                }
                else
                {
                    /// Auf RESTORE KEY TESTEN
                    if(C64KeyTable[64].SDLKeyCode == event->key.keysym.sym)
                        cpu->TriggerInterrupt(RESTORE_NMI);
                    else
                    {
                        for(int i=0;i<C64KeyNum;i++)
                        {
                            if(C64KeyTable[i].SDLKeyCode == event->key.keysym.sym && (event->key.keysym.mod != KMOD_LALT) && (event->key.keysym.mod != KMOD_RALT))
                            {
                                KeyEvent(C64KeyTable[i].MatrixCode,KEY_DOWN,C64KeyTable[i].Shift);
                            }
                        }
                    }
                }
            }

            /// VJoyStick abfrage ///
            /// Port1
            if((VPort1 >= 0) && (VPort1 < MAX_VJOYS))
            {
                for(int i=0;i<5;i++)
                {
                    if((VJoys[VPort1].KeyDown[i] == event->key.keysym.scancode) &&
                            (VJoys[VPort1].Type[i] == VJOY_TYPE_KEY))
                        GamePort1 |= 1<<i;
                }
            }

            /// Port2
            if((VPort2 >= 0) && (VPort2 < MAX_VJOYS))
            {
                for(int i=0;i<5;i++)
                {
                    if((VJoys[VPort2].KeyDown[i] == event->key.keysym.scancode) &&
                            (VJoys[VPort2].Type[i] == VJOY_TYPE_KEY))
                        GamePort2 |= 1<<i;
                }
            }
            break;
        }

        case SDL_KEYUP:
        {
            switch(event->key.keysym.sym)
            {

            case SDLK_RETURN:
                isReturnKeyDown = false;
                break;

            case SDLK_F12:
               RESET = true;
                break;

            default:
                break;
            }

            if(RecJoyMapping == true)
            {
                VJoys[RecJoySlotNr].Type[RecJoyMappingPos] = VJOY_TYPE_KEY;
                VJoys[RecJoySlotNr].KeyUp[RecJoyMappingPos] = static_cast<uint8_t>(event->key.keysym.scancode);

                RecJoyMappingPos++;
                if(RecJoyMappingPos == 5)
                {
                    /// Rec Mapping ist fertig ///
                    RecJoyMapping = false;
                }
            }
            else
            {
                /// Auf RESTORE KEY TESTEN
                if(C64KeyTable[64].SDLKeyCode == event->key.keysym.sym)
                    cpu->ClearInterrupt(RESTORE_NMI);
                else
                {
                    for(int i=0;i<C64KeyNum;i++)
                    {
                        if(C64KeyTable[i].SDLKeyCode == event->key.keysym.sym)
                        {
                            KeyEvent(C64KeyTable[i].MatrixCode,KEY_UP,C64KeyTable[i].Shift);
                        }
                    }
                }
            }

            /// VJoyStick abfrage ///
            /// Port1
            if((VPort1 >= 0) && (VPort1 < MAX_VJOYS))
            {
                for(int i=0;i<5;i++)
                {
                    if((VJoys[VPort1].KeyUp[i] == event->key.keysym.scancode) &&
                            (VJoys[VPort1].Type[i] == VJOY_TYPE_KEY))
                        GamePort1 &= ~(1<<i);
                }
            }

            /// Port2
            if((VPort2 >= 0) && (VPort2 < MAX_VJOYS))
            {
                for(int i=0;i<5;i++)
                {
                    if((VJoys[VPort2].KeyUp[i] == event->key.keysym.scancode) &&
                            (VJoys[VPort2].Type[i] == VJOY_TYPE_KEY))
                        GamePort2 &= ~(1<<i);
                }
            }
            break;
        }

        case SDL_MOUSEBUTTONDOWN:
            switch(event->button.button)
            {
            case SDL_BUTTON_LEFT:

                // Wenn Linke STRG Taste gedrück dann Mouse 1351 umschalten
                if(KMOD_LCTRL == SDL_GetModState())
                {
                    Mouse1351Enable = !Mouse1351Enable;
                    SDL_SetRelativeMouseMode(SDL_bool(Mouse1351Enable));
                    MouseXRel = MouseYRel = 0;
                }

                if(Mouse1351Enable)
                {
                    if(MousePort == 0)
                        GamePort1 |= 0x10;
                    else
                        GamePort2 |= 0x10;
                }
                break;

            case SDL_BUTTON_RIGHT:

                if(Mouse1351Enable)
                {
                    if(MousePort == 0)
                        GamePort1 |= 0x01;
                    else
                        GamePort2 |= 0x01;
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

            if(Mouse1351Enable)
            {
                if(MousePort == 0)
                    GamePort1 &= ~0x10;
                else GamePort2 &= ~0x10;
            }
            break;

        case SDL_BUTTON_RIGHT:

            if(Mouse1351Enable)
            {
                if(MousePort == 0)
                    GamePort1 &= ~0x01;
                else GamePort2 &= ~0x01;
            }
            break;

        default:
            break;
        }
        break;

        case SDL_MOUSEMOTION:

        if(!isFullscreen)
        {
            MouseHiddenCounter = 0;
            MouseIsHidden = false;
            SDL_ShowCursor(true);
        }

        if(Mouse1351Enable)
        {
            MouseXRel += event->motion.xrel;
            MouseYRel += event->motion.yrel;
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
    Distortion = value;
    CalcDistortionGrid();
}

void C64Class::SetMouseHiddenTime(int time)
{
    MouseHiddenTime = time;
}

void C64Class::GetWindowPos(int *x, int *y)
{
    SDL_GetWindowPosition(C64Window, x, y);
}

void C64Class::SetWindowPos(int x, int y)
{
    this->win_pos_x = x;
    this->win_pos_y = y;
    ChangeWindowPos = true;
}

void C64Class::GetWindowSize(int *w, int *h)
{
    SDL_GetWindowSize(C64Window, w, h);
}

void C64Class::SetWindowSize(int w, int h)
{
    this->win_size_w = w;
    this->win_size_h = h;
    ChangeWindowSize = true;
}

bool C64Class::LoadTapeImage(char *filename)
{
    return tape->LoadTapeImage(filename);
}

bool C64Class::RecordTapeImage(char *filename)
{
    return tape->RecordTapeImage(filename);
}

uint8_t C64Class::SetTapeKeys(uint8_t pressed_key)
{
    return tape->SetTapeKeys(pressed_key);
}

bool C64Class::GetTapeMotorStatus()
{
    return !(CPU_PORT.DATA_READ & 32);
}

bool C64Class::GetTapeRecordLedStatus()
{
    if((!(CPU_PORT.DATA_READ & 32)) && tape->IsPressedRecord()) return true;
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

void C64Class::SetC64Speed(int speed)
{
    sid1->SetC64Zyklen(C64Takt*(speed/100.f));
    sid2->SetC64Zyklen(C64Takt*(speed/100.f));
    tape->SetC64Zyklen(C64Takt*(speed/100.f));
}

void C64Class::EnableWarpMode(bool enabled)
{
    if(enabled == WarpMode) return;

    // WarpMode setzen
    WarpMode = enabled;

    if(WarpMode)
    {
        // WarpMode aktivieren
        SDL_PauseAudio(1);          // Audiostream pausieren
        SDL_LockMutex(mutex1);      // Warten auf Mutex1 und sperren
        warp_thread_end = false;
        warp_thread = SDL_CreateThread(SDLThreadWarp,"WarpThread",this);
    }
    else
    {
        // WarpMode deaktivieren
        warp_thread_end = true;
        SDL_UnlockMutex(mutex1);    // Mutex1 wieder freigeben
        SDL_PauseAudio(0);          // Audiostream wieder starten
    }
}

int SDLThreadLoad(void *userdat)
{
    uint16_t PRGStartAdresse;
    C64Class *c64 = static_cast<C64Class*>(userdat);

    switch(c64->AutoLoadMode)
    {
    case 0:
        c64->SetCommandLine(c64->AutoLoadCommandLine);
        break;
    case 1:
        c64->LoadPRG(c64->AutoLoadFilename,&PRGStartAdresse);
        if(PRGStartAdresse <= 0x0801) sprintf(c64->AutoLoadCommandLine,"RUN%c",13);
        else sprintf(c64->AutoLoadCommandLine,"SYS %d%c",PRGStartAdresse,13);
        c64->SetCommandLine(c64->AutoLoadCommandLine);
        break;
    case 2:
        c64->LoadPRG(c64->AutoLoadFilename,&PRGStartAdresse);
        //if(LoadPRG(c64->AutoLoadFilename,&PRGStartAdresse) == 5) return 4; Behandlung wenn mehr als 1 File in T64
        if(PRGStartAdresse <= 0x0801) sprintf(c64->AutoLoadCommandLine,"RUN%c",13);
        else sprintf(c64->AutoLoadCommandLine,"SYS %d%c",PRGStartAdresse,13);
        c64->SetCommandLine(c64->AutoLoadCommandLine);
        break;
    }
    return 0;
}

// ret 0=OK 1=nicht unterstütztes Format 2=D64 n.IO 3=G64 n.IO 4=OK nur es war ein CRT
int C64Class::LoadAutoRun(uint8_t floppy_nr, char *filename)
{
    char EXT[4];

    size_t len = strlen(filename);
    strcpy(EXT,filename+len-3);

    EXT[0]=static_cast<char>(toupper(EXT[0]));
    EXT[1]=static_cast<char>(toupper(EXT[1]));
    EXT[2]=static_cast<char>(toupper(EXT[2]));

    if(0==strcmp("D64",EXT))
    {
        if(!LoadDiskImage(floppy_nr,filename)) return 2;

        KillCommandLine();
        AutoLoadMode = 0;
        sprintf(AutoLoadCommandLine,"LOAD\"*\",%d,1%cRUN%c",floppy_nr+8,13,13);
        HardReset();
        WaitResetReady = true;
        C64ResetReady = false;
        FloppyResetReady[0] = false;
        return 0;
    }

    if(0==strcmp("G64",EXT))
    {
        if(!LoadDiskImage(floppy_nr,filename)) return 3;

        KillCommandLine();
        AutoLoadMode = 0;
        sprintf(AutoLoadCommandLine,"LOAD\"*\",%d,1%cRUN%c",floppy_nr+8,13,13);
        HardReset();
        WaitResetReady = true;
        C64ResetReady = false;
        FloppyResetReady[0] = false;
        return 0;
    }

    if(0==strcmp("PRG",EXT) || 0==strcmp("C64",EXT))
    {
        KillCommandLine();
        AutoLoadMode = 1;
        strcpy(AutoLoadFilename,filename);
        HardReset();
        WaitResetReady = true;
        C64ResetReady = false;
        FloppyResetReady[0] = false;
        return 0;
    }

    if(0==strcmp("T64",EXT))
    {
        KillCommandLine();
        AutoLoadMode = 2;
        strcpy(AutoLoadFilename,filename);
        HardReset();
        WaitResetReady = true;
        C64ResetReady = false;
        FloppyResetReady[0] = false;
        return 0;
    }

    if(0==strcmp("P00",EXT))
    {
        KillCommandLine();
        AutoLoadMode = 1;
        strcpy(AutoLoadFilename,filename);
        HardReset();
        WaitResetReady = true;
        C64ResetReady = false;
        FloppyResetReady[0] = false;
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

        LoadCRT(filename);
        return 4;
    }
    return 1;
}

int C64Class::LoadPRG(char *filename, uint16_t *ret_startadresse)
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
        LogText(filename);
        LogText(const_cast<char*>("\n"));

        uint16_t StartAdresse;
        size_t reading_bytes;
        uint8_t temp[2];
        file = fopen (filename, "rb");
        if (file == nullptr)
        {
            LogText(const_cast<char*>("<< ERROR: Datei konnte nicht geöffnet werden"));
            return 0x01;
        }
        reading_bytes = fread (&temp,1,2,file);
        StartAdresse = static_cast<uint16_t>(temp[0]|(temp[1]<<8));
        if(ret_startadresse != nullptr) *ret_startadresse = StartAdresse;

        reading_bytes=fread (RAM+StartAdresse,1,0xFFFF-StartAdresse,file);

        RAM[0x2B] = 0x01;
        RAM[0x2C] = 0x08;

        sprintf(str00,">>   SartAdresse: $%4.4X(%d)",StartAdresse,StartAdresse);
        LogText(str00);

        StartAdresse+=static_cast<uint16_t>(reading_bytes);
        RAM[0x2D] = static_cast<uint8_t>(StartAdresse);
        RAM[0x2E] = static_cast<uint8_t>(StartAdresse>>8);
        RAM[0xAE] = static_cast<uint8_t>(StartAdresse);
        RAM[0xAF] = static_cast<uint8_t>(StartAdresse>>8);

        fclose(file);

        sprintf(str00,"EndAdresse: $%4.4X(%d)\n",StartAdresse,StartAdresse);
        LogText(str00);

        return 0x00;
    }

    if(0==strcmp("T64",EXT))
    {
        LogText(const_cast<char*>(">> T64 laden: "));
        LogText(filename);
        LogText(const_cast<char*>("\n"));

        char Kennung[32];
        size_t reading_bytes;
        uint16_t T64Entries;
        uint16_t StartAdresse;
        uint16_t EndAdresse;
        int FileStartOffset;

        file = fopen (filename, "rb");
        if (file == nullptr)
        {
            LogText(const_cast<char*>("<< ERROR: Datei konnte nicht geöffnet werden"));
                return 0x01;
        }

        reading_bytes = fread(Kennung,1,32,file);
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
        reading_bytes = fread(&StartAdresse,1,2,file);
        if(ret_startadresse != nullptr) *ret_startadresse = StartAdresse;
        reading_bytes = fread(&EndAdresse,1,2,file);
        fseek(file,2,SEEK_CUR);
        reading_bytes = fread(&FileStartOffset,1,4,file);

        fseek(file,FileStartOffset,SEEK_SET);
        reading_bytes = fread(RAM+StartAdresse,1,EndAdresse-StartAdresse,file);
        fclose(file);

        RAM[0x2B] = 0x01;
        RAM[0x2C] = 0x08;

        RAM[0x2D] = static_cast<uint8_t>(EndAdresse);
        RAM[0x2E] = static_cast<uint8_t>(EndAdresse>>8);
        RAM[0xAE] = static_cast<uint8_t>(EndAdresse);
        RAM[0xAF] = static_cast<uint8_t>(EndAdresse>>8);

        sprintf(str00,">>   SartAdresse: $%4.4X(%d)",StartAdresse,StartAdresse);
        LogText(str00);
        sprintf(str00,"EndAdresse: $%4.4X(%d)\n",EndAdresse,EndAdresse);
        LogText(str00);

        return 0x00;
    }

    if(0==strcmp("P00",EXT))
    {
        LogText(const_cast<char*>(">> P00 laden: "));
        LogText(filename);
        LogText(const_cast<char*>("\n"));

        char Kennung[8];
        uint16_t StartAdresse;
        size_t reading_bytes;
        uint8_t temp[2];
        file = fopen (filename, "rb");
        if (file == nullptr)
        {
            LogText(const_cast<char*>("<< ERROR: Datei konnte nicht geöffnet werden"));
                return 0x01;
        }

        reading_bytes = fread(Kennung,1,7,file);
        Kennung[7]=0;
        if(0!=strcmp("C64File",Kennung))
        {
                fclose(file);
                return 0x06;
        }

        fseek(file,0x1A,SEEK_SET);

        reading_bytes = fread (&temp,1,2,file);
        StartAdresse = static_cast<uint16_t>(temp[0]|(temp[1]<<8));
        if(ret_startadresse != nullptr) *ret_startadresse = StartAdresse;

        reading_bytes=fread (RAM+StartAdresse,1,0xFFFF,file);

        RAM[0x2B] = 0x01;
        RAM[0x2C] = 0x08;

        sprintf(str00,">>   SartAdresse: $%4.4X(%d)",StartAdresse,StartAdresse);
        LogText(str00);

        StartAdresse += static_cast<uint16_t>(reading_bytes);
        RAM[0x2D] = static_cast<uint8_t>(StartAdresse);
        RAM[0x2E] = static_cast<uint8_t>(StartAdresse>>8);
        RAM[0xAE] = static_cast<uint8_t>(StartAdresse);
        RAM[0xAF] = static_cast<uint8_t>(StartAdresse>>8);

        fclose(file);

        sprintf(str00,"EndAdresse: $%4.4X(%d)\n",StartAdresse,StartAdresse);
        LogText(str00);

        return 0x00;
    }
    return 0x02;
}

int C64Class::LoadCRT(char *filename)
{
    reu->Remove();
    geo->Remove();

    int ret = crt->LoadCRTImage(filename);
    if(ret == 0)
    {
        IOSource = 1;

        KillCommandLine();
        HardReset();
    }
    return ret;
}

void C64Class::RemoveCRT()
{
    crt->RemoveCRTImage();
    IOSource = 0;
    KillCommandLine();
    HardReset();
}

int C64Class::CreateNewEasyFlashImage(char *filename, char *crt_name)
{
    return crt->CreateNewEasyFlashImage(filename, crt_name);
}

void C64Class::InsertREU()
{
    IOSource = 2;

    crt->RemoveCRTImage();
    geo->Remove();

    reu->Insert();
    ReuIsInsert = true;

    KillCommandLine();
    HardReset();
}

void C64Class::RemoveREU()
{
    reu->Remove();
    IOSource = 0;

    ReuIsInsert = false;

    KillCommandLine();
    HardReset();
}

int C64Class::LoadREUImage(char *filename)
{
    return reu->LoadRAM(filename);
}

int C64Class::SaveREUImage(char *filename)
{
    return reu->SaveRAM(filename);
}

void C64Class::ClearREURam()
{
    reu->ClearRAM();
}

void C64Class::InsertGEORAM()
{
    IOSource = 3;

    crt->RemoveCRTImage();
    reu->Remove();
    ReuIsInsert = false;

    geo->Insert();

    KillCommandLine();
    HardReset();
}

void C64Class::RemoveGEORAM()
{
    geo->Remove();
    IOSource = 0;

    KillCommandLine();
    HardReset();
}

int C64Class::LoadGEORAMImage(char *filename)
{
    return geo->LoadRAM(filename);
}

int C64Class::SaveGEORAMImage(char *filename)
{
    return geo->SaveRAM(filename);
}

void C64Class::ClearGEORAMRam()
{
    geo->ClearRAM();
}

void C64Class::SetMouse1351Port(uint8_t port)
{
    MousePort = port;
}

void C64Class::SetDebugMode(bool status)
{
    DebugMode = status;
    if(DebugMode)
    {
        OneZyk = false;
        OneOpc = false;
        sid1->SoundOutputEnable = false;
        sid2->SoundOutputEnable = false;
        for(int i=0; i<FLOPPY_ANZAHL; i++) floppy[i]->SetEnableFloppySound(false);
    }
    else
    {
        OneZyk = false;
        OneOpc = false;
        sid1->SoundOutputEnable = true;
        sid2->SoundOutputEnable = true;
        for(int i=0; i<FLOPPY_ANZAHL; i++) floppy[i]->SetEnableFloppySound(true);
    }
}

void C64Class::SetCpuExtLines(bool status)
{
    EnableExtLines = status;
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
    ExtRDY = status;
}

void C64Class::OneZyklus()
{
    DebugAnimation = false;
    OneZyk = true;
}

void C64Class::OneOpcode(int source)
{
    DebugAnimation = false;
    OneOpcSource = source;
    OneOpc = true;
}

void C64Class::SetDebugAnimation(bool status)
{
    DebugAnimation = status;
}

void C64Class::SetDebugAnimationSpeed(int cycle_sek)
{
    AnimationSpeedCounter = 0;
    AnimationSpeedAdd = static_cast<float_t>(AudioPufferSize) / static_cast<float_t>(AudioSampleRate) * static_cast<float_t>(cycle_sek);
}

void C64Class::GetC64CpuReg(REG_STRUCT *reg, IREG_STRUCT *ireg)
{
    cpu->GetInterneRegister(ireg);
    ireg->CycleCounter = CycleCounter;
    ireg->GAME = GAME;
    ireg->EXROM = EXROM;
    cpu->GetRegister(reg);
}

void C64Class::GetVicReg(VIC_STRUCT *vic_reg)
{
    vic->GetRegister(vic_reg);
    vic_reg->IRQ = cpu->GetInterrupts(VIC_IRQ);
}

void C64Class::GetIECStatus(IEC_STRUCT *iec)
{
    iec->ATN_OUT = !!(C64IEC & 16);
    iec->CLOCK_OUT = !!(C64IEC & 64);
    iec->DATA_OUT = !!(C64IEC & 128);
    iec->CLOCK_IN = !!(FloppyIEC & 64);
    iec->DATA_IN = !!(FloppyIEC & 128);
}

int C64Class::AddBreakGroup()
{
    if(BreakGroupAnz == MAX_BREAK_GROUPS) return -1;

    BreakGroup[BreakGroupAnz] = new BREAK_GROUP;
    memset(BreakGroup[BreakGroupAnz],0,sizeof(BREAK_GROUP));
    BreakGroup[BreakGroupAnz]->iRZZyklus = 1;
    BreakGroupAnz ++;
    return BreakGroupAnz - 1;
}

void C64Class::DelBreakGroup(int index)
{
    if(index >= BreakGroupAnz) return;
    delete BreakGroup[index];
    BreakGroupAnz--;
    for(int i = index; i < BreakGroupAnz; i++) BreakGroup[i] = BreakGroup[i+1];
    UpdateBreakGroup();
}

BREAK_GROUP* C64Class::GetBreakGroup(int index)
{
    if(index >= BreakGroupAnz) return nullptr;
    return BreakGroup[index];
}

void C64Class::UpdateBreakGroup()
{
    for(int i=0; i<0x10000;i++) Breakpoints[i] = 0;
    for(int i=0;i<BreakGroupAnz;i++)
    {
        BREAK_GROUP* bg = BreakGroup[i];
        if(bg->Enable)
        {
            if(bg->bPC) Breakpoints[bg->iPC] |= 1;
            if(bg->bAC) Breakpoints[bg->iAC] |= 2;
            if(bg->bXR) Breakpoints[bg->iXR] |= 4;
            if(bg->bYR) Breakpoints[bg->iYR] |= 8;
            if(bg->bRAdresse) Breakpoints[bg->iRAdresse] |= 16;
            if(bg->bWAdresse) Breakpoints[bg->iWAdresse] |= 32;
            if(bg->bRWert) Breakpoints[bg->iRWert] |= 64;
            if(bg->bWWert) Breakpoints[bg->iWWert] |= 128;
            if(bg->bRZ) Breakpoints[bg->iRZ] |= 256;
            if(bg->bRZZyklus) Breakpoints[bg->iRZZyklus] |= 512;
        }
    }

    for(int i=0;i<FLOPPY_ANZAHL;i++)
    {
        if(floppy[i]->GetEnableFloppy())
        {
            floppy[i]->UpdateBreakGroup();
        }
    }
}

void C64Class::DeleteAllBreakGroups()
{
    for(int i=0;i<BreakGroupAnz;i++)
    {
        delete BreakGroup[i];
    }
    BreakGroupAnz = 0;
    UpdateBreakGroup();
}

int C64Class::GetBreakGroupAnz()
{
    return BreakGroupAnz;
}

int C64Class::LoadBreakGroups(char *filename)
{
    FILE *file;
    char Kennung[10];
    char Version;
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
        reading_elements = fread(BreakGroup[i]->Name,sizeof(BreakGroup[i]->Name),1,file);
        reading_elements = fread(&BreakGroup[i]->Enable,sizeof(BreakGroup[i]->Enable),1,file);
        reading_elements = fread(&BreakGroup[i]->bPC,sizeof(BreakGroup[i]->bPC),1,file);
        reading_elements = fread(&BreakGroup[i]->iPC,sizeof(BreakGroup[i]->iPC),1,file);
        reading_elements = fread(&BreakGroup[i]->bAC,sizeof(BreakGroup[i]->bAC),1,file);
        reading_elements = fread(&BreakGroup[i]->iAC,sizeof(BreakGroup[i]->iAC),1,file);
        reading_elements = fread(&BreakGroup[i]->bXR,sizeof(BreakGroup[i]->bXR),1,file);
        reading_elements = fread(&BreakGroup[i]->iXR,sizeof(BreakGroup[i]->iXR),1,file);
        reading_elements = fread(&BreakGroup[i]->bYR,sizeof(BreakGroup[i]->bYR),1,file);
        reading_elements = fread(&BreakGroup[i]->iYR,sizeof(BreakGroup[i]->iYR),1,file);
        reading_elements = fread(&BreakGroup[i]->bRAdresse,sizeof(BreakGroup[i]->bRAdresse),1,file);
        reading_elements = fread(&BreakGroup[i]->iRAdresse,sizeof(BreakGroup[i]->iRAdresse),1,file);
        reading_elements = fread(&BreakGroup[i]->bWAdresse,sizeof(BreakGroup[i]->bWAdresse),1,file);
        reading_elements = fread(&BreakGroup[i]->iWAdresse,sizeof(BreakGroup[i]->iWAdresse),1,file);
        reading_elements = fread(&BreakGroup[i]->bRWert,sizeof(BreakGroup[i]->bRWert),1,file);
        reading_elements = fread(&BreakGroup[i]->iRWert,sizeof(BreakGroup[i]->iRWert),1,file);
        reading_elements = fread(&BreakGroup[i]->bWWert,sizeof(BreakGroup[i]->bWWert),1,file);
        reading_elements = fread(&BreakGroup[i]->iWWert,sizeof(BreakGroup[i]->iWWert),1,file);
        reading_elements = fread(&BreakGroup[i]->bRZ,sizeof(BreakGroup[i]->bRZ),1,file);
        reading_elements = fread(&BreakGroup[i]->iRZ,sizeof(BreakGroup[i]->iRZ),1,file);
        reading_elements = fread(&BreakGroup[i]->bRZZyklus,sizeof(BreakGroup[i]->bRZZyklus),1,file);
        reading_elements = fread(&BreakGroup[i]->iRZZyklus,sizeof(BreakGroup[i]->iRZZyklus),1,file);
    }
    return 0;
}

bool C64Class::SaveBreakGroups(char *filename)
{
    FILE *file;
    char Kennung[]  = "EMU64_BPT";
    char Version    = 1;

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
    fwrite(&BreakGroupAnz,sizeof(BreakGroupAnz),1,file);

    /// Groups ///
    for(int i=0;i<BreakGroupAnz;i++)
    {
        fwrite(BreakGroup[i]->Name,sizeof(BreakGroup[i]->Name),1,file);
        fwrite(&BreakGroup[i]->Enable,sizeof(BreakGroup[i]->Enable),1,file);
        fwrite(&BreakGroup[i]->bPC,sizeof(BreakGroup[i]->bPC),1,file);
        fwrite(&BreakGroup[i]->iPC,sizeof(BreakGroup[i]->iPC),1,file);
        fwrite(&BreakGroup[i]->bAC,sizeof(BreakGroup[i]->bAC),1,file);
        fwrite(&BreakGroup[i]->iAC,sizeof(BreakGroup[i]->iAC),1,file);
        fwrite(&BreakGroup[i]->bXR,sizeof(BreakGroup[i]->bXR),1,file);
        fwrite(&BreakGroup[i]->iXR,sizeof(BreakGroup[i]->iXR),1,file);
        fwrite(&BreakGroup[i]->bYR,sizeof(BreakGroup[i]->bYR),1,file);
        fwrite(&BreakGroup[i]->iYR,sizeof(BreakGroup[i]->iYR),1,file);
        fwrite(&BreakGroup[i]->bRAdresse,sizeof(BreakGroup[i]->bRAdresse),1,file);
        fwrite(&BreakGroup[i]->iRAdresse,sizeof(BreakGroup[i]->iRAdresse),1,file);
        fwrite(&BreakGroup[i]->bWAdresse,sizeof(BreakGroup[i]->bWAdresse),1,file);
        fwrite(&BreakGroup[i]->iWAdresse,sizeof(BreakGroup[i]->iWAdresse),1,file);
        fwrite(&BreakGroup[i]->bRWert,sizeof(BreakGroup[i]->bRWert),1,file);
        fwrite(&BreakGroup[i]->iRWert,sizeof(BreakGroup[i]->iRWert),1,file);
        fwrite(&BreakGroup[i]->bWWert,sizeof(BreakGroup[i]->bWWert),1,file);
        fwrite(&BreakGroup[i]->iWWert,sizeof(BreakGroup[i]->iWWert),1,file);
        fwrite(&BreakGroup[i]->bRZ,sizeof(BreakGroup[i]->bRZ),1,file);
        fwrite(&BreakGroup[i]->iRZ,sizeof(BreakGroup[i]->iRZ),1,file);
        fwrite(&BreakGroup[i]->bRZZyklus,sizeof(BreakGroup[i]->bRZZyklus),1,file);
        fwrite(&BreakGroup[i]->iRZZyklus,sizeof(BreakGroup[i]->iRZZyklus),1,file);
    }

    fclose(file);
    return true;
}

bool C64Class::ExportPRG(char *filename, uint16_t start_adresse, uint16_t end_adresse, int source)
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

bool C64Class::ExportASM(char *filename, uint16_t start_adresse, uint16_t end_adresse, int source)
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

void C64Class::SaveScreenshot(const char *filename, int format)
{

    strcpy(ScreenshotFilename,filename);
    ScreenshotFormat = format;
    StartScreenshot = true;
}

const char *C64Class::GetScreenshotFormatName(int format)
{
    if(format >= SCREENSHOT_FORMATS_COUNT)
        return nullptr;
    return ScreenschotFormatName[format];
}

void C64Class::SetExitScreenshot(const char *filename)
{
    strcpy(ExitScreenshotFilename,filename);
    ExitScreenshotEnable = true;
}

const char* C64Class::GetAVVersion()
{
    return VideoCapture->GetAVVersion();
}

bool C64Class::StartVideoRecord(const char *filename, int audio_bitrate, int video_bitrate)
{
    if(VideoCapture != nullptr)
    {
        VideoCapture->SetAudioBitrate(audio_bitrate);
        VideoCapture->SetVideoBitrate(video_bitrate);

        return VideoCapture->StartCapture(filename,"mp4",AktC64ScreenXW,AktC64ScreenYW);
    }
    return false;
}

void C64Class::SetPauseVideoRecord(bool status)
{
    VideoCapture->SetCapturePause(status);
}

void C64Class::StopVideoRecord()
{
    if(VideoCapture != nullptr)
    {
        VideoCapture->StopCapture();
    }
}

int C64Class::GetRecordedFrameCount()
{
    return VideoCapture->GetRecordedFrameCount();
}

bool C64Class::StartIECDump(const char *filename)
{
    char VersionString[256];

    sprintf(VersionString,"Emu64 Version %s",VERSION_STRING);

    IecVcdExport.SetVersionString(VersionString);
    IecVcdExport.SetTimeScaleString("989 ns");
    IecVcdExport.SetModuleString("IEC-BUS");

    if(!IecVcdExport.Open(filename)) return false;

    IecIsDumped = true;

    return true;
}

void C64Class::StopIECDump()
{
    IecIsDumped = false;
    IecVcdExport.Close();
}

void C64Class::SetSIDVolume(float_t volume)
{
    SIDVolume = volume;
}

void C64Class::SetFirstSidTyp(int sid_typ)
{
    sid1->SetChipType(sid_typ);
}

void C64Class::SetSecondSidTyp(int sid_typ)
{
    sid2->SetChipType(sid_typ);
}

void C64Class::EnableSecondSid(bool enable)
{
    StereoEnable = enable;
}

void C64Class::SetSecondSidAddress(uint16_t address)
{
    Sid2Adresse = address;
    if(Sid2Adresse == 0xD400)
        sid2->SetIODelayEnable(true);
    else
        sid2->SetIODelayEnable(false);
}

void C64Class::SetSid6ChannelMode(bool enable)
{
    Sid6ChannelMode = enable;
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
    if(var >= VicConfigSizeof) return;
    vic->VicConfig[var] = enable;
}

bool C64Class::GetVicConfig(int var)
{
    if(var >= VicConfigSizeof) return false;
    return vic->VicConfig[var];
}

void C64Class::SetVicDisplaySizePal(int first_line, int last_line)
{
    vic->SetVicVDisplayPalSize(first_line, last_line);
}

void C64Class::SetVicDisplaySizeNtsc(int first_line, int last_line)
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

    for (int i=0;i<BreakGroupAnz;i++)
    {
        BREAK_GROUP* bg = BreakGroup[i];
        int count1 = 0;
        int count2 = 0;

        if(bg->Enable)
        {
            if(bg->bPC)
            {
                count1++;
                if((BreakStatus&1) && (BreakWerte[0] == bg->iPC)) count2++;
            }
            if(bg->bAC)
            {
                count1++;
                if((BreakStatus&2) && (BreakWerte[1] == bg->iAC)) count2++;
            }
            if(bg->bXR)
            {
                count1++;
                if((BreakStatus&4) && (BreakWerte[2] == bg->iXR)) count2++;
            }
            if(bg->bYR)
            {
                count1++;
                if((BreakStatus&8) && (BreakWerte[3] == bg->iYR)) count2++;
            }
            if(bg->bRAdresse)
            {
                count1++;
                if((BreakStatus&16) && (BreakWerte[4] == bg->iRAdresse)) count2++;
            }
            if(bg->bWAdresse)
            {
                count1++;
                if((BreakStatus&32) && (BreakWerte[5] == bg->iWAdresse)) count2++;
            }
            if(bg->bRWert)
            {
                count1++;
                if((BreakStatus&64) && (BreakWerte[6] == bg->iRWert)) count2++;
            }
            if(bg->bWWert)
            {
                count1++;
                if((BreakStatus&128) && (BreakWerte[7] == bg->iWWert)) count2++;
            }
            if(bg->bRZ)
            {
                count1++;
                if((BreakStatus&256) && (BreakWerte[8] == bg->iRZ)) count2++;
            }
            if(bg->bRZZyklus)
            {
                count1++;
                if((BreakStatus&512) && (BreakWerte[9] == bg->iRZZyklus)) count2++;
            }
        }
        if((count1 == count2) && (count1 > 0))
        {
            BreakGroup[i]->bTrue = true;
            BreaksIO++;
        }
        else BreakGroup[i]->bTrue = false;
    }
    BreakStatus = 0;

    FloppyFoundBreakpoint = false;
    int floppy_break = 0;
    for(int i=0; i<FLOPPY_ANZAHL; i++)
    {
        if(floppy[i]->GetEnableFloppy())
        {
            if(floppy[i]->CheckBreakpoints()) floppy_break |= 1;
        }
    }

    if((BreaksIO > 0) || (floppy_break == 1))
    {
        if(!DebugMode)
        {
            DebugMode = true;
            DebugAnimation = false;
            OneZyk = false;
            OneOpc = false;
            sid1->SoundOutputEnable = false;
            sid2->SoundOutputEnable = false;
            for(int i=0; i<FLOPPY_ANZAHL; i++) floppy[i]->SetEnableFloppySound(false);
            if(BreakpointProc != nullptr) BreakpointProc();
            return true;
        }
        else
        {
            DebugAnimation = false;
            OneOpc = false;
            OneZyk = false;
            if(BreakpointProc != nullptr) BreakpointProc();
            return true;
        }
    }
    return false;
}

void C64Class::WriteSidIO(uint16_t adresse, uint8_t wert)
{
    if(StereoEnable)
    {
        if((adresse & 0xFFE0) == 0xD400) sid1->WriteIO(adresse,wert);
        if((adresse & 0xFFE0) == Sid2Adresse) sid2->WriteIO(adresse,wert);
    }
    else
    {
        sid1->WriteIO(adresse,wert);
    }
}

uint8_t C64Class::ReadSidIO(uint16_t adresse)
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

    return sid1->ReadIO(adresse);
}

/// $DE00
void C64Class::WriteIO1(uint16_t adresse, uint8_t wert)
{
    switch(IOSource)
    {
    case 0: // NO_MODUL
        break;
    case 1:	// CRT
        crt->WriteIO1(adresse,wert);
        break;
    case 2:	// REU
        reu->WriteIO1(adresse,wert);
        break;
    case 3:	// GEORAM
        geo->WriteIO1(adresse,wert);
        break;
    default:
        break;
    }
}

/// $DF00
void C64Class::WriteIO2(uint16_t adresse, uint8_t wert)
{
    switch(IOSource)
    {
    case 0: // NO_MODUL
        break;
    case 1: // CRT
        crt->WriteIO2(adresse,wert);
        break;
    case 2: // REU
        reu->WriteIO2(adresse,wert);
        break;
    case 3: // GEORAM
        geo->WriteIO2(adresse,wert);
        break;
    default:
        break;
    }
}

/// $DE00
uint8_t C64Class::ReadIO1(uint16_t adresse)
{
    switch(IOSource)
    {
    case 0: // NO_MODUL
        return 0;
    case 1: // CRT
        return crt->ReadIO1(adresse);
    case 2: // REU
        return reu->ReadIO1(adresse);
    case 3: // GEORAM
        return geo->ReadIO1(adresse);
    default:
        return 0;
    }
}

/// $DF00

uint8_t C64Class::ReadIO2(uint16_t adresse)
{
    switch(IOSource)
    {
    case 0: // NO_MODUL
        return 0;
    case 1: // CRT
        return crt->ReadIO2(adresse);
    case 2: // REU
        return reu->ReadIO2(adresse);
    case 3: // GEORAM
        return geo->ReadIO2(adresse);
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
    RecPollingWait = false;
    RecJoySlotNr = slot_nr;
    RecJoyMappingPos = 0;
    RecJoyMapping = true;
}

void C64Class::StopRecJoystickMapping()
{
    if(RecJoyMapping)
        RecJoyMapping = false;
}

void C64Class::ClearJoystickMapping(int slot_nr)
{
    char str00[32];
    sprintf(str00,"Slot %d",slot_nr+1);
    strcpy(VJoys[slot_nr].Name, str00);

    for(int i=0;i<5;i++)
    {
        VJoys[slot_nr].Type[i] = 0;
        VJoys[slot_nr].JoyIndex[i] = 0;
        VJoys[slot_nr].KeyDown[i] = 0;
        VJoys[slot_nr].KeyUp[i] = 0;
        VJoys[slot_nr].ButtonNr[i] = 0;
        VJoys[slot_nr].HatNr[i] = 0;
        VJoys[slot_nr].HatValue[i] = 0;
        VJoys[slot_nr].AxisNr[i] = 0;
        VJoys[slot_nr].AxisValue[i] = 0;
    }
}

void C64Class::IncMouseHiddenCounter()
{
    if(!MouseIsHidden && (MouseHiddenTime > 0))
    {
        MouseHiddenCounter++;
        if(MouseHiddenCounter >= MouseHiddenTime)
        {
            MouseIsHidden = true;
            SDL_ShowCursor(false);
        }
    }
}

void C64Class::StartRecKeyMap(uint8_t keymatrix_code)
{
    RecMatrixCode = keymatrix_code;
    IsKeyMapRec = true;
}

void C64Class::StopRecKeyMap()
{
    IsKeyMapRec = false;
}

bool C64Class::GetRecKeyMapStatus()
{
    return IsKeyMapRec;
}

C64_KEYS* C64Class::GetC64KeyTable()
{
    return C64KeyTable;
}

const char** C64Class::GetC64KeyNameTable()
{
    return C64KeyNames;
}

int C64Class::GetC64KeyTableSize()
{
    return C64KeyNum;
}

void C64Class::OpenSDLJoystick()
{
    JoyStickUdateIsStop = false;
    StopJoystickUpdate = true;
    int AbbruchCounter = 1000;
    while(!JoyStickUdateIsStop && (AbbruchCounter > 0))
    {
        AbbruchCounter--;
    }

    if(SDLJoystickIsOpen)
    {
        SDLJoystickIsOpen = false;
        SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
        LogText(const_cast<char*>(">> SDL Subsystem Joystick wurde geschlossen\n"));
    }

    if(SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0)
    {
        SDLJoystickIsOpen = false;
        LogText(const_cast<char*>("<< ERROR: SDL Subsystem Joystick konnte nicht geöffnet werden\n"));
    }
    else
    {
        SDLJoystickIsOpen = true;
        LogText(const_cast<char*>(">> SDL Subsytem Joystick wurde erfolgreich geoeffnet\n"));
        JoystickAnzahl = SDL_NumJoysticks();
        if(JoystickAnzahl > MAX_JOYSTICKS) JoystickAnzahl = MAX_JOYSTICKS;

        char str00[256];
        switch(JoystickAnzahl)
        {
        case 0:
            LogText(const_cast<char*>("<< SDL konnte keinen Joystick/Gamepad erkennen\n"));
            break;
        case 1:
            LogText(const_cast<char*>(">> SDL konnte 1 Joystick/Gamepad erkennen\n"));
            break;
        default:
            sprintf(str00,">> SDL konnte %d Joysticks/Gamepads erkennen\n",JoystickAnzahl);
            LogText(str00);
            break;
        }

        if(JoystickAnzahl > 0)
        {
            for(int i=0; i<JoystickAnzahl;i++)
            {
                Joystick[i] = SDL_JoystickOpen(i);
                JoystickNamen[i] = SDL_JoystickName(Joystick[i]);

                sprintf(str00,">>   [%2.2d] %s\n",i,JoystickNamen[i]);
                LogText(str00);
            }
        }
    }
    StopJoystickUpdate = true;
}

void C64Class::CloseSDLJoystick()
{
    JoyStickUdateIsStop = false;
    StopJoystickUpdate = true;
    int AbbruchCounter = 1000;
    while(!JoyStickUdateIsStop && (AbbruchCounter > 0))
    {
        AbbruchCounter--;
    }
    if(SDLJoystickIsOpen)
    {
        SDLJoystickIsOpen = false;
        SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
        LogText(const_cast<char*>(">> SDL Subsystem Joystick wurde geschlossen\n"));
    }
    StopJoystickUpdate = true;
}

///
/// \brief  Wird aufgerufen wenn der CIA1 Ausgang PA6 und PA7 sich ändert
///         Somit Schaltet IC 4066 POTX und POTY
///
void C64Class::ChangePOTSwitch()
{
    if(!Mouse1351Enable)
    {
        POT_X = 0xFF;
        POT_Y = 0xFF;
    }
    else
    {
        switch(CIA1_PA.GetOutput() >> 6)
        {
        case 0:
            POT_X = POT_Y = 0xFF;
            break;
        case 1:
            POT_X = POT_AX;
            POT_Y = POT_AY;
            break;
        case 2:
            POT_X = POT_BX;
            POT_Y = POT_BY;
            break;
        case 3:
            POT_X = POT_AX | POT_BX;
            POT_Y = POT_AY | POT_BY;
            break;
        }
    }

    sid1->SetPotXY(POT_X, POT_Y);
}

void C64Class::UpdateMouse(void)
{
    switch(MousePort)
    {
    case 0: // Maus an Port 1 angeschlossen
        POT_AX = (MouseXRel>>2) & 0x7F;
        POT_AY = (~MouseYRel>>2) & 0x7F;
        POT_BX = 0xFF;
        POT_BY = 0xFF;
        break;

    case 1: // Maus an Port 2 angeschlossen
        POT_AX = 0xFF;
        POT_AY = 0xFF;
        POT_BX = (MouseXRel>>2) & 0x7F;
        POT_BY = (~MouseYRel>>2) & 0x7F;
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
