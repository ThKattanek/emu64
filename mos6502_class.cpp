//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: mos6502_class.cpp                     //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 31.12.2013                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "mos6502_class.h"
#include "micro_code_tbl_6502.h"

MOS6502::MOS6502(void)
{
    ReadProcTbl = 0;
    WriteProcTbl = 0;

    MCT = ((unsigned char*)MicroCodeTable6502 + (0x100*MCTItemSize));
    Pointer = 0;
    Adresse = 0x0000;
    BranchAdresse = 0x0000;
    AktOpcodePC = 0x0100;
    JAMFlag = false;
    TMPByte = 0;    
    PC = 0;
    AC = 0;
    XR = 0;
    YR = 0;
    SP = 0;
    SR = 32;

    IRQLine = 0;
    for(int i=0;i<5;i++) IRQLinePuffer[i] = 0;
}

MOS6502::~MOS6502(void)
{
}

void MOS6502::Reset(void)
{
    SR=0x24;
    SP=0;
    PC=0;
    AC=0;
    XR=0;
    YR=0;

    IRQLine = 0;

    MCT = ((unsigned char*)MicroCodeTable6502 + (0x100*MCTItemSize));
    JAMFlag = false;
    AktOpcodePC = PC;
    PC++;
}

void MOS6502::TriggerInterrupt(int typ)
{
    switch (typ)
    {
    case VIA1_IRQ:
        IRQLine |= 0x01;
        break;
    case VIA2_IRQ:
        IRQLine |= 0x02;
        break;
    }
}

void MOS6502::ClearInterrupt(int typ)
{
    switch (typ)
    {
    case VIA1_IRQ:
        IRQLine &= ~0x01;
        break;
    case VIA2_IRQ:
        IRQLine &= ~0x02;
        break;
    }
}

void MOS6502::ClearJAMFlag(void)
{
    JAMFlag = false;
}

void MOS6502::SetRegister(REG_STRUCT *reg)
{
    if(reg == 0) return;

    unsigned char mask = reg->REG_MASK;
    if((mask&1) == 1)
    {
        PC = reg->PC;
        MCT = ((unsigned char*)MicroCodeTable6502 + 6);
    }
    mask>>=1;
    if((mask&1) == 1) AC = (unsigned char)reg->AC;
    mask>>=1;
    if((mask&1) == 1) XR = (unsigned char)reg->XR;
    mask>>=1;
    if((mask&1) == 1) YR = (unsigned char)reg->YR;
    mask>>=1;
    if((mask&1) == 1) SP = (unsigned char)reg->SP;
    mask>>=1;
    if((mask&1) == 1) SR = (unsigned char)reg->SR | 32;
}

void MOS6502::GetRegister(REG_STRUCT *reg)
{
    if(reg == 0) return;

    unsigned char mask = reg->REG_MASK;
    if((mask&1) == 1) reg->PC = PC;
    mask>>=1;
    if((mask&1) == 1) reg->AC = AC;
    mask>>=1;
    if((mask&1) == 1) reg->XR = XR;
    mask>>=1;
    if((mask&1) == 1) reg->YR = YR;
    mask>>=1;
    if((mask&1) == 1) reg->SP = SP;
    mask>>=1;
    if((mask&1) == 1) reg->SR = SR;
    mask>>=1;
    if((mask&1) == 1)
    {
        reg->IRQ = Read(0xFFFE);
        reg->IRQ |= Read(0xFFFF)<<8;
    }
    mask>>=1;
    if((mask&1) == 1)
    {
        reg->NMI = Read(0xFFFA);
        reg->NMI |= Read(0xFFFB)<<8;
    }
    reg->_0314 = Read(0x314);
    reg->_0314 |= Read(0x315)<<8;
    reg->_0318 = Read(0x318);
    reg->_0318 |= Read(0x319)<<8;
}

void MOS6502::GetInterneRegister(IREG_STRUCT* ireg)
{
    if(ireg == 0) return;
    ireg->AktOpcodePC = AktOpcodePC;
    ireg->AktOpcode = AktOpcode;
    ireg->AktMicroCode = *MCT;
    ireg->JAMFlag = JAMFlag;
    ireg->Pointer = Pointer;
    ireg->Adresse = Adresse;
    ireg->BranchAdresse = BranchAdresse;
    ireg->TMPByte = TMPByte;
    ireg->IRQ = IRQLine;
    ireg->RESET = *RESET;
}

