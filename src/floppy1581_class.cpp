//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: floppy1581_class.cpp                  //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 20.08.2021	       		//
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "floppy1581_class.h"

Floppy1581::Floppy1581(bool *reset)
{
	RESET = reset;
}

void Floppy1581::SetC64IEC(uint8_t *iec)
{

}

void Floppy1581::SetResetReady(bool *ResetReady, uint16_t ResetReadyAdr)
{
	//cpu->ResetReady = ResetReady;
	//cpu->ResetReadyAdr = ResetReadyAdr;
}

void Floppy1581::SetDeviceNumber(uint8_t number)
{
	if((number < 8) || (number > 11)) return;
	if(Jumper == (number-8)) return;

	Jumper = number - 8;

	//cpu->Reset();
}

bool Floppy1581::LoadDosRom(const char *filename)
{
	FILE *file;
	file = fopen(filename, "rb");
	if (file == NULL)
	{
		return false;
	}

	if(0x8000 != fread (ROM, 1, 0x8000, file))
	{
		fclose(file);
		return false;
	}

	fclose(file);
	return true;
}

void Floppy1581::SetEnableFloppy(bool status)
{
	if((!FloppyEnabled) && (status))
	{
		/*
		MotorStatusOld = false;
		DiskMotorOn = false;

		CycleCounter = 0;

		cpu->Reset();
		via1->Reset();
		via2->Reset();

		StepperIncWait = true;

		AktHalbSpur = -1; //1
		GCR_PTR = GCRSpurStart = GCRImage + ((AktHalbSpur)) * GCR_TRACK_SIZE;
		GCRSpurEnde = GCRSpurStart + TrackSize[AktHalbSpur];
		*/
	}
	else
	{
		// MotorLoopStatus = 0;
		// StepperLoopStatus = 0;
	}

	FloppyEnabled = status;
	if(!status)
		FloppyIECLocal = 0xff;
}

bool Floppy1581::OneCycle()
{
	if(!FloppyEnabled) return true;

	return true;
}
