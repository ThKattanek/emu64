//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: reu_class.cpp                         //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "./structs.h"
#include "./reu_class.h"

REUClass::REUClass()
{
    is_reu_insert = false;

    SetReuRamMode(REU_512KiB);  // Standardmäßig 512KiB, da dies die am meisten verbreitete REU Größe ist

    ClearRAM();

    DMAStatus = 0;

    Reset();
}

REUClass::~REUClass()
{
}

void REUClass::Insert(void)
{
    is_reu_insert = true;
    reu_wait_ff00 = false;
}

void REUClass::Remove(void)
{
    is_reu_insert = false;
}

void REUClass::SetReuRamMode(uint8_t mode)
{
    reu_ram_mode = mode;

    switch (reu_ram_mode)
    {
    case REU_128KiB:
        ram_bank_count = 2;   // 128KiB = 2 Bänke
        ram_bank_mask = 0x01; // Maske für die Banknummer (nur die niedrigste Bit ist relevant)
        break;
    case REU_256KiB:
        ram_bank_count = 4;   // 256KiB = 4 Bänke
        ram_bank_mask = 0x03; // Maske für die Banknummer (die 2 niedrigsten Bits sind relevant)
        break;
    case REU_512KiB:
        ram_bank_count = 8;   // 512KiB = 8 Bänke
        ram_bank_mask = 0x07; // Maske für die Banknummer (die 3 niedrigsten Bits sind relevant)
        break;
    case REU_1MiB:
        ram_bank_count = 16;  // 1024KiB = 16 Bänke
        ram_bank_mask = 0x0F; // Maske für die Banknummer (die 4 niedrigsten Bits sind relevant)
        break;
    case REU_2MiB:
        ram_bank_count = 32;  // 2048KiB = 32 Bänke
        ram_bank_mask = 0x1F; // Maske für die Banknummer (die 5 niedrigsten Bits sind relevant)
        break;
    case REU_4MiB:
        ram_bank_count = 64;  // 4096KiB = 64 Bänke
        ram_bank_mask = 0x3F; // Maske für die Banknummer (die 6 niedrigsten Bits sind relevant)
        break;
    case REU_8MiB:
        ram_bank_count = 128; // 8192KiB = 128 Bänke
        ram_bank_mask = 0x7F; // Maske für die Banknummer (die 7 niedrigsten Bits sind relevant)
        break;
    case REU_16MiB:
        ram_bank_count = 256; // 16384KiB = 256 Bänke
        ram_bank_mask = 0xFF; // Maske für die Banknummer (alle 8 Bits sind relevant)
        break;
    default:
        reu_ram_mode = REU_512KiB;
        ram_bank_count = 8;   // Standardmäßig 512KiB = 8 Bänke
        ram_bank_mask = 0x07; // Maske für die Banknummer (die 3 niedrigsten Bits sind relevant)
        break;
    }
}

uint8_t REUClass::GetReuRamMode()
{
    return reu_ram_mode;
}

int REUClass::LoadRAM(const char *filename)
{
    /// REU Inhalt laden ...
    FILE* file;
    file = fopen (filename,"rb");
    if (file == NULL)
    {
        // Datei konnte nicht geöffnet werden
        return -1;
    }

    // File size ermitteln
    fseek(file, 0, SEEK_END);   // Zeiger ans Ende setzen
    long file_size = ftell(file);    // Position ist Größe
    fseek(file, 0, SEEK_SET);   // Zeiger zurück zum Anfang setzen

    if(file_size > ram_bank_count * 0x10000)
    {
        // Datei ist zu groß für die Anzahl der verfügbaren RAM-Bänke
        fclose(file);
        return -2;
    }

    size_t reading_bytes;

    ClearRAM(); // Vor dem Laden den RAM löschen, um sicherzustellen, dass nicht geladene Bereiche auf 0 gesetzt sind
    for(int i=0; i < ram_bank_count; i++)
    {
        reading_bytes = fread(ram_baenke[i], 1, 0x10000,file);
        if(reading_bytes < 0x10000)
        {
            // Die Datei enthält nicht genügend Daten, um die aktuelle RAM-Bank vollständig zu laden
            fclose(file);
            return -3;
        }
    }

    fclose(file);
    return 0;
}

int REUClass::SaveRAM(const char *filename)
{
    /// REU Inhalt speichern ...
    FILE* file;
    file = fopen (filename, "wb");
    if (file == NULL)
    {
        return 1;
    }
    for(int i=0; i<256; i++) fwrite(ram_baenke[i], 1, 0x10000, file);
    fclose(file);
    return 0;
}

void REUClass::ClearRAM(void)
{
    for(int i=0; i<256; i++)
        for(int j=0; j<0x10000; j++)
            ram_baenke[i][j] = 0;
}

