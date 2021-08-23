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
// Letzte Änderung am 23.08.2021	       		//
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "floppy1581_class.h"

Floppy1581::Floppy1581(bool *reset)
{
	RESET = reset;
	CIA_IRQ = false;

	CycleCounter = 0;

	cpu = new MOS6502();
	cia = new MOS8520();
	disk_controller = new WD1770;

	cpu->RESET = reset;
	cia->reset = reset;
	disk_controller->reset = reset;

	cpu->ReadProcTbl = ReadProcTbl;
	cpu->WriteProcTbl = WriteProcTbl;
	cpu->History = History;
	cpu->HistoryPointer = &HistoryPointer;
	cpu->Breakpoints = Breakpoints;
	cpu->BreakStatus = &BreakStatus;
	cpu->BreakWerte = BreakWerte;
	HistoryPointer = 0;

	for(int i=0;i<256;i++)
	{
		ReadProcTbl[i] = std::bind(&Floppy1581::ReadNoMem, this, std::placeholders::_1);
		WriteProcTbl[i] = std::bind(&Floppy1581::WriteNoMem, this, std::placeholders::_1, std::placeholders::_2);
	}

	for(int i=0;i<32;i++)
	{
		ReadProcTbl[i] = std::bind(&Floppy1581::ReadRam, this, std::placeholders::_1);
		WriteProcTbl[i] = std::bind(&Floppy1581::WriteRam, this,std::placeholders::_1, std::placeholders::_2);
	}

	for(int i=0;i<128;i++)
	{
		 ReadProcTbl[i+0x80] = std::bind(&Floppy1581::ReadRom, this, std::placeholders::_1);
	}

	// MOS8520 0x4000 - 0x5FFF
	ReadProcTbl[0x40] = std::bind(&MOS8520::ReadIO, cia,std::placeholders::_1);
	WriteProcTbl[0x40] = std::bind(&MOS8520::WriteIO, cia,std::placeholders::_1, std::placeholders::_2);
	ReadProcTbl[0x50] = std::bind(&MOS8520::ReadIO, cia, std::placeholders::_1);
	WriteProcTbl[0x50] = std::bind(&MOS8520::WriteIO, cia, std::placeholders::_1, std::placeholders::_2);

	// MOS8520 0x6000 - 0x7FFF
	ReadProcTbl[0x60] = std::bind(&WD1770::ReadIO, disk_controller, std::placeholders::_1);
	WriteProcTbl[0x60] = std::bind(&WD1770::WriteIO, disk_controller, std::placeholders::_1, std::placeholders::_2);
	ReadProcTbl[0x70] = std::bind(&WD1770::ReadIO, disk_controller, std::placeholders::_1);
	WriteProcTbl[0x70] = std::bind(&WD1770::WriteIO, disk_controller, std::placeholders::_1, std::placeholders::_2);
}

Floppy1581::~Floppy1581()
{
	if(cpu != nullptr) delete cpu;
	if(cia != nullptr) delete cia;
	if(disk_controller != nullptr) delete disk_controller;
}

void Floppy1581::SetC64IEC(uint8_t *iec)
{

}

void Floppy1581::SetResetReady(bool *ResetReady, uint16_t ResetReadyAdr)
{
	cpu->ResetReady = ResetReady;
	cpu->ResetReadyAdr = ResetReadyAdr;
}

void Floppy1581::SetDeviceNumber(uint8_t number)
{
	if((number < 8) || (number > 11)) return;
	if(Jumper == (number-8)) return;

	Jumper = number - 8;

	cpu->Reset();
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
		CycleCounter = 0;

		cpu->Reset();

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

	CycleCounter++;

	// PHI1
	cia->OneZyklus();
	disk_controller->OneZyklus();

	if(CIA_IRQ == true) IRQ = true;
	else IRQ = false;

	cpu->Phi1();

	if(!*RESET)
	{

	}

	// PHI2
	bool ret = cpu->OneZyklus();
	if(BreakStatus != 0) *FoundBreakpoint = true;
	return ret;

	return true;
}

void Floppy1581::GetFloppyInfo(FLOPPY_1581_INFO *fi)
{
	fi->Power_Led = true;
	fi->Data_Led = true;
}

void Floppy1581::WriteNoMem(uint16_t /*address*/, uint8_t /*value*/)
{
}

uint8_t Floppy1581::ReadNoMem(uint16_t /*address*/)
{
	return 0x0;
}

void Floppy1581::WriteRam(uint16_t address, uint8_t value)
{
	RAM[address] = value;
}

uint8_t Floppy1581::ReadRam(uint16_t address)
{
	return RAM[address];
}

uint8_t Floppy1581::ReadRom(uint16_t address)
{
	return ROM[address-0x8000];
}
