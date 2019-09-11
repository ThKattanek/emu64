//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: mos6510_class.cpp                     //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 10.06.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "mos6510_class.h"
#include "micro_code_tbl_6510.h"

#define CHK_RDY	if(!*RDY && !CpuWait){CpuWait=true;MCT--;break;}
#define OLD_IRQHandling

MOS6510::MOS6510(void)
{
    ReadProcTbl = 0;
    WriteProcTbl = 0;

    MCT = ((unsigned char*)MicroCodeTable6510 + (0x100*MCTItemSize));
    Pointer = 0;
    Adresse = 0x0000;
    BranchAdresse = 0x0000;
    AktOpcodePC = 0x0100;
    JAMFlag = false;
    TMPByte = 0;
    CpuWait = false;
    LastOPC_CLI = false;

    PC = 0;
    AC = 0;
    XR = 0;
    YR = 0;
    SP = 0;
    SR = 32;

    EnableExtInterrupts = false;

    NMIState = false;
    IRQLine = 0;
    for(int i=0;i<5;i++)
    {
        NMIStatePuffer[i] = false;
        IRQLinePuffer[i] = 0;
    }

    EnableDebugCart = false;
    WRITE_DEBUG_CART = false;
}

MOS6510::~MOS6510(void)
{
}

void MOS6510::TriggerInterrupt(int typ)
{
    if(EnableExtInterrupts)
    {
        switch(typ)
        {
        case EXT_IRQ:
            typ = CIA_IRQ;
            break;
        case EXT_NMI:
            typ = CIA_NMI;
            break;
        default:
            typ = NOP_INT;
            break;
        }
    }

    switch (typ)
    {
    case CIA_IRQ:
            //Interrupts[CIA_IRQ] = true;
            IRQLine |= 0x01;
            break;
    case CIA_NMI:
            if(Interrupts[CIA_NMI] == true) return;
            Interrupts[CIA_NMI] = true;
            if((Interrupts[CRT_NMI] == false) && (Interrupts[RESTORE_NMI] == false)) NMIState = true;
            break;
    case VIC_IRQ:
            //Interrupts[VIC_IRQ] = true;
            IRQLine |= 0x02;
            break;
    case REU_IRQ:
            //Interrupts[REU_IRQ] = true;
            IRQLine |= 0x04;
            break;
    case CRT_NMI:
            if(Interrupts[CRT_NMI] == true) return;
            Interrupts[CRT_NMI] = true;
            if((Interrupts[CIA_NMI] == false) && (Interrupts[RESTORE_NMI] == false)) NMIState = true;
            break;
    case RESTORE_NMI:
            if(Interrupts[RESTORE_NMI] == true) return;
            Interrupts[RESTORE_NMI] = true;
            if((Interrupts[CIA_NMI] == false) && (Interrupts[CRT_NMI] == false)) NMIState = true;
            break;
    default:
            break;
    }
}

void MOS6510::ClearInterrupt(int typ)
{
    if(EnableExtInterrupts)
    {
        switch(typ)
        {
        case EXT_IRQ:
            typ = CIA_IRQ;
            break;
        case EXT_NMI:
            typ = CIA_NMI;
            break;
        default:
            typ = NOP_INT;
            break;
        }
    }

    switch (typ)
    {
    case CIA_IRQ:
            //Interrupts[CIA_IRQ] = false;
            IRQLine &= 0xFE;
            break;
    case CIA_NMI:
            Interrupts[CIA_NMI] = false;
            if(Interrupts[CRT_NMI] == false) NMIState = false;
            break;
    case VIC_IRQ:
            //Interrupts[VIC_IRQ] = false;
            IRQLine &= 0xFD;
            break;
    case REU_IRQ:
            //Interrupts[REU_IRQ] = false;
            IRQLine &= 0xFB;
            break;
    case CRT_NMI:
            Interrupts[CRT_NMI] = false;
            if(Interrupts[CIA_NMI] == false) NMIState = false;
            break;
    case RESTORE_NMI:
            Interrupts[RESTORE_NMI] = false;
            if((Interrupts[CIA_NMI] == false) && (Interrupts[CRT_NMI] == false)) NMIState = false;
            break;
    default:
            break;
    }
}

void MOS6510::ClearJAMFlag(void)
{
    JAMFlag = false;
}

void MOS6510::SetRegister(REG_STRUCT *reg)
{
    if(reg == 0) return;

    unsigned char mask = reg->reg_mask;
    if((mask&1) == 1)
    {
        PC = reg->pc;
        MCT = ((unsigned char*)MicroCodeTable6510 + 6);
    }
    mask>>=1;
    if((mask&1) == 1) AC = (unsigned char)reg->ac;
    mask>>=1;
    if((mask&1) == 1) XR = (unsigned char)reg->xr;
    mask>>=1;
    if((mask&1) == 1) YR = (unsigned char)reg->yr;
    mask>>=1;
    if((mask&1) == 1) SP = (unsigned char)reg->sp;
    mask>>=1;
    if((mask&1) == 1) SR = (unsigned char)reg->sr | 32;
}

bool MOS6510::GetInterrupts(int typ)
{
    return Interrupts[typ];
}

void MOS6510::GetRegister(REG_STRUCT *reg)
{
    if(reg == 0) return;

    unsigned char mask = reg->reg_mask;
    if((mask&1) == 1) reg->pc = PC;
    mask>>=1;
    if((mask&1) == 1) reg->ac = AC;
    mask>>=1;
    if((mask&1) == 1) reg->xr = XR;
    mask>>=1;
    if((mask&1) == 1) reg->yr = YR;
    mask>>=1;
    if((mask&1) == 1) reg->sp = SP;
    mask>>=1;
    if((mask&1) == 1) reg->sr = SR;
    mask>>=1;
    if((mask&1) == 1)
    {
        reg->irq = Read(0xFFFE);
        reg->irq |= Read(0xFFFF)<<8;
    }
    mask>>=1;
    if((mask&1) == 1)
    {
        reg->nmi = Read(0xFFFA);
        reg->nmi |= Read(0xFFFB)<<8;
    }
    reg->_0314 = Read(0x314);
    reg->_0314 |= Read(0x315)<<8;
    reg->_0318 = Read(0x318);
    reg->_0318 |= Read(0x319)<<8;
}

void MOS6510::GetInterneRegister(IREG_STRUCT* ireg)
{
    if(ireg == 0) return;
    ireg->current_opcode_pc = AktOpcodePC;
    ireg->current_opcode = AktOpcode;
    ireg->current_micro_code = *MCT;
    ireg->cpu_wait = CpuWait;
    ireg->jam_flag = JAMFlag;
    ireg->pointer = Pointer;
    ireg->address = Adresse;
    ireg->branch_address = BranchAdresse;
    ireg->tmp_byte = TMPByte;
    ireg->irq = IRQLine;
    ireg->nmi = Interrupts[CIA_NMI] | Interrupts[CRT_NMI] | Interrupts[RESTORE_NMI];
    ireg->rdy = *RDY;
    ireg->reset = *RESET;
}

void MOS6510::SetEnableDebugCart(bool enabled)
{
    EnableDebugCart = enabled;
    WRITE_DEBUG_CART = false;
}

unsigned char MOS6510::GetDebugCartValue()
{
    return DebugCartValue;
}

