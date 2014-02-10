//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: crt_class.cpp                         //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 09.07.2011		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#include "crt_class.h"
#include <stdio.h>
#include <cstring>

const char* TYPE_STRING[34] = { "Normal Cartridge","Action Replay","KCS Power Cartridge",
                                "Final Cartridge III","Simons Basic","Ocean type 1 (128K and 256K)*",
                                "Expert Cartridge","Fun Play","Super Games","Atomic Power",
                                "Epyx Fastload","Westermann","Rex","Final Cartridge I",
                                "Magic Formel","C64 Game System","Warpspeed","Dinamic","Zaxxon",
                                "Magic Desk, Domark, HES Australia","Super Snapshot 5","COMAL 80",
                                "Structured Basic","Ross","Dela EP64","Dela EP7x8","Dela EP256","Rex EP256",
                                "Unbekannt", //28	Platzhalter
                                "Unbekannt", //29	Platzhalter
                                "Unbekannt", //30	Platzhalter
                                "Unbekannt", //31 Platzhalter
                                "EasyFlash Cartridge", //32
                                "Unbekannt"};

CRTClass::CRTClass()
{
    CRTInsert = false;
    CRTTyp = 0;

    EasyFlashJumper = false;	// false = Boot
    am29f040Lo = new AM29F040Class(CRT_ROM_BANK1,1);
    am29f040Hi = new AM29F040Class(CRT_ROM_BANK2,1);

    ChangeLED = 0;
    ResetAllLEDS();
}

CRTClass::~CRTClass()
{
}

void CRTClass::ResetAllLEDS(void)
{
    LED_00=LED_00_OLD=LED_01=LED_01_OLD=false;
    if(ChangeLED != 0) ChangeLED(0,LED_00);
    if(ChangeLED != 0) ChangeLED(1,LED_01);
}

inline unsigned long CRTClass::conv_dword(unsigned long wert)
{
        unsigned short h,l;
        l = wert>>16;
        h = wert & 0xffff;
        l = l<<8 | l>>8;
        h = h<<8 | h>>8;
        wert = h<<16 | l;
        return wert;
}

int* CRTClass::GetFlash040Dirty(int nr)
{
        switch (nr)
        {
        case 0:
                return &am29f040Lo->Dirty;
                break;
        case 1:
                return &am29f040Hi->Dirty;
                break;
        default:
                return 0;
        }
}

unsigned char* CRTClass::GetFlash040Byte(int nr)
{
        switch (nr)
        {
        case 0:
                return &am29f040Lo->ProgrammByte;
                break;
        case 1:
                return &am29f040Hi->ProgrammByte;
                break;
        default:
                return 0;
        }
}

