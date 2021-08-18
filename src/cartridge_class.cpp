//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: cartridge_class.cpp                   //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 18.08.2021                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "./cartridge_class.h"
#include <cstring>

static const char* TYPE_STRING[34] = { "Normal Cartridge","Action Replay","KCS Power Cartridge",
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
    cartridge_is_insert = false;
    cartridge_type = 0;

    easyflash_jumper = false;	// false = Boot
    am29f040Lo = new AM29F040Class(rom_bank1,1);
    am29f040Hi = new AM29F040Class(rom_bank2,1);

    ResetAllLEDS();

    ar_freez = false;
}

CartridgeClass::~CartridgeClass()
{
}

void CartridgeClass::ResetAllLEDS(void)
{
    led_00 = led_00_old = led_01 = led_01_old = false;
    if(ChangeLED != nullptr) ChangeLED(0,led_00);
    if(ChangeLED != nullptr) ChangeLED(1,led_01);
}

void CartridgeClass::SetMemLogicAR(uint16_t address)
{
    if(!ar_active) return;
    uint8_t io2, pla_out;

    if((address >= 0xdf00) && (address <= 0xdfff))
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

    if(ar_freez)
        pla_address = ((ar_reg >> 5) & 1) | (ar_reg & 2) | ((ar_reg << 2) & 4) | io2 | ((address >> 9) & 16) | ((address >> 10) & 32) | ((address >> 8) & 64) | 0;
    else
        pla_address = ((ar_reg >> 5) & 1) | (ar_reg & 2) | ((ar_reg << 2) & 4) | io2 | ((address >> 9) & 16) | ((address >> 10) & 32) | ((address >> 8) & 64) | 128;

    // PLA Ausgang
    // Bit 0 - ROM Enable (CE)
    // Bit 1 - RAM Enable (CS)
    // Bit 2 - GAME
    // Bit 3 - EXROM
    pla_out = mk7pla[pla_address];

    ar_enable_ram = (~pla_out>>1 & 1);

    *game = (pla_out & 4);
    *exrom = (pla_out & 8);

    ChangeMemMapProc();
}

inline uint32_t CartridgeClass::ConvertDWord(uint32_t value)
{
    uint32_t h,l;

    l = (value >> 16) & 0xffff;
    h = value & 0xffff;

    l = ((l << 8) | (l >> 8)) & 0xffff;
    h = ((h << 8) | (h >> 8)) & 0xffff;

    value = h<<16 | l;

    return value;
}

bool* CartridgeClass::GetFlash040Dirty(uint16_t nr)
{
    switch (nr)
    {
    case 0:
        return &am29f040Lo->dirty;
    case 1:
        return &am29f040Hi->dirty;
    default:
        return nullptr;
    }
}

uint8_t *CartridgeClass::GetFlash040Byte(uint16_t nr)
{
    switch (nr)
    {
    case 0:
        return &am29f040Lo->programm_byte;
    case 1:
        return &am29f040Hi->programm_byte;
    default:
        return nullptr;
    }
}

