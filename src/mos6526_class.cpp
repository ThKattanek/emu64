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
// Letzte Änderung am 22.09.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "mos6526_class.h"

MOS6526::MOS6526(uint8_t cia_nr)
{
    this->cia_nr = cia_nr;

    c64_iec_wire = nullptr;
    floppy_iec_wire = nullptr;

    flag_pin = nullptr;
    flag_pin_old = 0;

    cnt_pin = true; // Default True da am Flag nichts angeschlossen ist !

    reset_wire_old = false;
}

MOS6526::~MOS6526()
{
}

void MOS6526::Reset()
{
    for(int i=0;i<16;i++) io[i] = 0;

    pa_latch = pb_latch = ddr_a = ddr_b = 0;
    pa->SetOutput(pa_latch | ~ddr_a);
    pb->SetOutput(pb_latch | ~ddr_b);

    timer_a_latch = 1;
    timer_b_latch = 1;
    timer_a = 0xFFFF;
    timer_b = 0xFFFF;

    clock_is_latched = false;
    clock_is_stopped = true;
    io[0x0B] = 1;
	
    if(io[14] & 0x80) clock_counter = clock_counter_latch = ClockCounterPAL; // PAL
    else clock_counter = clock_counter_latch = ClockCounterNTSC; // NTSC

	/////// Für REG 14 und 15 //////
    enable_pb6	= false;
    enable_pb7	= false;
    pb6_mode    = 0;
    pb7_mode	= 0;
    pb_6		= 0;
    pb_7		= 0;
	////////////////////////////////

    sdr	= icr = cra = crb = interupt_mask = 0;
	
    timer_a_status = timer_b_status = STOP;
    timer_a_cnt_phi2 = timer_b_cnt_phi2 = timer_b_cnt_timer_a = timer_b_cnt_cnt_pin = false;

    prev_lp = 0x10;

	return;
}

void MOS6526::OneZyklus()
{
    uint8_t t, pm;

    if((*reset_wire == true) && (reset_wire_old == false))
	{
		Reset();
	}
    reset_wire_old = *reset_wire;

    if(!pb6_mode) pb_6 = 0;
    if(!pb7_mode) pb_7 = 0;

	TimerCount();

    clock_counter--;
    if (clock_counter==0)
	{
        clock_counter = clock_counter_latch;
        if(!clock_is_stopped)
		{
            t = BCD2Byte(io[8]);
			t++;
            io[8] = Byte2BCD(t % 10);
			if(t >= 10)
			{
                t = BCD2Byte(io[9]);
				t++;
                io[9] = Byte2BCD(t % 60);
				if(t >= 60)
				{
                    t = BCD2Byte(io[10]);
					t++;
                    io[10] = Byte2BCD(t % 60);
					if(t >= 60)
					{
                        pm = io[11] & 0x80;
                        t = io[11] & 0x1F;
						if(t == 0x11) pm ^= 0x80;
						if(t == 0x12) t = 1;
						else if(++t == 10) t = 0x10;
                        io[11] = t | pm;
					}
				}
			}
			/// Auf Alarm rpüfen ///
            if(!memcmp(clock_alarm,io+8,4)) TriggerInterrupt(4);
		}
	}

    if(flag_pin != nullptr)
	{
        if(*flag_pin != flag_pin_old)
		{
			TriggerInterrupt(16);
		}
        flag_pin_old = *flag_pin;
	}
}

inline void MOS6526::TriggerInterrupt(int bit)
{
    icr |= bit;
    if (interupt_mask & bit)
	{
        icr |= 0x80;
        if(cia_nr == 0) CpuTriggerInterrupt(CIA_IRQ);
		else CpuTriggerInterrupt(CIA_NMI);
	}
}

inline void MOS6526::CheckLP()
{
    if(((pb_latch | ~ddr_b) & 0x10) != prev_lp) VicTriggerLP();
    prev_lp = (pb_latch | ~ddr_b) & 0x10;
}