int CRTClass::LoadCRTImage(char* filename)
{
        FILE *file;
        char Kennung[17];
        unsigned short Version;
        unsigned int HeaderLength;
        unsigned long akt_pos;
        unsigned char exrom,game;
        int ChipCount;
        unsigned short chip_adr;
        unsigned short chip_size;
        int Bank1Pos,Bank2Pos,Bank3Pos;

        file = fopen (filename, "rb");
        if (file == NULL)
        {
                return 0x01;
        }

        fread(Kennung,1,16,file);
        Kennung[16] = 0;

        if(0!=strcmp("C64 CARTRIDGE   ",Kennung))
        {
                fclose(file);
                return 0x02;
        }

        fread(&HeaderLength,1,sizeof(HeaderLength),file);
        HeaderLength = conv_dword(HeaderLength);

        fread(&Version,1,sizeof(Version),file);

        fread(&CRTTyp,1,sizeof(CRTTyp),file);
        CRTTyp = CRTTyp<<8 | CRTTyp>>8;

        fread(&exrom,1,sizeof(exrom),file);
        fread(&game,1,sizeof(game),file);

        if(exrom == 0) CRT_EXROM = false;
        else CRT_EXROM = true;
        *EXROM = CRT_EXROM;

        if(game == 0) CRT_GAME = false;
        else CRT_GAME = true;
        *GAME = CRT_GAME;

        fseek(file,0x0040,SEEK_SET);

        akt_pos = 0x40;
        ChipCount = 0;
        Bank1Pos = Bank2Pos = Bank3Pos = 0;
L1:
        fseek(file,akt_pos,SEEK_SET);
        if(4 > fread(Kennung,1,4,file)) goto L2;
        Kennung[4] = 0;
        if(0==strcmp("CHIP",Kennung))
        {
                if(ChipCount == 128)
                {
                        //MessageBox(0,"Fehler 128 in CRT Modul","Emu64",0);
                        goto L2;
                }
                fread(&HeaderLength,1,sizeof(HeaderLength),file);
                HeaderLength = conv_dword(HeaderLength);
                akt_pos += HeaderLength;

                fseek(file,4,SEEK_CUR);
                fread(&chip_adr,1,2,file);
                chip_adr = chip_adr<<8 | chip_adr>>8;
                fread(&chip_size,1,2,file);
                chip_size = chip_size<<8 | chip_size>>8;

                switch(chip_adr)
                {
                case 0x8000:
                        fread(CRT_ROM_BANK1 + Bank1Pos,1,0x2000,file);
                        Bank1Pos += 0x2000;
                        if(chip_size == 0x4000)
                        {
                                fread(CRT_ROM_BANK2 + Bank2Pos,1,0x2000,file);
                                Bank2Pos += 0x2000;
                        }
                        break;
                case 0xA000:
                        fread(CRT_ROM_BANK2 + Bank2Pos,1,0x2000,file);
                        Bank2Pos += 0x2000;
                        break;
                case 0xE000:
                                fread(CRT_ROM_BANK2 + Bank2Pos,1,0x2000,file);
                                Bank2Pos += 0x2000;
                        break;
                }
                ChipCount ++;
                goto L1;
        }
L2:
        fclose(file);

        ROM_LO = CRT_ROM_BANK1;
        ROM_HI = CRT_ROM_BANK2;

        CRTInsert = true;
        ResetAllLEDS();

        Reset();

        return 0;
}

void CRTClass::RemoveCRTImage()
{
        CRTInsert = false;
        *GAME = true;
        *EXROM = true;
        ResetAllLEDS();
}

int CRTClass::NewEasyFlashImage(char* filename,char* crt_name)
{
        const char* Kennung = "C64 CARTRIDGE   ";
        const char* ChipKennung = "CHIP";
        unsigned short Version;
        unsigned int HeaderLength;
        unsigned char exrom,game;
        unsigned short chip_adr;
        unsigned short chip_size;
        unsigned short chip_typ;
        unsigned short chip_bank;

        FILE *File;
        File = fopen (filename, "wb");
        if (File == NULL)
        {
                return 0x01;
        }

        fwrite(Kennung,1,16,File);

        /*****/ HeaderLength = 0x40;
        HeaderLength = conv_dword(HeaderLength);
        fwrite(&HeaderLength,1,sizeof(HeaderLength),File);

        /*****/ Version = 0x0001;
        fwrite(&Version,1,sizeof(Version),File);

        /*****/ CRTTyp = 32;
        CRTTyp = CRTTyp<<8 | CRTTyp>>8;
        fwrite(&CRTTyp,1,sizeof(CRTTyp),File);

        /*****/ exrom = 1;
        /*****/ game = 0;
        fwrite(&exrom,1,sizeof(exrom),File);
        fwrite(&game,1,sizeof(game),File);

        /*****/ // 6 Byte Reserviert
        fseek(File,0x0020,SEEK_SET);

        /*****/ // CRT Name
        fwrite(crt_name,1,32,File);

        for(int i=0;i<64;i++)
        {
                fwrite(ChipKennung,1,4,File);
                /*****/ HeaderLength = 0x00002010;
                HeaderLength = conv_dword(HeaderLength);
                fwrite(&HeaderLength,1,sizeof(HeaderLength),File);

                chip_typ = 2;
                chip_typ = chip_typ<<8 | chip_typ>>8;
                fwrite(&chip_typ,1,sizeof(chip_typ),File);

                chip_bank = (unsigned short)i;
                chip_bank = chip_bank<<8 | chip_bank>>8;
                fwrite(&chip_bank,1,sizeof(chip_bank),File);

                chip_adr = 0x8000;
                chip_adr = chip_adr<<8 | chip_adr>>8;
                fwrite(&chip_adr,1,sizeof(chip_adr),File);

                chip_size = 0x2000;
                chip_size = chip_size<<8 | chip_size>>8;
                fwrite(&chip_size,1,sizeof(chip_size),File);

                fseek(File,0x2000,SEEK_CUR);
        }

        for(int i=0;i<64;i++)
        {
                fwrite(ChipKennung,1,4,File);
                /*****/ HeaderLength = 0x00002010;
                HeaderLength = conv_dword(HeaderLength);
                fwrite(&HeaderLength,1,sizeof(HeaderLength),File);

                chip_typ = 2;
                chip_typ = chip_typ<<8 | chip_typ>>8;
                fwrite(&chip_typ,1,sizeof(chip_typ),File);

                chip_bank = (unsigned short)i;
                chip_bank = chip_bank<<8 | chip_bank>>8;
                fwrite(&chip_bank,1,sizeof(chip_bank),File);

                chip_adr = 0xA000;
                chip_adr = chip_adr<<8 | chip_adr>>8;
                fwrite(&chip_adr,1,sizeof(chip_adr),File);

                chip_size = 0x2000;
                chip_size = chip_size<<8 | chip_size>>8;
                fwrite(&chip_size,1,sizeof(chip_size),File);

                fseek(File,0x2000,SEEK_CUR);
        }

        fclose(File);

        return 0;
}

