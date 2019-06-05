//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: crt_class.cpp                         //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 05.06.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "cartridge_class.h"
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

CartridgeClass::CartridgeClass()
{
    CRTInsert = false;
    CRTTyp = 0;

    EasyFlashJumper = false;	// false = Boot
    am29f040Lo = new AM29F040Class(CRT_ROM_BANK1,1);
    am29f040Hi = new AM29F040Class(CRT_ROM_BANK2,1);

    ChangeLED = 0;
    ResetAllLEDS();

    ARFreez = false;
}

CartridgeClass::~CartridgeClass()
{
}

void CartridgeClass::ResetAllLEDS(void)
{
    LED_00=LED_00_OLD=LED_01=LED_01_OLD=false;
    if(ChangeLED != 0) ChangeLED(0,LED_00);
    if(ChangeLED != 0) ChangeLED(1,LED_01);
}

void CartridgeClass::SetMemLogicAR(unsigned short adresse)
{
    if(!ActionReplayAktiv) return;
    unsigned char io2, pla_adresse, pla_out;

    if((adresse >= 0xdf00) && (adresse <= 0xdfff))
        io2 = 0;
    else io2 = 8;

    // Die Adresse für die PLA wird wie folgt gebildet
    // Bit 0 - DatenBit 5
    // Bit 1 - DatenBit 1
    // Bit 2 - DatenBit 0
    // Bit 3 - IO2 (Low wenn Adresse DF00-DFFF)
    // Bit 4 - AdressBit 13
    // Bit 5 - AdressBit 15
    // Bit 6 - AdreesBit 14
    // Bit 7 - FreezBit

    if(ARFreez)
        pla_adresse = ((ARRegister >> 5) & 1) | (ARRegister & 2) | ((ARRegister << 2) & 4) | io2 | ((adresse >> 9) & 16) | ((adresse >> 10) & 32) | ((adresse >> 8) & 64) | 0;
    else
        pla_adresse = ((ARRegister >> 5) & 1) | (ARRegister & 2) | ((ARRegister << 2) & 4) | io2 | ((adresse >> 9) & 16) | ((adresse >> 10) & 32) | ((adresse >> 8) & 64) | 128;

    // PLA Ausgang
    // Bit 0 - ROM Enable (CE)
    // Bit 1 - RAM Enable (CS)
    // Bit 2 - GAME
    // Bit 3 - EXROM
    pla_out = mk7pla[pla_adresse];

    EnableActionReplayRam = (~pla_out>>1 & 1);

    *GAME = (pla_out & 4);
    *EXROM = (pla_out & 8);

    ChangeMemMapProc();
}

inline uint16_t CartridgeClass::ConvertDWord(uint32_t value)
{
        uint16_t h,l;
        l = value>>16;
        h = value & 0xffff;
        l = l<<8 | l>>8;
        h = h<<8 | h>>8;
        value = h<<16 | l;
        return value;
}

bool* CartridgeClass::GetFlash040Dirty(uint16_t nr)
{
        switch (nr)
        {
        case 0:
                return &am29f040Lo->dirty;
                break;
        case 1:
                return &am29f040Hi->dirty;
                break;
        default:
                return 0;
        }
}

uint8_t *CartridgeClass::GetFlash040Byte(uint16_t nr)
{
        switch (nr)
        {
        case 0:
                return &am29f040Lo->programm_byte;
                break;
        case 1:
                return &am29f040Hi->programm_byte;
                break;
        default:
                return 0;
        }
}

int CartridgeClass::LoadCRTImage(const char *filename)
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
        size_t reading_elements;

        file = fopen (filename, "rb");
        if (file == NULL)
        {
                return 0x01;
        }

        reading_elements = fread(Kennung,1,16,file);

        Kennung[16] = 0;

        if(0!=strcmp("C64 CARTRIDGE   ",Kennung))
        {
                fclose(file);
                return 0x02;
        }

        reading_elements = fread(&HeaderLength,1,sizeof(HeaderLength),file);
        HeaderLength = ConvertDWord(HeaderLength);

        reading_elements = fread(&Version,1,sizeof(Version),file);

        reading_elements = fread(&CRTTyp,1,sizeof(CRTTyp),file);
        CRTTyp = CRTTyp<<8 | CRTTyp>>8;

        reading_elements = fread(&exrom,1,sizeof(exrom),file);
        reading_elements = fread(&game,1,sizeof(game),file);

        if(exrom == 0) CRT_EXROM = false;
        else CRT_EXROM = true;
        *EXROM = CRT_EXROM;

        if(game == 0) CRT_GAME = false;
        else CRT_GAME = true;
        *GAME = CRT_GAME;

        fseek(file,0x0040,SEEK_SET);

        akt_pos = 0x40;
        ChipCount = 0;