int CartridgeClass::LoadCartridgeImage(FILE *file)
{
    char signature[17];
    uint16_t version;
    uint32_t header_lenght;
    uint32_t akt_pos;
    uint8_t exrom, game;
    uint16_t chip_count;
    uint16_t chip_address;
    uint16_t chip_size;
    size_t reading_bytes;

	if(file == nullptr) return 0x01;

    reading_bytes = fread(signature,1,16,file);

    signature[16] = 0;

    if(0!=strcmp("C64 CARTRIDGE   ",signature))
    {
        fclose(file);
        return 0x02;
    }

    reading_bytes = fread(&header_lenght,1,sizeof(header_lenght),file);
    header_lenght = ConvertDWord(header_lenght);

    reading_bytes = fread(&version,1,sizeof(version),file);

    reading_bytes = fread(&cartridge_type,1,sizeof(cartridge_type),file);
    cartridge_type = static_cast<uint16_t>(cartridge_type << 8) | (cartridge_type >> 8);

    reading_bytes = fread(&exrom,1,sizeof(exrom),file);
    reading_bytes = fread(&game,1,sizeof(game),file);

    if(exrom == 0) cartridge_exrom = false;
    else cartridge_exrom = true;
    *this->exrom = cartridge_exrom;

    if(game == 0) cartridge_game = false;
    else cartridge_game = true;
    *this->game = cartridge_game;

    fseek(file,0x0040,SEEK_SET);

    akt_pos = 0x40;
    chip_count = 0;
    L1:
    fseek(file,akt_pos,SEEK_SET);
    reading_bytes = fread(signature,1,4,file);
    if(4 > reading_bytes) goto L2;
    signature[4] = 0;
    if(0==strcmp("CHIP",signature))
    {
        if(chip_count == 128)
        {
            //MessageBox(0,"Fehler 128 in CRT Modul","Emu64",0);
            goto L2;
        }
        reading_bytes = fread(&header_lenght,1,sizeof(header_lenght),file);
        header_lenght = ConvertDWord(header_lenght);
        akt_pos += header_lenght;

        fseek(file,2,SEEK_CUR);

        uint16_t bank_pos;
        reading_bytes = fread(&bank_pos,1,2,file);
        bank_pos = static_cast<uint16_t>((bank_pos << 8) | (bank_pos >> 8));
        bank_pos &= 0xFF;    // Maximal 256 Bänke je rom_bank1 + rom_bank2

        reading_bytes = fread(&chip_address,1,2,file);
        chip_address = static_cast<uint16_t>((chip_address << 8) | (chip_address >> 8));
        reading_bytes = fread(&chip_size,1,2,file);
        chip_size = static_cast<uint16_t>((chip_size << 8) | (chip_size >> 8));

        switch(chip_address)
        {
        case 0x8000:
            reading_bytes = fread(rom_bank1 + (bank_pos * 0x2000),1,0x2000,file);
            if(chip_size == 0x4000)
            {
                reading_bytes = fread(rom_bank2 + (bank_pos * 0x2000),1,0x2000,file);
            }
            break;
        case 0xA000:
            reading_bytes = fread(rom_bank2 + (bank_pos * 0x2000),1,0x2000,file);
            break;
        case 0xE000:
            reading_bytes = fread(rom_bank2 + (bank_pos * 0x2000),1,0x2000,file);
            break;
        }
        chip_count ++;
        goto L1;
    }
    L2:

    lo_rom = rom_bank1;
    hi_rom = rom_bank2;

    cartridge_is_insert = true;
    ResetAllLEDS();

    Reset();

    return 0;
}

void CartridgeClass::RemoveCartridgeImage()
{
    cartridge_is_insert = false;
    *game = true;
    *exrom = true;
    ResetAllLEDS();
}