int CRTClass::WriteEasyFlashImage(char* filename)
{
        CRT_INFO_STRUCT crtinfo;
        const char* Kennung = "C64 CARTRIDGE   ";
        const char* ChipKennung = "CHIP";
        unsigned short Version;
        unsigned int HeaderLength;
        unsigned char exrom,game;
        unsigned short chip_adr;
        unsigned short chip_size;
        unsigned short chip_typ;
        unsigned short chip_bank;

        if(0 != GetCRTInfo(filename,&crtinfo))
        {
                return 0x01;
        }

        FILE *File;
        File = fopen (filename, "wb");
        if (File == NULL)
        {
                return 0x01;
        }

        fwrite(Kennung,1,16,File);

        /*****/ HeaderLength = 0x40;
        HeaderLength = conv_dword(HeaderLength);
        fwrite(&HeaderLength,1,sizeof(HeaderLength),File);

        /*****/ Version = 0x0001;
        fwrite(&Version,1,sizeof(Version),File);

        /*****/ CRTTyp = 32;
        CRTTyp = CRTTyp<<8 | CRTTyp>>8;
        fwrite(&CRTTyp,1,sizeof(CRTTyp),File);

        /*****/ exrom = 1;
        /*****/ game = 0;
        fwrite(&exrom,1,sizeof(exrom),File);
        fwrite(&game,1,sizeof(game),File);

        /*****/ // 6 Byte Reserviert
        fseek(File,0x0020,SEEK_SET);

        /*****/ // CRT Name
        fwrite(crtinfo.Name,1,32,File);


        for(int i=0;i<64;i++)
        {
                fwrite(ChipKennung,1,4,File);
                /*****/ HeaderLength = 0x00002010;
                HeaderLength = conv_dword(HeaderLength);
                fwrite(&HeaderLength,1,sizeof(HeaderLength),File);

                chip_typ = 2;
                chip_typ = chip_typ<<8 | chip_typ>>8;
                fwrite(&chip_typ,1,sizeof(chip_typ),File);

                chip_bank = (unsigned short)i;
                chip_bank = chip_bank<<8 | chip_bank>>8;
                fwrite(&chip_bank,1,sizeof(chip_bank),File);

                chip_adr = 0x8000;
                chip_adr = chip_adr<<8 | chip_adr>>8;
                fwrite(&chip_adr,1,sizeof(chip_adr),File);

                chip_size = 0x2000;
                chip_size = chip_size<<8 | chip_size>>8;
                fwrite(&chip_size,1,sizeof(chip_size),File);

                fwrite(&CRT_ROM_BANK1[i*0x2000],1,0x2000,File);
        }

        for(int i=0;i<64;i++)
        {
                fwrite(ChipKennung,1,4,File);
                /*****/ HeaderLength = 0x00002010;
                HeaderLength = conv_dword(HeaderLength);
                fwrite(&HeaderLength,1,sizeof(HeaderLength),File);

                chip_typ = 2;
                chip_typ = chip_typ<<8 | chip_typ>>8;
                fwrite(&chip_typ,1,sizeof(chip_typ),File);

                chip_bank = (unsigned short)i;
                chip_bank = chip_bank<<8 | chip_bank>>8;
                fwrite(&chip_bank,1,sizeof(chip_bank),File);

                chip_adr = 0xA000;
                chip_adr = chip_adr<<8 | chip_adr>>8;
                fwrite(&chip_adr,1,sizeof(chip_adr),File);

                chip_size = 0x2000;
                chip_size = chip_size<<8 | chip_size>>8;
                fwrite(&chip_size,1,sizeof(chip_size),File);

                fwrite(&CRT_ROM_BANK2[i*0x2000],1,0x2000,File);
        }

        fclose(File);

        return 0;
}

