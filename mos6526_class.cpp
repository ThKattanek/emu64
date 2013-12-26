//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: mos6526_class.cpp                     //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 27.12.2013                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "mos6526_class.h"

MOS6526::MOS6526(unsigned char cia_nr)
{
	CiaNr = cia_nr;

	C64IEC = NULL;
	FloppyIEC = NULL;

	FLAG_PIN = NULL;
	FLAG_PIN_OLD = 0;

	CNT_PIN = true; // Default True da am Flag nichts angeschlossen ist !

	RESET_OLD = false;
}

MOS6526::~MOS6526()
{
}

void MOS6526::Reset(void)
{
	for(int i=0;i<16;i++) IO[i] = 0;

	PALatch = PBLatch = DDRA = DDRB = 0;
	PA->SetOutput(PALatch | ~DDRA);
	PB->SetOutput(PBLatch | ~DDRB);

	TimerALatch = 1;
	TimerBLatch = 1;
	TimerA = 0xFFFF;
	TimerB = 0xFFFF;

	UhrLatched = false;
    UhrStopped = true;
	IO[0x0B] = 1;
	
	if(IO[14] & 0x80) UhrCounter = UhrCounterLatch = UhrCounterPAL; // PAL
	else UhrCounter = UhrCounterLatch = UhrCounterNTSC; // NTSC

	/////// Für REG 14 und 15 //////
	EnablePB6	= false;
	EnablePB7	= false;
	PB6Mode		= 0;
	PB7Mode		= 0;
	PB6			= 0;
	PB7			= 0;
	////////////////////////////////

	SDR	= ICR = CRA = CRB = IntMask = 0;
	
	TimerAStatus = TimerBStatus = STOP;
	TimerA_CNT_PHI2 = TimerB_CNT_PHI2 = TimerB_CNT_TimerA = TimerB_CNT_CNTPin = false;

	PrevLP = 0x10;

	return;
}

void MOS6526::OneZyklus(void)
{
	int t, pm;

	if((*RESET == true) && (RESET_OLD == false))
	{
		Reset();
	}
	RESET_OLD = *RESET;

	if(!PB6Mode) PB6 = 0;
	if(!PB7Mode) PB7 = 0;

	TimerCount();

	UhrCounter--;
	if (UhrCounter==0)
	{
		UhrCounter = UhrCounterLatch;
		if(!UhrStopped)
		{
			t = BCD2Byte(IO[8]);
			t++;
			IO[8] = Byte2BCD(t % 10);
			if(t >= 10)
			{
				t = BCD2Byte(IO[9]);
				t++;
				IO[9] = Byte2BCD(t % 60);
				if(t >= 60)
				{
					t = BCD2Byte(IO[10]);
					t++;
					IO[10] = Byte2BCD(t % 60);
					if(t >= 60)
					{
						pm = IO[11] & 0x80;
						t = IO[11] & 0x1F;
						if(t == 0x11) pm ^= 0x80;
						if(t == 0x12) t = 1;
						else if(++t == 10) t = 0x10;
						IO[11] = t | pm;
					}
				}
			}
			/// Auf Alarm rpüfen ///
			if(!memcmp(UhrAlarm,IO+8,4)) TriggerInterrupt(4);
		}
	}

	if(FLAG_PIN != NULL)
	{
		if(*FLAG_PIN != FLAG_PIN_OLD)
		{
			TriggerInterrupt(16);
		}
		FLAG_PIN_OLD = *FLAG_PIN;
	}
}

inline void MOS6526::TriggerInterrupt(int bit)
{
	ICR |= bit;
	if (IntMask & bit) 
	{
		ICR |= 0x80;
		if(CiaNr == 0) CpuTriggerInterrupt(CIA_IRQ);
		else CpuTriggerInterrupt(CIA_NMI);
	}
}

inline void MOS6526::CheckLP(void)
{
        if(((PBLatch | ~DDRB) & 0x10) != PrevLP) VicTriggerLP();
	PrevLP = (PBLatch | ~DDRB) & 0x10;
}