void REUClass::OneZyklus(void)
{
    static unsigned char tmp1;

    if(!is_reu_insert) return;
    if(!*RESET) Reset();
    if(!BA_STATUS) *BA = false;

    if(*WRITE_FF00)
    {
        *WRITE_FF00 = false;

        if(reu_wait_ff00)
        {
            BA_STATUS = false;		// CPU anhalten
            cpu_wait_counter = 3;		// 3 Zyklen Warten bis CPU anhält
            tarnsfer_start = true;
            switch(transfer_typ)
            {
            case 0:
                DMAStatus = 2;
                break;
            case 1:
                DMAStatus = 1;
                break;
            case 2:
                DMAStatus = 3;
                break;
            case 3:
                DMAStatus = 4;
                break;
            }
        }
    }

    if(tarnsfer_start)
    {
        if(cpu_wait_counter > 0)
        {
            cpu_wait_counter--;
        }
        else
        {
            switch(transfer_typ)
            {
            case 0:		// STASH  C64 ---> REU
                ram_baenke[current_bank & ram_bank_mask][reu_address] = Read(c64_address);
                if((io[10] & 128) != 128) c64_address++;
                if((io[10] & 64) != 64)
                {
                    reu_address++;
                    if(reu_address == 0) current_bank++;
                }
                counter--;
                if(counter == 0)
                {
                    tarnsfer_start = false;
                    io[0] |= 64;
                    BA_STATUS = true;
                    if((io[1] & 32) != 32)
                    {
                        io[2] = (unsigned char) c64_address;
                        io[3] = (unsigned char) (c64_address >> 8);
                        io[4] = (unsigned char) reu_address;
                        io[5] = (unsigned char) (reu_address >> 8);
                        io[6] = current_bank & ram_bank_mask;
                        io[7] = (unsigned char) counter;
                        io[8] = (unsigned char) (counter >> 8);
                    }
                    if((io[9] & 128) == 128)	// Interrup ist zugelasssen ?
                    {
                        // Ja
                        if((io[9] & 64) == 64) io[0] |= 128;
                    }
                }
                break;
            case 1:		// FETCH  REU ---> C64
                Write(c64_address, ram_baenke[current_bank & ram_bank_mask][reu_address]);

                if((io[10] & 128) != 128) c64_address++;
                if((io[10] & 64) != 64)
                {
                    reu_address++;
                    if(reu_address == 0) current_bank++;
                }
                counter--;
                if(counter == 0)
                {
                    tarnsfer_start = false;
                    io[0] |= 64;
                    BA_STATUS = true;
                    if((io[1] & 32) != 32)
                    {
                        io[2] = (unsigned char) c64_address;
                        io[3] = (unsigned char) (c64_address >> 8);
                        io[4] = (unsigned char) reu_address;
                        io[5] = (unsigned char) (reu_address >> 8);
                        io[6] = current_bank & ram_bank_mask;
                        io[7] = (unsigned char) counter;
                        io[8] = (unsigned char) (counter >> 8);
                    }
                    if((io[9] & 128) == 128)	// Interrup ist zugelasssen ?
                    {
                        // Ja
                        if((io[9] & 64) == 64) io[0] |= 128;
                    }
                }
                break;
            case 2:		// SWAP   C64 <--> REU
                tmp1 = Read(c64_address);
                Write(c64_address, ram_baenke[current_bank & ram_bank_mask][reu_address]);
                ram_baenke[current_bank & ram_bank_mask][reu_address] = tmp1;
                if((io[10] & 128) != 128) c64_address++;
                if((io[10] & 64) != 64)
                {
                    reu_address++;
                    if(reu_address == 0) current_bank++;
                }
                counter--;
                if(counter == 0)
                {
                    tarnsfer_start = false;
                    io[0] |= 64;
                    BA_STATUS = true;
                    if((io[1] & 32) != 32)
                    {
                        io[2] = (unsigned char) c64_address;
                        io[3] = (unsigned char) (c64_address >> 8);
                        io[4] = (unsigned char) reu_address;
                        io[5] = (unsigned char) (reu_address >> 8);
                        io[6] = current_bank & ram_bank_mask;
                        io[7] = (unsigned char) counter;
                        io[8] = (unsigned char) (counter >> 8);
                    }
                    if((io[9] & 128) == 128)	// Interrup ist zugelasssen ?
                    {
                        // Ja
                        if((io[9] & 64) == 64) io[0] |= 128;
                    }
                }
                break;
            case 3:		// VERIFY C64 ---- REU
                if(ram_baenke[current_bank & ram_bank_mask][reu_address] != Read(c64_address))
                {
                    tarnsfer_start = false;
                    io[0] |= 32;
                    io[0] |= 64;
                    BA_STATUS = true;
                    if((io[9] & 128) == 128)	// Interrup ist zugelasssen ?
                    {
                        // Ja
                        if((io[9] & 32) == 32) io[0] |= 128;
                    }
                }
                if((io[10] & 128) != 128) c64_address++;
                if((io[10] & 64) != 64)
                {
                    reu_address++;
                    if(reu_address == 0) current_bank++;
                }
                counter--;
                if(counter == 0)
                {
                    tarnsfer_start = false;
                    io[0] |= 64;
                    BA_STATUS = true;
                    if((io[1] & 32) != 32)
                    {
                        io[2] = (unsigned char) c64_address;
                        io[3] = (unsigned char) (c64_address >> 8);
                        io[4] = (unsigned char) reu_address;
                        io[5] = (unsigned char) (reu_address >> 8);
                        io[6] = current_bank & ram_bank_mask;
                        io[7] = (unsigned char) counter;
                        io[8] = (unsigned char) (counter >> 8);
                    }
                    if((io[9] & 128) == 128)	// Interrup ist zugelasssen ?
                    {
                        // Ja
                        if((io[9] & 64) == 64) io[0] |= 128;
                    }
                }
                break;
            }
        }
    }
    if((io[0] & 128) == 128) CpuTriggerInterrupt(REU_IRQ);
    else CpuClearInterrupt(REU_IRQ);
}