void CRTClass::SetEasyFlashJumper(bool enabled)
{
        EasyFlashJumper = enabled;
}

int CRTClass::GetCRTInfo(char* filename,CRT_INFO_STRUCT* crtinfo)
{
        FILE *File;
        char Kennung[17];
        unsigned short Version;
        unsigned int HeaderLength;
        unsigned long akt_pos;
        unsigned short tmp;
        int Bank1Pos,Bank2Pos,Bank3Pos;

        File = fopen (filename, "rb");
        if (File == NULL)
        {
                return 0x01;
        }

        fread(Kennung,1,16,File);
        Kennung[16] = 0;

        if(0!=strcmp("C64 CARTRIDGE   ",Kennung))
        {
                fclose(File);
                return 0x02;
        }

        fread(&HeaderLength,1,sizeof(HeaderLength),File);
        HeaderLength = conv_dword(HeaderLength);

        fread(&Version,1,sizeof(Version),File);

        sprintf(crtinfo->Version,"%X.%2.2X",(unsigned char)Version,Version>>8);

        fread(&crtinfo->HardwareType,1,sizeof(crtinfo->HardwareType),File);
        crtinfo->HardwareType = crtinfo->HardwareType<<8 | crtinfo->HardwareType>>8;
        if(crtinfo->HardwareType > 32) crtinfo->HardwareTypeString = (char*)TYPE_STRING[33];
        else crtinfo->HardwareTypeString = (char*)TYPE_STRING[crtinfo->HardwareType];

        fread(&crtinfo->EXROM,1,sizeof(crtinfo->EXROM),File);
        fread(&crtinfo->GAME,1,sizeof(crtinfo->GAME),File);

        fseek(File,0x0020,SEEK_SET);
        fread(crtinfo->Name,1,32,File);

        akt_pos = 0x40;
        crtinfo->ChipCount = 0;
        Bank1Pos = Bank2Pos = Bank3Pos = 0;
L1:
        fseek(File,akt_pos,SEEK_SET);
        if(4 > fread(Kennung,1,4,File)) goto L2;
        Kennung[4] = 0;
        if(0==strcmp("CHIP",Kennung))
        {
                fread(&HeaderLength,1,sizeof(HeaderLength),File);
                HeaderLength = conv_dword(HeaderLength);

                fread(&tmp,1,2,File);
                tmp = tmp<<8 | tmp>>8;
                crtinfo->ChipInfo[crtinfo->ChipCount].Type = tmp;
                fread(&tmp,1,2,File);
                tmp = tmp<<8 | tmp>>8;
                crtinfo->ChipInfo[crtinfo->ChipCount].BankLocation = tmp;
                fread(&tmp,1,2,File);
                tmp = tmp<<8 | tmp>>8;
                crtinfo->ChipInfo[crtinfo->ChipCount].LoadAdress = tmp;
                fread(&tmp,1,2,File);
                tmp = tmp<<8 | tmp>>8;
                crtinfo->ChipInfo[crtinfo->ChipCount].ChipSize = tmp;

                switch(crtinfo->ChipInfo[crtinfo->ChipCount].LoadAdress)
                {
                case 0x8000:
                        fread(CRT_ROM_BANK1_TMP + Bank1Pos,1,0x2000,File);
                        crtinfo->ChipInfo[crtinfo->ChipCount].BufferPointer = CRT_ROM_BANK1_TMP + Bank1Pos;
                        Bank1Pos += 0x2000;
                        if(tmp == 0x4000)
                        {
                                fread(CRT_ROM_BANK1_TMP + Bank1Pos,1,0x2000,File);
                                //crtinfo->ChipInfoHi[crtinfo->ChipCount].BufferPointer = CRT_ROM_BANK2_TMP + Bank2Pos;
                                Bank1Pos += 0x2000;
                        }
                        break;
                case 0xA000:
                        fread(CRT_ROM_BANK2_TMP + Bank2Pos,1,0x2000,File);
                        crtinfo->ChipInfo[crtinfo->ChipCount].BufferPointer = CRT_ROM_BANK2_TMP + Bank2Pos;
                        Bank2Pos += 0x2000;
                        break;
                case 0xE000:
                        fread(CRT_ROM_BANK2_TMP + Bank2Pos,1,0x2000,File);
                        crtinfo->ChipInfo[crtinfo->ChipCount].BufferPointer = CRT_ROM_BANK2_TMP + Bank2Pos;
                        Bank2Pos += 0x2000;
                        break;
                }

                crtinfo->ChipCount ++;
                akt_pos += HeaderLength;
                goto L1;
        }
L2:
        fclose(File);
        return 0;
}