int CartridgeClass::CreateNewEasyFlashImage(FILE *file, const char* crt_name)
{
    const char* signature = "C64 CARTRIDGE   ";
    const char* chip_signature = "CHIP";
    uint16_t version;
    uint32_t header_lenght;
    uint8_t exrom, game;
    uint16_t chip_address;
    uint16_t chip_size;
    uint16_t chip_typ;
    uint16_t chip_bank;

	if (file == nullptr)
		return 0x01;

	fwrite(signature,1,16,file);

    /*****/ header_lenght = 0x40;
    header_lenght = ConvertDWord(header_lenght);
	fwrite(&header_lenght,1,sizeof(header_lenght),file);

    /*****/ version = 0x0001;
	fwrite(&version,1,sizeof(version),file);

    /*****/ cartridge_type = 32;
    cartridge_type = static_cast<uint16_t>((cartridge_type << 8) | (cartridge_type >> 8));
	fwrite(&cartridge_type,1,sizeof(cartridge_type),file);

    /*****/ exrom = 1;
    /*****/ game = 0;
	fwrite(&exrom,1,sizeof(exrom),file);
	fwrite(&game,1,sizeof(game),file);

    /*****/ // 6 Byte Reserviert
	fseek(file,0x0020,SEEK_SET);

    /*****/ // CRT Name
	fwrite(crt_name,1,32,file);

    for(int i=0;i<64;i++)
    {
		fwrite(chip_signature,1,4,file);
        /*****/ header_lenght = 0x00002010;
        header_lenght = ConvertDWord(header_lenght);
		fwrite(&header_lenght,1,sizeof(header_lenght),file);

        chip_typ = 2;
        chip_typ = static_cast<uint16_t>((chip_typ << 8) | (chip_typ >> 8));
		fwrite(&chip_typ,1,sizeof(chip_typ),file);

        chip_bank = static_cast<uint16_t>(i);
        chip_bank = static_cast<uint16_t>((chip_bank << 8) | (chip_bank >> 8));
		fwrite(&chip_bank,1,sizeof(chip_bank),file);

        chip_address = 0x8000;
        chip_address = static_cast<uint16_t>((chip_address << 8) | (chip_address >> 8));
		fwrite(&chip_address,1,sizeof(chip_address),file);

        chip_size = 0x2000;
        chip_size = static_cast<uint16_t>((chip_size << 8) | (chip_size >> 8));
		fwrite(&chip_size,1,sizeof(chip_size),file);

		fseek(file,0x2000,SEEK_CUR);
    }

    for(int i=0;i<64;i++)
    {
		fwrite(chip_signature,1,4,file);
        /*****/ header_lenght = 0x00002010;
        header_lenght = ConvertDWord(header_lenght);
		fwrite(&header_lenght,1,sizeof(header_lenght),file);

        chip_typ = 2;
        chip_typ = static_cast<uint16_t>((chip_typ << 8 | chip_typ >> 8));
		fwrite(&chip_typ,1,sizeof(chip_typ),file);

        chip_bank = static_cast<uint16_t>(i);
        chip_bank = static_cast<uint16_t>((chip_bank << 8) | (chip_bank >> 8));
		fwrite(&chip_bank,1,sizeof(chip_bank),file);

        chip_address = 0xA000;
        chip_address = static_cast<uint16_t>((chip_address << 8) | (chip_address >> 8));
		fwrite(&chip_address,1,sizeof(chip_address),file);

        chip_size = 0x2000;
        chip_size = static_cast<uint16_t>((chip_size << 8) | (chip_size >> 8));
		fwrite(&chip_size,1,sizeof(chip_size),file);

		fseek(file,0x2000,SEEK_CUR);
    }

	fclose(file);

    return 0;
}