L1:
        fseek(file,akt_pos,SEEK_SET);
        reading_elements = fread(Kennung,1,4,file);
        if(4 > reading_elements) goto L2;
        Kennung[4] = 0;
        if(0==strcmp("CHIP",Kennung))
        {
                if(ChipCount == 128)
                {
                        //MessageBox(0,"Fehler 128 in CRT Modul","Emu64",0);
                        goto L2;
                }
                reading_elements = fread(&HeaderLength,1,sizeof(HeaderLength),file);
                HeaderLength = ConvertDWord(HeaderLength);
                akt_pos += HeaderLength;

                fseek(file,2,SEEK_CUR);

                unsigned short BankPos;
                reading_elements = fread(&BankPos,1,2,file);
                BankPos = BankPos<<8 | BankPos>>8;
                BankPos &= 0xFF;    // Maximal 256 Bänke je CRT_ROM_BANK1 + CRT_ROM_BANK2

                reading_elements = fread(&chip_adr,1,2,file);
                chip_adr = chip_adr<<8 | chip_adr>>8;
                reading_elements = fread(&chip_size,1,2,file);
                chip_size = chip_size<<8 | chip_size>>8;

                switch(chip_adr)
                {
                case 0x8000:
                        reading_elements = fread(CRT_ROM_BANK1 + (BankPos * 0x2000),1,0x2000,file);
                        if(chip_size == 0x4000)
                        {
                            reading_elements = fread(CRT_ROM_BANK2 + (BankPos * 0x2000),1,0x2000,file);
                        }
                        break;
                case 0xA000:
                        reading_elements = fread(CRT_ROM_BANK2 + (BankPos * 0x2000),1,0x2000,file);
                        break;
                case 0xE000:
                        reading_elements = fread(CRT_ROM_BANK2 + (BankPos * 0x2000),1,0x2000,file);
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

void CartridgeClass::RemoveCRTImage()
{
        CRTInsert = false;
        *GAME = true;
        *EXROM = true;
        ResetAllLEDS();
}

int CartridgeClass::CreateNewEasyFlashImage(const char* filename, const char* crt_name)
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
        HeaderLength = ConvertDWord(HeaderLength);
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
                HeaderLength = ConvertDWord(HeaderLength);
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
                HeaderLength = ConvertDWord(HeaderLength);
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

int CartridgeClass::WriteEasyFlashImage(const char *filename)
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
        HeaderLength = ConvertDWord(HeaderLength);
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
                HeaderLength = ConvertDWord(HeaderLength);
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
                HeaderLength = ConvertDWord(HeaderLength);
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

void CartridgeClass::SetEasyFlashJumper(bool enable)
{
        EasyFlashJumper = enable;
}

int CartridgeClass::GetCRTInfo(const char *filename, CRT_INFO_STRUCT* crt_info)
{
        FILE *File;
        char Kennung[17];
        unsigned short Version;
        unsigned int HeaderLength;
        unsigned long akt_pos;
        unsigned short tmp;
        int Bank1Pos,Bank2Pos,Bank3Pos;
        size_t reading_elements;

        File = fopen (filename, "rb");
        if (File == NULL)
        {
                return 0x01;
        }

        reading_elements = fread(Kennung,1,16,File);
        if(reading_elements != 16)
            return 0x01;

        Kennung[16] = 0;

        if(0!=strcmp("C64 CARTRIDGE   ",Kennung))
        {
                fclose(File);
                return 0x02;
        }

        reading_elements = fread(&HeaderLength,1,sizeof(HeaderLength),File);
        HeaderLength = ConvertDWord(HeaderLength);

        reading_elements = fread(&Version,1,sizeof(Version),File);

        sprintf(crt_info->Version,"%X.%2.2X",(unsigned char)Version,Version>>8);

        reading_elements = fread(&crt_info->HardwareType,1,sizeof(crt_info->HardwareType),File);
        crt_info->HardwareType = crt_info->HardwareType<<8 | crt_info->HardwareType>>8;
        if(crt_info->HardwareType > 32) crt_info->HardwareTypeString = (char*)TYPE_STRING[33];
        else crt_info->HardwareTypeString = (char*)TYPE_STRING[crt_info->HardwareType];

        reading_elements = fread(&crt_info->EXROM,1,sizeof(crt_info->EXROM),File);
        reading_elements = fread(&crt_info->GAME,1,sizeof(crt_info->GAME),File);

        fseek(File,0x0020,SEEK_SET);
        reading_elements = fread(crt_info->Name,1,32,File);

        akt_pos = 0x40;
        crt_info->ChipCount = 0;
        Bank1Pos = Bank2Pos = Bank3Pos = 0;
L1:
        fseek(File,akt_pos,SEEK_SET);
        if(4 > fread(Kennung,1,4,File)) goto L2;
        Kennung[4] = 0;
        if(0==strcmp("CHIP",Kennung))
        {
                reading_elements = fread(&HeaderLength,1,sizeof(HeaderLength),File);
                HeaderLength = ConvertDWord(HeaderLength);

                reading_elements = fread(&tmp,1,2,File);
                tmp = tmp<<8 | tmp>>8;
                crt_info->ChipInfo[crt_info->ChipCount].Type = tmp;
                reading_elements = fread(&tmp,1,2,File);
                tmp = tmp<<8 | tmp>>8;
                crt_info->ChipInfo[crt_info->ChipCount].BankLocation = tmp;
                reading_elements = fread(&tmp,1,2,File);
                tmp = tmp<<8 | tmp>>8;
                crt_info->ChipInfo[crt_info->ChipCount].LoadAdress = tmp;
                reading_elements = fread(&tmp,1,2,File);
                tmp = tmp<<8 | tmp>>8;
                crt_info->ChipInfo[crt_info->ChipCount].ChipSize = tmp;

                switch(crt_info->ChipInfo[crt_info->ChipCount].LoadAdress)
                {
                case 0x8000:
                        reading_elements = fread(CRT_ROM_BANK1_TMP + Bank1Pos,1,0x2000,File);
                        crt_info->ChipInfo[crt_info->ChipCount].BufferPointer = CRT_ROM_BANK1_TMP + Bank1Pos;
                        Bank1Pos += 0x2000;
                        if(tmp == 0x4000)
                        {
                                reading_elements = fread(CRT_ROM_BANK1_TMP + Bank1Pos,1,0x2000,File);
                                //crtinfo->ChipInfoHi[crtinfo->ChipCount].BufferPointer = CRT_ROM_BANK2_TMP + Bank2Pos;
                                Bank1Pos += 0x2000;
                        }
                        break;
                case 0xA000:
                        reading_elements = fread(CRT_ROM_BANK2_TMP + Bank2Pos,1,0x2000,File);
                        crt_info->ChipInfo[crt_info->ChipCount].BufferPointer = CRT_ROM_BANK2_TMP + Bank2Pos;
                        Bank2Pos += 0x2000;
                        break;
                case 0xE000:
                        reading_elements = fread(CRT_ROM_BANK2_TMP + Bank2Pos,1,0x2000,File);
                        crt_info->ChipInfo[crt_info->ChipCount].BufferPointer = CRT_ROM_BANK2_TMP + Bank2Pos;
                        Bank2Pos += 0x2000;
                        break;
                }

                crt_info->ChipCount ++;
                akt_pos += HeaderLength;
                goto L1;
        }
L2:
        fclose(File);
        return 0;
}

void CartridgeClass::Reset()
{
        ROM_LO = CRT_ROM_BANK1;
        ROM_HI = CRT_ROM_BANK2;

        if(CRTInsert)
        {
                switch(CRTTyp)
                {
                case 1:		// Action Replay 4/5/6
                        ActionReplayAktiv = true;
                        WriteIO1(0xDE00,0);
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

void CartridgeClass::Freeze()
{
        if(!CRTInsert) return;

        switch(CRTTyp)
        {
                case 1:
                /// ActionReplay 4/5/6
                    ARFreez = true;
                    WriteIO1(0xDE00,0);
                    CpuTriggerInterrupt(CRT_IRQ);
                    CpuTriggerInterrupt(CRT_NMI);
                    break;

                /// Final Cartridge III
                case 3:
                        WriteIO2(0xDFFF,16);
                        break;
        }
}

void CartridgeClass::WriteIO1(uint16_t adresse, uint8_t value)
{
        if(!CRTInsert) return;

        switch(CRTTyp)
        {
        case 1:		// Action Replay 4/5/6
                    if(ActionReplayAktiv)
                    {
                        if(value & 0x04){
                            ActionReplayAktiv = false;
                        }

                        RomLBank = (value>>3) & 0x03;

                        switch(RomLBank)
                        {
                        case 0x00:
                                ROM_LO = CRT_ROM_BANK1 + (0 * 0x2000);
                                ROM_HI = CRT_ROM_BANK1 + (0 * 0x2000);
                                break;
                        case 0x01:
                                ROM_LO = CRT_ROM_BANK1 + (1 * 0x2000);
                                ROM_HI = CRT_ROM_BANK1 + (1 * 0x2000);
                                break;
                        case 0x02:
                                ROM_LO = CRT_ROM_BANK1 + (2 * 0x2000);
                                ROM_HI = CRT_ROM_BANK1 + (2 * 0x2000);
                                break;
                        case 0x03:
                                ROM_LO = CRT_ROM_BANK1 + (3 * 0x2000);
                                ROM_HI = CRT_ROM_BANK1 + (3 * 0x2000);
                                break;
                        }

                        ARRegister = value;
                        SetMemLogicAR(adresse);

                        ARFreez = false;
                    }
                    RAM_C64[adresse] = value;
                break;
        case 4:
                if(adresse == 0xDE00)
                {
                        *GAME = false;
                        ChangeMemMapProc();
                }
                break;
        case 5:		// Ocean type 1
                value &= 0x3F;
                ROM_LO = CRT_ROM_BANK1 + (value * 0x2000);
                break;
        case 7:		// Fun Play
                if(adresse == 0xDE00)
                {
                        switch(value)
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
                value &= 0x3F;
                ROM_LO = CRT_ROM_BANK1 + (value * 0x2000);
                break;
        case 32:	// EasyFlash
                if((adresse & 0x02) == 0x00)
                {
                        EasyFlashBankReg = value & 0x3F;
                        ROM_LO = CRT_ROM_BANK1 + ((value & 0x3F) * 0x2000);
                        ROM_HI = CRT_ROM_BANK2 + ((value & 0x3F) * 0x2000);
                }
                if((adresse & 0x02) == 0x02)
                {
                        LED_01 = !!(value & 0x80);
                        if(LED_01 != LED_01_OLD) if(ChangeLED != 0) ChangeLED(1,LED_01);
                        LED_01_OLD = LED_01;
                        *EXROM = !!(~value & 0x02);

                        if(value & 0x04) *GAME = !!(~value & 0x01);
                        else *GAME = !!(EasyFlashJumper);
                        ChangeMemMapProc();
                }
                break;
        default:
                break;
        }
}

uint8_t CartridgeClass::ReadIO1(uint16_t address)
{
        if(!CRTInsert) return 0x00;	// Eigl. Zufallszahlen

        switch(CRTTyp)
        {
        case 3:		// Final Cartridge III
                return CRT_ROM_BANK1[0x1E00 + (RomLBank << 13) + (address & 0xFF)];
                break;
        case 4:
                if(address == 0xDE00)
                {
                        *GAME = true;
                        ChangeMemMapProc();
                }
                break;
        case 17:
                        ROM_LO = CRT_ROM_BANK1 + ((address&0x3F) * 0x2000);
                break;
        default:
                return 0x00;
        }
        return 0xAA;
}

void CartridgeClass::WriteIO2(uint16_t address, uint8_t value)
{
        if(!CRTInsert) return;
        switch(CRTTyp)
        {
        case 1:     // Action Replay 4/5/6
                ActionReplayRam[(address & 0xFF) + 0x1F00] = value;
                RAM_C64[address] = value;
            break;
        case 3:		// Final Cartridge III
                if((address & 0xFF) == 0xFF)
                {
                        RomLBank = value & 0x03;
                        ROM_LO = CRT_ROM_BANK1 + ((value&0x3) * 0x2000);
                        ROM_HI = CRT_ROM_BANK2 + ((value&0x3) * 0x2000);
                        *EXROM = ((~value >> 4) & 1) ^ 1;
                        *GAME = ((~value >> 5) & 1) ^ 1;
                        ChangeMemMapProc();

                        if((value & 0x30) == 0x10) CpuTriggerInterrupt(CRT_NMI);
                        if(value & 0x40) CpuClearInterrupt(CRT_NMI);
                        LED_00 = (~value>>7) & 1;
                        if(LED_00 != LED_00_OLD) if(ChangeLED != 0)  ChangeLED(0,LED_00);
                        LED_00_OLD = LED_00;
                }
                break;
        case 8:		// Super Games
                        ROM_LO = CRT_ROM_BANK1 + ((value&0x3) * 0x2000);
                        ROM_HI = CRT_ROM_BANK2 + ((value&0x3) * 0x2000);
                        if(value & 0x04)
                        {
                                *EXROM = 0;
                                *GAME = 1;
                        }
                        else *EXROM = *GAME = 0;
                        if(value == 0x0C) *EXROM = *GAME = 1;
                        ChangeMemMapProc();
                break;
        case 32:	// EasyFlash
                EasyFlashRam[address & 0xFF] = value;
                break;
        default:
                break;
        }
}

uint8_t CartridgeClass::ReadIO2(uint16_t address)
{
        if(!CRTInsert) return 0x00;	// Eigl. Zufallszahlen (Vic Phi)

        switch(CRTTyp)
        {
        case 1:		// Action Replay 4/5/6
            SetMemLogicAR(address);
            if(!ActionReplayAktiv) return 0x00;	// Eigl. Zufallszahlen (Vic Phi)
            if(EnableActionReplayRam) return ActionReplayRam[(address & 0xFF) + 0x1F00];
            else return ROM_LO[(address & 0xFF) + 0x1F00];

                break;
        case 3:		// Final Cartridge III
                switch (RomLBank)
                {
                        case 0:
                                return CRT_ROM_BANK1[address & 0x1fff];
                        case 1:
                                return CRT_ROM_BANK1[(address & 0x1fff) + 0x2000];
                        case 2:
                                return CRT_ROM_BANK1[(address & 0x1fff) + 0x4000];
                        case 3:
                                return CRT_ROM_BANK1[(address & 0x1fff) + 0x6000];
                }
                return 0;
                break;
        case 32:	// EasyFlash
                return EasyFlashRam[address & 0xFF];
                break;
        default:
                return 0x00;
                break;
        }
}

uint8_t CartridgeClass::ReadRom1(uint16_t address)
{
        if(!CRTInsert) return 0x55;	// Darf eigentlich nie vorkommen !!!!

        switch(CRTTyp)
        {
        case 1:		// Action Replay 4/5/6
                SetMemLogicAR(address);
                if(!EnableActionReplayRam) return ROM_LO[address & 0x1FFF];
                else return ActionReplayRam[address & 0x1FFF];
                break;
        case 32:	// EasyFlash
                return am29f040Lo->Read((address & 0x1FFF) | ((EasyFlashBankReg & 0x3F)<<13));
                break;
        default:
                return ROM_LO[address-0x8000];
                break;
        }
}

uint8_t CartridgeClass::ReadRom2(uint16_t address)
{
        if(!CRTInsert) return 0x55;	// Darf eigentlich nie vorkommen !!!!

        switch(CRTTyp)
        {
        case 32:	// EasyFlash
                return am29f040Hi->Read((address & 0x1FFF) | ((EasyFlashBankReg & 0x3F)<<13));
                break;
        default:
                return ROM_HI[address-0xA000];
                break;
        }
}

uint8_t CartridgeClass::ReadRom3(uint16_t address)
{
        if(!CRTInsert) return 0x55;	// Darf eigentlich nie vorkommen !!!!

        switch(CRTTyp)
        {
        case 32:	// EasyFlash
                return am29f040Hi->Read((address & 0x1FFF) | ((EasyFlashBankReg & 0x3F)<<13));
                break;
        default:
                return ROM_HI[address-0xE000];
                break;
        }
}

void CartridgeClass::WriteRom1(uint16_t address, uint8_t value)	// 0x8000
{
        if(!CRTInsert)
        {
                RAM_C64[address] = value;
                return;
        }

        switch(CRTTyp)
        {
        case 1:     // Action Replay 4/5/6
                SetMemLogicAR(address);
                if(EnableActionReplayRam)
                {
                    ActionReplayRam[address & 0x1FFF] = value;
                }
                RAM_C64[address] = value;
            break;
        case 32:	// EasyFlash
                am29f040Lo->Write((address & 0x1FFF) | ((EasyFlashBankReg & 0x3F)<<13),value);
                RAM_C64[address] = value;
                break;
        default:
                RAM_C64[address] = value;
                break;
        }
}

void CartridgeClass::WriteRom2(uint16_t address, uint8_t value)	// 0xA000
{
        if(!CRTInsert)
        {
                RAM_C64[address] = value;
                return;
        }

        switch(CRTTyp)
        {
        case 32:	// EasyFlash
                am29f040Hi->Write((address & 0x1FFF) | ((EasyFlashBankReg & 0x3F)<<13),value);
                RAM_C64[address] = value;
                break;
        default:
                RAM_C64[address] = value;
                break;
        }
}

void CartridgeClass::WriteRom3(uint16_t address, uint8_t value)	// 0xE000
{
        if(!CRTInsert)
        {
                RAM_C64[address] = value;
                return;
        }

        switch(CRTTyp)
        {
        case 32:	// EasyFlash
                am29f040Hi->Write((address & 0x1FFF) | ((EasyFlashBankReg & 0x3F)<<13),value);
                RAM_C64[address] = value;
                break;
        default:
                RAM_C64[address] = value;
                break;
        }
}