/*
bool MOS6510::SaveFreez(FILE *File)
{
    unsigned short MCTOffset = MCT-(unsigned char*)MicroCodeTable6510;
    fwrite(&MCTOffset,1,sizeof(MCTOffset),File);
    fwrite(&Pointer,1,sizeof(Pointer),File);
    fwrite(&Adresse,1,sizeof(Adresse),File);
    fwrite(&BranchAdresse,1,sizeof(BranchAdresse),File);
    fwrite(&AktOpcodePC,1,sizeof(AktOpcodePC),File);
    fwrite(&CpuWait,1,sizeof(CpuWait),File);
    fwrite(&JAMFlag,1,sizeof(JAMFlag),File);
    fwrite(&TMPByte,1,sizeof(TMPByte),File);
    fwrite(Interrupts,1,sizeof(Interrupts),File);
    fwrite(&NMIState,1,sizeof(NMIState),File);
    fwrite(&PC,1,sizeof(PC),File);
    fwrite(&AC,1,sizeof(AC),File);
    fwrite(&XR,1,sizeof(XR),File);
    fwrite(&YR,1,sizeof(YR),File);
    fwrite(&SP,1,sizeof(SP),File);
    fwrite(&SR,1,sizeof(SR),File);
    return true;
}

bool MOS6510::LoadFreez(FILE *File,unsigned short Version)
{
    unsigned short MCTOffset;

    switch(Version)
    {
    case 0x0100:
    case 0x0101:
        fread(&MCTOffset,1,sizeof(MCTOffset),File);
        MCT = MCTOffset + (unsigned char*)MicroCodeTable6510;
        fread(&Pointer,1,sizeof(Pointer),File);
        fread(&Adresse,1,sizeof(Adresse),File);
        fread(&BranchAdresse,1,sizeof(BranchAdresse),File);
        fread(&AktOpcodePC,1,sizeof(AktOpcodePC),File);
        fread(&CpuWait,1,sizeof(CpuWait),File);
        fread(&JAMFlag,1,sizeof(JAMFlag),File);
        fread(&TMPByte,1,sizeof(TMPByte),File);
        fread(Interrupts,1,sizeof(Interrupts),File);
        fread(&NMIState,1,sizeof(NMIState),File);
        fread(&PC,1,sizeof(PC),File);
        fread(&AC,1,sizeof(AC),File);
        fread(&XR,1,sizeof(XR),File);
        fread(&YR,1,sizeof(YR),File);
        fread(&SP,1,sizeof(SP),File);
        fread(&SR,1,sizeof(SR),File);
        break;
    }
    return true;
}
*/

inline void MOS6510::SET_SR_NZ(unsigned char wert)
{
    SR = (SR&0x7D)|(wert&0x80);
    if (wert==0) SR|=2;
}

inline void MOS6510::SET_SIGN(unsigned char wert)
{
    SR = (SR&127)|(wert&0x80);
}

inline void MOS6510::SET_ZERO(unsigned char wert)
{
    if(wert==0) SR|=2;
    else SR&=0xFD;
}

inline void MOS6510::SET_CARRY(unsigned char status)
{
    if (status!=0) SR|=1;
    else SR&=0xFE;
}

inline bool MOS6510::IF_CARRY(void)
{
    if(SR&1) return true;
    return false;
}

inline bool MOS6510::IF_DECIMAL(void)
{
    if(SR&8) return true;
    return false;
}

inline void MOS6510::SET_OVERFLOW(bool status)
{
    if (status!=0) SR|=64;
    else SR&=0xBF;
}

inline unsigned char MOS6510::Read(unsigned short adresse)
{
    unsigned char wert = ReadProcTbl[(adresse)>>8](adresse);

    if(Breakpoints[adresse] & 16)
    {
        *BreakStatus |=16;
        BreakWerte[4] = adresse;
    }

    if(Breakpoints[wert] & 64)
    {
        *BreakStatus |=64;
        BreakWerte[6] = wert;
    }
    return wert;
}

inline void MOS6510::Write(unsigned short adresse, unsigned char wert)
{
    if((EnableDebugCart == true) && (adresse == DEBUG_CART_ADRESS))
    {
        DebugCartValue = wert;
        WRITE_DEBUG_CART = true;
    }

    if(adresse == 0xFF00) WRITE_FF00 = true;
    if(Breakpoints[adresse] & 32)
    {
        *BreakStatus |=32;
        BreakWerte[5] = adresse;
    }

    if(Breakpoints[wert] & 128)
    {
        *BreakStatus |=128;
        BreakWerte[7] = wert;
    }
    WriteProcTbl[(adresse)>>8](adresse,wert);
}

