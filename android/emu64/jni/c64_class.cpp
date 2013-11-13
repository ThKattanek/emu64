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

C64Class::C64Class(int SoundBufferSize)
{
	LOGD(">> C64 Klasse wird gestratet...");


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

    RESET = true;
}

C64Class::~C64Class()
{
	if(sid1 != NULL)
	{
		delete sid1;
		sid1 = NULL;
	}
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
		sid1->OneZyklus();
	}

	for(int i=0;i<(laenge/2);i++)
	{
		*puffer_li = sid1->SoundBuffer[i];	// li
		*puffer_re = sid1->SoundBuffer[i];	// re

		puffer_li += 2;
		puffer_re += 2;
	}
}
