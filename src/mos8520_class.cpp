//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: mos8520_class.cpp                     //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 23.08.2021                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "mos8520_class.h"

MOS8520::MOS8520()
{

}

MOS8520::~MOS8520()
{
}

void MOS8520::Reset()
{
	for(int i=0;i<16;i++) io[i] = 0;

	pa_latch = pb_latch = ddr_a = ddr_b = 0;
	pa.SetOutput(pa_latch | ~ddr_a);
	pb.SetOutput(pb_latch | ~ddr_b);
}

void MOS8520::OneZyklus()
{
	if(*reset == false)
		Reset();
}

uint8_t MOS8520::ReadIO(uint16_t address)
{
	// Register 0 - 15
	// cout << "Read from MOS8520 IO-Address: " << std::hex << address << endl;

	switch (address & 0x0f)
	{
	case 0x00:		// PRA Peripheral Data Reg. A
		break;

	case 0x01:		// PRB Peripheral Data Reg. B
		break;

	case 0x02:		// DDRA Data Direction Reg. A
		break;

	case 0x03:		// DDRB Data Direction Reg. B
		break;

	case 0x04:		// TA LO Timer A Low Register
		break;

	case 0x05:		// TA HI Timer A High Register
		break;

	case 0x06:		// TB LO Timer B Low Register
		break;

	case 0x07:		// TB HI Timer B High Register
		break;

	case 0x08:		// Event LBS
		break;

	case 0x09:		// Event 8-15
		break;

	case 0x0a:		// Event MSB
		break;

	case 0x0b:		// No Connect
		break;

	case 0x0c:		// SDR Serial Data Register
		break;

	case 0x0d:		// ICR Interrupt Control Register
		break;

	case 0x0e:		// CRA Control Register A
		break;

	case 0x0f:		// CRB Control Register B
		break;
	}

	return 0;
}

void MOS8520::WriteIO(uint16_t address, uint8_t value)
{
	// Register 0 - 15
	// cout << "Write to MOS8520 IO-Address: " << std::hex << address << ", " << (uint16_t)value << endl;

	address &= 0x0f;

	switch (address)
	{
	case 0x00:		// PRA Peripheral Data Reg. A
		io[address]=value;
		pa_latch = value;
		pa.SetOutput(pa_latch | ~ddr_a);
		break;

	case 0x01:		// PRB Peripheral Data Reg. B
		io[address]=value;
		pb_latch = value;
		pb.SetOutput(pb_latch | ~ddr_b);
		break;

	case 0x02:		// DDRA Data Direction Reg. A
		io[address]=value;
		ddr_a = value;
		pa.SetOutput(pa_latch | ~ddr_a);
		break;

	case 0x03:		// DDRB Data Direction Reg. B
		io[address]=value;
		ddr_b = value;
		pb.SetOutput(pb_latch | ~ddr_b);
		break;

	case 0x04:		// TA LO Timer A Low Register
		break;

	case 0x05:		// TA HI Timer A High Register
		break;

	case 0x06:		// TB LO Timer B Low Register
		break;

	case 0x07:		// TB HI Timer B High Register
		break;

	case 0x08:		// Event LBS
		break;

	case 0x09:		// Event 8-15
		break;

	case 0x0a:		// Event MSB
		break;

	case 0x0b:		// No Connect
		break;

	case 0x0c:		// SDR Serial Data Register
		break;

	case 0x0d:		// ICR Interrupt Control Register
		break;

	case 0x0e:		// CRA Control Register A
		break;

	case 0x0f:		// CRB Control Register B
		break;
	}
}