bool MOS6510::OneZyklus(void)
{
    static bool RESET_OLD = false;
    static unsigned char   idxReg = 0;

    unsigned int    tmp;
    unsigned int    tmp1;
    unsigned short  tmp2;
    unsigned short  src;
    bool            carry_tmp;

    if(*RDY) CpuWait = false;

    if(!*RESET)
    {
        CpuWait=true;
    }

    if((*RESET == true) && (RESET_OLD == false))
    {
        CpuWait=false;
        Interrupts[CIA_NMI] = false;
        Interrupts[CRT_NMI] = false;
        Interrupts[RESTORE_NMI] = false;

        IRQLine = 0;
        for(int i=0;i<5;i++) IRQLinePuffer[i] = 0;

        NMIState = false;
        JAMFlag = false;
        SR = 0x04;
        MCT = ((unsigned char*)MicroCodeTable6510 + (0x100*MCTItemSize));
        AktOpcode = 0x100;
    }
    RESET_OLD = *RESET;

    // Unter Index 0 ist immer der aktuelle NMI Zustand in diesem Zyklus
    // Unter Index 1 ist der Zustand 1 Zklus aus der Vergangenheit
    // Unter Index 2 ist der Zustand 2 Zyklen aus der Vergangenheit
    // usw...
    for(int i=4;i>0;i--) NMIStatePuffer[i] = NMIStatePuffer[i-1];
    NMIStatePuffer[0] = NMIState;

    // Unter Index 0 ist immer der aktuelle IRQ Zustand in diesem Zyklus
    // Unter Index 1 ist der Zustand 1 Zklus aus der Vergangenheit
    // Unter Index 2 ist der Zustand 2 Zyklen aus der Vergangenheit
    // usw...
    for(int i=4;i>0;i--) IRQLinePuffer[i] = IRQLinePuffer[i-1];
    IRQLinePuffer[0] = IRQLine;

    if(!CpuWait)
    {
        switch(*MCT)
        {
        //R // Feetch Opcode
        case 0:
            if(JAMFlag) return false;

            CHK_RDY

            if((NMIStatePuffer[1] == true)) // NMIStatePuffer[CYCLES] --> 2 CYCLES Sagt zwei Zyklen vorher muss der NMI schon angelegen haben also vor dem letzten Zyklus des vorigen Befehls
            {
                NMIState = false;
                MCT = ((unsigned char*)MicroCodeTable6510 + (0x102*MCTItemSize));
                AktOpcode = 0x102;
                return false;
            }else if((IRQLinePuffer[1] > 0) && ((SR&4)==0)) // IRQLinePuffer[CYCLES] --> 2 CYCLES Sagt zwei Zyklen vorher muss der IRQ schon anliegen also vor dem letzten Zyklus des vorigen Befehls
            {
                MCT = ((unsigned char*)MicroCodeTable6510 + (0x101*MCTItemSize));
                AktOpcode = 0x101;
                return false;
            }

            MCT = ((unsigned char*)MicroCodeTable6510 + (Read(PC)*MCTItemSize));
            AktOpcode = ReadProcTbl[(AktOpcodePC)>>8](AktOpcodePC);

            *HistoryPointer = *HistoryPointer+1;
            History[*HistoryPointer] = AktOpcodePC;

            PC++;

            if(LastOPC_CLI)
            {
                LastOPC_CLI=false;
                SR &= 0xFB;
            }

            return false;
        break;

        //R // Lesen von PC-Adresse und verwerfen // PC++
        case 1:
            CHK_RDY
            Read(PC);
            PC++;
            break;
        //W // PC Hi -> Stack // SR|16 // SP--
        case 2:
            Write(0x0100+(SP),GetPCHi);
            SR|=16;
            SP--;
            break;
        //W // PC Lo -> Stack // SP--
        case 3:
            Write(0x0100+(SP),GetPCLo);
            SP--;
            break;
        //W // SR -> Stack // SR|4 // SP--
        case 4:
            Write(0x0100+(SP),SR);
            SR|=4;
            SP--;
            break;
        //R // PC Lo von 0xFFFE holen
        case 5:
            CHK_RDY
            SetPCLo(Read(0xFFFE));
            break;
        //R // PC Hi von 0xFFFF holen
        case 6:
            CHK_RDY
            SetPCHi(Read(0xFFFF));
            break;
        //R // Pointer von PC-Adresse holen // PC++
        case 7:
            CHK_RDY
            Pointer = Read(PC);
            PC++;
            break;
        //R // Lesen von Pointer und verwerfen // Pointer+XR
        case 8:
            CHK_RDY
            Read((unsigned short)Pointer);
            Pointer += XR;
            break;
        //R // Adresse Lo von Pointer-Adresse holen // Pointer++
        case 9:
                CHK_RDY
                SetAdresseLo(Read((unsigned short)Pointer));
                Pointer++;
                break;
        //R // Adresse Hi von Pointer-Adresse holen //
        case 10:
                CHK_RDY
                SetAdresseHi(Read((unsigned short)Pointer));
                break;
        //R // TMPByte von Adresse holen // AC or TMPByte // Set SR(NZ)
        case 11:
                CHK_RDY
                TMPByte = Read(Adresse);
                AC |= TMPByte;
                SET_SR_NZ(AC);
                break;
        //R // JAM
        case 12:
                CHK_RDY
                CpuWait=false;
                SR = 0x04;
                MCT = ((unsigned char*)MicroCodeTable6510 + (0x100*MCTItemSize));
                return 0;
        break;
        //R // TMPByte von Adresse holen
        case 13:
                CHK_RDY
                TMPByte = Read(Adresse);
                break;
        //W // TMPByte nach Adresse schreiben // ASL MEMORY // ORA
        case 14:
            Write(Adresse,TMPByte);

            SET_CARRY(TMPByte&0x80);	// ASL MEMORY
            TMPByte <<= 1;
            TMPByte &= 0xFF;

            AC|=TMPByte;				// ORA
            SET_SR_NZ(AC);

            break;
        //W // TMPByte nach Adresse schreiben
        case 15:
                Write(Adresse,TMPByte);
                break;
        //R // Adresse Hi = 0 // Adresse Lo von PC-Adresse holen // PC++
        case 16:
                CHK_RDY
                Adresse = Read(PC);
                PC++;
                break;
        //R // TMPByte von Adresse lesen // Adresse Lo + YR
        case 17:
                CHK_RDY
                TMPByte = Read(Adresse);
                Adresse += YR;
                Adresse &= 0xFF;
                break;
        //W // TMPByte nach Adresse schreiben // TMPByte<<1 // Set SR(NZC)
        case 18:
                Write(Adresse,TMPByte);
                SET_CARRY(TMPByte&0x80);
                TMPByte <<= 1;
                TMPByte &= 0xFF;
                SET_SIGN(TMPByte);
                SET_ZERO(TMPByte);
                break;
        //R // TMPByte von PC-Adresse holen
        case 19:
                CHK_RDY
                TMPByte = Read(PC);
                break;
        //W // SR nach SP+0x0100 schreiben // SP-1
        case 20:
                Write(SP+0x0100,SR|16);
                SP--;
                break;
        //R // TMPByte von PC-Adresse holen // AC or TMPByte // PC+1
        case 21:
                CHK_RDY
                TMPByte = Read(PC);
                AC|=TMPByte;
                SET_SR_NZ(AC);
                PC++;
                break;
        //R // TMPByte von PC-Adresse holen // AC<<1 // Set SR(NZC)
        case 22:
                CHK_RDY
                TMPByte = Read(PC);
                SET_CARRY(AC&0x80);
                AC <<= 1;
                AC &= 0xFF;
                SET_SIGN(AC);
                SET_ZERO(AC);
                break;
        //R // TMPByte von PC-Adresse holen // AC and TMPByte // Set SR(NZC) // PC+1
        case 23:
                CHK_RDY
                TMPByte = Read(PC);
                AC&=TMPByte;
                SET_SR_NZ(AC);
                SR&=0xFE;
                SR|=AC>>7;
                PC++;
                break;
        //R // Adresse Lo von PC-Adresse holen // PC+1
        case 24:
                CHK_RDY
                SetAdresseLo(Read(PC));
                PC++;
                break;
        //R // Adresse Hi von PC-Adresse holen // PC+1
        case 25:
                CHK_RDY
                SetAdresseHi(Read(PC));
                PC++;
                break;
        //R // TMPByte von PC-Adresse holen // PC+1 // SR(N) auf FALSE prÃŒfen (BPL)
        case 26:
                CHK_RDY
                TMPByte = Read(PC);
                PC++;
                if((SR&0x80)!=0x00) MCT+=2;
                break;
        //R // TMPByte von PC-Adresse holen // PC+1 // SR(N) auf TRUE prÃŒfen (BMI)
        case 27:
                CHK_RDY
                TMPByte = Read(PC);
                PC++;
                if((SR&0x80)!=0x80) MCT+=2;
                break;
        //R // TMPByte von PC-Adresse holen // PC+1 // SR(V) auf FALSE prÃŒfen (BVC)
        case 28:
                CHK_RDY
                TMPByte = Read(PC);
                PC++;
                if((SR&0x40)!=0x00) MCT+=2;
                break;
        //R // TMPByte von PC-Adresse holen // PC+1 // SR(V) auf TRUE prÃŒfen (BVS)
        case 29:
                CHK_RDY
                TMPByte = Read(PC);
                PC++;
                if((SR&0x40)!=0x40) MCT+=2;
                break;
        //R // TMPByte von PC-Adresse holen // PC+1 // SR(C) auf FALSE prÃŒfen (BCC)
        case 30:
                CHK_RDY
                TMPByte = Read(PC);
                PC++;
                if((SR&0x01)!=0x00) MCT+=2;
                break;
        //R // TMPByte von PC-Adresse holen // PC+1 // SR(C) auf TRUE prÃŒfen (BCS)
        case 31:
                CHK_RDY
                TMPByte = Read(PC);
                PC++;
                if((SR&0x01)!=0x01) MCT+=2;
                break;
        //R // TMPByte von PC-Adresse holen // PC+1 // SR(Z) auf FALSE prÃŒfen (BNE)
        case 32:
                CHK_RDY
                TMPByte = Read(PC);
                PC++;
                if((SR&0x02)!=0x00) MCT+=2;
                break;
        //R // TMPByte von PC-Adresse holen // PC+1 // SR(Z) auf TRUE prÃŒfen (BEQ)
        case 33:
                CHK_RDY
                TMPByte = Read(PC);
                PC++;
                if((SR&0x02)!=0x02) MCT+=2;
                break;
        //R // Lesen von PC-Adresse und verwerfen // BranchAdresse=PC+TMPByte
        case 34:
                CHK_RDY
                Read(PC);
                BranchAdresse = PC + (signed char)(TMPByte);
                if ((PC ^ BranchAdresse) & 0xFF00)
                {
                        PC = (PC&0xFF00)|(BranchAdresse&0xFF);
                }
                else
                {
                        PC = BranchAdresse;
                        MCT+=1;
                }
                break;
        //R // FIX PC Hi Adresse (Im Branchbefehl)
        case 35:
                CHK_RDY
                Read(PC);
                PC = BranchAdresse;
                break;
        //R // Adresse Hi von Pointer-Adresse holen // Adresse+YR
        case 36:
                CHK_RDY
                SetAdresseHi(Read((unsigned short)Pointer));
                Adresse += YR;
                idxReg = YR;
                break;
        //R // TMPByte von Adresse holen // Fix Adresse Hi MCT+1 // AC or TMPByte //
        case 37:
                CHK_RDY
                if((Adresse&0xFF) >= idxReg)
                {
                        TMPByte = Read(Adresse);
                        AC|=TMPByte;
                        SET_SR_NZ(AC);
                        MCT++;
                }
                break;
        //R // Adresse Hi von PC-Adresse holen // PC=Adresse
        case 38:
                CHK_RDY
                SetAdresseHi(Read(PC));
                PC = Adresse;
                break;
        //R // Lesen von PC-Adresse und verwerfen // XR=AC // Set SR(NZ)
        case 39:
                CHK_RDY
                Read(PC);
                XR = AC;
                SET_SR_NZ(XR);
                break;
        //R // Lesen von PC-Adresse und verwerfen // YR=AC // Set SR(NZ)
        case 40:
                CHK_RDY
                Read(PC);
                YR = AC;
                SET_SR_NZ(YR);
                break;
        //R // Lesen von PC-Adresse und verwerfen // XR=SP // Set SR(NZ)
        case 41:
                CHK_RDY
                Read(PC);
                XR = SP;
                SET_SR_NZ(XR);
                break;
        //R // Lesen von PC-Adresse und verwerfen // AC=XR // Set SR(NZ)
        case 42:
                CHK_RDY
                Read(PC);
                AC = XR;
                SET_SR_NZ(AC);
                break;
        //R // Lesen von PC-Adresse und verwerfen // SP=XR
        case 43:
                CHK_RDY
                Read(PC);
                SP = XR;
                break;
        //R // Lesen von PC-Adresse und verwerfen // AC=YR // Set SR(NZ)
        case 44:
                CHK_RDY
                Read(PC);
                AC = YR;
                SET_SR_NZ(AC);
                break;
        //W // AC nach SP+0x0100 schreiben // SP-1
        case 45:
                Write(SP+0x0100,AC);
                SP--;
                break;
        //R // AC von SP+0x0100 lesen // SP+1
        case 46:
                CHK_RDY
                AC = Read(SP+0x0100);
                SP++;
                break;
        //R // AC von SP+0x0100 lesen // Set SR(NZ)
        case 47:
                CHK_RDY
                AC = Read(SP+0x0100);
                SET_SR_NZ(AC);
                break;
        //R // SR von SP+0x0100 lesen // SP+1
        case 48:
                CHK_RDY
                SR = Read(SP+0x0100)|32;
                SP++;
                break;
        //R // SR von SP+0x0100 lesen
        case 49:
                CHK_RDY
                SR = Read(SP+0x0100)|32;
                break;
        //R // TMPByte von PC-Adresse lesen // AC + TMPByte + Carry // PC+1
        case 50:
                CHK_RDY
                tmp1 = Read(PC);

                if (IF_DECIMAL())
                {
                        tmp = (AC & 0xF) + (tmp1 & 0xF) + (SR & 0x1);
                        if (tmp > 0x9) tmp += 0x6;
                        if (tmp <= 0x0F) tmp = (tmp & 0xF) + (AC & 0xF0) + (tmp1 & 0xF0);
                        else tmp = (tmp & 0xF) + (AC & 0xF0) + (tmp1 & 0xF0) + 0x10;
                        SET_ZERO(((AC + tmp1 + (SR & 0x1)) & 0xFF));
                        SET_SIGN(tmp & 0x80);
                        SET_OVERFLOW(((AC ^ tmp) & 0x80) && !((AC ^ tmp1) & 0x80));
                        if ((tmp & 0x1F0) > 0x90) tmp += 0x60;
                        SET_CARRY((tmp & 0xFF0) > 0xF0);
                }
                else
                {
                        tmp = tmp1 + AC + (SR & 0x01);
                        SET_SR_NZ(tmp & 0xff);
                        SET_OVERFLOW(!((AC ^ tmp1) & 0x80) && ((AC ^ tmp) & 0x80));
                        SET_CARRY(tmp > 0xFF);
                }
                AC = (unsigned char)tmp;
                PC++;
                break;

        //R // TMPByte von Adresse lesen // AC + TMPByte + Carry
        case 51:
                CHK_RDY
                tmp1 = Read(Adresse);

                if (IF_DECIMAL())
                {
                        tmp = (AC & 0xF) + (tmp1 & 0xF) + (SR & 0x1);
                        if (tmp > 0x9) tmp += 0x6;
                        if (tmp <= 0x0F) tmp = (tmp & 0xF) + (AC & 0xF0) + (tmp1 & 0xF0);
                        else tmp = (tmp & 0xF) + (AC & 0xF0) + (tmp1 & 0xF0) + 0x10;
                        SET_ZERO(((AC + tmp1 + (SR & 0x1)) & 0xFF));
                        SET_SIGN(tmp & 0x80);
                        SET_OVERFLOW(((AC ^ tmp) & 0x80) && !((AC ^ tmp1) & 0x80));
                        if ((tmp & 0x1F0) > 0x90) tmp += 0x60;
                        SET_CARRY((tmp & 0xFF0) > 0xF0);
                }
                else
                {
                        tmp = tmp1 + AC + (SR & 0x01);
                        SET_SR_NZ(tmp & 0xff);
                        SET_OVERFLOW(!((AC ^ tmp1) & 0x80) && ((AC ^ tmp) & 0x80));
                        SET_CARRY(tmp > 0xFF);
                }
                AC = (unsigned char)tmp;
                break;

        //R // TMPByte von Adresse lesen // Adresse Lo + XR
        case 52:
                CHK_RDY
                TMPByte = Read(Adresse);
                Adresse += XR;
                Adresse &= 0xFF;
                break;
        //R // Adresse Hi von PC-Adresse holen // Adresse+XR  // PC+1 //
        case 53:
                CHK_RDY
                SetAdresseHi(Read(PC));
                Adresse += XR;
                idxReg = XR;
                PC++;
                break;
        //R // Adresse Hi von PC-Adresse holen // Adresse+YR  // PC+1 //
        case 54:
                CHK_RDY
                SetAdresseHi(Read(PC));
                Adresse += YR;
                idxReg = YR;
                PC++;
                break;
        //R // TMPByte von Adresse lesen // AC + TMPByte + Carry // if(idxReg<Adresse Lo) MCT++
        case 55:
                CHK_RDY
                if((Adresse&0xFF)>=(idxReg))
                {
                        tmp1 = Read(Adresse);
                        if (IF_DECIMAL())
                        {
                                tmp = (AC & 0xF) + (tmp1 & 0xF) + (SR & 0x1);
                                if (tmp > 0x9) tmp += 0x6;
                                if (tmp <= 0x0F) tmp = (tmp & 0xF) + (AC & 0xF0) + (tmp1 & 0xF0);
                                else tmp = (tmp & 0xF) + (AC & 0xF0) + (tmp1 & 0xF0) + 0x10;
                                SET_ZERO(((AC + tmp1 + (SR & 0x1)) & 0xFF));
                                SET_SIGN(tmp & 0x80);
                                SET_OVERFLOW(((AC ^ tmp) & 0x80) && !((AC ^ tmp1) & 0x80));
                                if ((tmp & 0x1F0) > 0x90) tmp += 0x60;
                                SET_CARRY((tmp & 0xFF0) > 0xF0);
                        }
                        else
                        {
                                tmp = tmp1 + AC + (SR & 0x01);
                                SET_SR_NZ(tmp & 0xff);
                                SET_OVERFLOW(!((AC ^ tmp1) & 0x80) && ((AC ^ tmp) & 0x80));
                                SET_CARRY(tmp > 0xFF);
                        }
                        AC = (unsigned char)tmp;
                        MCT++;
                }
                break;
        //R // TMPByte von PC-Adresse lesen // AC - TMPByte - Carry // PC+1
        case 56:
                CHK_RDY
                src=Read(PC);
                tmp2 = AC - src - ((SR & 0x01) ? 0 : 1);
                if (IF_DECIMAL())
                {
                        tmp = (AC & 0xF) - (src & 0xF) - ((SR & 0x1) ? 0 : 1);
                        if (tmp & 0x10) tmp = ((tmp - 6) & 0xF)| ((AC & 0xF0) - (src & 0xF0) - 0x10);
                        else tmp = (tmp & 0xF) | ((AC & 0xF0) - (src & 0xF0));
                        if (tmp & 0x100) tmp -= 0x60;
                        SET_CARRY(tmp2 < 0x100);
                        SET_SR_NZ(tmp2 & 0xFF);
                        SET_OVERFLOW(((AC ^ tmp2) & 0x80) && ((AC ^ src) & 0x80));
                        AC = (unsigned char) tmp;
                }
                else
                {
                        SET_SR_NZ(tmp2 & 0xff);
                        SET_CARRY(tmp2 < 0x100);
                        SET_OVERFLOW(((AC ^ tmp2) & 0x80)
                                                         && ((AC ^ src) & 0x80));
                        AC = (unsigned char) tmp2;
                }
                PC++;
                break;
        //R // TMPByte von Adresse lesen // AC - TMPByte - Carry
        case 57:
                CHK_RDY
                src=Read(Adresse);
                tmp2 = AC - src - ((SR & 0x01) ? 0 : 1);
                if (IF_DECIMAL())
                {
                        tmp = (AC & 0xF) - (src & 0xF) - ((SR & 0x1) ? 0 : 1);
                        if (tmp & 0x10) tmp = ((tmp - 6) & 0xF)| ((AC & 0xF0) - (src & 0xF0) - 0x10);
                        else tmp = (tmp & 0xF) | ((AC & 0xF0) - (src & 0xF0));
                        if (tmp & 0x100) tmp -= 0x60;
                        SET_CARRY(tmp2 < 0x100);
                        SET_SR_NZ(tmp2 & 0xFF);
                        SET_OVERFLOW(((AC ^ tmp2) & 0x80) && ((AC ^ src) & 0x80));
                        AC = (unsigned char) tmp;
                }
                else
                {
                        SET_SR_NZ(tmp2 & 0xff);
                        SET_CARRY(tmp2 < 0x100);
                        SET_OVERFLOW(((AC ^ tmp2) & 0x80)
                                                         && ((AC ^ src) & 0x80));
                        AC = (unsigned char) tmp2;
                }
                break;
        //R // TMPByte von Adresse lesen // AC - TMPByte - Carry // if(idxReg<Adresse Lo) MCT++
        case 58:
                CHK_RDY
                if((Adresse&0xFF)>=(idxReg))
                {
                        src=Read(Adresse);
                        tmp2 = AC - src - ((SR & 0x01) ? 0 : 1);
                        if (IF_DECIMAL())
                        {
                                tmp = (AC & 0xF) - (src & 0xF) - ((SR & 0x1) ? 0 : 1);
                                if (tmp & 0x10) tmp = ((tmp - 6) & 0xF)| ((AC & 0xF0) - (src & 0xF0) - 0x10);
                                else tmp = (tmp & 0xF) | ((AC & 0xF0) - (src & 0xF0));
                                if (tmp & 0x100) tmp -= 0x60;
                                SET_CARRY(tmp2 < 0x100);
                                SET_SR_NZ(tmp2 & 0xFF);
                                SET_OVERFLOW(((AC ^ tmp2) & 0x80) && ((AC ^ src) & 0x80));
                                AC = (unsigned char) tmp;
                        }
                        else
                        {
                                SET_SR_NZ(tmp2 & 0xff);
                                SET_CARRY(tmp2 < 0x100);
                                SET_OVERFLOW(((AC ^ tmp2) & 0x80)
                                                                 && ((AC ^ src) & 0x80));
                                AC = (unsigned char) tmp2;
                        }
                        MCT++;
                }
                break;
        //R // TMPByte von SP+0x0100 holen
        case 59:
                CHK_RDY
                TMPByte=Read(SP + 0x0100);
                break;
        //W // PC-Adresse Hi nach SP+0x0100 schreiben // SP-1
        case 60:
                Write(SP+0x0100,GetPCHi);
                SP--;
                break;
        //W // PC-Adresse Lo nach SP+0x0100 schreiben // SP-1
        case 61:
                Write(SP+0x0100,GetPCLo);
                SP--;
                break;
        //R // TMPByte von SP+0x0100 holen // SP+1
        case 62:
                CHK_RDY
                TMPByte = Read(SP+0x0100);
                SP++;
                break;
        //R // PC-Adresse Lo von SP+0x0100 holen // SP+1
        case 63:
                CHK_RDY
                SetPCLo(Read(SP+0x0100));
                SP++;
                break;
        //R // PC-Adresse Hi von SP+0x0100 holen
        case 64:
                CHK_RDY
                SetPCHi(Read(SP+0x0100));
                break;
        //R // TMPByte von PC-Adresse laden // PC+1
        case 65:
                CHK_RDY
                TMPByte = Read(PC);
                PC++;
                break;
        //R // TMPByte von PC-Adresse lesen // AC and TMPByte // Set SR(NZ) // PC+1
        case 66:
                CHK_RDY
                TMPByte=Read(PC);
                AC &= TMPByte;
                SET_SR_NZ(AC);
                PC++;
                break;
        //R // TMPByte von Adresse lesen // AC and TMPByte // Set SR(NZ)
        case 67:
                CHK_RDY
                TMPByte=Read(Adresse);
                AC &= TMPByte;
                SET_SR_NZ(AC);
                break;
        //R // TMPByte von Adresse lesen // AC and TMPByte // Set SR(NZ) // if(idxReg<Adresse Lo) MCT++
        case 68:
                CHK_RDY
                if((Adresse&0xFF)>=(idxReg))
                {
                        TMPByte=Read(Adresse);
                        AC &= TMPByte;
                        SET_SR_NZ(AC);
                        MCT++;
                }
                break;
        //R // TMPByte von Adresse lesen // CarrayFalg=0
        case 69:
                CHK_RDY
                TMPByte = Read(PC);
                SR &= 0xFE;
                break;
        //R // TMPByte von Adresse lesen // DezimalFalg=0
        case 70:
                CHK_RDY
                TMPByte = Read(PC);
                SR &= 0xF7;
                break;
        //R // TMPByte von Adresse lesen // InterruptFalg=0
        case 71:
                CHK_RDY
                TMPByte = Read(PC);
                //
                LastOPC_CLI = true;
                break;
        //R // TMPByte von Adresse lesen // OverflowFalg=0
        case 72:
                CHK_RDY
                TMPByte = Read(PC);
                SR &= 0xBF;
                break;
        //R // TMPByte von Adresse lesen // CarrayFalg=1
        case 73:
                CHK_RDY
                TMPByte = Read(PC);
                SR |= 0x01;
                break;
        //R // TMPByte von Adresse lesen // DezimalFalg=1
        case 74:
                CHK_RDY
                TMPByte = Read(PC);
                SR |= 0x08;
                break;
        //R // TMPByte von Adresse lesen // InterruptFalg=1
        case 75:
                CHK_RDY
                TMPByte = Read(PC);
                SR |= 0x04;
                break;
        //R // TMPByte von Adresse lesen // BIT Operation
        case 76:
                CHK_RDY
                TMPByte = Read(Adresse);
                SET_ZERO(AC & TMPByte);
                SR = (TMPByte & 0xC0) | (SR & 0x3F);
                break;
        //W // AC nach Adresse schreiben
        case 77:
                Write(Adresse,AC);
                break;
        //W // XR nach Adresse schreiben
        case 78:
                Write(Adresse,XR);
                break;
        //W // YR nach Adresse schreiben
        case 79:
                Write(Adresse,YR);
                break;
        //R // AC von PC-Adresse lesen // Set SR(NZ) // PC+1
        case 80:
                CHK_RDY
                AC = Read(PC);
                SET_SR_NZ(AC);
                PC++;
                break;
        //R // AC von Adresse lesen // Set SR(NZ)
        case 81:
                CHK_RDY
                AC = Read(Adresse);
                SET_SR_NZ(AC);
                break;
        //R // AC von Adresse lesen // Set SR(NZ) // if(idxReg<Adresse Lo) MCT++
        case 82:
                CHK_RDY
                if((Adresse&0xFF)>=(idxReg))
                {
                        TMPByte=Read(Adresse);
                        AC = TMPByte;
                        SET_SR_NZ(AC);
                        MCT++;
                }
                break;
        //R // XR von PC-Adresse lesen // Set SR(NZ) // PC+1
        case 83:
                CHK_RDY
                XR = Read(PC);
                SET_SR_NZ(XR);
                PC++;
                break;
        //R // XR von Adresse lesen // Set SR(NZ)
        case 84:
                CHK_RDY
                XR = Read(Adresse);
                SET_SR_NZ(XR);
                break;
        //R // XR von Adresse lesen // Set SR(NZ) // if(idxReg<Adresse Lo) MCT++
        case 85:
                CHK_RDY
                if((Adresse&0xFF)>=(idxReg))
                {
                        TMPByte=Read(Adresse);
                        XR = TMPByte;
                        SET_SR_NZ(XR);
                        MCT++;
                }
                break;
        //R // YR von PC-Adresse lesen // Set SR(NZ) // PC+1
        case 86:
                CHK_RDY
                YR = Read(PC);
                SET_SR_NZ(YR);
                PC++;
                break;
        //R // YR von Adresse lesen // Set SR(NZ)
        case 87:
                CHK_RDY
                YR = Read(Adresse);
                SET_SR_NZ(YR);
                break;
        //R // YR von Adresse lesen // Set SR(NZ) // if(idxReg<Adresse Lo) MCT++
        case 88:
                CHK_RDY
                if((Adresse&0xFF)>=(idxReg))
                {
                        TMPByte=Read(Adresse);
                        YR = TMPByte;
                        SET_SR_NZ(YR);
                        MCT++;
                }
                break;
        //R // TMPByte von Adresse lesen // XR+1 // Set SR(NZ)
        case 89:
                CHK_RDY
                TMPByte = Read(PC);
                XR ++;
                SET_SR_NZ(XR);
                break;
        //R // TMPByte von Adresse lesen // YR+1 // Set SR(NZ)
        case 90:
                CHK_RDY
                TMPByte = Read(PC);
                YR ++;
                SET_SR_NZ(YR);
                break;
        //R // TMPByte von Adresse lesen // XR-1 // Set SR(NZ)
        case 91:
                CHK_RDY
                TMPByte = Read(PC);
                XR --;
                SET_SR_NZ(XR);
                break;
        //R // TMPByte von Adresse lesen // YR-1 // Set SR(NZ)
        case 92:
                CHK_RDY
                TMPByte = Read(PC);
                YR --;
                SET_SR_NZ(YR);
                break;
        //R // Illegale Opcode //
        case 93:
                //// Wird nie angesprungen !!! ////
                break;
        //R // PC LO von Adresse lesen // AdresseLo+1
        case 94:
                CHK_RDY
                SetPCLo(Read(Adresse));
                Adresse = (Adresse&0xFF00)|((Adresse+1)&0x00FF);
                break;
        //R // PC HI von Adresse lesen
        case 95:
                CHK_RDY
                SetPCHi(Read(Adresse));
                break;
        //R // PC LO von $FFFC lesen
        case 96:
                CHK_RDY
                SetPCLo(Read(0xFFFC));
                break;
        //R // PC HI von $FFFD lesen
        case 97:
                CHK_RDY
                SetPCHi(Read(0xFFFD));
                break;
        //R // TMPByte von PC-Adresse lesen // AC - TMPByte (AC wird nicht verÃ€ndert) // Set SR(NZC) // PC+1
        case 98:
                CHK_RDY
                TMPByte=Read(PC);
                tmp = AC - TMPByte;
                SET_CARRY(tmp < 0x100);
                SET_SIGN(tmp);
                SET_ZERO(tmp &= 0xFF);
                PC++;
                break;
        //R // TMPByte von Adresse lesen // AC - TMPByte (AC wird nicht verÃ€ndert) // Set SR(NZC)
        case 99:
                CHK_RDY
                TMPByte=Read(Adresse);
                tmp = AC - TMPByte;
                SET_CARRY(tmp < 0x100);
                SET_SIGN(tmp);
                SET_ZERO(tmp &= 0xFF);
                break;
        //R // TMPByte von Adresse lesen // AC - TMPByte (AC wird nicht verÃ€ndert) // if(idxReg<Adresse Lo) MCT++
        case 100:
                CHK_RDY
                if((Adresse&0xFF)>=(idxReg))
                {
                        TMPByte=Read(Adresse);
                        tmp = AC - TMPByte;
                        SET_CARRY(tmp < 0x100);
                        SET_SIGN(tmp);
                        SET_ZERO(tmp &= 0xFF);
                        MCT++;
                }
                break;
        //R // TMPByte von PC-Adresse lesen // XR - TMPByte (XR wird nicht verÃ€ndert) // Set SR(NZC) // PC+1
        case 101:
                CHK_RDY
                TMPByte=Read(PC);
                tmp = XR - TMPByte;
                SET_CARRY(tmp < 0x100);
                SET_SIGN(tmp);
                SET_ZERO(tmp &= 0xFF);
                PC++;
                break;
        //R // TMPByte von Adresse lesen // XR - TMPByte (XR wird nicht verÃ€ndert) // Set SR(NZC)
        case 102:
                CHK_RDY
                TMPByte=Read(Adresse);
                tmp = XR - TMPByte;
                SET_CARRY(tmp < 0x100);
                SET_SIGN(tmp);
                SET_ZERO(tmp &= 0xFF);
                break;
        //R // TMPByte von PC-Adresse lesen // YR - TMPByte (XR wird nicht verÃ€ndert) // Set SR(NZC) // PC+1
        case 103:
                CHK_RDY
                TMPByte=Read(PC);
                tmp = YR - TMPByte;
                SET_CARRY(tmp < 0x100);
                SET_SIGN(tmp);
                SET_ZERO(tmp &= 0xFF);
                PC++;
                break;
        //R // TMPByte von Adresse lesen // YR - TMPByte (XR wird nicht verÃ€ndert) // Set SR(NZC)
        case 104:
                CHK_RDY
                TMPByte=Read(Adresse);
                tmp = YR - TMPByte;
                SET_CARRY(tmp < 0x100);
                SET_SIGN(tmp);
                SET_ZERO(tmp &= 0xFF);
                break;
        //R // TMPByte von PC-Adresse lesen // AC XOR TMPByte // Set SR(NZC) // PC+1
        case 105:
                CHK_RDY
                TMPByte=Read(PC);
                AC^=TMPByte;
                SET_SR_NZ(AC);
                PC++;
                break;
        //R // TMPByte von Adresse lesen // AC XOR TMPByte // Set SR(NZC)
        case 106:
                CHK_RDY
                TMPByte=Read(Adresse);
                AC^=TMPByte;
                SET_SR_NZ(AC);
                break;
        //R // TMPByte von Adresse lesen // AC XOR TMPByte // if(idxReg<Adresse Lo) MCT++
        case 107:
                CHK_RDY
                if((Adresse&0xFF)>=(idxReg))
                {
                        TMPByte=Read(Adresse);
                        AC^=TMPByte;
                        SET_SR_NZ(AC);
                        MCT++;
                }
                break;
        //R // TMPByte von PC-Adresse holen // AC>>1 // Set SR(NZC)
        case 108:
                CHK_RDY
                TMPByte = Read(PC);
                SET_CARRY(AC&0x01);
                AC >>= 1;
                SET_SIGN(0);
                SET_ZERO(AC);
                break;
        //W // TMPByte nach Adresse schreiben // TMPByte>>1 // Set SR(NZC)
        case 109:
                Write(Adresse,TMPByte);
                SET_CARRY(TMPByte&0x01);
                TMPByte >>= 1;
                SET_SIGN(0);
                SET_ZERO(TMPByte);
                break;
        //R // TMPByte von PC-Adresse holen // C<-AC<<1<-C // Set SR(NZC)
        case 110:
                CHK_RDY
                TMPByte = Read(PC);
                tmp = AC;
                tmp <<= 1;
                if (IF_CARRY()) tmp |= 0x1;
                SET_CARRY(tmp > 0xff);
                tmp &= 0xff;
                SET_SIGN(tmp);
                SET_ZERO(tmp);
                AC = tmp;
                break;
        //W // TMPByte nach Adresse schreiben // C<-TMPByte<<1<-C // Set SR(NZC)
        case 111:
                Write(Adresse,TMPByte);
                tmp = TMPByte;
                tmp <<= 1;
                if (IF_CARRY()) tmp |= 0x1;
                SET_CARRY(tmp > 0xff);
                tmp &= 0xff;
                SET_SIGN(tmp);
                SET_ZERO(tmp);
                TMPByte = tmp;
                break;
        //R // TMPByte von PC-Adresse holen // C->AC>>1->C // Set SR(NZC)
        case 112:
                CHK_RDY
                TMPByte = Read(PC);
                carry_tmp=IF_CARRY();
                SET_CARRY(AC & 0x01);
                AC >>= 1;
                if(carry_tmp) AC |= 0x80;
                SET_SIGN(AC);
                SET_ZERO(AC);
                break;
        //W // TMPByte nach Adresse schreiben // C->TMPByte>>1->C // Set SR(NZC)
        case 113:
                Write(Adresse,TMPByte);
                carry_tmp=IF_CARRY();
                SET_CARRY(TMPByte & 0x01);
                TMPByte >>= 1;
                if(carry_tmp) TMPByte |= 0x80;
                SET_SIGN(TMPByte);
                SET_ZERO(TMPByte);
                break;
        //W // TMPByte nach Adresse schreiben // TMPByte+1 // Set SR(NZ)
        case 114:
                Write(Adresse,TMPByte);
                TMPByte++;
                SET_SR_NZ(TMPByte);
                break;
        //W // TMPByte nach Adresse schreiben // TMPByte-1 // Set SR(NZ)
        case 115:
                Write(Adresse,TMPByte);
                TMPByte--;
                SET_SR_NZ(TMPByte);
                break;
        //W // SR nach 0x100+SP schreiben // SP-- // IFlag setzen // BFlag lÃ¶schen
        case 116:
                SR&=239;
                Write(0x0100+SP,SR);
                SP--;
                SR|=4;
                break;
        //R // PC Lo von 0xFFFA holen
        case 117:
                CHK_RDY
                SetPCLo(Read(0xFFFA));
                break;
        //R // PC Hi von 0xFFFB holen
        case 118:
                CHK_RDY
                SetPCHi(Read(0xFFFB));
                break;
        //R // TMPByte von Adresse holen // Fix Adresse Hi MCT+1
        case 119:
                CHK_RDY
                if((Adresse&0xFF)>=(idxReg))
                {
                        TMPByte = Read(Adresse);
                        MCT++;
                }
                break;
        //W // TMPByte nach Adresse schreiben // Illegal [SLO]
        case 120:
                Write(Adresse,TMPByte);
                SET_CARRY(TMPByte&0x80);	// ASL MEMORY
                TMPByte <<= 1;
                AC |= TMPByte;				// ORA
                SET_SR_NZ(AC);
                break;
        //W // TMPByte nach Adresse schreiben // Illegal [RLA]
        case 121:
                Write(Adresse,TMPByte);
                tmp = TMPByte;
                tmp <<= 1;					// ROL
                if (IF_CARRY()) tmp |= 0x1;
                SET_CARRY(tmp > 0xFF);
                tmp &= 0xFF;
                TMPByte = tmp;
                AC &= TMPByte;				// AND
                SET_SR_NZ(AC);
                break;
        //W // TMPByte nach Adresse schreiben // Illegal [SRE]
        case 122:
                Write(Adresse,TMPByte);
                SET_CARRY(TMPByte & 0x01);	// LSR
                TMPByte >>= 1;
                AC ^= TMPByte;				// EOR
                SET_SR_NZ(AC);
                break;
        //W // TMPByte nach Adresse schreiben // Illegal [RRA]
        case 123:
                Write(Adresse,TMPByte);

                carry_tmp = IF_CARRY();		// ROR
                SET_CARRY(TMPByte & 0x01);
                TMPByte >>= 1;
                if(carry_tmp) TMPByte |= 0x80;

                tmp1 = TMPByte;				// ADC
                if (IF_DECIMAL())
                {
                        tmp = (AC & 0xF) + (tmp1 & 0xF) + (SR & 0x1);
                        if (tmp > 0x9) tmp += 0x6;
                        if (tmp <= 0x0F) tmp = (tmp & 0xF) + (AC & 0xF0) + (tmp1 & 0xF0);
                        else tmp = (tmp & 0xF) + (AC & 0xF0) + (tmp1 & 0xF0) + 0x10;
                        SET_ZERO(((AC + tmp1 + (SR & 0x1)) & 0xFF));
                        SET_SIGN(tmp & 0x80);
                        SET_OVERFLOW(((AC ^ tmp) & 0x80) && !((AC ^ tmp1) & 0x80));
                        if ((tmp & 0x1F0) > 0x90) tmp += 0x60;
                        SET_CARRY((tmp & 0xFF0) > 0xF0);
                }
                else
                {
                        tmp = tmp1 + AC + (SR & 0x01);
                        SET_SR_NZ(tmp & 0xff);
                        SET_OVERFLOW(!((AC ^ tmp1) & 0x80) && ((AC ^ tmp) & 0x80));
                        SET_CARRY(tmp > 0xFF);
                }
                AC = (unsigned char)tmp;
                break;
        //W // TMPByte nach Adresse schreiben // Illegal [DCP]
        case 124:
                Write(Adresse,TMPByte);
                TMPByte--;					//DEC

                tmp = AC - TMPByte;			//CMP
                SET_CARRY(tmp < 0x100);
                SET_SIGN(tmp);
                SET_ZERO(tmp &= 0xFF);
                break;
        //W // TMPByte nach Adresse schreiben // Illegal [ISB]
        case 125:
                Write(Adresse,TMPByte);
                TMPByte++;					//INC

                src=TMPByte;				//SBC
                tmp2 = AC - src - ((SR & 0x01) ? 0 : 1);
                if (IF_DECIMAL())
                {
                        tmp = (AC & 0xF) - (src & 0xF) - ((SR & 0x1) ? 0 : 1);
                        if (tmp & 0x10) tmp = ((tmp - 6) & 0xF)| ((AC & 0xF0) - (src & 0xF0) - 0x10);
                        else tmp = (tmp & 0xF) | ((AC & 0xF0) - (src & 0xF0));
                        if (tmp & 0x100) tmp -= 0x60;
                        SET_CARRY(tmp2 < 0x100);
                        SET_SR_NZ(tmp2 & 0xFF);
                        SET_OVERFLOW(((AC ^ tmp2) & 0x80) && ((AC ^ src) & 0x80));
                        AC = (unsigned char) tmp;
                }
                else
                {
                        SET_SR_NZ(tmp2 & 0xff);
                        SET_CARRY(tmp2 < 0x100);
                        SET_OVERFLOW(((AC ^ tmp2) & 0x80)
                                                         && ((AC ^ src) & 0x80));
                        AC = (unsigned char) tmp2;
                }
                break;
        //R // AC von Adresse lesen // AC -> XR // Set SR(NZ) // Illegal [LAX]
        case 126:
                CHK_RDY
                AC = Read(Adresse);
                XR = AC;
                SET_SR_NZ(AC);
                break;
        //R // AC von Adresse lesen // AC -> XR // Set SR(NZ) // if(idxReg<Adresse Lo) MCT++ // Illegal [LAX]
        case 127:
                CHK_RDY
                if((Adresse&0xFF)>=(idxReg))
                {
                        TMPByte=Read(Adresse);
                        AC = TMPByte;
                        XR = AC;
                        SET_SR_NZ(AC);
                        MCT++;
                }
                break;
        //W // TMPByte = AC & XR // TMPByte nach Adresse schreiben // Set SR(NZ) // Illegal [SAX]
        case 128:
                TMPByte = AC & XR;
                Write(Adresse,TMPByte);
                break;
        //R // Illegal [ASR]
        case 129:
                CHK_RDY
                AC &= Read(PC);				// AND

                SET_CARRY(AC & 0x01);		// LSR
                AC >>= 1;
                SET_SIGN(0);
                SET_ZERO(AC);
                PC++;
                break;
        //R // Illegal [ARR]
        case 130:
                CHK_RDY
                tmp2 = Read(PC) & AC;
                AC = ((SR & 0x01) ? (tmp2 >> 1) | 0x80 : tmp2 >> 1);
                if (!IF_DECIMAL())
                {
                        SET_SR_NZ(AC);
                        SET_CARRY(AC & 0x40);
                        SET_OVERFLOW(!!((AC & 0x40) ^ ((AC & 0x20) << 1)));
                }
                else
                {
                        SET_SIGN((SR & 0x01) ? 0x80 : 0);
                        SET_ZERO(AC);
                        SET_OVERFLOW(!!((tmp2 ^ AC) & 0x40));
                        if ((tmp2 & 0x0F) + (tmp2 & 0x01) > 5) AC = (AC & 0xF0) | ((AC + 6) & 0x0F);
                        SET_CARRY(((tmp2 + (tmp2 & 0x10)) & 0x1F0)>0x50);
                        if(IF_CARRY()) AC += 0x60;
                }
                PC++;
                break;
        //R // Illegal [ANE]
        case 131:
                CHK_RDY
                TMPByte = Read(PC);
                AC = (AC | 0xEE) & XR & TMPByte;
                SET_SR_NZ(AC);
                PC++;
                break;
        //R // Illegal [LXA]
        case 132:
                CHK_RDY
                TMPByte = Read(PC);
                AC = XR = ((AC | 0xEE) & TMPByte);
                SET_SR_NZ(AC);
                PC++;
                break;
        //R // Illegal [SBX]
        case 133:
                CHK_RDY
                tmp = Read(PC);
                tmp = (AC & XR) - tmp;
                SET_CARRY(tmp < 0x100);
                XR = tmp & 0xFF;
                SET_SR_NZ(XR);
                PC++;
                break;
        //W // Illegal [SHY]
        case 134:
                Write(Adresse,YR & ((((Adresse) + XR) >> 8)));
                break;
        //W // Illegal [SHX]
        case 135:
                Write(Adresse,XR & ((((Adresse) + YR) >> 8)));
                break;
        //W // Illegal [SHA]
        case 136:
                Write(Adresse,AC & XR & ((((Adresse) + YR) >> 8)));
                break;
        //W // Illegal [SHS]
        case 137:
                Write(Adresse,AC & XR & ((((Adresse) + YR) >> 8)));
                SP = AC & XR;
                break;
        //R // Illegal [ANC]
        case 138:
                CHK_RDY
                AC &= Read(PC);
                SET_SR_NZ(AC);
                SET_CARRY(SR&0x80);
                PC++;
                break;
        //R // Illegal [LAE]
        case 139:
                CHK_RDY
                TMPByte = Read(Adresse);
                AC = XR = SP = SP & (TMPByte);
                SET_SR_NZ(AC);
                break;
        //R // Illegal [LAE]
        case 140:
                CHK_RDY
                if((Adresse&0xFF)>=(idxReg))
                {
                        TMPByte=Read(Adresse);
                        AC = XR = SP = SP & (TMPByte);
                        SET_SR_NZ(AC);
                        MCT++;
                }
                break;
        }
        MCT++;

        if(*MCT == 0)
        {
            /*
            if(NMIState == true)
            {
                isNMI = true;
                return false;
            }
            else if((Interrupts[VIC_IRQ] || Interrupts[CIA_IRQ] || Interrupts[REU_IRQ]) && ((SR&4)==0))
            {
                isIRQ = true;
                return false;
            }
            */

            AktOpcodePC = PC;
            if(Breakpoints[PC] & 1)
            {
                    *BreakStatus |=1;
                    BreakWerte[0] = PC;
            }
            if(Breakpoints[AC] & 2)
            {
                    *BreakStatus |=2;
                    BreakWerte[1] = AC;
            }
            if(Breakpoints[XR] & 4)
            {
                    *BreakStatus |=4;
                    BreakWerte[2] = XR;
            }
            if(Breakpoints[YR] & 8)
            {
                    *BreakStatus |=8;
                    BreakWerte[3] = YR;
            }

            if(ResetReadyAdr == PC)
            {
                *ResetReady = true;
            }
            return true;
        }
        else return false;
    }
    return false;
}