void CRTClass::Reset(void)
{
        ROM_LO = CRT_ROM_BANK1;
        ROM_HI = CRT_ROM_BANK2;

        if(CRTInsert)
        {
                switch(CRTTyp)
                {
                case 1:		// Action Replay 4
                        ActionReplayAktiv = true;
                        break;
                case 3:		//Final Cartgide III
                        WriteIO2(0xDFFF,0x00);
                        break;

                case 9:		//Atomic Power
                        break;

                case 32:	//EasyFlash
                        WriteIO1(0xDE00,0);
                        WriteIO1(0xDE02,0);
                        break;

                default:
                        *GAME = CRT_GAME;
                        *EXROM = CRT_EXROM;
                        break;
                }
        }
        else
        {
                *GAME = true;
                *EXROM = true;
        }
}

void CRTClass::Freeze(void)
{
        if(!CRTInsert) return;

        switch(CRTTyp)
        {
                /// Final Cartridge III
                case 3:
                        WriteIO2(0xDFFF,16);
                        break;
        }
}

void CRTClass::WriteIO1(unsigned short adresse,unsigned char wert)
{
        if(!CRTInsert) return;

        switch(CRTTyp)
        {
        case 1:		// Action Replay 4
                if(adresse == 0xDE00)
                {
                        RomLBank = (wert >> 4) & 1;
                        RomLBank = ((wert & 1) | (RomLBank << 1)) & 3;
                        *EXROM = ((~wert >> 3) & 1)^1;
                        *GAME = ((~wert >> 1) & 1)^1;

                        if(ActionReplayAktiv)
                        {
                                ChangeMemMapProc();
                                if(wert & 0x04) ActionReplayAktiv = false;
                        }
                }
                break;
        case 4:
                if(adresse == 0xDE00)
                {
                        *GAME = false;
                        ChangeMemMapProc();
                }
                break;
        case 5:		// Ocean type 1
                wert &= 0x3F;
                ROM_LO = CRT_ROM_BANK1 + (wert * 0x2000);
                break;
        case 7:		// Fun Play
                if(adresse == 0xDE00)
                {
                        switch(wert)
                        {
                        case 0x00:
                                ROM_LO = CRT_ROM_BANK1 + (0 * 0x2000);
                                break;
                        case 0x08:
                                ROM_LO = CRT_ROM_BANK1 + (1 * 0x2000);
                                break;
                        case 0x10:
                                ROM_LO = CRT_ROM_BANK1 + (2 * 0x2000);
                                break;
                        case 0x18:
                                ROM_LO = CRT_ROM_BANK1 + (3 * 0x2000);
                                break;
                        case 0x20:
                                ROM_LO = CRT_ROM_BANK1 + (4 * 0x2000);
                                break;
                        case 0x28:
                                ROM_LO = CRT_ROM_BANK1 + (5 * 0x2000);
                                break;
                        case 0x30:
                                ROM_LO = CRT_ROM_BANK1 + (6 * 0x2000);
                                break;
                        case 0x38:
                                ROM_LO = CRT_ROM_BANK1 + (7 * 0x2000);
                                break;
                        case 0x01:
                                ROM_LO = CRT_ROM_BANK1 + (8 * 0x2000);
                                break;
                        case 0x09:
                                ROM_LO = CRT_ROM_BANK1 + (9 * 0x2000);
                                break;
                        case 0x11:
                                ROM_LO = CRT_ROM_BANK1 + (10 * 0x2000);
                                break;
                        case 0x19:
                                ROM_LO = CRT_ROM_BANK1 + (11 * 0x2000);
                                break;
                        case 0x21:
                                ROM_LO = CRT_ROM_BANK1 + (12 * 0x2000);
                                break;
                        case 0x29:
                                ROM_LO = CRT_ROM_BANK1 + (13 * 0x2000);
                                break;
                        case 0x31:
                                ROM_LO = CRT_ROM_BANK1 + (14 * 0x2000);
                                break;
                        case 0x39:
                                ROM_LO = CRT_ROM_BANK1 + (15 * 0x2000);
                                break;
                        }
                }
                break;
        case 15:	// C64 Gamesystem
                wert &= 0x3F;
                ROM_LO = CRT_ROM_BANK1 + (wert * 0x2000);
                break;
        case 32:	// EasyFlash
                if(adresse == 0xDE00)
                {
                        EasyFlashBankReg = wert & 0x3F;
                        ROM_LO = CRT_ROM_BANK1 + ((wert & 0x3F) * 0x2000);
                        ROM_HI = CRT_ROM_BANK2 + ((wert & 0x3F) * 0x2000);
                }
                if(adresse == 0xDE02)
                {
                        LED_01 = !!(wert & 0x80);
                        if(LED_01 != LED_01_OLD) if(ChangeLED != 0) ChangeLED(1,LED_01);
                        LED_01_OLD = LED_01;
                        *EXROM = !!(~wert & 0x02);

                        if(wert & 0x04) *GAME = !!(~wert & 0x01);
                        else *GAME = !!(EasyFlashJumper);
                        ChangeMemMapProc();
                }
                break;
        default:
                break;
        }
}