void MOS6526::GetRegister(CIA_STRUCT *cia_reg)
{
    cia_reg->CtrlA = io[14];
    cia_reg->CtrlB = io[15];
    cia_reg->DDRA = io[2];
    cia_reg->DDRB = io[3];
    cia_reg->PRA = io[0];
    cia_reg->PRB = io[1];
    cia_reg->IntData = icr;
    cia_reg->IntMask = interupt_mask;
    cia_reg->SDR = io[12];
    cia_reg->TimerA = timer_a;
    cia_reg->TimerB = timer_b;
    cia_reg->TimerALatch = timer_a_latch;
    cia_reg->TimerBLatch = timer_b_latch;
    cia_reg->Tod10 = BCD2Byte(io[8]);
    cia_reg->TodSec = BCD2Byte(io[9]);
    cia_reg->TodMin = BCD2Byte(io[10]);
    cia_reg->TodHr = BCD2Byte(io[11]);
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

inline void MOS6526::TimerCount()
{
    bool timer_a_underflow = false;

	/// TimerA Status ///
    switch (timer_a_status)
	{
        case WAIT_THEN_COUNT:
            timer_a_status = COUNT;
        [[clang::fallthrough]]; case STOP:
            goto TimerA_Idle;
		case LOAD_THEN_STOP:
            timer_a_status = STOP;
            timer_a = timer_a_latch;
			goto TimerA_Idle;
		case LOAD_THEN_COUNT:
            timer_a_status = COUNT;
            timer_a = timer_a_latch;
			goto TimerA_Idle;
		case LOAD_THEN_WAIT_THEN_COUNT:
            timer_a_status = WAIT_THEN_COUNT;
            if (timer_a == 0) goto TimerA_Interrupt;
			else 
			{
                timer_a = timer_a_latch;
				goto TimerA_Idle;
			}
		case COUNT:
			goto TimerA_Count;
		case COUNT_THEN_STOP:
            timer_a_status = STOP;
			goto TimerA_Count;
	}

	/// Timer A runterzählen ///
TimerA_Count:
    if (timer_a_cnt_phi2)
        if (!timer_a || !--timer_a)
		{
            if (timer_a_status != STOP)
			{
TimerA_Interrupt:
                timer_a = timer_a_latch;
                TriggerInterrupt(1);
                icr |= 1;
                if (cra & 8)
				{
                    cra &= 0xFE;
                    new_cra &= 0xFE;
                    timer_a_status = LOAD_THEN_STOP;
				} 
                else timer_a_status = LOAD_THEN_COUNT;
                if(!pb6_mode) pb_6 = 1;
                else pb_6 = ~pb_6;
			}
            timer_a_underflow = true;
		}

	// Delayed write to CRA?
TimerA_Idle:
    if (writing_new_cra)
	{
        switch (timer_a_status)
		{
			case STOP:
			case LOAD_THEN_STOP:
                if (new_cra & 1)
				{
                    if (new_cra & 0x10) timer_a_status = LOAD_THEN_WAIT_THEN_COUNT;
                    else timer_a_status = WAIT_THEN_COUNT;
				} 
				else 
				{
                    if (new_cra & 0x10) timer_a_status = LOAD_THEN_STOP;
				}
				break;
			case COUNT:
                if (new_cra & 1)
				{
                    if (new_cra & 0x10) timer_a_status = LOAD_THEN_WAIT_THEN_COUNT;
				}
				else
				{
                    if (new_cra & 0x10) timer_a_status = LOAD_THEN_STOP;
                    else timer_a_status = COUNT_THEN_STOP;
				}
				break;
			case LOAD_THEN_COUNT:
			case WAIT_THEN_COUNT:
                if (new_cra & 1)
				{
                    if (new_cra & 8)
					{
                        new_cra &= 0xFE;
                        timer_a_status = STOP;
					}
                    else if (new_cra & 0x10) timer_a_status = LOAD_THEN_WAIT_THEN_COUNT;
                    if(!pb6_mode) pb_6 = 1;
                    else pb_6 = ~pb_6;
                } else timer_a_status = STOP;
				break;
		}
        cra = new_cra & 0xEF;
        writing_new_cra = false;
	}

	/// TimerB Status ///
    switch (timer_b_status)
	{
        case WAIT_THEN_COUNT:
            timer_b_status = COUNT;
        [[clang::fallthrough]]; case STOP:
            goto TimerB_Idle;
		case LOAD_THEN_STOP:
            timer_b_status = STOP;
            timer_b = timer_b_latch;
			goto TimerB_Idle;
		case LOAD_THEN_COUNT:
            timer_b_status = COUNT;
            timer_b = timer_b_latch;
			goto TimerB_Idle;					//?????? Es Stand TimerA_Idle
		case LOAD_THEN_WAIT_THEN_COUNT:
            timer_b_status = WAIT_THEN_COUNT;
            if (timer_b == 0)
				goto TimerB_Interrupt;
			else 
			{
                timer_b = timer_b_latch;
				goto TimerB_Idle;
			}
		case COUNT:
			goto TimerB_Count;
		case COUNT_THEN_STOP:
            timer_b_status = STOP;
			goto TimerB_Count;
	}

	/// Timer B runterzählen ///
TimerB_Count:
    if (timer_b_cnt_phi2 || (timer_b_cnt_timer_a && timer_a_underflow) || (timer_b_cnt_cnt_pin && timer_a_underflow && cnt_pin))
        if (!timer_b || !--timer_b)
		{
            if (timer_b_status != STOP)
			{
TimerB_Interrupt:
                timer_b = timer_b_latch;
                TriggerInterrupt(2);
                icr |= 2;
                if (crb & 8)
				{
                    crb &= 0xFE;
                    new_crb &= 0xFE;
                    timer_b_status = LOAD_THEN_STOP;
				}
                else timer_b_status = LOAD_THEN_COUNT;
                if(!pb7_mode) pb_7 = 1;
                else pb_7 = ~pb_7;
			}
		}

	// Delayed write to CRB?
TimerB_Idle:
    if (writing_new_crb)
	{
        switch (timer_b_status)
		{
			case STOP:
			case LOAD_THEN_STOP:
                if (new_crb & 1)
				{
                    if (new_crb & 0x10) timer_b_status = LOAD_THEN_WAIT_THEN_COUNT;
                    else timer_b_status = WAIT_THEN_COUNT;
				}
				else 
				{
                    if (new_crb & 0x10) timer_b_status = LOAD_THEN_STOP;
				}
				break;
			case COUNT:
                if (new_crb & 1)
				{
                    if (new_crb & 0x10) timer_b_status = LOAD_THEN_WAIT_THEN_COUNT;
				}
				else 
				{
                    if (new_crb & 0x10) timer_b_status = LOAD_THEN_STOP;
                    else timer_b_status = COUNT_THEN_STOP;
				}
				break;
			case LOAD_THEN_COUNT:
			case WAIT_THEN_COUNT:
                if (new_crb & 1)
				{
                    if (new_crb & 8)
					{
                        new_crb &= 0xFE;
                        timer_b_status = STOP;
					}
                    else if (new_crb & 0x10) timer_b_status = LOAD_THEN_WAIT_THEN_COUNT;
                    if(!pb7_mode) pb_7 = 1;
                    else pb_7 = ~pb_7;
				}
                else timer_b_status = STOP;
				break;
		}
        crb = new_crb & 0xEF;
        writing_new_crb = false;
	}
}

void MOS6526::WriteIO(uint16_t address, uint8_t value)
{
    address &= 0x0F;
	
    switch(address)
	{
	case 0:
                io[address]=value;
                pa_latch = value;
                pa->SetOutput(pa_latch | ~ddr_a);

                if(cia_nr == 0)  /// Nur bei CIA1
                    ChangePOTSwitch();

                if(cia_nr == 1)	/// Nur bei CIA2
                {
                    value = ~pa_latch & ddr_a;
                    //unsigned char  OLD_LINES = *C64IEC;
                    *c64_iec_wire = ((value << 2) & 0x80)          // DATA
                            | ((value << 2) & 0x40)          // CLK
                            | ((value << 1) & 0x10);         // ATN
                }
		break;

	case 1:
                io[address]=value;
                pb_latch = value;
                pb->SetOutput(pb_latch | ~ddr_b);
                if(cia_nr == 0)CheckLP();
		break;

	case 2:
                io[address]=value;
                ddr_a = value;
                pa->SetOutput(pa_latch | ~ddr_a);

                if(cia_nr == 0)  /// Nur bei CIA1
                    ChangePOTSwitch();
		break;

	case 3:
                io[address]=value;
                ddr_b = value;
                pb->SetOutput(pb_latch | ~ddr_b);
                if(cia_nr == 0)CheckLP();
		break;

	case 4:
                timer_a_latch = (timer_a_latch & 0xFF00) | value;
		break;
	case 5:
                timer_a_latch = static_cast<uint16_t>((timer_a_latch & 0x00FF) | (value << 8));
                if(!(cra & 1)) timer_a = timer_a_latch;
		break;

	case 6:
                timer_b_latch = (timer_b_latch & 0xFF00) | value;
		break;
	case 7:
                timer_b_latch = static_cast<uint16_t>((timer_b_latch & 0x00FF) | (value << 8));
                if(!(crb & 1)) timer_b = timer_b_latch;
		break;

	case 8: case 9: case 10: case 11:
        if((address) == 11)
		{
            value &= 0x9F;
                        if((value & 0x1F) == 0x12 && !(crb & 0x80)) value ^= 0x80;
		}
                if(crb & 0x80)
		{
                        clock_alarm[address - 8] = value;
		}
		else
		{
                        if(address == 8)  clock_is_stopped = false;
                        if(address == 11) clock_is_stopped = true;
                        io[address]=value;
		}
		break;

	case 12:
                io[address]=value;
                sdr = value;
                if((cra & 0x40))
		{
                TriggerInterrupt(8);	// Fake SDR interrupt for programs that need it
		}
		break;

	case 13:
                io[address] = value;
        if ((value&128)==128)
		{
                        interupt_mask |= value;
		}
		else
		{
                        interupt_mask &= ~value;
		}
        if (value & 0x80)
		{
                        interupt_mask |= value & 0x7f;
		}
                else interupt_mask &= ~value;

                        if (icr & interupt_mask & 0x1f)
			{ // Trigger IRQ if pending
                                icr |= 0x80;
                                if(cia_nr == 0) CpuTriggerInterrupt(CIA_IRQ);
                                else CpuTriggerInterrupt(CIA_NMI);
			}
		break;

	case 14:
                io[address] = value;
                writing_new_cra = true;
                new_cra = value;
                timer_a_cnt_phi2 = ((value & 0x20) == 0x00);
                if (value & 0x80) clock_counter = clock_counter_latch = ClockCounterPAL; // PAL
                else clock_counter = clock_counter_latch = ClockCounterNTSC; // NTSC
                enable_pb6 = (value & 0x02) >> 1;
                pb6_mode = (value & 0x04) >> 2;
		break;

	case 15:
                io[address] = value;
                writing_new_crb = true;
                new_crb = value;
                timer_b_cnt_phi2 = ((value & 0x60) == 0x00);
                timer_b_cnt_timer_a = ((value & 0x60) == 0x40);
                timer_b_cnt_cnt_pin = ((value & 0x60) == 0x60);
                enable_pb7 = (value & 0x02) >> 1;
                pb7_mode = (value & 0x04) >> 2;
		break;
	}
	return;
}

static unsigned char temp;
uint8_t MOS6526::ReadIO(uint16_t address)
{
	unsigned char ret;

    address &= 0x0F;

    switch(address)
	{
	case 0:
        if(cia_nr == 1)	/// Nur bei CIA2
		{
                return ((pa_latch | ~ddr_a) & 0x3F) | (*c64_iec_wire & *floppy_iec_wire);
		}
                return (0xFF & (pa_latch | ~(ddr_a)) & pa->GetInput());
	case 1:
                ret = (0xFF & (pb_latch | ~(ddr_b))) & pb->GetInput();
		
		// Unterlauf Timer A an PIN PB6 anzeigen //
                if(enable_pb6)
		{
			ret &= 0xBF;
            ret |= (pb_6 & 0x01) << 6;
		}

		// Unterlauf Timer B an PIN PB7 anzeigen //
                if(enable_pb7)
		{
			ret &= 0x7F;
            ret |= (pb_7 & 0x01) << 7;
		}
		return ret;

	case 2:
        return ddr_a;

	case 3:
        return ddr_b;

	case 4:
        return static_cast<uint8_t>(timer_a);

	case 5:
        return static_cast<uint8_t>(timer_a>>8);

	case 6:
        return static_cast<uint8_t>(timer_b);

	case 7:
        return static_cast<uint8_t>(timer_b>>8);

	case 8: case 9: case 10: case 11:
		/// Wenn UhrLatched = false --> Uhrzeit ins Latch schieben ///
        if(!clock_is_latched) memcpy(clock_latch,io+8,4);
        if(address == 8)  clock_is_latched = false;
        if(address == 11) clock_is_latched = true;
        return clock_latch[address - 8];

	case 12:
        return sdr;

	case 13:
        temp = icr;
        icr = 0;
        if(cia_nr == 0) CpuClearInterrupt(CIA_IRQ);
        else CpuClearInterrupt(CIA_NMI);
		return temp;

	case 14:
        return cra;

	case 15:
        return crb;

	default:
        return io[address];
	}
}

void MOS6526::SetSDR(uint8_t value)
{
    if(!(io[14] & 0x40))
	{
                sdr = value;
		TriggerInterrupt(8);
	}
}