int CartridgeClass::WriteEasyFlashImage(FILE *file, CARTRIDGE_INFO_STRUCT *cartridge_info)
{
    const char* signature = "C64 CARTRIDGE   ";
    const char* chip_signature = "CHIP";
    uint16_t version;
    uint32_t header_lenght;
    uint8_t exrom, game;
    uint16_t chip_address;
    uint16_t chip_size;
    uint16_t chip_typ;
    uint16_t chip_bank;

	if (file == nullptr || cartridge_info == nullptr)
		return 0x01;

	fwrite(signature,1,16,file);

    /*****/ header_lenght = 0x40;
    header_lenght = ConvertDWord(header_lenght);
	fwrite(&header_lenght,1,sizeof(header_lenght),file);

    /*****/ version = 0x0001;
	fwrite(&version,1,sizeof(version),file);

    /*****/ cartridge_type = 32;
    cartridge_type = static_cast<uint16_t>((cartridge_type << 8) | (cartridge_type >> 8));
	fwrite(&cartridge_type,1,sizeof(cartridge_type),file);

    /*****/ exrom = 1;
    /*****/ game = 0;
	fwrite(&exrom,1,sizeof(exrom),file);
	fwrite(&game,1,sizeof(game),file);

    /*****/ // 6 Byte Reserviert
	fseek(file,0x0020,SEEK_SET);

    /*****/ // CRT Name
	fwrite(cartridge_info->Name,1,32,file);


    for(int i=0;i<64;i++)
    {
		fwrite(chip_signature,1,4,file);
        /*****/ header_lenght = 0x00002010;
        header_lenght = ConvertDWord(header_lenght);
		fwrite(&header_lenght,1,sizeof(header_lenght),file);

        chip_typ = 2;
        chip_typ = static_cast<uint16_t>((chip_typ << 8) | (chip_typ >> 8));
		fwrite(&chip_typ,1,sizeof(chip_typ),file);

        chip_bank = static_cast<uint16_t>(i);
        chip_bank = static_cast<uint16_t>((chip_bank << 8) | (chip_bank >> 8));
		fwrite(&chip_bank,1,sizeof(chip_bank),file);

        chip_address = 0x8000;
        chip_address = static_cast<uint16_t>((chip_address << 8) | (chip_address >> 8));
		fwrite(&chip_address,1,sizeof(chip_address),file);

        chip_size = 0x2000;
        chip_size = static_cast<uint16_t>((chip_size << 8) | (chip_size >> 8));
		fwrite(&chip_size,1,sizeof(chip_size),file);

		fwrite(&rom_bank1[i*0x2000],1,0x2000,file);
    }

    for(int i=0;i<64;i++)
    {
		fwrite(chip_signature,1,4,file);
        /*****/ header_lenght = 0x00002010;
        header_lenght = ConvertDWord(header_lenght);
		fwrite(&header_lenght,1,sizeof(header_lenght),file);

        chip_typ = 2;
        chip_typ = static_cast<uint16_t>((chip_typ << 8) | (chip_typ >> 8));
		fwrite(&chip_typ,1,sizeof(chip_typ),file);

        chip_bank = static_cast<uint16_t>(i);
        chip_bank = static_cast<uint16_t>((chip_bank << 8) | (chip_bank >> 8));
		fwrite(&chip_bank,1,sizeof(chip_bank),file);

        chip_address = 0xA000;
        chip_address = static_cast<uint16_t>((chip_address << 8) | (chip_address >> 8));
		fwrite(&chip_address,1,sizeof(chip_address),file);

        chip_size = 0x2000;
        chip_size = static_cast<uint16_t>((chip_size << 8) | (chip_size >> 8));
		fwrite(&chip_size,1,sizeof(chip_size),file);

		fwrite(&rom_bank2[i*0x2000],1,0x2000,file);
    }

	fclose(file);

    return 0;
}

void CartridgeClass::SetEasyFlashJumper(bool enable)
{
    easyflash_jumper = enable;
}