unsigned char CRTClass::ReadIO1(unsigned short adresse)
{
        if(!CRTInsert) return 0x00;	// Eigl. Zufallszahlen

        switch(CRTTyp)
        {
        case 3:		// Final Cartridge III
                return CRT_ROM_BANK1[0x1E00 + (RomLBank << 13) + (adresse & 0xFF)];
                break;
        case 4:
                if(adresse == 0xDE00)
                {
                        *GAME = true;
                        ChangeMemMapProc();
                }
                break;
        case 17:
                        ROM_LO = CRT_ROM_BANK1 + ((adresse&0x3F) * 0x2000);
                break;
        default:
                return 0x00;
        }
        return 0xAA;
}

void CRTClass::WriteIO2(unsigned short adresse,unsigned char wert)
{
        if(!CRTInsert) return;
        switch(CRTTyp)
        {
        case 3:		// Final Cartridge III
                if((adresse & 0xFF) == 0xFF)
                {
                        RomLBank = wert & 0x03;
                        ROM_LO = CRT_ROM_BANK1 + ((wert&0x3) * 0x2000);
                        ROM_HI = CRT_ROM_BANK2 + ((wert&0x3) * 0x2000);
                        *EXROM = ((~wert >> 4) & 1) ^ 1;
                        *GAME = ((~wert >> 5) & 1) ^ 1;
                        ChangeMemMapProc();

                        if((wert & 0x30) == 0x10) CpuTriggerInterrupt(CRT_NMI);
                        if(wert & 0x40) CpuClearInterrupt(CRT_NMI);
                        LED_00 = (~wert>>7) & 1;
                        if(LED_00 != LED_00_OLD) if(ChangeLED != 0)  ChangeLED(0,LED_00);
                        LED_00_OLD = LED_00;
                }
                break;
        case 8:		// Super Games
                        ROM_LO = CRT_ROM_BANK1 + ((wert&0x3) * 0x2000);
                        ROM_HI = CRT_ROM_BANK2 + ((wert&0x3) * 0x2000);
                        if(wert & 0x04)
                        {
                                *EXROM = 0;
                                *GAME = 1;
                        }
                        else *EXROM = *GAME = 0;
                        if(wert == 0x0C) *EXROM = *GAME = 1;
                        ChangeMemMapProc();
                break;
        case 32:	// EasyFlash
                EasyFlashRam[adresse & 0xFF] = wert;
                break;
        default:
                break;
        }
}

