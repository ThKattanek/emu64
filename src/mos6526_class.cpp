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
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include <cstring>
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

    TimerReset(&timer_a);
    TimerReset(&timer_b);

    clock_is_latched = false;
    clock_is_stopped = true;
    io[0x0B] = 1;

    if(io[14] & 0x80)
        clock_counter = clock_counter_latch = ClockCounterPAL; // PAL
    else
        clock_counter = clock_counter_latch = ClockCounterNTSC; // NTSC

    /////// Für REG 14 und 15 //////
    enable_pb6	= false;
    enable_pb7	= false;
    pb6_mode    = 0;
    pb7_mode	= 0;
    pb_6		= 0;
    pb_7		= 0;
    ////////////////////////////////

    sdr	= icr = cra = crb = interupt_mask = 0;

    icr_new = 0;
    icr_ack = 0;
    irq_line = false;
    ifr_done = false;
    ifr_ack1 = ifr_ack0 = false;
    ifr_d7set1 = ifr_d7set0 = false;
    ifr_raise1 = ifr_raise0 = false;
    ifr_read0 = ifr_read1 = false;

    timer_b_mode = 0;
    timer_a_underflow_old = false;
    timer_b_underflow_old = false;
    cnt_pin_old = false;

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

    // Interrupt Pipeline fuer diesen Takt ausfuehren (falls das nicht schon durch einen Zugriff auf das ICR geschehen ist)
    if(!ifr_done) IfrRun();
    ifr_done = false;

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
            if(!memcmp(clock_alarm,io+8,4))
                TriggerInterrupt(4);
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

// Interrupt Flag setzen (ein evtl. noch ausstehendes Loeschen dieses Flags wird verworfen)
inline void MOS6526::TriggerInterrupt(int bit)
{
    icr |= bit;
    icr_new |= bit;
    icr_ack &= ~bit;
}

// Interrupt Ausgang setzen (IRQ bei CIA1, NMI bei CIA2)
inline void MOS6526::SetIrqLine(bool state)
{
    if(irq_line == state)
        return;

    irq_line = state;

    if(state)
    {
        if(cia_nr == 0)
            CpuTriggerInterrupt(CIA_IRQ);
        else
            CpuTriggerInterrupt(CIA_NMI);
    }
    else
    {
        if(cia_nr == 0)
            CpuClearInterrupt(CIA_IRQ);
        else
            CpuClearInterrupt(CIA_NMI);
    }
}

// Interrupt Pipeline einen Takt weiter schalten (muss pro Takt genau einmal aufgerufen werden)
inline void MOS6526::IfrRun()
{
    // Das Lesen vom ICR loescht die Flags erst zwei Takte spaeter
    if(ifr_ack0)
    {
        icr &= ~icr_ack;
        icr_ack = 0;
    }

    // Ein neues Flag, das freigegeben ist, hebt den Interrupt an. Wurde das ICR im vorigen Takt gelesen, dann einen Takt spaeter.
    if(icr_new & interupt_mask & 0x1F)
    {
        if(ifr_read0)
        {
            ifr_raise1 = true;
            ifr_d7set1 = true;
        }
        else
        {
            ifr_raise0 = true;
            ifr_d7set0 = true;
        }
    }

    if(ifr_d7set0) icr |= 0x80;
    if(ifr_raise0) SetIrqLine(true);

    icr_new = 0;

    // Alle Stufen einen Takt weiter schieben
    ifr_ack0 = ifr_ack1;
    ifr_ack1 = false;
    ifr_d7set0 = ifr_d7set1;
    ifr_d7set1 = false;
    ifr_raise0 = ifr_raise1;
    ifr_raise1 = false;
    ifr_read1 = ifr_read0;
    ifr_read0 = false;
}

inline void MOS6526::UpdateIecWire()
{
    uint8_t value = ~pa_latch & ddr_a;
    *c64_iec_wire = ((value << 2) & 0x80)    // DATA
                    | ((value << 2) & 0x40)  // CLK
                    | ((value << 1) & 0x10); // ATN
}