void MOS6526::GetRegister(CIA_STRUCT *cia_reg)
{
	cia_reg->CtrlA = IO[14];
	cia_reg->CtrlB = IO[15];
	cia_reg->DDRA = IO[2];
	cia_reg->DDRB = IO[3];
	cia_reg->PRA = IO[0];
	cia_reg->PRB = IO[1];
	cia_reg->IntData = ICR;
	cia_reg->IntMask = IntMask;
	cia_reg->SDR = IO[12];
	cia_reg->TimerA = TimerA;
	cia_reg->TimerB = TimerB;
	cia_reg->TimerALatch = TimerALatch;
	cia_reg->TimerBLatch = TimerBLatch;
	cia_reg->Tod10 = BCD2Byte(IO[8]);
	cia_reg->TodSec = BCD2Byte(IO[9]);
	cia_reg->TodMin = BCD2Byte(IO[10]);
	cia_reg->TodHr = BCD2Byte(IO[11]);
}

/*
#define frz_write(var) fwrite(&var,1,sizeof(var),File)
bool MOS6526::SaveFreez(FILE *File)
{
	frz_write(RESET_OLD);
	frz_write(PBLatch);
	frz_write(SDR);
	frz_write(FLAG_PIN_OLD);
	fwrite(IO,1,16,File);
	frz_write(IntMask);
	frz_write(CRA);
	frz_write(CRB);
	frz_write(ICR);
	frz_write(NEW_CRA);
	frz_write(NEW_CRB);
	frz_write(WRITE_NEW_CRA);
	frz_write(WRITE_NEW_CRB);
	frz_write(TimerA_CNT_PHI2);
	frz_write(TimerB_CNT_PHI2);
	frz_write(TimerB_CNT_TimerA);
	frz_write(TimerA_IRQ_NEXT_CYCLE);
	frz_write(TimerB_IRQ_NEXT_CYCLE);
	frz_write(TimerAStatus);
	frz_write(TimerBStatus);
	frz_write(TimerA);
	frz_write(TimerALatch);
	frz_write(TimerB);
	frz_write(TimerBLatch);
	frz_write(DDRA);
	frz_write(DDRB);
	frz_write(EnablePB6);
	frz_write(EnablePB7);
	frz_write(PB6Mode);
	frz_write(PB7Mode);
	frz_write(PB6);
	frz_write(PB7);
	frz_write(InMode);
	frz_write(TimerANull);
	return true;
}

#define frz_read(var) fread(&var,1,sizeof(var),File)
bool MOS6526::LoadFreez(FILE *File,unsigned short Version)
{
	unsigned short StartAdresse;
	
	switch(Version)
	{
	case 0x0100:
	case 0x0101:
		frz_read(RESET_OLD);
		frz_read(PBLatch);
		frz_read(SDR);
		frz_read(FLAG_PIN_OLD);
		fwrite(IO,1,16,File);
		frz_read(IntMask);
		frz_read(CRA);
		frz_read(CRB);
		frz_read(ICR);
		frz_read(NEW_CRA);
		frz_read(NEW_CRB);
		frz_read(WRITE_NEW_CRA);
		frz_read(WRITE_NEW_CRB);
		frz_read(TimerA_CNT_PHI2);
		frz_read(TimerB_CNT_PHI2);
		frz_read(TimerB_CNT_TimerA);
		frz_read(TimerA_IRQ_NEXT_CYCLE);
		frz_read(TimerB_IRQ_NEXT_CYCLE);
		frz_read(TimerAStatus);
		frz_read(TimerBStatus);
		frz_read(TimerA);
		frz_read(TimerALatch);
		frz_read(TimerB);
		frz_read(TimerBLatch);
		frz_read(DDRA);
		frz_read(DDRB);
		frz_read(EnablePB6);
		frz_read(EnablePB7);
		frz_read(PB6Mode);
		frz_read(PB7Mode);
		frz_read(PB6);
		frz_read(PB7);
		frz_read(InMode);
		frz_read(TimerANull);
		break;
	}
	return true;
}
*/