void MOS6502::SET_SR_BIT6(void)
{
    SR |= 64;
}

inline void MOS6502::SET_SR_NZ(unsigned char wert)
{
    SR = (SR&0x7D)|(wert&0x80);
    if (wert==0) SR|=2;
}

inline void MOS6502::SET_SIGN(unsigned char wert)
{
    SR = (SR&127)|(wert&0x80);
}

inline void MOS6502::SET_ZERO(unsigned char wert)
{
    if(wert==0) SR|=2;
    else SR&=0xFD;
}

inline void MOS6502::SET_CARRY(unsigned char status)
{
    if (status!=0) SR|=1;
    else SR&=0xFE;
}

inline bool MOS6502::IF_CARRY(void)
{
    if(SR&1) return true;
    return false;
}

inline bool MOS6502::IF_DECIMAL(void)
{
    if(SR&8) return true;
    return false;
}

inline void MOS6502::SET_OVERFLOW(bool status)
{
    if (status!=0) SR|=64;
    else SR&=0xBF;
}

inline unsigned char MOS6502::Read(unsigned short adresse)
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

inline void MOS6502::Write(unsigned short adresse, unsigned char wert)
{
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

bool MOS6502::OneZyklus(void)
{
    if(!*RESET)
    {
        IRQLine = 0;
        SR=0x24;
        JAMFlag = false;
        AktOpcodePC = PC;
        PC++;
        MCT = ((unsigned char*)MicroCodeTable6502 + (0x100*MCTItemSize));
        AktOpcode = 0x100;
    }

    switch(*MCT)
    {
    //R // Feetch Opcode
    case 0:
        if(JAMFlag) return false;

        if(IRQLinePuffer[0] > 0 && ((SR&4)==0))
        {
            MCT = ((unsigned char*)MicroCodeTable6502 + (0x101*MCTItemSize));
            AktOpcode = 0x101;
            return false;
        }
        else
        {
            MCT = ((unsigned char*)MicroCodeTable6502 + (Read(PC)*MCTItemSize));
            AktOpcode = ReadProcTbl[(AktOpcodePC)>>8](AktOpcodePC);

            *HistoryPointer = *HistoryPointer+1;
            History[*HistoryPointer] = AktOpcodePC;

            PC++;
            return false;
        }
        break;

    //R // Lesen von PC-Adresse und verwerfen // PC++
    case 1:
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
        SetPCLo(Read(0xFFFE));
        break;
    //R // PC Hi von 0xFFFF holen
    case 6:
        SetPCHi(Read(0xFFFF));
        break;
    //R // Pointer von PC-Adresse holen // PC++
    case 7:
        Pointer = Read(PC);
        PC++;
        break;
    //R // Lesen von Pointer und verwerfen // Pointer+XR
    case 8:
        Read((unsigned short)Pointer);
        Pointer += XR;
        break;
    //R // Adresse Lo von Pointer-Adresse holen // Pointer++
    case 9:

        SetAdresseLo(Read((unsigned short)Pointer));
        Pointer++;
        break;
    //R // Adresse Hi von Pointer-Adresse holen //
    case 10:
        SetAdresseHi(Read((unsigned short)Pointer));
        break;
    //R // TMPByte von Adresse holen // AC or TMPByte // Set SR(NZ)
    case 11:
        TMPByte = Read(Adresse);
        AC |= TMPByte;
        SET_SR_NZ(AC);
        break;
    //R // JAM
    case 12:
        JAMFlag = true;
        //IRQOld = true;
        //NMIOld = true;
        //IRQCounter=0;
        //NMICounter=0;
        //*IRQ = false;
        //*NMI = false;
        //SR = 0x04;
        //MCT = ((unsigned char*)MicroCodeTable6502 + (0x100*MCTItemSize));
        return 0;
    break;
    //R // TMPByte von Adresse holen
    case 13:
        TMPByte = Read(Adresse);
        break;
    //W // TMPByte nach Adresse schreiben // ASL MEMORY // ORA
    case 14:
    Write(Adresse,TMPByte);

    SET_CARRY(TMPByte&0x80);	// ASL MEMORY
    TMPByte <<= 1;
    TMPByte &= 0xFF;

    AC|=TMPByte;                // ORA
    SET_SR_NZ(AC);

    break;
    //W // TMPByte nach Adresse schreiben
    case 15:
        Write(Adresse,TMPByte);
        break;
    //R // Adresse Hi = 0 // Adresse Lo von PC-Adresse holen // PC++
    case 16:
        Adresse = Read(PC);
        PC++;
        break;
    //R // TMPByte von Adresse lesen // Adresse Lo + YR
    case 17:
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
        TMPByte = Read(PC);
        break;
    //W // SR nach SP+0x0100 schreiben // SP-1
    case 20:
        Write(SP+0x0100,SR|16);
        SP--;
        break;
    //R // TMPByte von PC-Adresse holen // AC or TMPByte // PC+1
    case 21:
        TMPByte = Read(PC);
        AC|=TMPByte;
        SET_SR_NZ(AC);
        PC++;
        break;
    //R // TMPByte von PC-Adresse holen // AC<<1 // Set SR(NZC)
    case 22:
        TMPByte = Read(PC);
        SET_CARRY(AC&0x80);
        AC <<= 1;
        AC &= 0xFF;
        SET_SIGN(AC);
        SET_ZERO(AC);
        break;
    //R // TMPByte von PC-Adresse holen // AC and TMPByte // Set SR(NZC) // PC+1
    case 23:
        TMPByte = Read(PC);
        AC&=TMPByte;
        SET_SR_NZ(AC);
        SR&=0xFE;
        SR|=AC>>7;
        PC++;
        break;
    //R // Adresse Lo von PC-Adresse holen // PC+1
    case 24:
        SetAdresseLo(Read(PC));
        PC++;
        break;
    //R // Adresse Hi von PC-Adresse holen // PC+1
    case 25:
        SetAdresseHi(Read(PC));
        PC++;
        break;
    //R // TMPByte von PC-Adresse holen // PC+1 // SR(N) auf FALSE prüfen (BPL)
    case 26:
        TMPByte = Read(PC);
        PC++;
        if((SR&0x80)!=0x00) MCT+=2;
        break;
    //R // TMPByte von PC-Adresse holen // PC+1 // SR(N) auf TRUE prüfen (BMI)
    case 27:
        TMPByte = Read(PC);
        PC++;
        if((SR&0x80)!=0x80) MCT+=2;
        break;
    //R // TMPByte von PC-Adresse holen // PC+1 // SR(V) auf FALSE prüfen (BVC)
    case 28:
        TMPByte = Read(PC);
        PC++;
        if((SR&0x40)!=0x00) MCT+=2;
        break;
    //R // TMPByte von PC-Adresse holen // PC+1 // SR(V) auf TRUE prüfen (BVS)
    case 29:
        TMPByte = Read(PC);
        PC++;
        if((SR&0x40)!=0x40) MCT+=2;
        break;
    //R // TMPByte von PC-Adresse holen // PC+1 // SR(C) auf FALSE prüfen (BCC)
    case 30:
        TMPByte = Read(PC);
        PC++;
        if((SR&0x01)!=0x00) MCT+=2;
        break;
    //R // TMPByte von PC-Adresse holen // PC+1 // SR(C) auf TRUE prüfen (BCS)
    case 31:
        TMPByte = Read(PC);
        PC++;
        if((SR&0x01)!=0x01) MCT+=2;
        break;
    //R // TMPByte von PC-Adresse holen // PC+1 // SR(Z) auf FALSE prüfen (BNE)
    case 32:
        TMPByte = Read(PC);
        PC++;
        if((SR&0x02)!=0x00) MCT+=2;
        break;
    //R // TMPByte von PC-Adresse holen // PC+1 // SR(Z) auf TRUE prüfen (BEQ)
    case 33:
        TMPByte = Read(PC);
        PC++;
        if((SR&0x02)!=0x02) MCT+=2;
        break;
    //R // Lesen von PC-Adresse und verwerfen // BranchAdresse=PC+TMPByte
    case 34:
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
        Read(PC);
        PC = BranchAdresse;
        break;
    //R // Adresse Hi von Pointer-Adresse holen // Adresse+YR
    case 36:
        SetAdresseHi(Read((unsigned short)Pointer));
        Adresse += YR;
        idxReg = YR;
        break;
    //R // TMPByte von Adresse holen // Fix Adresse Hi MCT+1 // AC or TMPByte //
    case 37:
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
        SetAdresseHi(Read(PC));
        PC = Adresse;
        break;
    //R // Lesen von PC-Adresse und verwerfen // XR=AC // Set SR(NZ)
    case 39:
        Read(PC);
        XR = AC;
        SET_SR_NZ(XR);
        break;
    //R // Lesen von PC-Adresse und verwerfen // YR=AC // Set SR(NZ)
    case 40:
        Read(PC);
        YR = AC;
        SET_SR_NZ(YR);
        break;
    //R // Lesen von PC-Adresse und verwerfen // XR=SP // Set SR(NZ)
    case 41:
        Read(PC);
        XR = SP;
        SET_SR_NZ(XR);
        break;
    //R // Lesen von PC-Adresse und verwerfen // AC=XR // Set SR(NZ)
    case 42:
        Read(PC);
        AC = XR;
        SET_SR_NZ(AC);
        break;
    //R // Lesen von PC-Adresse und verwerfen // SP=XR
    case 43:
        Read(PC);
        SP = XR;
        break;
    //R // Lesen von PC-Adresse und verwerfen // AC=YR // Set SR(NZ)
    case 44:
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
        AC = Read(SP+0x0100);
        SP++;
        break;
    //R // AC von SP+0x0100 lesen // Set SR(NZ)
    case 47:
        AC = Read(SP+0x0100);
        SET_SR_NZ(AC);
        break;
    //R // SR von SP+0x0100 lesen // SP+1
    case 48:
        SR = Read(SP+0x0100)|32;
        SP++;
        break;
    //R // SR von SP+0x0100 lesen
    case 49:
        SR = Read(SP+0x0100)|32;
        break;
    //R // TMPByte von PC-Adresse lesen // AC + TMPByte + Carry // PC+1
    case 50:
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
        TMPByte = Read(Adresse);
        Adresse += XR;
        Adresse &= 0xFF;
        break;
    //R // Adresse Hi von PC-Adresse holen // Adresse+XR  // PC+1 //
    case 53:
        SetAdresseHi(Read(PC));
        Adresse += XR;
        idxReg = XR;
        PC++;
        break;
    //R // Adresse Hi von PC-Adresse holen // Adresse+YR  // PC+1 //
    case 54:
        SetAdresseHi(Read(PC));
        Adresse += YR;
        idxReg = YR;
        PC++;
        break;
    //R // TMPByte von Adresse lesen // AC + TMPByte + Carry // if(idxReg<Adresse Lo) MCT++
    case 55:
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
            SET_OVERFLOW(((AC ^ tmp2) & 0x80) && ((AC ^ src) & 0x80));
            AC = (unsigned char) tmp2;
        }
        PC++;
        break;
    //R // TMPByte von Adresse lesen // AC - TMPByte - Carry
    case 57:
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
            SET_OVERFLOW(((AC ^ tmp2) & 0x80) && ((AC ^ src) & 0x80));
            AC = (unsigned char) tmp2;
        }
        break;
    //R // TMPByte von Adresse lesen // AC - TMPByte - Carry // if(idxReg<Adresse Lo) MCT++
    case 58:
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
                SET_OVERFLOW(((AC ^ tmp2) & 0x80) && ((AC ^ src) & 0x80));
                AC = (unsigned char) tmp2;
            }
            MCT++;
        }
        break;
    //R // TMPByte von SP+0x0100 holen
    case 59:
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
        TMPByte = Read(SP+0x0100);
        SP++;
        break;
    //R // PC-Adresse Lo von SP+0x0100 holen // SP+1
    case 63:
        SetPCLo(Read(SP+0x0100));
        SP++;
        break;
    //R // PC-Adresse Hi von SP+0x0100 holen
    case 64:
        SetPCHi(Read(SP+0x0100));
        break;
    //R // TMPByte von PC-Adresse laden // PC+1
    case 65:
        TMPByte = Read(PC);
        PC++;
        break;
    //R // TMPByte von PC-Adresse lesen // AC and TMPByte // Set SR(NZ) // PC+1
    case 66:
        TMPByte=Read(PC);
        AC &= TMPByte;
        SET_SR_NZ(AC);
        PC++;
        break;
    //R // TMPByte von Adresse lesen // AC and TMPByte // Set SR(NZ)
    case 67:
        TMPByte=Read(Adresse);
        AC &= TMPByte;
        SET_SR_NZ(AC);
        break;
    //R // TMPByte von Adresse lesen // AC and TMPByte // Set SR(NZ) // if(idxReg<Adresse Lo) MCT++
    case 68:
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
        TMPByte = Read(PC);
        SR &= 0xFE;
        break;
    //R // TMPByte von Adresse lesen // DezimalFalg=0
    case 70:
        TMPByte = Read(PC);
        SR &= 0xF7;
        break;
    //R // TMPByte von Adresse lesen // InterruptFalg=0
    case 71:
        TMPByte = Read(PC);
        SR &= 0xFB;
        break;
    //R // TMPByte von Adresse lesen // OverflowFalg=0
    case 72:
        TMPByte = Read(PC);
        SR &= 0xBF;
        break;
    //R // TMPByte von Adresse lesen // CarrayFalg=1
    case 73:
        TMPByte = Read(PC);
        SR |= 0x01;
        break;
    //R // TMPByte von Adresse lesen // DezimalFalg=1
    case 74:
        TMPByte = Read(PC);
        SR |= 0x08;
        break;
    //R // TMPByte von Adresse lesen // InterruptFalg=1
    case 75:
        TMPByte = Read(PC);
        SR |= 0x04;
        break;
    //R // TMPByte von Adresse lesen // BIT Operation
    case 76:
        TMPByte = Read(Adresse);
        SET_SR_NZ(AC & TMPByte);
        SR = (TMPByte&192)|(SR&63);
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
        AC = Read(PC);
        SET_SR_NZ(AC);
        PC++;
        break;
    //R // AC von Adresse lesen // Set SR(NZ)
    case 81:
        AC = Read(Adresse);
        SET_SR_NZ(AC);
        break;
    //R // AC von Adresse lesen // Set SR(NZ) // if(idxReg<Adresse Lo) MCT++
    case 82:
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
        XR = Read(PC);
        SET_SR_NZ(XR);
        PC++;
        break;
    //R // XR von Adresse lesen // Set SR(NZ)
    case 84:
        XR = Read(Adresse);
        SET_SR_NZ(XR);
        break;
    //R // XR von Adresse lesen // Set SR(NZ) // if(idxReg<Adresse Lo) MCT++
    case 85:
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
        YR = Read(PC);
        SET_SR_NZ(YR);
        PC++;
        break;
    //R // YR von Adresse lesen // Set SR(NZ)
    case 87:
        YR = Read(Adresse);
        SET_SR_NZ(YR);
        break;
    //R // YR von Adresse lesen // Set SR(NZ) // if(idxReg<Adresse Lo) MCT++
    case 88:
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
        TMPByte = Read(PC);
        XR ++;
        SET_SR_NZ(XR);
        break;
    //R // TMPByte von Adresse lesen // YR+1 // Set SR(NZ)
    case 90:
        TMPByte = Read(PC);
        YR ++;
        SET_SR_NZ(YR);
        break;
    //R // TMPByte von Adresse lesen // XR-1 // Set SR(NZ)
    case 91:
        TMPByte = Read(PC);
        XR --;
        SET_SR_NZ(XR);
        break;
    //R // TMPByte von Adresse lesen // YR-1 // Set SR(NZ)
    case 92:
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
        SetPCLo(Read(Adresse));
        Adresse = (Adresse&0xFF00)|((Adresse+1)&0x00FF);
        break;
    //R // PC HI von Adresse lesen
    case 95:
        SetPCHi(Read(Adresse));
        break;
    //R // PC LO von $FFFC lesen
    case 96:
        SetPCLo(Read(0xFFFC));
        break;
    //R // PC HI von $FFFD lesen
    case 97:
        SetPCHi(Read(0xFFFD));
        break;
    //R // TMPByte von PC-Adresse lesen // AC - TMPByte (AC wird nicht verändert) // Set SR(NZC) // PC+1
    case 98:
        TMPByte=Read(PC);
        tmp = AC - TMPByte;
        SET_CARRY(tmp < 0x100);
        SET_SIGN(tmp);
        SET_ZERO(tmp &= 0xFF);
        PC++;
        break;
    //R // TMPByte von Adresse lesen // AC - TMPByte (AC wird nicht verändert) // Set SR(NZC)
    case 99:
        TMPByte=Read(Adresse);
        tmp = AC - TMPByte;
        SET_CARRY(tmp < 0x100);
        SET_SIGN(tmp);
        SET_ZERO(tmp &= 0xFF);
        break;
    //R // TMPByte von Adresse lesen // AC - TMPByte (AC wird nicht verändert) // if(idxReg<Adresse Lo) MCT++
    case 100:
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
    //R // TMPByte von PC-Adresse lesen // XR - TMPByte (XR wird nicht verändert) // Set SR(NZC) // PC+1
    case 101:
        TMPByte=Read(PC);
        tmp = XR - TMPByte;
        SET_CARRY(tmp < 0x100);
        SET_SIGN(tmp);
        SET_ZERO(tmp &= 0xFF);
        PC++;
        break;
    //R // TMPByte von Adresse lesen // XR - TMPByte (XR wird nicht verändert) // Set SR(NZC)
    case 102:
        TMPByte=Read(Adresse);
        tmp = XR - TMPByte;
        SET_CARRY(tmp < 0x100);
        SET_SIGN(tmp);
        SET_ZERO(tmp &= 0xFF);
        break;
    //R // TMPByte von PC-Adresse lesen // YR - TMPByte (XR wird nicht verändert) // Set SR(NZC) // PC+1
    case 103:
        TMPByte=Read(PC);
        tmp = YR - TMPByte;
        SET_CARRY(tmp < 0x100);
        SET_SIGN(tmp);
        SET_ZERO(tmp &= 0xFF);
        PC++;
        break;
    //R // TMPByte von Adresse lesen // YR - TMPByte (XR wird nicht verändert) // Set SR(NZC)
    case 104:
        TMPByte=Read(Adresse);
        tmp = YR - TMPByte;
        SET_CARRY(tmp < 0x100);
        SET_SIGN(tmp);
        SET_ZERO(tmp &= 0xFF);
        break;
    //R // TMPByte von PC-Adresse lesen // AC XOR TMPByte // Set SR(NZC) // PC+1
    case 105:
        TMPByte=Read(PC);
        AC^=TMPByte;
        SET_SR_NZ(AC);
        PC++;
        break;
    //R // TMPByte von Adresse lesen // AC XOR TMPByte // Set SR(NZC)
    case 106:
        TMPByte=Read(Adresse);
        AC^=TMPByte;
        SET_SR_NZ(AC);
        break;
    //R // TMPByte von Adresse lesen // AC XOR TMPByte // if(idxReg<Adresse Lo) MCT++
    case 107:
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
    //W // SR nach 0x100+SP schreiben // SP-- // IFlag setzen // BFlag löschen
    case 116:
        SR&=239;
        Write(0x0100+SP,SR);
        SP--;
        SR|=4;
        break;
    //R // PC Lo von 0xFFFA holen
    case 117:
        SetPCLo(Read(0xFFFA));
        break;
    //R // PC Hi von 0xFFFB holen
    case 118:
        SetPCHi(Read(0xFFFB));
        break;
    //R // TMPByte von Adresse holen // Fix Adresse Hi MCT+1
    case 119:
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
        AC = Read(Adresse);
        XR = AC;
        SET_SR_NZ(AC);
        break;
    //R // AC von Adresse lesen // AC -> XR // Set SR(NZ) // if(idxReg<Adresse Lo) MCT++ // Illegal [LAX]
    case 127:
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
        AC &= Read(PC);				// AND

        SET_CARRY(AC & 0x01);		// LSR
        AC >>= 1;
        SET_SIGN(0);
        SET_ZERO(AC);
        PC++;
        break;
    //R // Illegal [ARR]
    case 130:
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
        TMPByte = Read(PC);
        AC = (AC | 0xEE) & XR & TMPByte;
        SET_SR_NZ(AC);
        PC++;
        break;
    //R // Illegal [LXA]
    case 132:
        TMPByte = Read(PC);
        AC = XR = ((AC | 0xEE) & TMPByte);
        SET_SR_NZ(AC);
        PC++;
        break;
    //R // Illegal [SBX]
    case 133:
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
        AC &= Read(PC);
        SET_SR_NZ(AC);
        SET_CARRY(SR&0x80);
        PC++;
        break;
    //R // Illegal [LAE]
    case 139:
        TMPByte = Read(Adresse);
        AC = XR = SP = SP & (TMPByte);
        SET_SR_NZ(AC);
        break;
    //R // Illegal [LAE]
    case 140:
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

    for(int i=4;i>0;i--) IRQLinePuffer[i] = IRQLinePuffer[i-1];
    IRQLinePuffer[0] = IRQLine;

    if(*MCT == 0)
    {
        AktOpcodePC = PC;
        if(ResetReadyAdr == PC) *ResetReady = true;

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
