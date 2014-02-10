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
// Letzte Änderung am 12.01.2014                //
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

#define AudioSampleRate 44100
#define AudioPufferSize 882    // 882 bei 44.100 Khz
#define RecPollingWaitStart 20

#define C64ScreenXW 384         //384
#define C64ScreenYW 272         //272
#define C64FirstViewedPixel 104 //104

C64Class::C64Class(int *ret_error,VideoPalClass *_pal,bool OpenGLOn, function<void(char*)> log_function, const char* gfx_path):
    mmu(NULL),cpu(NULL),vic(NULL),sid1(NULL),sid2(NULL),cia1(NULL),cia2(NULL),crt(NULL)
{   
    JoyStickUdateIsStop = true,
    RecJoyMapping = false,
    StopJoystickUpdate = true;
    SDLJoystickIsOpen = false;
    JoystickAnzahl = 0;

    VPort1 = 0;
    VPort2 = 1;

    LogText = log_function;
    GfxPath = gfx_path;

    LogText((char*)">> C64 Klasse wurde gestartet...\n");
    LogText((char*)">> GfxPath = ");
    LogText((char*)GfxPath);
    LogText((char*)"\n");

    pal = _pal;
    BreakGroupAnz = 0;
    FloppyFoundBreakpoint = false;
    EnableExtLines = false;

    C64Screen = 0;
    C64ScreenBack = 0;
    DrawScreenBack = false;
    StartScreenshot = false;
    FrameSkipCounter=1;
    NewVicRefresh = false;

    OpenGLEnable = OpenGLOn;
    DistortionEnable = true;
    SetDistortion(-0.05);

    /// SDL Installieren ///

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
         LogText((char*)"<< ERROR: Fehler beim installieren von SDL\n");
        *ret_error = -1;
    }
    else
        LogText((char*)">> SDL wurde installiert\n");

    char filename[FILENAME_MAX];
    sprintf(filename,"%spfeil0.png",GfxPath);
    Pfeil0 = IMG_Load(filename);
    if(!Pfeil0)
    {
        LogText((char*)"<< ERROR: Folgendes Bild konnte nicht geladen werden --- ");
        LogText(filename);
        LogText((char*)"\n");

        //*ret_error = -2;
    }
    else
    {
        LogText((char*)">> Folgendes Bild wurde erfolgreich geladen --- ");
        LogText(filename);
        LogText((char*)"\n");

        if ( (Pfeil0->w & (Pfeil0->w - 1)) != 0 ) LogText((char*)"<< WARNUNG: Die Breite ist keine Potenz von 2^n\n");
        if ( (Pfeil0->h & (Pfeil0->h - 1)) != 0 ) LogText((char*)"<< WARNUNG: Die Höhe ist keine Potenz von 2^n\n");
    }

    sprintf(filename,"%spfeil1.png",GfxPath);
    Pfeil1 = IMG_Load(filename);
    if(!Pfeil1)
    {
        LogText((char*)"<< ERROR: Folgendes Bild konnte nicht geladen werden --- ");
        LogText(filename);
        LogText((char*)"\n");

        //*ret_error = -3;
    }
    else
    {
        LogText((char*)">> Folgendes Bild wurde erfolgreich geladen --- ");
        LogText(filename);
        LogText((char*)"\n");

        if ( (Pfeil1->w & (Pfeil1->w - 1)) != 0 ) LogText((char*)"<< WARNUNG: Die Breite ist keine Potenz von 2^n\n");
        if ( (Pfeil1->h & (Pfeil1->h - 1)) != 0 ) LogText((char*)"<< WARNUNG: Die Höhe ist keine Potenz von 2^n\n");
    }

    sprintf(filename,"%skreis0.png",GfxPath);
    Kreis0 = IMG_Load(filename);
    if(!Kreis0)
    {
        LogText((char*)"<< ERROR: Folgendes Bild konnte nicht geladen werden --- ");
        LogText(filename);
        LogText((char*)"\n");

        //*ret_error = -4;
    }
    else
    {
        LogText((char*)">> Folgendes Bild wurde erfolgreich geladen --- ");
        LogText(filename);
        LogText((char*)"\n");

        if ( (Kreis0->w & (Kreis0->w - 1)) != 0 ) LogText((char*)"<< WARNUNG: Die Breite ist keine Potenz von 2^n\n");
        if ( (Kreis0->h & (Kreis0->h - 1)) != 0 ) LogText((char*)"<< WARNUNG: Die Höhe ist keine Potenz von 2^n\n");
    }

    sprintf(filename,"%skreis1.png",GfxPath);
    Kreis1 = IMG_Load(filename);
    if(!Kreis1)
    {
        LogText((char*)"<< ERROR: Folgendes Bild konnte nicht geladen werden --- ");
        LogText(filename);
        LogText((char*)"\n");

       // *ret_error = -5;
    }
    else
    {
        LogText((char*)">> Folgendes Bild wurde erfolgreich geladen --- ");
        LogText(filename);
        LogText((char*)"\n");

        if ( (Kreis1->w & (Kreis1->w - 1)) != 0 ) LogText((char*)"<< WARNUNG: Die Breite ist keine Potenz von 2^n\n");
        if ( (Kreis1->h & (Kreis1->h - 1)) != 0 ) LogText((char*)"<< WARNUNG: Die Höhe ist keine Potenz von 2^n\n");
    }

    /// SLD Audio Installieren (C64 Emulation) ///
    SDL_AudioSpec format;
    format.freq = AudioSampleRate;
    format.format = AUDIO_S16;
    format.channels = 1;
    format.samples = AudioPufferSize;
    format.callback = AudioMix;
    format.userdata = this;

    if(SDL_OpenAudio(&format,NULL) < 0)
    {
        LogText((char*)"<< ERROR: Fehler beim installieren von SDL_Audio\n");
        *ret_error = -6;
    }
    LogText((char*)">> SDL_Audio wurde installiert\n");

    OpenSDLJoystick();

    SetGrafikModi(pal->StartC64isColorBit32,pal->StartC64isDoublesize,pal->StartC64isPalmode,false);

    sprintf(filename,"%ssdl_icon.png",GfxPath);
    C64ScreenIcon = IMG_Load(filename);
    if(C64ScreenIcon != NULL)
    {
        SDL_SetColorKey(C64ScreenIcon,SDL_SRCCOLORKEY,SDL_MapRGB(C64ScreenIcon->format,0,0,0));
        SDL_WM_SetIcon(C64ScreenIcon,NULL);
    }
    else
        LogText((char*)"<< ERROR: Fehler beim laden des SLDFenster Icons\n");

    SDL_initFramerate(&fps_manager);
    SDL_setFramerate(&fps_manager,50);

    GamePort1 = 0;
    GamePort2 = 0;

    /// Init Classes ///
    mmu = new MMU();
    cpu = new MOS6510();
    vic = new VICII();
    sid1 = new MOS6581_8085(0,format.freq,format.samples,ret_error);
    sid2 = new MOS6581_8085(1,format.freq,format.samples,ret_error);
    cia1 = new MOS6526(0);
    cia2 = new MOS6526(1);
    crt = new CRTClass();
    reu = new REUClass();
    geo = new GEORAMClass();

    vic_puffer = vic->VideoPuffer;

    cia2->FloppyIEC = &FloppyIEC;
    cia2->C64IEC = &C64IEC;

    /// Floppy mit C64 verbinden ///

    for(int i=0;i<FloppyAnzahl;i++)
    {
        floppy[i] = new Floppy1541(&RESET,format.freq,format.samples,&FloppyFoundBreakpoint);
        floppy[i]->SetResetReady(&FloppyResetReady[i],0xEBFF);
        floppy[i]->SetC64IEC(&C64IEC);
        floppy[i]->SetDeviceNummer(8+i);
        floppy[i]->LoadDosRom((char*)"roms/1541.rom");
        floppy[i]->LoadFloppySounds((char*)"floppy_sounds/motor.raw",(char*)"floppy_sounds/motor_on.raw",(char*)"floppy_sounds/motor_off.raw",(char*)"floppy_sounds/anschlag.raw",(char*)"floppy_sounds/stepper_inc.raw",(char*)"floppy_sounds/stepper_dec.raw");
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
    DebugAnimation = false;
    AnimationRefreshProc = 0;
    AnimationSpeedAdd = (double)AudioPufferSize/(double)AudioSampleRate;
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
    cia1->PA = &CIA1_PA;
    cia1->PB = &CIA1_PB;
    cia2->RESET = &RESET;
    cia2->CpuTriggerInterrupt = bind(&MOS6510::TriggerInterrupt,cpu,_1);
    cia2->CpuClearInterrupt = bind(&MOS6510::ClearInterrupt,cpu,_1);
    cia2->PA = &CIA2_PA;
    cia2->PB = &CIA2_PB;
    vic->BA = &RDY_BA;
    vic->RESET = &RESET;
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

    /// Datasette mit C64 verbinden ///
    //tape->CPU_PORT = &CPU_PORT;
    //cia1_FLAG_PIN = &tape->CassRead;

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

    sid1->RESET = &RESET;
    sid1->SetC64Zyklen(985248);     // 985248
    sid1->SetChipType(MOS_8580);
    sid1->SoundOutputEnable = true;
    sid1->CycleExact = true;
    sid1->FilterOn = true;
    sid1->Reset();

    sid2->RESET = &RESET;
    sid2->SetC64Zyklen(985248);     // 985248
    sid2->SetChipType(MOS_8580);
    sid2->SoundOutputEnable = false;
    sid2->CycleExact = true;
    sid2->FilterOn = true;
    sid2->Reset();

    StereoEnable = false;
    Sid2Adresse = 0xD420;

    vic->RESET = &RESET;
    vic->BA = &RDY_BA;
    vic->CIA2_PA = cia2->PA->GetOutputBitsPointer();

    /// Breakpoints ///
    BreakpointProc = 0;
    cpu->BreakStatus = &BreakStatus;
    cpu->BreakWerte = BreakWerte;
    cpu->Breakpoints = Breakpoints;

    vic->BreakStatus = &BreakStatus;
    vic->BreakWerte = BreakWerte;
    vic->Breakpoints = Breakpoints;

    cpu->History = C64History;
    cpu->HistoryPointer = &C64HistoryPointer;

    for(int i=0;i<0x10000;i++) Breakpoints[i] = 0;

    video_cap = new VideoCaptureClass();
}