inline void MOS6526::TimerCount(void)
{
	bool TimerAUnterlauf = false;

	/// TimerA Status ///
	switch (TimerAStatus) 
	{
		case WAIT_THEN_COUNT:
			TimerAStatus = COUNT;
		case STOP:
			goto TimerA_Idle;
		case LOAD_THEN_STOP:
			TimerAStatus = STOP;
			TimerA = TimerALatch;
			goto TimerA_Idle;
		case LOAD_THEN_COUNT:
			TimerAStatus = COUNT;
			TimerA = TimerALatch;
			goto TimerA_Idle;
		case LOAD_THEN_WAIT_THEN_COUNT:
			TimerAStatus = WAIT_THEN_COUNT;
			if (TimerA == 0) goto TimerA_Interrupt;
			else 
			{
				TimerA = TimerALatch;
				goto TimerA_Idle;
			}
		case COUNT:
			goto TimerA_Count;
		case COUNT_THEN_STOP:
			TimerAStatus = STOP;
			goto TimerA_Count;
	}

	/// Timer A runterzählen ///
TimerA_Count:
	if (TimerA_CNT_PHI2)
		if (!TimerA || !--TimerA) 
		{
			if (TimerAStatus != STOP) 
			{
TimerA_Interrupt:
				TimerA = TimerALatch;
                TriggerInterrupt(1);
				ICR |= 1;
				if (CRA & 8) 
				{
					CRA &= 0xFE;
					NEW_CRA &= 0xFE;
					TimerAStatus = LOAD_THEN_STOP;
				} 
				else TimerAStatus = LOAD_THEN_COUNT;
				if(!PB6Mode) PB6 = 1;
				else PB6 = ~PB6;
			}
			TimerAUnterlauf = true;
		}

	// Delayed write to CRA?
TimerA_Idle:
	if (WRITE_NEW_CRA) 
	{
		switch (TimerAStatus) 
		{
			case STOP:
			case LOAD_THEN_STOP:
				if (NEW_CRA & 1) 
				{
					if (NEW_CRA & 0x10) TimerAStatus = LOAD_THEN_WAIT_THEN_COUNT;
					else TimerAStatus = WAIT_THEN_COUNT;
				} 
				else 
				{
					if (NEW_CRA & 0x10) TimerAStatus = LOAD_THEN_STOP;
				}
				break;
			case COUNT:
				if (NEW_CRA & 1) 
				{
					if (NEW_CRA & 0x10) TimerAStatus = LOAD_THEN_WAIT_THEN_COUNT;
				}
				else
				{
					if (NEW_CRA & 0x10) TimerAStatus = LOAD_THEN_STOP;
					else TimerAStatus = COUNT_THEN_STOP;
				}
				break;
			case LOAD_THEN_COUNT:
			case WAIT_THEN_COUNT:
				if (NEW_CRA & 1) 
				{
					if (NEW_CRA & 8) 
					{
						NEW_CRA &= 0xFE;
						TimerAStatus = STOP;
					}
					else if (NEW_CRA & 0x10) TimerAStatus = LOAD_THEN_WAIT_THEN_COUNT;
					if(!PB6Mode) PB6 = 1;
					else PB6 = ~PB6;
				} else TimerAStatus = STOP;
				break;
		}
		CRA = NEW_CRA & 0xEF;
		WRITE_NEW_CRA = false;
	}

	/// TimerB Status ///
	switch (TimerBStatus) 
	{
		case WAIT_THEN_COUNT:
			TimerBStatus = COUNT;
		case STOP:
			goto TimerB_Idle;
		case LOAD_THEN_STOP:
			TimerBStatus = STOP;
			TimerB = TimerBLatch;
			goto TimerB_Idle;
		case LOAD_THEN_COUNT:
			TimerBStatus = COUNT;
			TimerB = TimerBLatch;
			goto TimerB_Idle;					//?????? Es Stand TimerA_Idle
		case LOAD_THEN_WAIT_THEN_COUNT:
			TimerBStatus = WAIT_THEN_COUNT;
			if (TimerB == 0)
				goto TimerB_Interrupt;
			else 
			{
				TimerB = TimerBLatch;
				goto TimerB_Idle;
			}
		case COUNT:
			goto TimerB_Count;
		case COUNT_THEN_STOP:
			TimerBStatus = STOP;
			goto TimerB_Count;
	}

	/// Timer B runterzählen ///
TimerB_Count:
	if (TimerB_CNT_PHI2 || (TimerB_CNT_TimerA && TimerAUnterlauf) || (TimerB_CNT_CNTPin && TimerAUnterlauf && CNT_PIN))
		if (!TimerB || !--TimerB) 
		{
			if (TimerBStatus != STOP) 
			{
TimerB_Interrupt:
				TimerB = TimerBLatch;
                TriggerInterrupt(2);
				ICR |= 2;
				if (CRB & 8) 
				{
					CRB &= 0xFE;
					NEW_CRB &= 0xFE;
					TimerBStatus = LOAD_THEN_STOP;
				}
				else TimerBStatus = LOAD_THEN_COUNT;
				if(!PB7Mode) PB7 = 1;
				else PB7 = ~PB7;
			}
		}

	// Delayed write to CRB?
TimerB_Idle:
	if (WRITE_NEW_CRB) 
	{
		switch (TimerBStatus)
		{
			case STOP:
			case LOAD_THEN_STOP:
				if (NEW_CRB & 1) 
				{
					if (NEW_CRB & 0x10) TimerBStatus = LOAD_THEN_WAIT_THEN_COUNT;
					else TimerBStatus = WAIT_THEN_COUNT;
				}
				else 
				{
					if (NEW_CRB & 0x10) TimerBStatus = LOAD_THEN_STOP;
				}
				break;
			case COUNT:
				if (NEW_CRB & 1) 
				{
					if (NEW_CRB & 0x10) TimerBStatus = LOAD_THEN_WAIT_THEN_COUNT;
				}
				else 
				{
					if (NEW_CRB & 0x10) TimerBStatus = LOAD_THEN_STOP;
					else TimerBStatus = COUNT_THEN_STOP;
				}
				break;
			case LOAD_THEN_COUNT:
			case WAIT_THEN_COUNT:
				if (NEW_CRB & 1) 
				{
					if (NEW_CRB & 8) 
					{
						NEW_CRB &= 0xFE;
						TimerBStatus = STOP;
					}
					else if (NEW_CRB & 0x10) TimerBStatus = LOAD_THEN_WAIT_THEN_COUNT;
					if(!PB7Mode) PB7 = 1;
					else PB7 = ~PB7;
				}
				else TimerBStatus = STOP;
				break;
		}
		CRB = NEW_CRB & 0xEF;
		WRITE_NEW_CRB = false;
	}
}

