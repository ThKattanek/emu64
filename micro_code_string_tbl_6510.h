//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: micro_code_string_tbl_6510.h          //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 29.05.2011		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#ifndef MICRO_CODE_STRING_TBL_6510_H
#define MICRO_CODE_STRING_TBL_6510_H

const char* MicroCodeStringTable6510[]=
{
    /*000*/ "Opcode von PC-Adresse holen // PC+1",
    /*001*/ "Lesen von PC-Adresse und verwerfen // PC+1",
    /*002*/ "PC Hi -> Stack // SR|16 // SP-1",
    /*003*/ "PC Lo -> Stack // SP-1",
    /*004*/ "SR -> Stack // SR|4 // SP-1",
    /*005*/ "PC Lo von 0xFFFE holen",
    /*006*/ "PC Hi von 0xFFFF holen",
    /*007*/ "Pointer von PC-Adresse holen // PC+1",
    /*008*/ "Lesen von Pointer und verwerfen // Pointer+XR",
    /*009*/ "Adresse Lo von Pointer-Adresse holen // Pointer+1",
    /*010*/ "Adresse Hi von Pointer-Adresse holen",
    /*011*/ "TMPByte von Adresse holen // AC or TMPByte // Set SR(NZ)",
    /*012*/ "JAM",
    /*013*/ "TMPByte von Adresse holen",
    /*014*/ "TMPByte nach Adresse schreiben // ASL MEMORY // ORA",
    /*015*/ "TMPByte nach Adresse schreiben",
    /*016*/ "Adresse Hi = 0 // Adresse Lo von PC-Adresse holen // PC+1",
    /*017*/ "TMPByte von Adresse holen // AC or TMPByte // Set SR(NZ)",
    /*018*/ "TMPByte nach Adresse schreiben // TMPByte<<1 // Set SR(NZC)",
    /*019*/ "TMPByte von PC-Adresse holen",
    /*020*/ "SR nach SP+0x0100 schreiben // SP-1",
    /*021*/ "TMPByte von PC-Adresse holen // AC or TMPByte // Set SR(NZ) // PC+1",
    /*022*/ "TMPByte von PC-Adresse holen // AC<<1 // Set SR(NZC)",
    /*023*/ "TMPByte von PC-Adresse holen // AC and TMPByte // Set SR(NZC) // PC+1",
    /*024*/ "Adresse Lo von PC-Adresse holen // PC+1",
    /*025*/ "Adresse Hi von PC-Adresse holen // PC+1",
    /*026*/ "TMPByte von PC-Adresse holen // PC+1 // SR(N) auf FALSE prüfen (BPL)",
    /*027*/ "TMPByte von PC-Adresse holen // PC+1 // SR(N) auf TRUE prüfen (BMI)",
    /*028*/ "TMPByte von PC-Adresse holen // PC+1 // SR(V) auf FALSE prüfen (BVC)",
    /*029*/ "TMPByte von PC-Adresse holen // PC+1 // SR(V) auf TRUE prüfen (BVS)",
    /*030*/ "TMPByte von PC-Adresse holen // PC+1 // SR(C) auf FALSE prüfen (BCC)",
    /*031*/ "TMPByte von PC-Adresse holen // PC+1 // SR(C) auf TRUE prüfen (BCS)",
    /*032*/ "TMPByte von PC-Adresse holen // PC+1 // SR(Z) auf FALSE prüfen (BNE)",
    /*033*/ "TMPByte von PC-Adresse holen // PC+1 // SR(Z) auf TRUE prüfen (BEQ)",
    /*034*/ "Lesen von PC-Adresse und verwerfen // BranchAdresse=PC+TMPByte",
    /*035*/ "FIX PC Hi Adresse",
    /*036*/ "Adresse Hi von Pointer-Adresse holen // Adresse+YR",
    /*037*/ "TMPByte von Adresse holen // Fix Adresse Hi MCT+1 // AC or TMPByte",
    /*038*/ "Adresse Hi von PC-Adresse holen // PC=Adresse",
    /*039*/ "Lesen von PC-Adresse und verwerfen // XR=AC // Set SR(NZ)",
    /*040*/ "Lesen von PC-Adresse und verwerfen // YR=AC // Set SR(NZ)",
    /*041*/ "Lesen von PC-Adresse und verwerfen // XR=SP // Set SR(NZ)",
    /*042*/ "Lesen von PC-Adresse und verwerfen // AC=XR // Set SR(NZ)",
    /*043*/ "Lesen von PC-Adresse und verwerfen // SP=XR",
    /*044*/ "Lesen von PC-Adresse und verwerfen // AC=YR // Set SR(NZ)",
    /*045*/ "AC nach SP+0x0100 schreiben // SP-1",
    /*046*/ "AC von SP+0x0100 lesen // SP+1",
    /*047*/ "AC von SP+0x0100 lesen // Set SR(NZ)",
    /*048*/ "SR von SP+0x0100 lesen // SP+1",
    /*049*/ "SR von SP+0x0100 lesen",
    /*050*/ "TMPByte von PC-Adresse lesen // AC + TMPByte + Carry // PC+1",
    /*051*/ "TMPByte von Adresse lesen // AC + TMPByte + Carry",
    /*052*/ "TMPByte von Adresse lesen // Adresse Lo + XR",
    /*053*/ "Adresse Hi von PC-Adresse holen // Adresse+XR  // PC+1",
    /*054*/ "Adresse Hi von PC-Adresse holen // Adresse+YR  // PC+1",
    /*055*/ "TMPByte von Adresse lesen // AC + TMPByte + Carry // if(idxReg<Adresse Lo) MCT++",
    /*056*/ "TMPByte von PC-Adresse lesen // AC - TMPByte - Carry // PC+1",
    /*057*/ "TMPByte von Adresse lesen // AC - TMPByte - Carry",
    /*058*/ "TMPByte von Adresse lesen // AC - TMPByte - Carry // if(idxReg<Adresse Lo) MCT++",
    /*059*/ "TMPByte von SP+0x0100 holen",
    /*060*/ "PC-Adresse Hi nach SP+0x0100 schreiben // SP--",
    /*061*/ "PC-Adresse Lo nach SP+0x0100 schreiben // SP--",
    /*062*/ "TMPByte von SP+0x0100 holen // SP+1",
    /*063*/ "PC-Adresse Lo von SP+0x0100 holen // SP+1",
    /*064*/ "PC-Adresse Hi von SP+0x0100 holen",
    /*065*/ "TMPByte von PC-Adresse laden // PC+1",
    /*066*/ "TMPByte von PC-Adresse lesen // AC and TMPByte // Set SR(NZ) // PC+1",
    /*067*/ "TMPByte von Adresse lesen // AC and TMPByte // Set SR(NZ)",
    /*068*/ "TMPByte von Adresse lesen // AC and TMPByte // Set SR(NZ) // if(idxReg<Adresse Lo) MCT++",
    /*069*/ "TMPByte von Adresse lesen // CarrayFalg=0",
    /*070*/ "TMPByte von Adresse lesen // DezimalFalg=0",
    /*071*/ "TMPByte von Adresse lesen // InterruptFalg=0",
    /*072*/ "TMPByte von Adresse lesen // OverflowFalg=0",
    /*073*/ "TMPByte von Adresse lesen // CarrayFalg=1	",
    /*074*/ "TMPByte von Adresse lesen // DezimalFalg=1",
    /*075*/ "TMPByte von Adresse lesen // InterruptFalg=1",
    /*076*/ "TMPByte von Adresse lesen // BIT Operation",
    /*077*/ "AC nach Adresse schreiben",
    /*078*/ "XR nach Adresse schreiben",
    /*079*/ "YR nach Adresse schreiben",
    /*080*/ "AC von PC-Adresse lesen // Set SR(NZ) // PC+1",
    /*081*/ "AC von PC-Adresse lesen // Set SR(NZ)",
    /*082*/ "AC von PC-Adresse lesen // Set SR(NZ) // if(idxReg<Adresse Lo) MCT++",
    /*083*/ "XR von PC-Adresse lesen // Set SR(NZ) // PC+1",
    /*084*/ "XR von Adresse lesen // Set SR(NZ)",
    /*085*/ "XR von Adresse lesen // Set SR(NZ) // if(idxReg<Adresse Lo) MCT++",
    /*086*/ "YR von PC-Adresse lesen // Set SR(NZ) // PC+1",
    /*087*/ "YR von Adresse lesen // Set SR(NZ)",
    /*088*/ "YR von Adresse lesen // Set SR(NZ) // if(idxReg<Adresse Lo) MCT++",
    /*089*/ "TMPByte von Adresse lesen // XR+1 // Set SR(NZ)",
    /*090*/ "TMPByte von Adresse lesen // YR+1 // Set SR(NZ)",
    /*091*/ "TMPByte von Adresse lesen // XR-1 // Set SR(NZ)",
    /*092*/ "TMPByte von Adresse lesen // YR-1 // Set SR(NZ)",
    /*093*/ "Illegaler Opcode - wird noch nicht unterstützt // Reset",
    /*094*/ "PC LO von Adresse lesen // Adresse+1 (Nur Low Wert)",
    /*095*/ "PC HI von Adresse lesen",
    /*096*/ "PC LO von $FFFC lesen",
    /*097*/ "PC HI von $FFFD lesen",
    /*098*/ "TMPByte von PC-Adresse lesen // AC - TMPByte (AC wird nicht verändert) // Set SR(NZC) // PC+1",
    /*099*/ "TMPByte von Adresse lesen // AC - TMPByte (AC wird nicht verändert) // Set SR(NZC)",
    /*100*/ "TMPByte von Adresse lesen // AC - TMPByte (AC wird nicht verändert) // if(idxReg<Adresse Lo) MCT++",
    /*101*/ "TMPByte von PC-Adresse lesen // XR - TMPByte (XR wird nicht verändert) // Set SR(NZC) // PC+1",
    /*102*/ "TMPByte von Adresse lesen // XR - TMPByte (XR wird nicht verändert) // Set SR(NZC)",
    /*103*/ "TMPByte von PC-Adresse lesen // YR - TMPByte (XR wird nicht verändert) // Set SR(NZC) // PC+1",
    /*104*/ "TMPByte von Adresse lesen // YR - TMPByte (XR wird nicht verändert) // Set SR(NZC)",
    /*105*/ "TMPByte von PC-Adresse lesen // AC XOR TMPByte // Set SR(NZC) // PC+1",
    /*106*/ "TMPByte von Adresse lesen // AC XOR TMPByte // Set SR(NZC)",
    /*107*/ "TMPByte von Adresse lesen // AC XOR TMPByte // if(idxReg<Adresse Lo) MCT++",
    /*108*/ "TMPByte von PC-Adresse holen // AC>>1 // Set SR(NZC)",
    /*109*/ "TMPByte nach Adresse schreiben // TMPByte>>1 // Set SR(NZC)",
    /*110*/ "TMPByte von PC-Adresse holen // C<-AC<<1<-C // Set SR(NZC)",
    /*111*/ "TMPByte nach Adresse schreiben // C<-TMPByte<<1<-C // Set SR(NZC)",
    /*112*/ "TMPByte von PC-Adresse holen // C->AC>>1->C // Set SR(NZC)",
    /*113*/ "TMPByte nach Adresse schreiben // C->TMPByte>>1->C // Set SR(NZC)",
    /*114*/ "TMPByte nach Adresse schreiben // TMPByte+1 // Set SR(NZ)",
    /*115*/ "TMPByte nach Adresse schreiben // TMPByte-1 // Set SR(NZ)",
    /*116*/ "SR nach 0x100+SP schreiben // SP-- // IFlag setzen // BFlag löschen",
    /*117*/ "PC Lo von 0xFFFA holen",
    /*118*/ "PC Hi von 0xFFFB holen",
    /*119*/ "TMPByte von Adresse holen // Fix Adresse Hi MCT+1",
    /*120*/ "TMPByte nach Adresse schreiben // Illegal [SLO]",
    /*121*/ "TMPByte nach Adresse schreiben // Illegal [RLA]",
    /*122*/ "TMPByte nach Adresse schreiben // Illegal [SRE]",
    /*123*/ "TMPByte nach Adresse schreiben // Illegal [RRA]",
    /*124*/ "TMPByte nach Adresse schreiben // Illegal [DCP]",
    /*125*/ "TMPByte nach Adresse schreiben // Illegal [ISB]",
    /*126*/ "AC von Adresse lesen // AC -> XR // Set SR(NZ) // Illegal [LAX]",
    /*127*/ "AC von Adresse lesen // AC -> XR // Set SR(NZ) // if(idxReg<Adresse Lo) MCT++ // Illegal [LAX]",
    /*128*/ "AC von Adresse lesen // AC -> XR // Set SR(NZ) // Illegal [LAX]",
    /*129*/ "Illegal [ASR]",
    /*130*/ "Illegal [ARR]",
    /*131*/ "Illegal [ANE]",
    /*132*/ "Illegal [LXA]",
    /*133*/ "Illegal [SBX]",
    /*134*/ "Illegal [SHY]",
    /*135*/ "Illegal [SHX]",
    /*136*/ "Illegal [SHA]",
    /*137*/ "Illegal [SHS]",
    /*138*/ "Illegal [ANC]",
    /*139*/ "Illegal [LAE]",
    /*140*/ "Illegal [LAE] // if(idxReg<Adresse Lo) MCT++"
};

#define _r 0
#define _w 1
static unsigned char MicroCodeRWTable6510[141]=
{
    //00
    _r,_r,_w,_w,_w,_r,_r,_r,_r,_r,_r,_r,_r,_r,_w,_w,_r,_r,_w,_r,
    //20
    _w,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,
    //40
    _r,_r,_r,_r,_r,_w,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,
    //60
    _w,_w,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_w,_w,_w,
    //80
    _r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,_r,
    //100
    _r,_r,_r,_r,_r,_r,_r,_r,_r,_w,_r,_w,_r,_w,_w,_w,_w,_r,_r,_r,
    //120
    _w,_w,_w,_w,_w,_w,_r,_r,_w,_r,_r,_r,_r,_r,_w,_w,_w,_w,_r,_r,
    //140
    _r

};
#endif // MICRO_CODE_STRING_TBL_6510_H
