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
// Letzte Änderung am 01.08.2013                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include <c64_class.h>
#include <stddef.h>

#include <android/log.h>
#define  LOG_TAG    "NATIVE"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

C64Class::C64Class(int SoundBufferSize ,VideoPalClass *pal):
	mmu(NULL),cpu(NULL),vic(NULL),sid1(NULL),sid2(NULL),cia1(NULL),cia2(NULL),crt(NULL)
{
	LOGD(">> C64 Klasse wird gestratet...");

	RESET = false;		// RESET auf LOW setzen somit startet die Emualtion noch nicht

	this->pal = pal;
	AktC64ScreenXW = 384; // 384;
	AktC64ScreenYW = 272; // 272;
	AktWindowColorBits = 16;
	C64ScreenBuffer = new unsigned char[AktC64ScreenXW*AktC64ScreenYW*2];

	pal->SetDisplayMode(AktWindowColorBits);
	pal->EnablePALOutput(false);
	pal->EnableVideoDoubleSize(false);
	pal->SetC64Palette(0);

    VPort1 = 0;
    VPort2 = 1;

    GamePort1 = 0;
    GamePort2 = 0;

    int ret_error;

    /// Init Classes ///
    mmu = new MMU();
    cpu = new MOS6510();
    vic = new VICII();
    sid1 = new MOS6581_8085(0,44100,SoundBufferSize,&ret_error);
    sid2 = new MOS6581_8085(1,44100,SoundBufferSize,&ret_error);
    cia1 = new MOS6526(0);
    cia2 = new MOS6526(1);
    crt = new CRTClass();

    cia2->FloppyIEC = &FloppyIEC;
    cia2->C64IEC = &C64IEC;

    /// Floppy mit C64 verbinden ///

    for(int i=0;i<FloppyAnzahl;i++)
    {
        floppy[i] = new Floppy1541(&RESET,44100,SoundBufferSize,NULL);
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
    AnimationRefreshProc = NULL;
    AnimationSpeedAdd = (double)SoundBufferSize/(double)44100;
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

     //reu->SetReadProcTable( mmu_GetCPUReadProcTable());
     //reu->SetWriteProcTable(mmu_GetCPUWriteProcTable());

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
      //reu->BA = &RDY_BA;
      //reu->CpuTriggerInterrupt = (TRIGGER_INTERRUPT)cpu_TriggerInterrupt;
      //reu->CpuClearInterrupt = (CLEAR_INTERRUPT)cpu_ClearInterrupt;
      //reu->RESET = &RESET;
      //reu->WRITE_FF00 = &cpu_WRITE_FF00;

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
       sid1->SetC64Zyklen(985248);
       sid1->SetChipType(MOS_8580);
       sid1->SoundOutputEnable = true;
       sid1->CycleExact = false;
       sid1->FilterOn = false;
       sid1->Reset();

       sid2->RESET = &RESET;
       sid2->SetC64Zyklen(985248);
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
}

C64Class::~C64Class()
{
	LOGD(">> C64 Klasse wird beendet...");

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
}

void C64Class::FillAudioBuffer(short *stream, int laenge)
{
	static unsigned short *puffer_li;
	static unsigned short *puffer_re;

	puffer_li = (unsigned short*) stream;
	puffer_re = puffer_li+1;

	sid1->SoundBufferPos = 0;

	while(sid1->SoundBufferPos < (laenge/2))
	{
        FloppyIEC = 0;
         for(int i=0; i<FloppyAnzahl; i++)
         {
             //floppy[i]->OneZyklus();

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
         cpu->OneZyklus();
	}

	for(int i=0;i<(laenge/2);i++)
	{
		*puffer_li = sid1->SoundBuffer[i];	// li
		*puffer_re = sid1->SoundBuffer[i];	// re

		puffer_li += 2;
		puffer_re += 2;
	}
}

void C64Class::SoftReset()
{
    WaitResetReady = false;
    SetReset(false,true);
    //usleep(20);
    SetReset(true,true);
}

void C64Class::HardReset()
{
    WaitResetReady = false;
    SetReset(false,false);
    //usleep(20);
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

void C64Class::KillCommandLine(void)
{
    ComandZeileSize=0;
    ComandZeileStatus=false;
    ComandZeileCountS=false;
}

void C64Class::VicRefresh(unsigned char *vic_puffer)
{
	/*
    if(HoldVicRefresh)
    {
        VicRefreshIsHold = true;
        return;
    }
    */

    if(pal == NULL) return;

    /// OpenGL Version ///
	pal->ConvertVideo((void*)C64ScreenBuffer,AktC64ScreenXW*2,vic_puffer,AktC64ScreenXW,AktC64ScreenYW,504,312,false);
}

void C64Class::GetC64CpuReg(REG_STRUCT *reg,IREG_STRUCT *ireg)
{
    cpu->GetInterneRegister(ireg);
    ireg->CycleCounter = CycleCounter;
    ireg->GAME = GAME;
    ireg->EXROM = EXROM;
    cpu->GetRegister(reg);
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

unsigned char C64Class::ReadSidIO(unsigned short)
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
    return 0;
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
        //reu->WriteIO1(adresse,wert);
        break;
    case 3:	// GEORAM
        //georam->WriteIO1(adresse,wert);
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
        //reu->WriteIO2(adresse,wert);
        break;
    case 3: // GEORAM
        //georam->WriteIO2(adresse,wert);
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
        //return reu->ReadIO1(adresse);
        break;
    case 3: // GEORAM
        //return georam->ReadIO1(adresse);
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
        // return reu->ReadIO2(adresse);
        break;
    case 3: // GEORAM
        //return georam->ReadIO2(adresse);
        break;
    default:
        return 0;
        break;
    }
    return 0;
}