MOS6510_PORT::MOS6510_PORT(void)
{
	Reset();
}

MOS6510_PORT::~MOS6510_PORT(void)
{
}

void MOS6510_PORT::Reset(void)
{
    DATA = 0x3f;
    DATA_OUT = 0x3f;
    DATA_READ = 0x3f;
    DIR = 0;
    DIR_READ = 0;
    DATA_SET_BIT6 = 0;
    DATA_SET_BIT7 = 0;
    DATA_FALLOFF_BIT6 = 0;
    DATA_FALLOFF_BIT7 = 0;
}

void MOS6510_PORT::ConfigChanged(int tape_sense, int caps_sense,unsigned char pullup)
{
	TAPE_SENSE = !!(tape_sense);
	/* Tape Motor Status.  */
	static unsigned char OLD_DATA_OUT = 0xFF;
	/* Tape Write Line Status */
	static unsigned char OLD_WRITE_BIT = 0xFF;

    DATA_OUT = (DATA_OUT & ~DIR)|(DATA & DIR);
    DATA_READ = (DATA | ~DIR) & (DATA_OUT | pullup);

    if ((pullup & 0x40) && !caps_sense) DATA_READ &= 0xBF;
	if (!(DIR & 0x20)) DATA_READ &= 0xDF;
    if (tape_sense && !(DIR & 0x10))DATA_READ &= 0xEF;
    if (((DIR & DATA) & 0x20) != OLD_DATA_OUT)
	{
        OLD_DATA_OUT = (DIR & DATA) & 0x20;
        DATASETTE_MOTOR = !OLD_DATA_OUT;
    }

    if (((~DIR | DATA) & 0x8) != OLD_WRITE_BIT) 
	{
        OLD_WRITE_BIT = (~DIR | DATA) & 0x8;
        //datasette_toggle_write_bit((~pport.dir | pport.data) & 0x8);
    }
    DIR_READ = DIR;
}
/*
bool MOS6510_PORT::SaveFreez(FILE* File)
{
	fwrite(&DIR,1,sizeof(DIR),File);
	fwrite(&DATA,1,sizeof(DATA),File);
	fwrite(&DIR_READ,1,sizeof(DIR_READ),File);
	fwrite(&DATA_READ,1,sizeof(DATA_READ),File);
	fwrite(&DATA_OUT,1,sizeof(DATA_OUT),File);
	fwrite(&DATA_SET_CLK_BIT6,1,sizeof(DATA_SET_CLK_BIT6),File);
	fwrite(&DATA_SET_CLK_BIT7,1,sizeof(DATA_SET_CLK_BIT7),File);
	fwrite(&DATA_SET_BIT6,1,sizeof(DATA_SET_BIT6),File);
	fwrite(&DATA_SET_BIT7,1,sizeof(DATA_SET_BIT7),File);
	fwrite(&DATA_FALLOFF_BIT6,1,sizeof(DATA_FALLOFF_BIT6),File);
	fwrite(&DATA_FALLOFF_BIT7,1,sizeof(DATA_FALLOFF_BIT7),File);
	return true;
}

bool MOS6510_PORT::LoadFreez(FILE* File,unsigned short Version)
{
	switch(Version)
	{
	case 0x0100:
	case 0x0101:
		fread(&DIR,1,sizeof(DIR),File);
		fread(&DATA,1,sizeof(DATA),File);
		fread(&DIR_READ,1,sizeof(DIR_READ),File);
		fread(&DATA_READ,1,sizeof(DATA_READ),File);
		fread(&DATA_OUT,1,sizeof(DATA_OUT),File);
		fread(&DATA_SET_CLK_BIT6,1,sizeof(DATA_SET_CLK_BIT6),File);
		fread(&DATA_SET_CLK_BIT7,1,sizeof(DATA_SET_CLK_BIT7),File);
		fread(&DATA_SET_BIT6,1,sizeof(DATA_SET_BIT6),File);
		fread(&DATA_SET_BIT7,1,sizeof(DATA_SET_BIT7),File);
		fread(&DATA_FALLOFF_BIT6,1,sizeof(DATA_FALLOFF_BIT6),File);
		fread(&DATA_FALLOFF_BIT7,1,sizeof(DATA_FALLOFF_BIT7),File);
		break;
	}
	return true;
}
*/