int CartridgeClass::GetCartridgeInfo(FILE *file, CARTRIDGE_INFO_STRUCT* crt_info)
{
    char signature[17];
    uint16_t version;
    uint32_t header_lenght;
    uint32_t akt_pos;
    uint16_t tmp;
    uint16_t Bank1Pos, Bank2Pos, Bank3Pos;
    size_t reading_elements;

	if (file == nullptr)
    {
        return 0x01;
    }

	reading_elements = fread(signature,1,16,file);
    if(reading_elements != 16)
        return 0x01;

    signature[16] = 0;

    if(0!=strcmp("C64 CARTRIDGE   ",signature))
    {
		fclose(file);
        return 0x02;
    }

	reading_elements = fread(&header_lenght,1,sizeof(header_lenght),file);
    header_lenght = ConvertDWord(header_lenght);

	reading_elements = fread(&version,1,sizeof(version),file);

    sprintf(crt_info->Version,"%X.%2.2X",static_cast<uint8_t>(version), version>>8);

	reading_elements = fread(&crt_info->HardwareType,1,sizeof(crt_info->HardwareType),file);
    crt_info->HardwareType = static_cast<uint16_t>((crt_info->HardwareType << 8) | (crt_info->HardwareType >> 8));
    if(crt_info->HardwareType > 32) crt_info->HardwareTypeString = const_cast<char*>(TYPE_STRING[33]);
    else crt_info->HardwareTypeString = const_cast<char*>(TYPE_STRING[crt_info->HardwareType]);

	reading_elements = fread(&crt_info->EXROM,1,sizeof(crt_info->EXROM),file);
	reading_elements = fread(&crt_info->GAME,1,sizeof(crt_info->GAME),file);

	fseek(file,0x0020,SEEK_SET);
	reading_elements = fread(crt_info->Name,1,32,file);

    akt_pos = 0x40;
    crt_info->ChipCount = 0;
    Bank1Pos = Bank2Pos = Bank3Pos = 0;
L1:
	fseek(file,akt_pos,SEEK_SET);
	if(4 > fread(signature,1,4,file)) goto L2;
    signature[4] = 0;
    if(0==strcmp("CHIP",signature))
    {
		reading_elements = fread(&header_lenght,1,sizeof(header_lenght),file);
        header_lenght = ConvertDWord(header_lenght);

		reading_elements = fread(&tmp,1,2,file);
        tmp = static_cast<uint16_t>((tmp << 8) | (tmp >> 8));
        crt_info->ChipInfo[crt_info->ChipCount].Type = tmp;
		reading_elements = fread(&tmp,1,2,file);
        tmp = static_cast<uint16_t>((tmp << 8) | (tmp >> 8));
        crt_info->ChipInfo[crt_info->ChipCount].BankLocation = tmp;
		reading_elements = fread(&tmp,1,2,file);
        tmp = static_cast<uint16_t>((tmp << 8) | (tmp >> 8));
        crt_info->ChipInfo[crt_info->ChipCount].LoadAdress = tmp;
		reading_elements = fread(&tmp,1,2,file);
        tmp = static_cast<uint16_t>((tmp << 8) | (tmp >> 8));
        crt_info->ChipInfo[crt_info->ChipCount].ChipSize = tmp;

        switch(crt_info->ChipInfo[crt_info->ChipCount].LoadAdress)
        {
        case 0x8000:
			reading_elements = fread(rom_bank1_tmp + Bank1Pos,1,0x2000,file);
            crt_info->ChipInfo[crt_info->ChipCount].BufferPointer = rom_bank1_tmp + Bank1Pos;
            Bank1Pos += 0x2000;
            if(tmp == 0x4000)
            {
				reading_elements = fread(rom_bank1_tmp + Bank1Pos,1,0x2000,file);
                //crtinfo->ChipInfoHi[crtinfo->ChipCount].BufferPointer = rom_bank2_TMP + Bank2Pos;
                Bank1Pos += 0x2000;
            }
            break;
        case 0xA000:
			reading_elements = fread(rom_bank2_tmp + Bank2Pos,1,0x2000,file);
            crt_info->ChipInfo[crt_info->ChipCount].BufferPointer = rom_bank2_tmp + Bank2Pos;
            Bank2Pos += 0x2000;
            break;
        case 0xE000:
			reading_elements = fread(rom_bank2_tmp + Bank2Pos,1,0x2000,file);
            crt_info->ChipInfo[crt_info->ChipCount].BufferPointer = rom_bank2_tmp + Bank2Pos;
            Bank2Pos += 0x2000;
            break;
        }

        crt_info->ChipCount ++;
        akt_pos += header_lenght;
        goto L1;
    }
L2:
		fclose(file);
        return 0;
}

void CartridgeClass::Reset()
{
    lo_rom = rom_bank1;
    hi_rom = rom_bank2;

    if(cartridge_is_insert)
    {
        switch(cartridge_type)
        {
        case 1:		// Action Replay 4/5/6
            ar_active = true;
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
            *game = cartridge_game;
            *exrom = cartridge_exrom;
            break;
        }
    }
    else
    {
        *game = true;
        *exrom = true;
    }
}