C64Class::~C64Class()
{
    if(video_cap != NULL) delete video_cap;

    LoopThreadEnd = true;
    SDL_WaitThread(sdl_thread,0);

    CloseSDLJoystick();
    SDL_VideoQuit();
    SDL_AudioQuit();

    for(int i=0;i<FloppyAnzahl;i++)
    {
        if(floppy[i] != NULL) delete floppy[i];
    }

    if(mmu != NULL) delete mmu;
    if(cpu != NULL) delete cpu;
    if(vic != NULL) delete vic;
    if(sid1 != NULL) delete sid1;
    if(sid2 != NULL) delete sid2;
    if(cia1 != NULL) delete cia1;
    if(cia2 != NULL) delete cia2;
    if(crt != NULL) delete crt;
    if(reu != NULL) delete reu;
    if(geo != NULL) delete geo;
}

void C64Class::StartEmulation()
{
    /// SLD Thread starten (ab hier startet auch die C64 Emulation ///
    sdl_thread = SDL_CreateThread(SDLThread, this);
    SDL_PauseAudio(0);
}

void AudioMix(void *userdat, Uint8 *stream, int laenge)
{
    C64Class *c64 = (C64Class*)userdat;

    if(c64->RecPollingWait)
    {
        c64->RecPollingWaitCounter--;
        if(c64->RecPollingWaitCounter == 0) c64->RecPollingWait = false;
    }

    c64->FillAudioBuffer(stream,laenge);
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
    C64Class *c64 = (C64Class*)userdat;
    SDL_Event event;
    c64->LoopThreadEnd = false;
    c64->sdl_thread_pause = false;

    GLuint  Pfeil0Texture;
    GLuint  Pfeil1Texture;
    GLuint  Kreis0Texture;
    GLuint  Kreis1Texture;

    GLuint  C64ScreenTexture;

    c64->C64ScreenBuffer = 0;

    while (!c64->LoopThreadEnd)
    {
        /// Wird immer ausgeführt wenn die Funktion SetGrafikModi ausgefürt wurde ///
        if(c64->ChangeGrafikModi)
        {
            c64->ChangeGrafikModi = false;

            /// VicRefresh stoppen und solange warten bis wirklich Stop ist ///
            c64->HoldVicRefresh = true;
            c64->VicRefreshIsHold = false;
            while(!c64->VicRefreshIsHold)
            {
                  SDL_Delay(1);
            }

            /// Allegmeine Einstellungen ///

            if((c64->FullResXW != 0) && (c64->FullResYW != 0))
                c64->isFullscreen  = true;
            else c64->isFullscreen = false;

            c64->AktWindowXW = C64ScreenXW;
            c64->AktWindowYW = C64ScreenYW;

            if(c64->DoubleSize)
            {
                c64->AktWindowXW *=2;
                c64->AktWindowYW *=2;
            }

            c64->AktC64ScreenXW = c64->AktWindowXW;
            c64->AktC64ScreenYW = c64->AktWindowYW;

            if(c64->ColBits32)
            {
                c64->AktWindowColorBits = 32;
                if(c64->C64ScreenBuffer != 0)
                {
                    delete[] c64->C64ScreenBuffer;
                }
                c64->C64ScreenBuffer = new unsigned char[c64->AktC64ScreenXW*c64->AktC64ScreenYW*4];
            }
            else
            {
                c64->AktWindowColorBits = 16;
                if(c64->C64ScreenBuffer != 0)
                {
                    delete[] c64->C64ScreenBuffer;
                }
                c64->C64ScreenBuffer = new unsigned char[c64->AktC64ScreenXW*c64->AktC64ScreenYW*2];
            }

            c64->pal->SetDisplayMode(c64->AktWindowColorBits);
            c64->pal->EnablePALOutput(c64->PalEnable);
            c64->pal->EnableVideoDoubleSize(c64->DoubleSize);

            if(c64->OpenGLEnable)
            {
                /// OpenGL Version ///
                c64->C64Screen = SDL_SetVideoMode(c64->AktWindowXW,c64->AktWindowYW,c64->AktWindowColorBits,SDL_VIDEORESIZE | SDL_OPENGL);

                SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
                SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL,0);

                glShadeModel(GL_SMOOTH);
                glEnable(GL_TEXTURE_2D);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                glViewport(0,0,c64->AktWindowXW,c64->AktWindowYW);
                glMatrixMode(GL_PROJECTION);
                glOrtho(0,c64->AktWindowXW,c64->AktWindowYW,0,-1,1);
                glLoadIdentity();

                glGenTextures(1,&C64ScreenTexture);
                glBindTexture( GL_TEXTURE_2D, C64ScreenTexture);

                // Set the texture's stretching properties
                if(c64->FilterEnable)
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

                glTexImage2D( GL_TEXTURE_2D, 0, 4, c64->AktWindowXW, c64->AktWindowYW, 0,GL_BGRA, GL_UNSIGNED_BYTE, c64->C64ScreenBuffer );
                //gluBuild2DMipmaps(GL_TEXTURE_2D, 4, c64->AktWindowXW, c64->AktWindowXW,GL_RGBA, GL_UNSIGNED_BYTE, c64->C64ScreenBuffer );

                GLenum  TextureFormat = 0;
                GLint   NofColors = 0;

                if(c64->Pfeil0 != NULL)
                {
                    NofColors = c64->Pfeil0->format->BytesPerPixel;

                    if(NofColors == 4)
                    {
                        if(c64->Pfeil0->format->Rmask==0x000000ff)
                            TextureFormat=GL_RGBA;
                        else
                            TextureFormat=GL_BGRA;
                    }
                    else if(NofColors == 3) //no alpha channel
                    {
                        if(c64->Pfeil0->format->Rmask==0x000000ff)
                            TextureFormat=GL_RGB;
                        else
                            TextureFormat=GL_BGR;
                    }
                }

                // Ich setze mal vorraus das alle 4 Images das selbe Format haben !! //

                if(c64->Pfeil0 != NULL)
                {
                    glGenTextures(1,&Pfeil0Texture);
                    glBindTexture( GL_TEXTURE_2D, Pfeil0Texture );
                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
                    //glTexImage2D( GL_TEXTURE_2D, 0, NofColors, c64->Pfeil0->w, c64->Pfeil0->h, 0,TextureFormat, GL_UNSIGNED_BYTE, c64->Pfeil0->pixels );
                    gluBuild2DMipmaps(GL_TEXTURE_2D, NofColors, c64->Pfeil0->w, c64->Pfeil0->h,TextureFormat, GL_UNSIGNED_BYTE, c64->Pfeil0->pixels );
                }

                if(c64->Pfeil1 != NULL)
                {
                    glGenTextures(1,&Pfeil1Texture);
                    glBindTexture( GL_TEXTURE_2D, Pfeil1Texture );
                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
                    //glTexImage2D( GL_TEXTURE_2D, 0, NofColors, c64->Pfeil1->w, c64->Pfeil1->h, 0,TextureFormat, GL_UNSIGNED_BYTE, c64->Pfeil1->pixels );
                    gluBuild2DMipmaps(GL_TEXTURE_2D, NofColors, c64->Pfeil1->w, c64->Pfeil1->h,TextureFormat, GL_UNSIGNED_BYTE, c64->Pfeil1->pixels );
                }

                if(c64->Kreis0 != NULL)
                {
                    glGenTextures(1,&Kreis0Texture);
                    glBindTexture( GL_TEXTURE_2D, Kreis0Texture );
                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
                    //glTexImage2D( GL_TEXTURE_2D, 0, NofColors, c64->Kreis0->w, c64->Kreis0->h, 0,TextureFormat, GL_UNSIGNED_BYTE, c64->Kreis0->pixels );
                    gluBuild2DMipmaps(GL_TEXTURE_2D, NofColors, c64->Kreis0->w, c64->Kreis0->h,TextureFormat, GL_UNSIGNED_BYTE, c64->Kreis0->pixels );
                }

                if(c64->Kreis1 != NULL)
                {
                    glGenTextures(1,&Kreis1Texture);
                    glBindTexture( GL_TEXTURE_2D, Kreis1Texture );
                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
                    //glTexImage2D( GL_TEXTURE_2D, 0, NofColors, c64->Kreis1->w, c64->Kreis1->h, 0,TextureFormat, GL_UNSIGNED_BYTE, c64->Kreis1->pixels );
                    gluBuild2DMipmaps(GL_TEXTURE_2D, NofColors, c64->Kreis1->w, c64->Kreis1->h,TextureFormat, GL_UNSIGNED_BYTE, c64->Kreis1->pixels );
                }
            }
            else
            {
                /// SDL Version ///
                c64->C64Screen = SDL_SetVideoMode(c64->AktWindowXW,c64->AktWindowYW,c64->AktWindowColorBits,SDL_VIDEORESIZE | SDL_SWSURFACE | SDL_DOUBLEBUF);
                c64->C64ScreenBack = SDL_CreateRGBSurface(0,c64->AktWindowXW,c64->AktWindowYW,c64->AktWindowColorBits,0,0,0,0);
            }

            /// VicRefresh wieder zulassen ///
            c64->HoldVicRefresh = false;
        }

        if(!c64->sdl_thread_pause)
        {
            while (SDL_PollEvent (&event))
            {
                switch (event.type)
                {
                    case SDL_VIDEORESIZE:
                    {
                        if(!c64->isFullscreen)
                        {
                            c64->AktWindowXW = event.resize.w;
                            c64->AktWindowYW = event.resize.h;

                            if(c64->OpenGLEnable)
                            {
                                /// OpenGL Version ///
                                c64->C64Screen = SDL_SetVideoMode(c64->AktWindowXW,c64->AktWindowYW,c64->AktWindowColorBits,SDL_VIDEORESIZE | SDL_OPENGL);
                                glViewport(0,0,c64->AktWindowXW,c64->AktWindowYW);
                                glMatrixMode(GL_PROJECTION);
                                glLoadIdentity();

                                glOrtho(0,c64->AktWindowXW,c64->AktWindowYW,0,-1,1);
                                glLoadIdentity();
                            }
                            else
                            {
                                /// SDL Version ///
                                c64->C64Screen = SDL_SetVideoMode(c64->AktWindowXW,c64->AktWindowYW,c64->AktWindowColorBits,SDL_VIDEORESIZE | SDL_SWSURFACE | SDL_DOUBLEBUF);
                            }
                        }
                    break;
                    }

                    case SDL_JOYBUTTONDOWN:
                    case SDL_JOYBUTTONUP:
                    {
                        if((c64->RecJoyMapping == true) && (event.jbutton.type == SDL_JOYBUTTONDOWN))
                        {
                            c64->VJoys[c64->RecJoySlotNr].Type[c64->RecJoyMappingPos] = VJOY_TYPE_BUTTON;
                            c64->VJoys[c64->RecJoySlotNr].JoyIndex[c64->RecJoyMappingPos] = event.jbutton.which;
                            c64->VJoys[c64->RecJoySlotNr].ButtonNr[c64->RecJoyMappingPos] = event.jbutton.button;
                        }
                        else if((c64->RecJoyMapping == true) && (event.jbutton.type == SDL_JOYBUTTONUP) && (c64->RecPollingWait == false))
                        {
                            c64->RecJoyMappingPos++;
                            if(c64->RecJoyMappingPos == 5)
                            {
                                /// Rec Mapping ist fertig ///
                                c64->RecJoyMapping = false;
                            }
                            else
                            {
                                c64->RecPollingWait = true;
                                c64->RecPollingWaitCounter = RecPollingWaitStart;
                            }
                        }
                        else
                        {
                            /// VJoyStick abfrage ///
                            /// Port1
                            if((c64->VPort1 >= 0) && (c64->VPort1 < MAX_VJOYS))
                            {
                                for(int i=0;i<5;i++)
                                {
                                    if((c64->VJoys[c64->VPort1].ButtonNr[i] == event.jbutton.button) &&
                                            (c64->VJoys[c64->VPort1].Type[i] == VJOY_TYPE_BUTTON) &&
                                            (c64->VJoys[c64->VPort1].JoyIndex[i] == event.jbutton.which))
                                    {
                                        if(event.jbutton.state == 1) c64->GamePort1 |= 1<<i;
                                        else c64->GamePort1 &= ~(1<<i);
                                    }
                                }
                            }

                            /// Port2
                            if((c64->VPort2 >= 0) && (c64->VPort2 < MAX_VJOYS))
                            {
                                for(int i=0;i<5;i++)
                                {
                                    if((c64->VJoys[c64->VPort2].ButtonNr[i] == event.jbutton.button) &&
                                            (c64->VJoys[c64->VPort2].Type[i] == VJOY_TYPE_BUTTON) &&
                                            (c64->VJoys[c64->VPort2].JoyIndex[i] == event.jbutton.which))
                                    {
                                        if(event.jbutton.state == 1) c64->GamePort2 |= 1<<i;
                                        else c64->GamePort2 &= ~(1<<i);
                                    }
                                }
                            }
                        }
                        break;
                    }

                    case SDL_JOYHATMOTION:
                    {
                        if((c64->RecJoyMapping == true) && (c64->RecPollingWait == false))
                        {
                            if(event.jhat.value > 0)
                            {
                                c64->VJoys[c64->RecJoySlotNr].Type[c64->RecJoyMappingPos] = VJOY_TYPE_HAT;
                                c64->VJoys[c64->RecJoySlotNr].JoyIndex[c64->RecJoyMappingPos] = event.jhat.which;
                                c64->VJoys[c64->RecJoySlotNr].HatNr[c64->RecJoyMappingPos] = event.jhat.hat;
                                c64->VJoys[c64->RecJoySlotNr].HatValue[c64->RecJoyMappingPos] = event.jhat.value;
                            }
                            else
                            {
                                c64->RecJoyMappingPos++;
                                if(c64->RecJoyMappingPos == 5)
                                {
                                    /// Rec Mapping ist fertig ///
                                    c64->RecJoyMapping = false;
                                }
                                else
                                {
                                    c64->RecPollingWait = true;
                                    c64->RecPollingWaitCounter = RecPollingWaitStart;
                                }
                            }
                        }
                        else
                        {
                            /// VJoyStick abfrage ///
                            /// Port1
                            if((c64->VPort1 >= 0) && (c64->VPort1 < MAX_VJOYS))
                            {
                                for(int i=0;i<5;i++)
                                {
                                    if((c64->VJoys[c64->VPort1].HatNr[i] == event.jhat.hat) &&
                                            (c64->VJoys[c64->VPort1].Type[i] == VJOY_TYPE_HAT) &&
                                            (c64->VJoys[c64->VPort1].JoyIndex[i] == event.jhat.which))
                                    {
                                        if((event.jhat.value & c64->VJoys[c64->VPort1].HatValue[i]) == c64->VJoys[c64->VPort1].HatValue[i]) c64->GamePort1 |= 1<<i;
                                        else c64->GamePort1 &= ~(1<<i);
                                    }
                                }
                            }

                            /// VJoyStick abfrage ///
                            /// Port2
                            if((c64->VPort2 >= 0) && (c64->VPort2 < MAX_VJOYS))
                            {
                                for(int i=0;i<5;i++)
                                {
                                    if((c64->VJoys[c64->VPort2].HatNr[i] == event.jhat.hat) &&
                                            (c64->VJoys[c64->VPort2].Type[i] == VJOY_TYPE_HAT) &&
                                            (c64->VJoys[c64->VPort2].JoyIndex[i] == event.jhat.which))
                                    {
                                        if((event.jhat.value & c64->VJoys[c64->VPort2].HatValue[i]) == c64->VJoys[c64->VPort2].HatValue[i]) c64->GamePort2 |= 1<<i;
                                        else c64->GamePort2 &= ~(1<<i);
                                    }
                                }
                            }

                        }
                      break;
                    }

                    case SDL_JOYAXISMOTION:
                    {
                        if((c64->RecJoyMapping == true) && (c64->RecPollingWait == false))
                        {
                            if(!((event.jaxis.value >= -10) && (event.jaxis.value <= 10)))
                            {
                                c64->VJoys[c64->RecJoySlotNr].Type[c64->RecJoyMappingPos] = VJOY_TYPE_AXIS;
                                c64->VJoys[c64->RecJoySlotNr].JoyIndex[c64->RecJoyMappingPos] = event.jaxis.which;
                                c64->VJoys[c64->RecJoySlotNr].AxisNr[c64->RecJoyMappingPos] = event.jaxis.axis;
                                if(event.jaxis.value > 0) c64->VJoys[c64->RecJoySlotNr].AxisValue[c64->RecJoyMappingPos] = 0;
                                else c64->VJoys[c64->RecJoySlotNr].AxisValue[c64->RecJoyMappingPos] = 1;
                            }
                            else
                            {
                                c64->RecJoyMappingPos++;
                                if(c64->RecJoyMappingPos == 5)
                                {
                                    /// Rec Mapping ist fertig ///
                                    c64->RecJoyMapping = false;
                                }
                                else
                                {
                                    c64->RecPollingWait = true;
                                    c64->RecPollingWaitCounter = RecPollingWaitStart;
                                }
                            }
                        }
                        else
                        {
                            /// VJoyStick abfrage ///
                            /// Port1
                            if((c64->VPort1 >= 0) && (c64->VPort1 < MAX_VJOYS))
                            {
                                for(int i=0;i<5;i++)
                                {
                                    if((c64->VJoys[c64->VPort1].AxisNr[i] == event.jaxis.axis) &&
                                            (c64->VJoys[c64->VPort1].Type[i] == VJOY_TYPE_AXIS) &&
                                            (c64->VJoys[c64->VPort1].JoyIndex[i] == event.jaxis.which))
                                    {
                                        if(!((event.jaxis.value >= -10) && (event.jaxis.value <= 10)))
                                        {
                                            if(event.jaxis.value > 16000)
                                            {
                                                if(c64->VJoys[c64->VPort1].AxisValue[i] == 0) c64->GamePort1 |= 1<<i;
                                            }
                                            else if(event.jaxis.value < -16000)
                                            {
                                                if(c64->VJoys[c64->VPort1].AxisValue[i] == 1) c64->GamePort1 |= 1<<i;
                                            }
                                        }
                                        else c64->GamePort1 &= ~(1<<i);
                                    }
                                }
                            }

                            /// VJoyStick abfrage ///
                            /// Port2
                            if((c64->VPort2 >= 0) && (c64->VPort2 < MAX_VJOYS))
                            {
                                for(int i=0;i<5;i++)
                                {
                                    if((c64->VJoys[c64->VPort2].AxisNr[i] == event.jaxis.axis) &&
                                            (c64->VJoys[c64->VPort2].Type[i] == VJOY_TYPE_AXIS) &&
                                            (c64->VJoys[c64->VPort2].JoyIndex[i] == event.jaxis.which))
                                    {
                                        if(!((event.jaxis.value >= -10) && (event.jaxis.value <= 10)))
                                        {
                                            if(event.jaxis.value > 16000)
                                            {
                                                if(c64->VJoys[c64->VPort2].AxisValue[i] == 0) c64->GamePort2 |= 1<<i;
                                            }
                                            else if(event.jaxis.value < -16000)
                                            {
                                                if(c64->VJoys[c64->VPort2].AxisValue[i] == 1) c64->GamePort2 |= 1<<i;
                                            }
                                        }
                                        else c64->GamePort2 &= ~(1<<i);
                                    }
                                }
                            }
                        }
                      break;
                    }

                    case SDL_KEYDOWN:
                    {
                        switch(event.key.keysym.sym)
                        {
                        case SDLK_ESCAPE:
                            if(c64->RecJoyMapping)
                            {
                                c64->RecJoyMapping = false;
                            }
                            else
                            {
                                c64->WaitResetReady = false;
                                c64->RESET = false;
                            }
                            break;

                        case SDLK_F12:
                            c64->isFullscreen = !c64->isFullscreen;

                            if(c64->OpenGLEnable)
                            {
                                /// OpenGL Version ///
                            }
                            else
                            {
                                /// SDL Version ///
                                if(c64->isFullscreen)
                                {
                                    SDL_ShowCursor(false);
                                    c64->C64Screen = SDL_SetVideoMode(1280,1024,c64->AktWindowColorBits,SDL_FULLSCREEN | SDL_VIDEORESIZE | SDL_SWSURFACE | SDL_DOUBLEBUF);
                                }
                                else
                                {
                                    SDL_ShowCursor(true);
                                    c64->C64Screen = SDL_SetVideoMode(c64->AktWindowXW,c64->AktWindowYW,c64->AktWindowColorBits,SDL_VIDEORESIZE | SDL_SWSURFACE | SDL_DOUBLEBUF);
                                }
                            }
                            break;

                        default:
                            break;
                        }

                        if(c64->RecJoyMapping == true)
                        {
                            c64->VJoys[c64->RecJoySlotNr].Type[c64->RecJoyMappingPos] = VJOY_TYPE_KEY;
                            c64->VJoys[c64->RecJoySlotNr].KeyDown[c64->RecJoyMappingPos] = event.key.keysym.scancode;
                        }
                        else
                        {
                            for(int i=0;i<C64KeyNum;i++)
                            {
                                if(C64KeyTable[i].SDLKeyCode == event.key.keysym.sym)
                                {
                                    c64->KeyEvent(C64KeyTable[i].MatrixCode,KEY_DOWN,C64KeyTable[i].Shift);
                                }
                            }
                        }

                        /// VJoyStick abfrage ///
                        /// Port1
                        if((c64->VPort1 >= 0) && (c64->VPort1 < MAX_VJOYS))
                        {
                            for(int i=0;i<5;i++)
                            {
                                if((c64->VJoys[c64->VPort1].KeyDown[i] == event.key.keysym.scancode) &&
                                        (c64->VJoys[c64->VPort1].Type[i] == VJOY_TYPE_KEY))
                                    c64->GamePort1 |= 1<<i;
                            }
                        }

                        /// Port2
                        if((c64->VPort2 >= 0) && (c64->VPort2 < MAX_VJOYS))
                        {
                            for(int i=0;i<5;i++)
                            {
                                if((c64->VJoys[c64->VPort2].KeyDown[i] == event.key.keysym.scancode) &&
                                        (c64->VJoys[c64->VPort2].Type[i] == VJOY_TYPE_KEY))
                                    c64->GamePort2 |= 1<<i;
                            }
                        }
                        break;
                    }

                    case SDL_KEYUP:
                    {
                        switch(event.key.keysym.sym)
                        {
                        case SDLK_ESCAPE:
                            c64->RESET = true;
                        case SDLK_F12:

                            break;
                        default:
                            break;
                        }

                        if(c64->RecJoyMapping == true)
                        {
                            c64->VJoys[c64->RecJoySlotNr].Type[c64->RecJoyMappingPos] = VJOY_TYPE_KEY;
                            c64->VJoys[c64->RecJoySlotNr].KeyUp[c64->RecJoyMappingPos] = event.key.keysym.scancode;

                            c64->RecJoyMappingPos++;
                            if(c64->RecJoyMappingPos == 5)
                            {
                                /// Rec Mapping ist fertig ///
                                c64->RecJoyMapping = false;
                            }
                        }
                        else
                        {
                            for(int i=0;i<C64KeyNum;i++)
                            {
                                if(C64KeyTable[i].SDLKeyCode == event.key.keysym.sym)
                                {
                                    c64->KeyEvent(C64KeyTable[i].MatrixCode,KEY_UP,C64KeyTable[i].Shift);
                                }
                            }
                        }

                        /// VJoyStick abfrage ///
                        /// Port1
                        if((c64->VPort1 >= 0) && (c64->VPort1 < MAX_VJOYS))
                        {
                            for(int i=0;i<5;i++)
                            {
                                if((c64->VJoys[c64->VPort1].KeyUp[i] == event.key.keysym.scancode) &&
                                        (c64->VJoys[c64->VPort1].Type[i] == VJOY_TYPE_KEY))
                                    c64->GamePort1 &= ~(1<<i);
                            }
                        }

                        /// Port2
                        if((c64->VPort2 >= 0) && (c64->VPort2 < MAX_VJOYS))
                        {
                            for(int i=0;i<5;i++)
                            {
                                if((c64->VJoys[c64->VPort2].KeyUp[i] == event.key.keysym.scancode) &&
                                        (c64->VJoys[c64->VPort2].Type[i] == VJOY_TYPE_KEY))
                                    c64->GamePort2 &= ~(1<<i);
                            }
                        }
                        break;
                    }

                    case SDL_MOUSEMOTION:
                        break;

                    case SDL_QUIT:
    #ifdef _WIN32
                    c64->LoopThreadEnd = true;
    #endif
                    break;

                    default:
                        break;
                }
            }
            if(!c64->LoopThreadEnd)
            {
                if(c64->OpenGLEnable)
                {
                    static int counter = 0;
                    static unsigned char test = 1;
                    counter++;
                    if(counter == 5)
                    {
                        counter = 0;
                        test *= 2;
                        if(test == 16) test = 1;
                    }

                    /// OpenGL Version ///

                    glClearColor( 0.0, 0.0, 0.0, 0.0 ); // Farbe zum Löschen setzen
                    glClear( GL_COLOR_BUFFER_BIT );     // Aktuellen Bildpuffer löschen

                    if(!c64->DistortionEnable)
                    {
                        glBindTexture(GL_TEXTURE_2D,C64ScreenTexture);
                        c64->pal->ConvertVideo((void*)c64->C64ScreenBuffer,c64->AktC64ScreenXW*4,c64->vic_puffer + C64FirstViewedPixel,c64->AktC64ScreenXW,c64->AktC64ScreenYW,504,311,false);
                        glTexSubImage2D(GL_TEXTURE_2D,0,0,0, c64->AktC64ScreenXW, c64->AktC64ScreenYW,GL_BGRA, GL_UNSIGNED_BYTE, c64->C64ScreenBuffer);

                        glBegin(GL_QUADS);
                        glTexCoord2i(0,1);
                        glVertex3f(-1,-1,0.0);
                        glTexCoord2i(1,1);
                        glVertex3f(1,-1,0);
                        glTexCoord2i(1,0);
                        glVertex3f(1,1,0);
                        glTexCoord2i(0,0);
                        glVertex3f(-1,1,0);
                        glEnd();
                    }
                    else
                    {
                        //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

                        glBindTexture(GL_TEXTURE_2D,C64ScreenTexture);
                        c64->pal->ConvertVideo((void*)c64->C64ScreenBuffer,c64->AktC64ScreenXW*4,c64->vic_puffer + C64FirstViewedPixel,c64->AktC64ScreenXW,c64->AktC64ScreenYW,504,311,false);
                        glTexSubImage2D(GL_TEXTURE_2D,0,0,0, c64->AktC64ScreenXW, c64->AktC64ScreenYW,GL_BGRA, GL_UNSIGNED_BYTE, c64->C64ScreenBuffer);

                        glBegin(GL_QUADS);
                        for(int i=0; i < SUBDIVS_SCREEN*SUBDIVS_SCREEN; i++)
                        {
                            glTexCoord2f(c64->DistortionGridTex[i*4+0].x,c64->DistortionGridTex[i*4+0].y);
                            glVertex3f(c64->DistortionGrid[i*4+0].x,c64->DistortionGrid[i*4+0].y,0.0);
                            glTexCoord2f(c64->DistortionGridTex[i*4+1].x,c64->DistortionGridTex[i*4+1].y);
                            glVertex3f(c64->DistortionGrid[i*4+1].x,c64->DistortionGrid[i*4+1].y,0.0);
                            glTexCoord2f(c64->DistortionGridTex[i*4+2].x,c64->DistortionGridTex[i*4+2].y);
                            glVertex3f(c64->DistortionGrid[i*4+2].x,c64->DistortionGrid[i*4+2].y,0.0);
                            glTexCoord2f(c64->DistortionGridTex[i*4+3].x,c64->DistortionGridTex[i*4+3].y);
                            glVertex3f(c64->DistortionGrid[i*4+3].x,c64->DistortionGrid[i*4+3].y,0.0);
                        }
                        glEnd();
                    }

                    // Video Capture //
                    if(c64->NewVicRefresh)
                    {
                        c64->NewVicRefresh = false;
                        c64->video_cap->AddFrame(c64->vic_puffer,504,311);
                    }

                    /// Die Pfeile werden nur beim lernen angezeigt ///
                    if(c64->RecJoyMapping)
                    {
                        /// Nach Oben ///
                        if(c64->RecJoyMappingPos == 0) glBindTexture(GL_TEXTURE_2D,Pfeil1Texture);
                        else glBindTexture(GL_TEXTURE_2D,Pfeil0Texture);
                        glBegin(GL_QUADS);
                        glTexCoord2i(1,0);
                        glVertex3f(0.3,1,0.0);
                        glTexCoord2i(1,1);
                        glVertex3f(0.3,0.1,0);
                        glTexCoord2i(0,1);
                        glVertex3f(-0.3,0.1,0);
                        glTexCoord2i(0,0);
                        glVertex3f(-0.3,1,0);
                        glEnd();

                        /// Nach Unten ///
                        if(c64->RecJoyMappingPos == 1) glBindTexture(GL_TEXTURE_2D,Pfeil1Texture);
                        else glBindTexture(GL_TEXTURE_2D,Pfeil0Texture);
                        glBegin(GL_QUADS);
                        glTexCoord2i(1,0);
                        glVertex3f(0.3,-1,0.0);
                        glTexCoord2i(1,1);
                        glVertex3f(0.3,-0.1,0);
                        glTexCoord2i(0,1);
                        glVertex3f(-0.3,-0.1,0);
                        glTexCoord2i(0,0);
                        glVertex3f(-0.3,-1,0);
                        glEnd();

                        /// Nach Links ///
                        if(c64->RecJoyMappingPos == 2) glBindTexture(GL_TEXTURE_2D,Pfeil1Texture);
                        else glBindTexture(GL_TEXTURE_2D,Pfeil0Texture);
                        glBegin(GL_QUADS);
                        glTexCoord2i(1,0);
                        glVertex3f(-0.75,-0.4,0);
                        glTexCoord2i(1,1);
                        glVertex3f(-0.08,-0.4,0.0);
                        glTexCoord2i(0,1);
                        glVertex3f(-0.08,0.4,0);
                        glTexCoord2i(0,0);
                        glVertex3f(-0.75,0.4,0);
                        glEnd();

                        /// Nach Rechts ///
                        if(c64->RecJoyMappingPos == 3) glBindTexture(GL_TEXTURE_2D,Pfeil1Texture);
                        else glBindTexture(GL_TEXTURE_2D,Pfeil0Texture);
                        glBegin(GL_QUADS);
                        glTexCoord2i(1,0);
                        glVertex3f(0.75,-0.4,0);
                        glTexCoord2i(1,1);
                        glVertex3f(0.08,-0.4,0.0);
                        glTexCoord2i(0,1);
                        glVertex3f(0.08,0.4,0);
                        glTexCoord2i(0,0);
                        glVertex3f(0.75,0.4,0);
                        glEnd();

                        /// Fire ///
                        if(c64->RecJoyMappingPos == 4) glBindTexture(GL_TEXTURE_2D,Kreis1Texture);
                        else glBindTexture(GL_TEXTURE_2D,Kreis0Texture);
                        glBegin(GL_QUADS);
                        glTexCoord2i(1,0);
                        glVertex3f(0.07,-0.09,0);
                        glTexCoord2i(1,1);
                        glVertex3f(0.07,0.09,0.0);
                        glTexCoord2i(0,1);
                        glVertex3f(-0.07,0.09,0);
                        glTexCoord2i(0,0);
                        glVertex3f(-0.07,-0.09,0);
                        glEnd();
                    }

                    if(c64->StartScreenshot)
                    {
                        c64->StartScreenshot = false;
                        SDL_Surface * temp = SDL_CreateRGBSurface(SDL_SWSURFACE, c64->AktC64ScreenXW, c64->AktC64ScreenYW, 24,
                        #if SDL_BYTEORDER == SDL_LIL_ENDIAN
                        0x000000FF, 0x0000FF00, 0x00FF0000, 0
                        #else
                        0x00FF0000, 0x0000FF00, 0x000000FF, 0
                        #endif
                        );
                        if (temp == NULL)
                        return -1;

                        unsigned char* pixels = new unsigned char[3 * c64->AktC64ScreenXW * c64->AktC64ScreenYW];
                        if (pixels == NULL)
                        {
                        SDL_FreeSurface(temp);
                        return -1;
                        }

                        glReadPixels(0, 0, c64->AktC64ScreenXW, c64->AktC64ScreenYW, GL_RGB, GL_UNSIGNED_BYTE, pixels);

                        for (int i=0; i<c64->AktC64ScreenYW; i++)
                        memcpy(((char *) temp->pixels) + temp->pitch * i, pixels + 3*c64->AktC64ScreenXW * (c64->AktC64ScreenYW-i-1), c64->AktC64ScreenXW*3);
                        delete pixels;

                        SDL_SaveBMP(temp, c64->ScreenshotFilename);
                        SDL_FreeSurface(temp);
                    }

                    SDL_GL_SwapBuffers();

                    //SDL_framerateDelay(&c64->fps_manager);
                    SDL_Delay(5);

                    c64->DrawScreenBack = false;
                }
                else
                {
                    /// SDL Version ///
                    if(c64->DrawScreenBack)
                    {
                        SDL_Rect rec_src = {0,0,c64->AktC64ScreenXW,c64->AktC64ScreenYW};
                        SDL_Rect rec_dst = {0,0,(unsigned short)c64->C64Screen->w,(unsigned short)c64->C64Screen->h};

                        /// Auf Screenshot Start prüfen ///
                        if(c64->StartScreenshot)
                        {
                            c64->StartScreenshot = false;
                            SDL_SaveBMP(c64->C64ScreenBack,c64->ScreenshotFilename);
                        }

                        SDL_SoftStretch(c64->C64ScreenBack,&rec_src,c64->C64Screen,&rec_dst);
                        SDL_Flip(c64->C64Screen);

                        SDL_framerateDelay(&c64->fps_manager);
                        c64->DrawScreenBack = false;
                    }
                    else SDL_Delay(1);
                }
            }
        }
        else
        {
            SDL_Delay(10);
            c64->sdl_thread_is_paused = true;
        }
    }

    c64->ChangeGrafikModi = false;

    /// VicRefresh stoppen und solange warten bis wirklich Stop ist ///
    c64->HoldVicRefresh = true;
    c64->VicRefreshIsHold = false;

    if(c64->C64ScreenBuffer != 0)
        delete[] c64->C64ScreenBuffer;

    SDL_Quit();

    return 0;
}