inline unsigned char REUClass::Read(unsigned short adresse)
{
    unsigned char wert = ReadProcTbl[(adresse)>>8](adresse);
    return wert;
}

inline void REUClass::Write(unsigned short adresse, unsigned char wert)
{
    WriteProcTbl[(adresse)>>8](adresse,wert);
}

void REUClass::WriteIO1(uint16_t /*adresse*/, uint8_t /*wert*/)
{
}

void REUClass::WriteIO2(uint16_t adresse, uint8_t wert)
{
    if(!is_reu_insert) return;
    switch(adresse)
    {
    case 0xdf00:	// READ ONLY
        break;
    case 0xdf01:
        io[1] = wert;
        current_bank = io[6];
        c64_address = io[2] | (io[3]<<8);
        reu_address = io[4] | (io[5]<<8);
        counter = io[7] | (io[8]<<8);
        transfer_typ = wert & 3;
        if((wert & 128) == 128)	// Befehl ausführen
        {
            if((io[1] & 0x10) == 0x10)
            {
                BA_STATUS = false;		// CPU anhalten
                cpu_wait_counter = 3;		// 3 Zyklen Warten bis CPU anhält
                tarnsfer_start = true;
                switch(transfer_typ)
                {
                case 0:
                    DMAStatus = 2;
                    break;
                case 1:
                    DMAStatus = 1;
                    break;
                case 2:
                    DMAStatus = 3;
                    break;
                case 3:
                    DMAStatus = 4;
                    break;
                }
            }
            else
            {
                reu_wait_ff00 = true;
            }
        }
        break;
    case 0xdf02:
        io[2] = wert;
        break;
    case 0xdf03:
        io[3] = wert;
        break;
    case 0xdf04:
        io[4] = wert;
        break;
    case 0xdf05:
        io[5] = wert;
        break;
    case 0xdf06:
        io[6] = wert;
        break;
    case 0xdf07:
        io[7] = wert;
        break;
    case 0xdf08:
        io[8] = wert;
        break;
    case 0xdf09:
        io[9] = wert;
        break;
    case 0xdf0a:
        io[10] = wert;
        break;
    default:
        break;
    }
}

uint8_t REUClass::ReadIO1(uint16_t /*adresse*/)
{
    return 0;
}

uint8_t REUClass::ReadIO2(uint16_t adresse)
{
    static unsigned char ret;

    if(!is_reu_insert) return 0x00;	// Eigl. Zufallszahlen
    switch(adresse)
    {
    case 0xdf00:
        ret = io[0];
        io[0] &= 31; // BIT 5/6/7 löschen
        return ret;
        break;
    case 0xdf01:
        return io[1];
        break;
    case 0xdf02:
        return io[2];
        break;
    case 0xdf03:
        return io[3];
        break;
    case 0xdf04:
        return io[4];
        break;
    case 0xdf05:
        return io[5];
        break;
    case 0xdf06:
        return io[6];
        break;
    case 0xdf07:
        return io[7];
        break;
    case 0xdf08:
        return io[8];
        break;
    case 0xdf09:
        return io[9];
        break;
    case 0xdf0a:
        return io[10];
        break;
    default:
        return 0;	// Eigtl. Zufallszahlen
        break;
    }
}

inline void REUClass::Reset(void)
{
    BA_STATUS = true;
    tarnsfer_start = false;
    io[0] = 16;
    io[1] = 16;
    io[2] = 0;
    io[3] = 0;
    io[4] = 0;
    io[5] = 0;
    io[6] = 0;
    io[7] = 255;
    io[8] = 255;
    io[9] = 31;
    io[10] = 63;
}

bool REUClass::SaveFreez(FILE *File)
{
    /// REU Inhalt speichern ...
    for(int i=0; i<32; i++) fwrite(ram_baenke[i], 1, 0x10000, File);
    return true;
}

bool REUClass::LoadFreez(FILE *File,unsigned short Version)
{
    size_t reading_bytes;

    switch(Version)
    {
    case 0x0101:
        /// REU Inhalt laden ...
        for(int i=0; i<32; i++)
        {
            reading_bytes = fread(ram_baenke[i], 1, 0x10000, File);
            if(reading_bytes < 0x10000){}
        }
        break;
    }
    return true;
}