void MOS6526::WriteIO(unsigned short adresse,unsigned char wert)
{
	adresse &= 0x0F;
	
        switch(adresse)
	{
	case 0:
                IO[adresse]=wert;
                PALatch = wert;
                PA->SetOutput(PALatch | ~DDRA);

                if(CiaNr == 1)	/// Nur bei CIA2
		{
                        wert = ~PALatch & DDRA;
                        //unsigned char  OLD_LINES = *C64IEC;
                        *C64IEC = ((wert << 2) & 0x80)          // DATA
                                | ((wert << 2) & 0x40)          // CLK
                                | ((wert << 1) & 0x10);         // ATN
		}
		break;

	case 1:
                IO[adresse]=wert;
                PBLatch = wert;
                PB->SetOutput(PBLatch | ~DDRB);
                if(CiaNr == 0)CheckLP();
		break;

	case 2:
                IO[adresse]=wert;
                DDRA = wert;
                PA->SetOutput(PALatch | ~DDRA);
		break;

	case 3:
                IO[adresse]=wert;
                DDRB = wert;
                PB->SetOutput(PBLatch | ~DDRB);
                if(CiaNr == 0)CheckLP();
		break;

	case 4:
                TimerALatch = (TimerALatch & 0xFF00) | wert;
		break;
	case 5:
                TimerALatch = (TimerALatch & 0x00FF) | (wert << 8);
                if(!(CRA & 1)) TimerA = TimerALatch;
		break;

	case 6:
                TimerBLatch = (TimerBLatch & 0xFF00) | wert;
		break;
	case 7:
                TimerBLatch = (TimerBLatch & 0x00FF) | (wert << 8);
                if(!(CRB & 1)) TimerB = TimerBLatch;
		break;

	case 8: case 9: case 10: case 11:
		if((adresse) == 11)
		{
			wert &= 0x9F;
                        if((wert & 0x1F) == 0x12 && !(CRB & 0x80)) wert ^= 0x80;
		}
                if(CRB & 0x80)
		{
                        UhrAlarm[adresse - 8] = wert;
		}
		else
		{
                        if(adresse == 8)  UhrStopped = false;
                        if(adresse == 11) UhrStopped = true;
                        IO[adresse]=wert;
		}
		break;

	case 12:
                IO[adresse]=wert;
                SDR = wert;
                if((CRA & 0x40))
		{
                        TriggerInterrupt(8);	// Fake SDR interrupt for programs that need it
		}
		break;

	case 13:
                IO[adresse] = wert;
		if ((wert&128)==128)
		{
                        IntMask |= wert;
		}
		else
		{
                        IntMask &= ~wert;
		}
		if (wert & 0x80)
		{
                        IntMask |= wert & 0x7f;
		}
                else IntMask &= ~wert;

                        if (ICR & IntMask & 0x1f)
			{ // Trigger IRQ if pending
                                ICR |= 0x80;
                                if(CiaNr == 0) CpuTriggerInterrupt(CIA_IRQ);
                                else CpuTriggerInterrupt(CIA_NMI);
			}
		break;

	case 14:
                IO[adresse] = wert;
                WRITE_NEW_CRA = true;
                NEW_CRA = wert;
                TimerA_CNT_PHI2 = ((wert & 0x20) == 0x00);
                if (wert & 0x80) UhrCounter = UhrCounterLatch = UhrCounterPAL; // PAL
                else UhrCounter = UhrCounterLatch = UhrCounterNTSC; // NTSC
                EnablePB6 = (wert & 0x02) >> 1;
                PB6Mode = (wert & 0x04) >> 2;
		break;

	case 15:
                IO[adresse] = wert;
                WRITE_NEW_CRB = true;
                NEW_CRB = wert;
                TimerB_CNT_PHI2 = ((wert & 0x60) == 0x00);
                TimerB_CNT_TimerA = ((wert & 0x60) == 0x40);
                TimerB_CNT_CNTPin = ((wert & 0x60) == 0x60);
                EnablePB7 = (wert & 0x02) >> 1;
                PB7Mode = (wert & 0x04) >> 2;
		break;
	}
	return;
}