void C64Class::CalcDistortionGrid()
{
    float div = 2.0f / SUBDIVS_SCREEN;
    for(int y=0; y<SUBDIVS_SCREEN+1; y++)
        for(int x=0; x<SUBDIVS_SCREEN+1; x++)
        {
            float X = x*div-1.0f;
            float Y = y*div-1.0f;
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

void C64Class::VicRefresh(unsigned char *vic_puffer)
{
    if(HoldVicRefresh)
    {
        VicRefreshIsHold = true;
        return;
    }

    if((pal == 0) || (sdl_thread_pause == true)) return;

    if(OpenGLEnable)
    {
        /// OpenGL Version ///

        //if(DrawScreenBack) SDL_Delay(2);
        //pal->ConvertVideo((void*)C64ScreenBuffer,AktC64ScreenXW*4,vic_puffer,AktC64ScreenXW,AktC64ScreenYW,504,312,false);
        this->vic_puffer = vic_puffer;
        vic->SwitchVideoPuffer();

        DrawScreenBack = true;
    }
    else
    {
        /// SDL Version ///
        if(DrawScreenBack) SDL_Delay(2);

        SDL_LockSurface(C64ScreenBack);
        //pal->ConvertVideo((void*)C64ScreenBack->pixels,C64ScreenBack->pitch,vic_puffer,AktC64ScreenXW,AktC64ScreenYW,504,312,false);
        this->vic_puffer = vic_puffer;
        vic->SwitchVideoPuffer();

        SDL_UnlockSurface(C64ScreenBack);
        DrawScreenBack = true;
    }

    NewVicRefresh = true;
}

void C64Class::FillAudioBuffer(unsigned char *stream, int laenge)
{
    unsigned short* puffer = (unsigned short*) stream;
    static unsigned int counter_plus=0;

    sid1->SoundBufferPos = 0;
    sid2->SoundBufferPos = 0;
    for(int i=0; i< FloppyAnzahl; i++)
        floppy[i]->ZeroSoundBufferPos();

    if(!DebugMode)
    {
        while((sid1->SoundBufferPos < (laenge/2) && (DebugMode == false)))
        {
            CheckKeys();
            CycleCounter++;

            /// Für Externe Erweiterungen ///
            //if(ExtZyklus) ZyklusProcExt();

            FloppyIEC = 0;
            for(int i=0; i<FloppyAnzahl; i++)
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
            //sid2->OneZyklus();
            reu->OneZyklus();

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
                            SDL_CreateThread(SDLThreadLoad,this);
                            WaitResetReady = false;
                        }
                    }
                    else
                    {
                        if((C64ResetReady == true) && (FloppyResetReady[0] == true))
                        {
                            SDL_CreateThread(SDLThreadLoad,this);
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
                            WriteC64Byte(0x277,ComandZeile[ComandZeileCount]);
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
        SDL_MixAudio((unsigned char*)puffer,(unsigned char*)sid1->SoundBuffer,laenge,100);
        //SDL_MixAudio((unsigned char*)puffer,(unsigned char*)sid2->SoundBuffer,laenge,100);
        for(int i=0; i< FloppyAnzahl; i++)
        {
            if(floppy[i]->GetEnableFloppySound()) SDL_MixAudio((unsigned char*)puffer,(unsigned char*)floppy[i]->GetSoundBuffer(),laenge,255);
        }
    }
    else
    {
        if(DebugAnimation)
        {
            AnimationSpeedCounter += AnimationSpeedAdd;
            if(AnimationSpeedCounter >= 1)
            {
                for(int i=0;i<(int)AnimationSpeedCounter;i++)
                {
                    CheckKeys();
                    CycleCounter++;

                    /// Für Externe Erweiterungen ///
                    //if(ExtZyklus) ZyklusProcExt();

                    FloppyIEC = 0;
                    for(int i=0; i<FloppyAnzahl; i++)
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
                    //sid2->OneZyklus();
                    reu->OneZyklus();

                    if(EnableExtLines) RDY_BA = ExtRDY;
                    cpu->OneZyklus();

                    if((BreakStatus != 0) || (FloppyFoundBreakpoint == true )) if(CheckBreakpoints()) break;
                }
                AnimationSpeedCounter = 0;
            }
            if(AnimationRefreshProc != 0) AnimationRefreshProc();
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
                for(int i=0; i<FloppyAnzahl; i++)
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
                //sid2->OneZyklus();
                reu->OneZyklus();

                if(EnableExtLines) RDY_BA = ExtRDY;
                cpu->OneZyklus();

                if((BreakStatus != 0) || (FloppyFoundBreakpoint == true )) CheckBreakpoints();

                if(AnimationRefreshProc != 0) AnimationRefreshProc();
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
                //sid2->OneZyklus();
                reu->OneZyklus();

                if(OneOpcSource > 0)
                {
                    int FloppyNr = OneOpcSource - 1;
                    FloppyIEC = 0;
                    for(int i=0; i<FloppyAnzahl; i++)
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

                if(AnimationRefreshProc != 0) AnimationRefreshProc();
            }
        }
    }
}

void C64Class::KeyEvent(unsigned char matrix_code,KeyStatus key_status, bool isAutoShift)
{
    const unsigned char AutoShift = 0x17; // C64 Shift Links

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
    SDL_Delay(20);
    SetReset(true,true);
}

void C64Class::HardReset()
{
    WaitResetReady = false;
    SetReset(false,false);
    SDL_Delay(20);
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

inline void C64Class::CheckKeys(void)
{
    unsigned char OUT_PA, OUT_PB;
    unsigned char IN_PA, IN_PB;

    OUT_PA = ~CIA1_PA.GetOutput();
    OUT_PB = ~CIA1_PB.GetOutput();

    IN_PA = IN_PB = 0;

    unsigned char cbit = 1;
    for(int i=0;i<8;i++)
    {
        if(OUT_PA & cbit) IN_PB |= (KeyboardMatrixToPB[i]|KeyboardMatrixToPBExt[i]);
        if(OUT_PB & cbit) IN_PA |= (KeyboardMatrixToPA[i]|KeyboardMatrixToPAExt[i]);
        cbit <<= 1;
    }
    CIA1_PA.SetInput(~(IN_PA|GamePort2));
    CIA1_PB.SetInput(~(IN_PB|GamePort1));
}

void C64Class::ResetC64CycleCounter(void)
{
    CycleCounter = 0;
}

bool C64Class::LoadC64Roms(char *kernalrom,char *basicrom,char *charrom)
{
    if(!mmu->LoadKernalRom(kernalrom)) return false;
    if(!mmu->LoadBasicRom(basicrom)) return false;
    if(!mmu->LoadCharRom(charrom)) return false;
    return true;
}

bool C64Class::LoadFloppyRom(int floppy_nr,char *dos1541rom)
{
    if(floppy_nr < FloppyAnzahl)
    {
        if(!floppy[floppy_nr]->LoadDosRom(dos1541rom)) return false;
        return true;
    }
    return false;
}

bool C64Class::LoadDiskImage(int floppy_nr, char *filename)
{
    if(floppy_nr < FloppyAnzahl)
    {
        return floppy[floppy_nr]->LoadDiskImage(filename);
    }
    return false;
}

void C64Class::SetCommandLine(char* c64_command)
{
    strcpy(ComandZeile,c64_command);
    ComandZeileSize=(int)strlen(ComandZeile);
    ComandZeileCount=0;
    ComandZeileStatus=true;
    ComandZeileCountS=true;
}

void C64Class::KillCommandLine(void)
{
    ComandZeileSize=0;
    ComandZeileStatus=false;
    ComandZeileCountS=false;
}

unsigned char C64Class::ReadC64Byte(unsigned short adresse)
{
    return ReadProcTbl[(adresse)>>8](adresse);
}

void C64Class::WriteC64Byte(unsigned short adresse,unsigned char wert)
{
    WriteProcTbl[(adresse)>>8](adresse,wert);
}

unsigned char* C64Class::GetRAMPointer(unsigned short adresse)
{
    return mmu->GetRAMPointer() + adresse;
}

void C64Class::SetGrafikModi(bool colbits32, bool doublesize,bool pal_enable,bool filter_enable, int fullres_xw, int fullres_yw)
{
    ColBits32 = colbits32;
    DoubleSize = doublesize;
    PalEnable =  pal_enable;
    FilterEnable = filter_enable;
    FullResXW = fullres_xw;
    FullResYW = fullres_yw;

    ChangeGrafikModi = true;

    char str00[100];
    sprintf(str00,">>   32Bit = %d\n>>   Doublesize = %d\n>>   PAL = %d\n>>   Filter = %d\n>>   FullResXW = %d\n>>   FullResrYW = %d\n",ColBits32,DoubleSize,PalEnable,FilterEnable,FullResXW,FullResYW);

    LogText((char*)">> Grafikmodus wurde gesetzt:\n");
    LogText(str00);
}

void C64Class::SetDistortion(float value)
{
    Distortion = value;
    CalcDistortionGrid();
}

void C64Class::SetC64Speed(int speed)
{
    sid1->SetC64Zyklen(985248.f*(float)speed/100.f);
}



int SDLThreadLoad(void *userdat)
{
    unsigned short PRGStartAdresse;
    C64Class *c64 = (C64Class*)userdat;

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
int C64Class::LoadAutoRun(int floppy_nr, char *filename)
{
    char EXT[4];

    int len = (int)strlen(filename);
    strcpy(EXT,filename+len-3);

    EXT[0]=toupper(EXT[0]);
    EXT[1]=toupper(EXT[1]);
    EXT[2]=toupper(EXT[2]);

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

    if(0==strcmp("PRG",EXT))
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

int C64Class::LoadPRG(char *filename, unsigned short* ret_startadresse)
{
    unsigned char *RAM = mmu->GetRAMPointer();
    FILE *file;
    char EXT[4];

    int len = (int)strlen(filename);
    strcpy(EXT,filename+len-3);

    EXT[0]=toupper(EXT[0]);
    EXT[1]=toupper(EXT[1]);
    EXT[2]=toupper(EXT[2]);

    char str00[256];

    if(0==strcmp("PRG",EXT))
    {
        LogText((char*)">> PRG laden: ");
        LogText(filename);
        LogText((char*)"\n");

        unsigned short StartAdresse;
        int reading_bytes;
        unsigned char temp[2];
        file = fopen (filename, "rb");
        if (file == NULL)
        {
            LogText((char*)"<< ERROR: Datei konnte nicht geöffnet werden");
            return 0x01;
        }
        reading_bytes = fread (&temp,1,2,file);
        StartAdresse=temp[0]|(temp[1]<<8);
        if(ret_startadresse != 0) *ret_startadresse = StartAdresse;

        reading_bytes=fread (RAM+StartAdresse,1,0xFFFF,file);

        RAM[0x2B] = 0x01;
        RAM[0x2C] = 0x08;

        sprintf(str00,">>   SartAdresse: $%4.4X(%d)",StartAdresse,StartAdresse);
        LogText(str00);

        StartAdresse+=(unsigned short)reading_bytes;
        RAM[0x2D] = (unsigned char)StartAdresse;
        RAM[0x2E] = (unsigned char)(StartAdresse>>8);
        RAM[0xAE] = (unsigned char)StartAdresse;
        RAM[0xAF] = (unsigned char)(StartAdresse>>8);

        fclose(file);

        sprintf(str00,"EndAdresse: $%4.4X(%d)\n",StartAdresse,StartAdresse);
        LogText(str00);

        return 0x00;
    }

    if(0==strcmp("T64",EXT))
    {
        LogText((char*)">> T64 laden: ");
        LogText(filename);
        LogText((char*)"\n");

        char Kennung[32];
        unsigned short T64Entries;
        unsigned short StartAdresse;
        unsigned short EndAdresse;
        int FileStartOffset;

        file = fopen (filename, "rb");
        if (file == NULL)
        {
            LogText((char*)"<< ERROR: Datei konnte nicht geöffnet werden");
                return 0x01;
        }

        fread(Kennung,1,32,file);

        fseek(file,4,SEEK_CUR);
        fread(&T64Entries,1,2,file);

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
        fread(&StartAdresse,1,2,file);
        if(ret_startadresse != 0) *ret_startadresse = StartAdresse;
        fread(&EndAdresse,1,2,file);
        fseek(file,2,SEEK_CUR);
        fread(&FileStartOffset,1,4,file);

        fseek(file,FileStartOffset,SEEK_SET);
        fread(RAM+StartAdresse,1,EndAdresse-StartAdresse,file);
        fclose(file);

        RAM[0x2B] = 0x01;
        RAM[0x2C] = 0x08;

        RAM[0x2D] = (unsigned char)EndAdresse;
        RAM[0x2E] = (unsigned char)(EndAdresse>>8);
        RAM[0xAE] = (unsigned char)EndAdresse;
        RAM[0xAF] = (unsigned char)(EndAdresse>>8);

        sprintf(str00,">>   SartAdresse: $%4.4X(%d)",StartAdresse,StartAdresse);
        LogText(str00);
        sprintf(str00,"EndAdresse: $%4.4X(%d)\n",EndAdresse,EndAdresse);
        LogText(str00);

        return 0x00;
    }

    if(0==strcmp("P00",EXT))
    {
        LogText((char*)">> P00 laden: ");
        LogText(filename);
        LogText((char*)"\n");

        char Kennung[8];
        unsigned short StartAdresse;
        int reading_bytes;
        unsigned char temp[2];
        file = fopen (filename, "rb");
        if (file == NULL)
        {
            LogText((char*)"<< ERROR: Datei konnte nicht geöffnet werden");
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
        StartAdresse=temp[0]|(temp[1]<<8);
        if(ret_startadresse != 0) *ret_startadresse = StartAdresse;

        reading_bytes=fread (RAM+StartAdresse,1,0xFFFF,file);

        RAM[0x2B] = 0x01;
        RAM[0x2C] = 0x08;

        sprintf(str00,">>   SartAdresse: $%4.4X(%d)",StartAdresse,StartAdresse);
        LogText(str00);

        StartAdresse+=(unsigned short)reading_bytes;
        RAM[0x2D] = (unsigned char)StartAdresse;
        RAM[0x2E] = (unsigned char)(StartAdresse>>8);
        RAM[0xAE] = (unsigned char)StartAdresse;
        RAM[0xAF] = (unsigned char)(StartAdresse>>8);

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

void C64Class::RemoveCRT(void)
{
    crt->RemoveCRTImage();
    IOSource = 0;
    KillCommandLine();
    HardReset();
}

void C64Class::InsertREU(void)
{
    IOSource = 2;

    crt->RemoveCRTImage();
    geo->Remove();

    reu->Insert();
    ReuIsInsert = true;

    KillCommandLine();
    HardReset();
}

void C64Class::RemoveREU(void)
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

void C64Class::InsertGEORAM(void)
{
    IOSource = 3;

    crt->RemoveCRTImage();
    reu->Remove();
    ReuIsInsert = false;

    geo->Insert();

    KillCommandLine();
    HardReset();
}

void C64Class::RemoveGEORAM(void)
{
    geo->Remove();
    IOSource = 0;

    KillCommandLine();
    HardReset();
}

int C64Class::LoadGEORAMImage(char* filename)
{
    return geo->LoadRAM(filename);
}

int C64Class::SaveGEORAMImage(char* filename)
{
    return geo->SaveRAM(filename);
}

void C64Class::ClearGEORAMRam(void)
{
    geo->ClearRAM();
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
        for(int i=0; i<FloppyAnzahl; i++) floppy[i]->SetEnableFloppySound(false);
    }
    else
    {
        OneZyk = false;
        OneOpc = false;
        sid1->SoundOutputEnable = true;
        sid2->SoundOutputEnable = true;
        for(int i=0; i<FloppyAnzahl; i++) floppy[i]->SetEnableFloppySound(true);
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

void C64Class::OneZyklus(void)
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
    AnimationSpeedAdd = (double)AudioPufferSize/(double)AudioSampleRate*(double)cycle_sek;
}

void C64Class::GetC64CpuReg(REG_STRUCT *reg,IREG_STRUCT *ireg)
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
    iec->ATN_OUT = (bool)!!(C64IEC & 16);
    iec->CLOCK_OUT = (bool)!!(C64IEC & 64);
    iec->DATA_OUT = (bool)!!(C64IEC & 128);
    iec->CLOCK_IN = (bool)!!(FloppyIEC & 64);
    iec->DATA_IN = (bool)!!(FloppyIEC & 128);
}

int C64Class::AddBreakGroup(void)
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
    if(index >= BreakGroupAnz) return 0;
    return BreakGroup[index];
}

void C64Class::UpdateBreakGroup(void)
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

    for(int i=0;i<FloppyAnzahl;i++)
    {
        if(floppy[i]->GetEnableFloppy())
        {
            floppy[i]->UpdateBreakGroup();
        }
    }
}

void C64Class::DeleteAllBreakGroups(void)
{
    for(int i=0;i<BreakGroupAnz;i++)
    {
        delete BreakGroup[i];
    }
    BreakGroupAnz = 0;
    UpdateBreakGroup();
}

int C64Class::GetBreakGroupAnz(void)
{
    return BreakGroupAnz;
}

int C64Class::LoadBreakGroups(char *filename)
{
    FILE *file;
    char Kennung[10];
    char Version;
    unsigned char Groupanzahl;

    DeleteAllBreakGroups();

    file = fopen (filename, "rb");
    if (file == NULL)
    {
            /// Datei konnte nicht geöffnet werden ///
            return -1;
    }

    /// Kennung ///
    fread(Kennung,sizeof(Kennung),1,file);
    if(0 != strcmp("EMU64_BPT",Kennung))
    {
        /// Kein Emu64 Format ///
        fclose(file);
        return -2;
    }

    /// Version ///
    fread(&Version,sizeof(Version),1,file);
    if(Version > 1) return -3;

    /// Groupanzahl ///
    fread(&Groupanzahl,sizeof(Groupanzahl),1,file);
    if(Groupanzahl == 0) return -4;

    /// Groups ///
    for(int ii=0;ii<Groupanzahl;ii++)
    {
        int i = AddBreakGroup();
        fread(BreakGroup[i]->Name,sizeof(BreakGroup[i]->Name),1,file);
        fread(&BreakGroup[i]->Enable,sizeof(BreakGroup[i]->Enable),1,file);
        fread(&BreakGroup[i]->bPC,sizeof(BreakGroup[i]->bPC),1,file);
        fread(&BreakGroup[i]->iPC,sizeof(BreakGroup[i]->iPC),1,file);
        fread(&BreakGroup[i]->bAC,sizeof(BreakGroup[i]->bAC),1,file);
        fread(&BreakGroup[i]->iAC,sizeof(BreakGroup[i]->iAC),1,file);
        fread(&BreakGroup[i]->bXR,sizeof(BreakGroup[i]->bXR),1,file);
        fread(&BreakGroup[i]->iXR,sizeof(BreakGroup[i]->iXR),1,file);
        fread(&BreakGroup[i]->bYR,sizeof(BreakGroup[i]->bYR),1,file);
        fread(&BreakGroup[i]->iYR,sizeof(BreakGroup[i]->iYR),1,file);
        fread(&BreakGroup[i]->bRAdresse,sizeof(BreakGroup[i]->bRAdresse),1,file);
        fread(&BreakGroup[i]->iRAdresse,sizeof(BreakGroup[i]->iRAdresse),1,file);
        fread(&BreakGroup[i]->bWAdresse,sizeof(BreakGroup[i]->bWAdresse),1,file);
        fread(&BreakGroup[i]->iWAdresse,sizeof(BreakGroup[i]->iWAdresse),1,file);
        fread(&BreakGroup[i]->bRWert,sizeof(BreakGroup[i]->bRWert),1,file);
        fread(&BreakGroup[i]->iRWert,sizeof(BreakGroup[i]->iRWert),1,file);
        fread(&BreakGroup[i]->bWWert,sizeof(BreakGroup[i]->bWWert),1,file);
        fread(&BreakGroup[i]->iWWert,sizeof(BreakGroup[i]->iWWert),1,file);
        fread(&BreakGroup[i]->bRZ,sizeof(BreakGroup[i]->bRZ),1,file);
        fread(&BreakGroup[i]->iRZ,sizeof(BreakGroup[i]->iRZ),1,file);
        fread(&BreakGroup[i]->bRZZyklus,sizeof(BreakGroup[i]->bRZZyklus),1,file);
        fread(&BreakGroup[i]->iRZZyklus,sizeof(BreakGroup[i]->iRZZyklus),1,file);
    }
    return 0;
}

bool C64Class::SaveBreakGroups(char *filename)
{
    FILE *file;
    char Kennung[]  = "EMU64_BPT";
    char Version    = 1;

    file = fopen (filename, "wb");
    if (file == NULL)
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

bool C64Class::ExportPRG(char *filename, unsigned short start_adresse, unsigned short end_adresse, int source)
{
    FILE *file;
    unsigned char *RAM;

    if(source > 0)
    {
        RAM = floppy[source-1]->GetRamPointer();
    }
    else RAM = mmu->GetRAMPointer();

    if(RAM == 0) return false;

    file = fopen (filename, "wb");
    if (file == NULL)
    {
        return false;
    }
    fwrite(&start_adresse,sizeof(start_adresse),1,file);
    for(int i=start_adresse;i<end_adresse;i++) fwrite(&RAM[i],1,1,file);
    fclose(file);

    return true;
}

bool C64Class::ExportRAW(char *filename, unsigned short start_adresse, unsigned short end_adresse, int source)
{
    FILE *file;
    unsigned char *RAM;

    if(source > 0)
    {
        RAM = floppy[source-1]->GetRamPointer();
    }
    else RAM = mmu->GetRAMPointer();

    if(RAM == 0) return false;

    file = fopen (filename, "wb");
    if (file == NULL)
    {
        return false;
    }
    for(int i=start_adresse;i<end_adresse;i++) fwrite(&RAM[i],1,1,file);
    fclose(file);

    return true;
}

bool C64Class::ExportASM(char *filename, unsigned short start_adresse, unsigned short end_adresse, int source)
{
    FILE *file;

    file = fopen (filename, "w");
    if (file == NULL)
    {
        return false;
    }

    char *source_str = (char*)"";

    switch(source)
    {
    case 0:
        source_str = (char*)"C64";
        break;
    case 1:
        source_str = (char*)"Floppy #08";
        break;
    case 2:
        source_str = (char*)"Floppy #09";
        break;
    case 3:
        source_str = (char*)"Floppy #10";
        break;
    case 4:
        source_str = (char*)"Floppy #11";
        break;
    }

    fprintf(file,"Disassembler Listing wurde mit Emu64 %s erzeugt\n",str_emu64_version);
    fprintf(file,"-------------------------------------------------\n");
    fprintf(file,"Von: $%4.4X Bis: $%4.4X Quelle: %s",start_adresse,end_adresse,source_str);
    fprintf(file,"\n");
    fprintf(file,"\n");

    int pc = start_adresse;
L10:
    pc = DisAss(file,pc,true,source);
    if(pc < end_adresse) goto L10;

    fclose(file);

    return true;
}

unsigned char C64Class::GetMapReadSource(unsigned char page)
{
    return mmu->GetReadSource(page);
}

unsigned char C64Class::GetMapWriteDestination(unsigned char page)
{
    return mmu->GetWriteDestination(page);
}

void C64Class::SaveScreenshot(char *filename)
{
    strcpy(ScreenshotFilename,filename);
    StartScreenshot = true;
}

int C64Class::StartVideoCapture(char *filename)
{
    return video_cap->Start(filename,4000000,504,312,50);
}

void C64Class::StopVideoCapture()
{
    video_cap->Stop();
}

int C64Class::DisAss(FILE *file, int PC, bool line_draw, int source)
{
    char Ausgabe[36];Ausgabe[0]=0;
    char Adresse[7];Adresse[0]=0;
    char Speicher[14];Speicher[0]=0;
    char Opcode[5];Opcode[0]=0;
    char Anhang[10];Anhang[0]=0;

    int TMP;
    int OPC;

    unsigned short int word;
    unsigned short int A;
    char B;

    unsigned char ram0;
    unsigned char ram1;
    unsigned char ram2;

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
    case 9:
        sprintf(Speicher,"$%2.2X $%2.2X      ",ram0,ram1);
        B=ram1;
        A=(PC+2)+B;
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

bool C64Class::CheckBreakpoints(void)
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
    for(int i=0;i<FloppyAnzahl;i++)
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
            for(int i=0; i<FloppyAnzahl; i++) floppy[i]->SetEnableFloppySound(false);
            if(BreakpointProc != NULL) BreakpointProc();
            return true;
        }
        else
        {
            DebugAnimation = false;
            OneOpc = false;
            OneZyk = false;
            if(BreakpointProc != NULL) BreakpointProc();
            return true;
        }
    }
    return false;
}

void C64Class::WriteSidIO(unsigned short adresse,unsigned char wert)
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

unsigned char C64Class::ReadSidIO(unsigned short adresse)
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
void C64Class::WriteIO1(unsigned short adresse,unsigned char wert)
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
void C64Class::WriteIO2(unsigned short adresse,unsigned char wert)
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
unsigned char C64Class::ReadIO1(unsigned short adresse)
{
    switch(IOSource)
    {
    case 0: // NO_MODUL
        return 0;
        break;
    case 1: // CRT
        return crt->ReadIO1(adresse);
        break;
    case 2: // REU
        return reu->ReadIO1(adresse);
        break;
    case 3: // GEORAM
        return geo->ReadIO1(adresse);
        break;
    default:
        return 0;
        break;
    }
    return 0;
}

/// $DF00

unsigned char C64Class::ReadIO2(unsigned short adresse)
{
    switch(IOSource)
    {
    case 0: // NO_MODUL
        return 0;
        break;
    case 1: // CRT
        return crt->ReadIO2(adresse);
        break;
    case 2: // REU
        return reu->ReadIO2(adresse);
        break;
    case 3: // GEORAM
        return geo->ReadIO2(adresse);
        break;
    default:
        return 0;
        break;
    }
    return 0;
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

void C64Class::OpenSDLJoystick()
{
    JoyStickUdateIsStop = false;
    StopJoystickUpdate = true;
    int AbbruchCounter = 1000;
    while(!JoyStickUdateIsStop && (AbbruchCounter > 0))
    {
        SDL_Delay(1);
        AbbruchCounter--;
    }

    if(SDLJoystickIsOpen)
    {
        SDLJoystickIsOpen = false;
        SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
        LogText((char*)">> SDL Subsystem Joystick wurde geschlossen\n");
    }

    if(SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0)
    {
        SDLJoystickIsOpen = false;
        LogText((char*)"<< ERROR: SDL Subsystem Joystick konnte nicht geöffnet werden\n");
    }
    else
    {
        SDLJoystickIsOpen = true;
        LogText((char*)">> SDL Subsytem Joystick wurde erfolgreich geöffnet\n");
        JoystickAnzahl = SDL_NumJoysticks();
        if(JoystickAnzahl > MAX_JOYSTICKS) JoystickAnzahl = MAX_JOYSTICKS;

        char str00[256];
        switch(JoystickAnzahl)
        {
        case 0:
            LogText((char*)"<< SDL konnte keinen Joystick/Gamepad erkennen\n");
            break;
        case 1:
            LogText((char*)">> SDL konnte 1 Joystick/Gamepad erkennen\n");
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
                JoystickNamen[i] = SDL_JoystickName(i);
                sprintf(str00,">>   [%2.2d] %s\n",i,JoystickNamen[i]);
                LogText(str00);
                SDL_JoystickOpen(i);
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
        SDL_Delay(1);
        AbbruchCounter--;
    }
    if(SDLJoystickIsOpen)
    {
        SDLJoystickIsOpen = false;
        SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
        LogText((char*)">> SDL Subsystem Joystick wurde geschlossen\n");
    }
    StopJoystickUpdate = true;
}