unsigned char CRTClass::ReadIO2(unsigned short adresse)
{
        if(!CRTInsert) return 0x00;	// Eigl. Zufallszahlen (Vic Phi)

        switch(CRTTyp)
        {
        case 1:		// Action Replay 4
                if(!ActionReplayAktiv) return 0x00;	// Eigl. Zufallszahlen (Vic Phi)
                switch (RomLBank)
                {
                        case 0:
                                return CRT_ROM_BANK1[adresse & 0x1fff];
                        case 1:
                                return CRT_ROM_BANK1[(adresse & 0x1fff) + 0x2000];
                        case 2:
                                return CRT_ROM_BANK1[(adresse & 0x1fff) + 0x4000];
                        case 3:
                                return CRT_ROM_BANK1[(adresse & 0x1fff) + 0x6000];
                }
                return 0;
                break;
        case 3:		// Final Cartridge III
                switch (RomLBank)
                {
                        case 0:
                                return CRT_ROM_BANK1[adresse & 0x1fff];
                        case 1:
                                return CRT_ROM_BANK1[(adresse & 0x1fff) + 0x2000];
                        case 2:
                                return CRT_ROM_BANK1[(adresse & 0x1fff) + 0x4000];
                        case 3:
                                return CRT_ROM_BANK1[(adresse & 0x1fff) + 0x6000];
                }
                return 0;
                break;
        case 32:	// EasyFlash
                return EasyFlashRam[adresse & 0xFF];
                break;
        default:
                return 0x00;
                break;
        }
}

unsigned char CRTClass::ReadRom1(unsigned short adresse)
{
        if(!CRTInsert) return 0x55;	// Darf eigentlich nie vorkommen !!!!

        switch(CRTTyp)
        {
        case 1:		// Action Replay 4
                return CRT_ROM_BANK1[(adresse & 0x1FFF) + (RomLBank << 13)];
                break;
        case 32:	// EasyFlash
                return am29f040Lo->Read((adresse & 0x1FFF) | ((EasyFlashBankReg & 0x3F)<<13));
                break;
        default:
                return ROM_LO[adresse-0x8000];
                break;
        }
}

unsigned char CRTClass::ReadRom2(unsigned short adresse)
{
        if(!CRTInsert) return 0x55;	// Darf eigentlich nie vorkommen !!!!

        switch(CRTTyp)
        {
        case 32:	// EasyFlash
                return am29f040Hi->Read((adresse & 0x1FFF) | ((EasyFlashBankReg & 0x3F)<<13));
                break;
        default:
                return ROM_HI[adresse-0xA000];
                break;
        }
}

unsigned char CRTClass::ReadRom3(unsigned short adresse)
{
        if(!CRTInsert) return 0x55;	// Darf eigentlich nie vorkommen !!!!

        switch(CRTTyp)
        {
        case 32:	// EasyFlash
                return am29f040Hi->Read((adresse & 0x1FFF) | ((EasyFlashBankReg & 0x3F)<<13));
                break;
        default:
                return ROM_HI[adresse-0xE000];
                break;
        }
}

void CRTClass::WriteRom1(unsigned short adresse,unsigned char wert)	// 0x8000
{
        if(!CRTInsert)
        {
                RAM_C64[adresse] = wert;
                return;
        }

        switch(CRTTyp)
        {
        case 32:	// EasyFlash
                am29f040Lo->Write((adresse & 0x1FFF) | ((EasyFlashBankReg & 0x3F)<<13),wert);
                RAM_C64[adresse] = wert;
                break;
        default:
                RAM_C64[adresse] = wert;
                break;
        }
}

void CRTClass::WriteRom2(unsigned short adresse,unsigned char wert)	// 0xA000
{
        if(!CRTInsert)
        {
                RAM_C64[adresse] = wert;
                return;
        }

        switch(CRTTyp)
        {
        case 32:	// EasyFlash
                am29f040Hi->Write((adresse & 0x1FFF) | ((EasyFlashBankReg & 0x3F)<<13),wert);
                RAM_C64[adresse] = wert;
                break;
        default:
                RAM_C64[adresse] = wert;
                break;
        }
}

void CRTClass::WriteRom3(unsigned short adresse,unsigned char wert)	// 0xE000
{
        if(!CRTInsert)
        {
                RAM_C64[adresse] = wert;
                return;
        }

        switch(CRTTyp)
        {
        case 32:	// EasyFlash
                am29f040Hi->Write((adresse & 0x1FFF) | ((EasyFlashBankReg & 0x3F)<<13),wert);
                RAM_C64[adresse] = wert;
                break;
        default:
                RAM_C64[adresse] = wert;
                break;
        }
}