unsigned char temp;
unsigned char MOS6526::ReadIO(unsigned short adresse)
{
	unsigned char ret;

	adresse &= 0x0F;

	switch(adresse)
	{
	case 0:
                if(CiaNr == 1)	/// Nur bei CIA2
		{
                        return ((PALatch | ~DDRA) & 0x3F) | (*C64IEC & *FloppyIEC);
		}
                return (0xFF & (PALatch | (~(DDRA))) & PA->GetInput());
		break;

	case 1:
                ret = (0xFF & (PBLatch | ~(DDRB))) & PB->GetInput();
		
		// Unterlauf Timer A an PIN PB6 anzeigen //
                if(EnablePB6)
		{
			ret &= 0xBF;
                        ret |= (PB6 & 0x01) << 6;
		}

		// Unterlauf Timer B an PIN PB7 anzeigen //
                if(EnablePB7)
		{
			ret &= 0x7F;
                        ret |= (PB7 & 0x01) << 7;
		}
		return ret;
		break;

	case 2:
                return DDRA;
		break;

	case 3:
                return DDRB;
		break;

	case 4:
                return (unsigned char)TimerA;
		break;

	case 5:
                return (unsigned char)(TimerA>>8);
		break;

	case 6:
                return (unsigned char)TimerB;
		break;

	case 7:
                return (unsigned char)(TimerB>>8);
		break;

	case 8: case 9: case 10: case 11:
		/// Wenn UhrLatched = false --> Uhrzeit ins Latch schieben ///
                if(UhrLatched == false) memcpy(UhrLatch,IO+8,4);
                if(adresse == 8)  UhrLatched = false;
                if(adresse == 11) UhrLatched = true;
                return UhrLatch[adresse - 8];
		break;

	case 12:
                return SDR;
		break;

	case 13:
                temp = ICR;
                ICR = 0;
                if(CiaNr == 0) CpuClearInterrupt(CIA_IRQ);
                else CpuClearInterrupt(CIA_NMI);
		return temp;
		break;

	case 14:
                return CRA;
		break;

	case 15:
                return CRB;
		break;

	default:
                return IO[adresse];
		break;
	}
}

void MOS6526::SetSDR(unsigned char wert)
{
        if(!(IO[14] & 0x40))
	{
                SDR = wert;
		TriggerInterrupt(8);
	}
}