inline void MOS6526::CheckLP()
{
    if(((pb_latch | ~ddr_b) & 0x10) != prev_lp)
        VicTriggerLP();

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
    cia_reg->TimerA = timer_a.counter;
    cia_reg->TimerB = timer_b.counter;
    cia_reg->TimerALatch = timer_a.latch;
    cia_reg->TimerBLatch = timer_b.latch;
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

inline void MOS6526::TimerReset(TIMER *timer)
{
    timer->counter = 0xffff;
    timer->latch = 0xffff;
    timer->cr_start = false;
    timer->cr_oneshot = false;
    timer->cr_force_load = false;
    timer->count_phi2 = true;
    timer->count_step = false;
    timer->count2 = false;
    timer->count3 = false;
    timer->load1 = false;
    timer->load = false;
    timer->oneshot0 = false;
    timer->oneshot = false;
}

// Ein Timer um einen Takt weiter schalten (liefert true wenn ein Unterlauf stattfand)
inline bool MOS6526::TimerStep(TIMER *timer)
{
    bool underflow = false;

    // Zaehler verringern (nur in Stufe 3 der Zaehlpipeline)
    if(timer->counter != 0 && timer->count3)
        timer->counter--;

    // Pipeline einen Takt weiter schieben
    bool new_count2 = timer->cr_start && timer->count_phi2;
    bool new_count3 = timer->count2 || (timer->count_step && timer->cr_start);
    bool new_load1 = timer->cr_force_load;
    bool new_load = timer->load1;
    bool new_oneshot0 = timer->cr_oneshot;
    bool new_oneshot = timer->oneshot0;

    timer->cr_force_load = false;
    timer->count_step = false;
    timer->count2 = new_count2;
    timer->count3 = new_count3;
    timer->load1 = new_load1;
    timer->load = new_load;
    timer->oneshot0 = new_oneshot0;
    timer->oneshot = new_oneshot;

    // Unterlauf
    if(timer->counter == 0 && timer->count3)
    {
        timer->load = true;
        underflow = true;
    }

    // Latch in den Zaehler laden (Unterlauf oder Force Load)
    if(timer->load)
    {
        timer->counter = timer->latch;
        timer->count3 = false;
    }

    // Im OneShot Modus stoppt der Timer nach einem Unterlauf
    if(underflow && (timer->oneshot || timer->oneshot0))
    {
        timer->cr_start = false;
        timer->count2 = false;
    }

    return underflow;
}

inline void MOS6526::TimerCount()
{
    // Zaehlimpulse vom CNT Pin
    bool cnt_pin_rising = (cnt_pin && !cnt_pin_old);
    cnt_pin_old = cnt_pin;

    if(!timer_a.count_phi2 && cnt_pin_rising)
        timer_a.count_step = true;

    // Timer B zaehlt entweder CNT Impulse oder die Unterlaeufe von Timer A (aus dem vorigen Takt)
    if(timer_b_mode == 1 && cnt_pin_rising)
        timer_b.count_step = true;

    if(timer_b_mode >= 2 && timer_a_underflow_old)
        timer_b.count_step = true;

    bool timer_a_underflow = TimerStep(&timer_a);
    bool timer_b_underflow = TimerStep(&timer_b);
    timer_a_underflow_old = timer_a_underflow;
    timer_b_underflow_old = timer_b_underflow;

    /// Timer A Unterlauf ///
    if(timer_a_underflow)
    {
        TriggerInterrupt(1);
        pb_6 ^= 0x01;
    }

    /// Timer B Unterlauf ///
    if(timer_b_underflow)
    {
        TriggerInterrupt(2);
        pb_7 ^= 0x01;
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
            UpdateIecWire();
        }
        break;

    case 1:
        io[address]=value;
        pb_latch = value;
        pb->SetOutput(pb_latch | ~ddr_b);
        if(cia_nr == 0)
            CheckLP();
        break;

    case 2:
        io[address]=value;
        ddr_a = value;
        pa->SetOutput(pa_latch | ~ddr_a);

        if(cia_nr == 0)  /// Nur bei CIA1
            ChangePOTSwitch();

        if(cia_nr == 1)	/// Nur bei CIA2 (Datenrichtung bestimmt, welche IEC Leitungen getrieben werden)
            UpdateIecWire();
        break;

    case 3:
        io[address]=value;
        ddr_b = value;
        pb->SetOutput(pb_latch | ~ddr_b);
        if(cia_nr == 0)
            CheckLP();
        break;

    case 4:
        timer_a.latch = (timer_a.latch & 0xFF00) | value;
        if(timer_a.load)
            timer_a.counter = (timer_a.counter & 0xFF00) | value;
        break;
    case 5:
        timer_a.latch = static_cast<uint16_t>((timer_a.latch & 0x00FF) | (value << 8));
        if(timer_a.load || !timer_a.cr_start)
            timer_a.counter = timer_a.latch;
        break;

    case 6:
        timer_b.latch = (timer_b.latch & 0xFF00) | value;
        if(timer_b.load)
            timer_b.counter = (timer_b.counter & 0xFF00) | value;
        break;
    case 7:
        timer_b.latch = static_cast<uint16_t>((timer_b.latch & 0x00FF) | (value << 8));
        if(timer_b.load || !timer_b.cr_start)
            timer_b.counter = timer_b.latch;
        break;

    case 8: case 9: case 10: case 11:
        if((address) == 11)
        {
            value &= 0x9F;
            if((value & 0x1F) == 0x12 && !(crb & 0x80))
                value ^= 0x80;
        }
        if(crb & 0x80)
        {
            clock_alarm[address - 8] = value;
        }
        else
        {
            if(address == 8)
                clock_is_stopped = false;
            if(address == 11)
                clock_is_stopped = true;

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
        if(!ifr_done)
        {
            IfrRun();
            ifr_done = true;
        }

        if(value & 0x80)
            interupt_mask |= value & 0x7F;
        else
            interupt_mask &= ~(value & 0x7F);

        // Ein anstehendes freigegebenes Flag hebt den Interrupt an (nicht wenn das ICR im vorigen Takt gelesen wurde)
        if((icr & interupt_mask & 0x7F) && !irq_line && !ifr_read1)
        {
            ifr_raise0 = true;
            ifr_d7set0 = true;
        }
        break;

    case 14:
        io[address] = value;
        cra = value & 0xEF;
        if((value & 0x01) && !timer_a.cr_start)
            pb_6 = 1;                                   // Beim Start von Timer A ist der Toggle Ausgang high
        timer_a.cr_start = ((value & 0x01) != 0);
        timer_a.cr_oneshot = ((value & 0x08) != 0);
        timer_a.cr_force_load = ((value & 0x10) != 0);
        timer_a.count_phi2 = ((value & 0x20) == 0x00);
        if (value & 0x80)
            clock_counter = clock_counter_latch = ClockCounterPAL; // PAL
        else
            clock_counter = clock_counter_latch = ClockCounterNTSC; // NTSC
        enable_pb6 = (value & 0x02) >> 1;
        pb6_mode = (value & 0x04) >> 2;
        break;

    case 15:
        io[address] = value;
        crb = value & 0xEF;
        if((value & 0x01) && !timer_b.cr_start)
            pb_7 = 1;                                   // Beim Start von Timer B ist der Toggle Ausgang high
        timer_b_mode = (value >> 5) & 0x03;
        timer_b.cr_start = ((value & 0x01) != 0);
        timer_b.cr_oneshot = ((value & 0x08) != 0);
        timer_b.cr_force_load = ((value & 0x10) != 0);
        timer_b.count_phi2 = (timer_b_mode == 0);
        enable_pb7 = (value & 0x02) >> 1;
        pb7_mode = (value & 0x04) >> 2;
        break;
    }
    return;
}

uint8_t MOS6526::ReadIO(uint16_t address)
{
    unsigned char ret;

    address &= 0x0F;

    switch(address)
    {
    case 0:
        if(cia_nr == 1)	/// Nur bei CIA2
        {
            uint8_t value = ((pa_latch | ~ddr_a) & 0x3f) | (*c64_iec_wire & *floppy_iec_wire & 0xc0);
            if((pa_latch | ~ddr_a) & 0x10)
                value &= ~0x40;
            if((pa_latch | ~ddr_a) & 0x20)
                value &= ~0x80;
            return value;
        }
        return ((pa_latch | ~(ddr_a)) & pa->GetInput());
    case 1:
        ret = ((pb_latch | ~(ddr_b))) & pb->GetInput();

        // Ausgang von Timer A an PIN PB6 anzeigen (Toggle: Zustand des Flipflops, Puls: Unterlauf im letzten Takt) //
        if(enable_pb6)
        {
            ret &= 0xBF;
            if(pb6_mode ? (pb_6 & 0x01) : timer_a_underflow_old) ret |= 0x40;
        }

        // Ausgang von Timer B an PIN PB7 anzeigen //
        if(enable_pb7)
        {
            ret &= 0x7F;
            if(pb7_mode ? (pb_7 & 0x01) : timer_b_underflow_old) ret |= 0x80;
        }
        return ret;

    case 2:
        return ddr_a;

    case 3:
        return ddr_b;

    case 4:
        return static_cast<uint8_t>(timer_a.counter);

    case 5:
        return static_cast<uint8_t>(timer_a.counter>>8);

    case 6:
        return static_cast<uint8_t>(timer_b.counter);

    case 7:
        return static_cast<uint8_t>(timer_b.counter>>8);

    case 8: case 9: case 10: case 11:
        /// Wenn UhrLatched = false --> Uhrzeit ins Latch schieben ///
        if(!clock_is_latched)
            memcpy(clock_latch,io+8,4);

        if(address == 8)
            clock_is_latched = false;

        if(address == 11)
            clock_is_latched = true;

        return clock_latch[address - 8];

    case 12:
        return sdr;

    case 13:
        if(!ifr_done)
        {
            IfrRun();
            ifr_done = true;
        }

        // Steht der Interrupt kurz vor dem Anheben, dann ist Bit 7 schon zu sehen
        if(ifr_raise0 && (icr & 0x1F))
            icr |= 0x80;

        // Die gelesenen Flags werden zwei Takte spaeter geloescht
        if(icr & 0x9F)
            icr_ack |= (icr & 0x9F) | 0x80;

        ifr_ack1 = true;
        ifr_raise0 = false;
        ifr_d7set0 = false;
        ifr_read0 = true;

        ret = icr;
        SetIrqLine(false);
        return ret;

    case 14:
        return (cra & 0xFE) | (timer_a.cr_start ? 0x01 : 0x00);

    case 15:
        return (crb & 0xFE) | (timer_b.cr_start ? 0x01 : 0x00);

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