void CartridgeClass::Freeze()
{
    if(!cartridge_is_insert) return;

    switch(cartridge_type)
    {
        case 1:
        /// ActionReplay 4/5/6
            ar_freez = true;
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
    if(!cartridge_is_insert) return;

    switch(cartridge_type)
    {
    case 1:		// Action Replay 4/5/6
        if(ar_active)
        {
            if(value & 0x04){
                ar_active = false;
            }

            rom_lo_bank = (value>>3) & 0x03;

            switch(rom_lo_bank)
            {
            case 0x00:
                    lo_rom = rom_bank1 + (0 * 0x2000);
                    hi_rom = rom_bank1 + (0 * 0x2000);
                    break;
            case 0x01:
                    lo_rom = rom_bank1 + (1 * 0x2000);
                    hi_rom = rom_bank1 + (1 * 0x2000);
                    break;
            case 0x02:
                    lo_rom = rom_bank1 + (2 * 0x2000);
                    hi_rom = rom_bank1 + (2 * 0x2000);
                    break;
            case 0x03:
                    lo_rom = rom_bank1 + (3 * 0x2000);
                    hi_rom = rom_bank1 + (3 * 0x2000);
                    break;
            }

            ar_reg = value;
            SetMemLogicAR(adresse);

            ar_freez = false;
        }
        c64_ram[adresse] = value;
        break;
    case 4:
        if(adresse == 0xDE00)
        {
            *game = false;
            ChangeMemMapProc();
        }
        break;
    case 5:		// Ocean type 1
        value &= 0x3F;
        lo_rom = rom_bank1 + (value * 0x2000);
        break;
    case 7:		// Fun Play
        if(adresse == 0xDE00)
        {
            switch(value)
            {
            case 0x00:
                lo_rom = rom_bank1 + (0 * 0x2000);
                break;
            case 0x08:
                lo_rom = rom_bank1 + (1 * 0x2000);
                break;
            case 0x10:
                lo_rom = rom_bank1 + (2 * 0x2000);
                break;
            case 0x18:
                lo_rom = rom_bank1 + (3 * 0x2000);
                break;
            case 0x20:
                lo_rom = rom_bank1 + (4 * 0x2000);
                break;
            case 0x28:
                lo_rom = rom_bank1 + (5 * 0x2000);
                break;
            case 0x30:
                lo_rom = rom_bank1 + (6 * 0x2000);
                break;
            case 0x38:
                lo_rom = rom_bank1 + (7 * 0x2000);
                break;
            case 0x01:
                lo_rom = rom_bank1 + (8 * 0x2000);
                break;
            case 0x09:
                lo_rom = rom_bank1 + (9 * 0x2000);
                break;
            case 0x11:
                lo_rom = rom_bank1 + (10 * 0x2000);
                break;
            case 0x19:
                lo_rom = rom_bank1 + (11 * 0x2000);
                break;
            case 0x21:
                lo_rom = rom_bank1 + (12 * 0x2000);
                break;
            case 0x29:
                lo_rom = rom_bank1 + (13 * 0x2000);
                break;
            case 0x31:
                lo_rom = rom_bank1 + (14 * 0x2000);
                break;
            case 0x39:
                lo_rom = rom_bank1 + (15 * 0x2000);
                break;
            }
        }
        break;
    case 15:	// C64 Gamesystem
        value &= 0x3F;
        lo_rom = rom_bank1 + (value * 0x2000);
        break;
    case 19:    // Magic Desk Cartridge 512k (Bis jetzt read-only)

        // Bit 6 ist mit dem CE vom 29F040 verbunden
        // Bit 7 ist mit EXROM verbunden

        value &= 0x3F;
        lo_rom = rom_bank1 + (value * 0x2000);
        *exrom = !!(value & 0x80);
        ChangeMemMapProc();
        break;
    case 32:	// EasyFlash
        if((adresse & 0x02) == 0x00)
        {
            easyflash_bank_reg = value & 0x3F;
            lo_rom = rom_bank1 + ((value & 0x3F) * 0x2000);
            hi_rom = rom_bank2 + ((value & 0x3F) * 0x2000);
        }
        if((adresse & 0x02) == 0x02)
        {
            led_01 = !!(value & 0x80);
            if(led_01 != led_01_old) if(ChangeLED != nullptr) ChangeLED(1,led_01);
            led_01_old = led_01;
            *exrom = !!(~value & 0x02);

            if(value & 0x04) *game = !!(~value & 0x01);
            else *game = !!(easyflash_jumper);
            ChangeMemMapProc();
        }
        break;
    default:
        break;
    }
}

uint8_t CartridgeClass::ReadIO1(uint16_t address)
{
    if(!cartridge_is_insert) return 0x00;	// Eigl. Zufallszahlen

    switch(cartridge_type)
    {
    case 3:		// Final Cartridge III
        return rom_bank1[0x1E00 + (rom_lo_bank << 13) + (address & 0xFF)];
    case 4:
        if(address == 0xDE00)
        {
                *game = true;
                ChangeMemMapProc();
        }
        break;
    case 17:
        lo_rom = rom_bank1 + ((address&0x3F) * 0x2000);
        break;
    default:
        return 0x00;
    }
    return 0xAA;
}

void CartridgeClass::WriteIO2(uint16_t address, uint8_t value)
{
    if(!cartridge_is_insert) return;
    switch(cartridge_type)
    {
    case 1:     // Action Replay 4/5/6
        ar_ram[(address & 0xFF) + 0x1F00] = value;
        c64_ram[address] = value;
        break;
    case 3:		// Final Cartridge III
        if((address & 0xFF) == 0xFF)
        {
            rom_lo_bank = value & 0x03;
            lo_rom = rom_bank1 + ((value&0x3) * 0x2000);
            hi_rom = rom_bank2 + ((value&0x3) * 0x2000);
            *exrom = ((~value >> 4) & 1) ^ 1;
            *game = ((~value >> 5) & 1) ^ 1;
            ChangeMemMapProc();

            if((value & 0x30) == 0x10) CpuTriggerInterrupt(CRT_NMI);
            if(value & 0x40) CpuClearInterrupt(CRT_NMI);
            led_00 = (~value>>7) & 1;
            if(led_00 != led_00_old) if(ChangeLED != nullptr)  ChangeLED(0,led_00);
            led_00_old = led_00;
        }
        break;
    case 8:		// Super Games
        lo_rom = rom_bank1 + ((value&0x3) * 0x2000);
        hi_rom = rom_bank2 + ((value&0x3) * 0x2000);
        if(value & 0x04)
        {
            *exrom = 0;
            *game = 1;
        }
        else *exrom = *game = 0;
        if(value == 0x0C) *exrom = *game = 1;
        ChangeMemMapProc();
        break;
    case 32:	// EasyFlash
        easyflash_ram[address & 0xFF] = value;
        break;
    default:
        break;
    }
}

uint8_t CartridgeClass::ReadIO2(uint16_t address)
{
    if(!cartridge_is_insert) return 0x00;	// Eigl. Zufallszahlen (Vic Phi)

    switch(cartridge_type)
    {
    case 1:		// Action Replay 4/5/6
        SetMemLogicAR(address);
        if(!ar_active) return 0x00;	// Eigl. Zufallszahlen (Vic Phi)
        if(ar_enable_ram) return ar_ram[(address & 0xFF) + 0x1F00];
        else return lo_rom[(address & 0xFF) + 0x1F00];

    case 3:		// Final Cartridge III
        switch (rom_lo_bank)
        {
            case 0:
                return rom_bank1[address & 0x1fff];
            case 1:
                return rom_bank1[(address & 0x1fff) + 0x2000];
            case 2:
                return rom_bank1[(address & 0x1fff) + 0x4000];
            case 3:
                return rom_bank1[(address & 0x1fff) + 0x6000];
        }
        return 0;

    case 32:	// EasyFlash
        return easyflash_ram[address & 0xFF];

    default:
        return 0x00;
    }
}

uint8_t CartridgeClass::ReadRom1(uint16_t address)
{
    if(!cartridge_is_insert) return 0x55;	// Darf eigentlich nie vorkommen !!!!

    switch(cartridge_type)
    {
    case 1:		// Action Replay 4/5/6
        SetMemLogicAR(address);
        if(!ar_enable_ram) return lo_rom[address & 0x1FFF];
        else return ar_ram[address & 0x1FFF];

    case 32:	// EasyFlash
        return am29f040Lo->Read(static_cast<uint32_t>((address & 0x1FFF) | ((easyflash_bank_reg & 0x3F)<<13)));

    default:
        return lo_rom[address-0x8000];
    }
}

uint8_t CartridgeClass::ReadRom2(uint16_t address)
{
    if(!cartridge_is_insert) return 0x55;	// Darf eigentlich nie vorkommen !!!!

    switch(cartridge_type)
    {
    case 32:	// EasyFlash
        return am29f040Hi->Read(static_cast<uint32_t>((address & 0x1FFF) | ((easyflash_bank_reg & 0x3F)<<13)));

    default:
        return hi_rom[address-0xA000];
    }
}

uint8_t CartridgeClass::ReadRom3(uint16_t address)
{
    if(!cartridge_is_insert) return 0x55;	// Darf eigentlich nie vorkommen !!!!

    switch(cartridge_type)
    {
    case 32:	// EasyFlash
        return am29f040Hi->Read(static_cast<uint32_t>((address & 0x1FFF) | ((easyflash_bank_reg & 0x3F)<<13)));

    default:
        return hi_rom[address-0xE000];
    }
}

void CartridgeClass::WriteRom1(uint16_t address, uint8_t value)	// 0x8000
{
    if(!cartridge_is_insert)
    {
        c64_ram[address] = value;
        return;
    }

    switch(cartridge_type)
    {
    case 1:     // Action Replay 4/5/6
        SetMemLogicAR(address);
        if(ar_enable_ram)
        {
            ar_ram[address & 0x1FFF] = value;
        }
        c64_ram[address] = value;
        break;
    case 32:	// EasyFlash
        am29f040Lo->Write(static_cast<uint32_t>((address & 0x1FFF) | ((easyflash_bank_reg & 0x3F)<<13)), value);
        c64_ram[address] = value;
        break;
    default:
        c64_ram[address] = value;
        break;
    }
}

void CartridgeClass::WriteRom2(uint16_t address, uint8_t value)	// 0xA000
{
    if(!cartridge_is_insert)
    {
        c64_ram[address] = value;
        return;
    }

    switch(cartridge_type)
    {
    case 32:	// EasyFlash
        am29f040Hi->Write(static_cast<uint32_t>((address & 0x1FFF) | ((easyflash_bank_reg & 0x3F)<<13)),value);
        c64_ram[address] = value;
        break;
    default:
        c64_ram[address] = value;
        break;
    }
}

void CartridgeClass::WriteRom3(uint16_t address, uint8_t value)	// 0xE000
{
    if(!cartridge_is_insert)
    {
        c64_ram[address] = value;
        return;
    }

    switch(cartridge_type)
    {
    case 32:	// EasyFlash
        am29f040Hi->Write(static_cast<uint32_t>((address & 0x1FFF) | ((easyflash_bank_reg & 0x3F)<<13)), value);
        c64_ram[address] = value;
        break;
    default:
        c64_ram[address] = value;
        break;
    }
}
