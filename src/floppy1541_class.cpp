//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: floppy1541_class.cpp                  //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "./floppy1541_class.h"
#include "./c64_file_types.h"

#include <cstring>

const uint8_t Floppy1541::num_sectors[] = {21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,19,19,19,19,19,19,19,18,18,18,18,18,18,17,17,17,17,17,17,17,17,17,17,17,17};
const uint8_t Floppy1541::d64_track_zone[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                              1,1,1,1,1,1,1,
                                              2,2,2,2,2,2,
                                              3,3,3,3,3,3,3,3,3,3};

// Zeiger auf Track Anfang //	<------------------------- Track 1-17 ----------------------> <------ Track 18-24 ------> <---- Track 25-30 ----> <---- Track 31-36 ---->
const uint16_t track_index[]={0,0,21,42,63,84,105,126,147,168,189,210,231,252,273,294,315,336,357,376,395,414,433,452,471,490,508,526,544,562,580,598,615,632,649,666,683,700,717,734,751,768,785,802};

const uint8_t Floppy1541::d64_sector_gap[] = {12, 21, 16, 13};  // von GPZ Code übermommen imggen
//const uint8_t Floppy1541::d64_sector_gap[] = {1, 10, 5, 2};   // Meine alten Werte


Floppy1541::Floppy1541(bool *reset, int samplerate, int buffersize, bool *floppy_found_breakpoint):
    FloppyEnabled(false)

{
    RESET = reset;
    GCR_PTR = nullptr;
    breakgroup_count = 0;
    image_file = nullptr;

    CycleCounter = 0;
    GCRBitTrackSize = 0;
    GCRBitTrackPos = 0;
    BitAccu = 0;
    BitCounter = 0;
    OnesCount = 0;
    SyncActive = false;
    ReadShift = 0;
    ReadLatch = 0;
    WriteShift = 0;

    cpu = new MOS6502();
    via1 = new MOS6522(0);
    via2 = new MOS6522(1);

    FoundBreakpoint = floppy_found_breakpoint;

    cpu->ReadProcTbl = ReadProcTbl;
    cpu->WriteProcTbl = WriteProcTbl;
    cpu->History = History;
    cpu->HistoryPointer = &HistoryPointer;
    cpu->Breakpoints = Breakpoints;
    cpu->BreakStatus = &BreakStatus;
    cpu->BreakWerte = BreakWerte;
    HistoryPointer = 0;

    via1->FloppyIEC = &FloppyIECLocal;

    for(int i=0;i<256;i++)
    {
        ReadProcTbl[i] = std::bind(&Floppy1541::ReadNoMem,this,std::placeholders::_1);
        WriteProcTbl[i] = std::bind(&Floppy1541::WriteNoMem,this,std::placeholders::_1,std::placeholders::_2);
    }

    for(int i=0;i<8;i++)
    {
        ReadProcTbl[i] = std::bind(&Floppy1541::ReadRam,this,std::placeholders::_1);
        WriteProcTbl[i] = std::bind(&Floppy1541::WriteRam,this,std::placeholders::_1,std::placeholders::_2);
    }

    for(int i=0;i<64;i++)
    {
        ReadProcTbl[i+0xC0] = std::bind(&Floppy1541::ReadRom,this,std::placeholders::_1);
    }

    ReadProcTbl[0x18] = std::bind(&MOS6522::ReadIO,via1,std::placeholders::_1);
    WriteProcTbl[0x18] = std::bind(&MOS6522::WriteIO,via1,std::placeholders::_1,std::placeholders::_2);

    ReadProcTbl[0x1C] = std::bind(&MOS6522::ReadIO,via2,std::placeholders::_1);
    WriteProcTbl[0x1C] = std::bind(&MOS6522::WriteIO,via2,std::placeholders::_1,std::placeholders::_2);

    via2->SyncFound = std::bind(&Floppy1541::SyncFound,this);
    via2->ReadGCRByte = std::bind(&Floppy1541::ReadGCRByte,this);
    via2->WriteGCRByte = std::bind(&Floppy1541::WriteGCRByte,this,std::placeholders::_1);
    via2->SpurDec = std::bind(&Floppy1541::SpurDec,this);
    via2->SpurInc = std::bind(&Floppy1541::SpurInc,this);

    via1->TriggerInterrupt = std::bind(&MOS6502::TriggerInterrupt,cpu,std::placeholders::_1);
    via1->ClearInterrupt = std::bind(&MOS6502::ClearInterrupt,cpu,std::placeholders::_1);

    via2->TriggerInterrupt = std::bind(&MOS6502::TriggerInterrupt,cpu,std::placeholders::_1);
    via2->ClearInterrupt = std::bind(&MOS6502::ClearInterrupt,cpu,std::placeholders::_1);

    Jumper = 0;
    via1->Jumper = &Jumper;
    WriteProtect = WriteProtectAkt = false;
    via2->WriteProtect = &WriteProtect;
    via2->DiskMotorOn = &DiskMotorOn;

    DiskChangeSimState = 0;

    cpu->RESET = reset;
    via1->RESET = reset;
    via2->RESET = reset;

    D64Tracks = 35;
    D64HasErrorMap = false;
    memset(D64ErrorMap, 0, sizeof(D64ErrorMap));
    for(int i=0;i<D64_IMAGE_SIZE_MAX;i++) D64Image[i]=0x00;
    D64ImageToGCRImage();
    UnLoadDiskImage();

    AktHalbSpur = 33;    // 1

    /// Für Floppysound ///

    Volume = 0.3f;

    FloppySoundsLoaded = false;
    FloppySoundEnabled = false;

    Samplerate = samplerate;
    FreqConvAddWert = 1.0f/(985248.0f/Samplerate);
    FreqConvCounter = 0.0;
    SoundBufferPos = 0;
    SoundBufferSize = buffersize;
    SoundBuffer = new short[SoundBufferSize*2];
    for(int i=0;i<SoundBufferSize*2;i++) SoundBuffer[i] = 0;

    memset(Breakpoints, 0, sizeof Breakpoints);
}

Floppy1541::~Floppy1541()
{
    CheckImageWrite();

    FloppySoundEnabled = false;
    FloppyEnabled = false;
    delete[] FloppySound00;
    delete[] FloppySound01;
    delete[] FloppySound02;
    delete[] FloppySound03;
    delete[] FloppySound04;
    delete[] FloppySound05;
    delete[] SoundBuffer;
    delete cpu;
    delete via1;
    delete via2;
}

void Floppy1541::SetEnableFloppy(bool status)
{
    if((!FloppyEnabled) && (status))
    {
        MotorStatusOld = false;
        DiskMotorOn = false;

        CycleCounter = 0;

        cpu->Reset();
        via1->Reset();
        via2->Reset();

        StepperIncWait = true;

        AktHalbSpur = 33; // -1
        UpdateGCRPointer();
    }
    else
    {
        MotorLoopStatus = 0;
        StepperLoopStatus = 0;
    }

    FloppyEnabled = status;
    if(!status)
        FloppyIECLocal = 0xFF;
}

void Floppy1541::PrepareHardReset()
{
    // Damit ein Hard Reset immer zum gleichen Laufwerkszustand führt, unabhängig davon
    // wie lange das Laufwerk vorher schon lief
    GCRBitTrackPos = 0;
    BitAccu = 0;
    BitCounter = 0;
    OnesCount = 0;
    SyncActive = false;
    ReadShift = 0;
    ReadLatch = 0;
    WriteShift = 0;

    via1->ResetTimers();
    via2->ResetTimers();
}

bool Floppy1541::GetEnableFloppy()
{
    return FloppyEnabled;
}

void Floppy1541::SetEnableFloppySound(bool status)
{
    if(status)
    {
        FloppySoundEnabled = status;
    }
    else
    {
        FloppySoundEnabled = status;
        for(int i=0;i<SoundBufferSize;i++) SoundBuffer[i] = 0;
    }
}

bool Floppy1541::GetEnableFloppySound()
{
    return FloppySoundEnabled;
}

void* Floppy1541::GetSoundBuffer()
{
    return SoundBuffer;
}

void Floppy1541::ZeroSoundBufferPos()
{
    SoundBufferPos = 0;
}

void Floppy1541::SetFloppySoundVolume(float_t volume)
{
    Volume = volume;
}

bool Floppy1541::LoadDiskImage(FILE *file, int typ)
{
    size_t reading_elements;

    if(typ != -1)
        UnLoadDiskImage();

    image_file = file;

    switch(typ)
    {
    case D64:
        // Diskwechsel simulieren
        StartDiskChange();

        if (file == nullptr)
            return false;

        for(int i=0;i<G64_IMAGE_SIZE;i++) GCRImage[i]=0x00;

        // Spurzahl und Fehlerinfo ergeben sich aus der Dateigroesse
        {
            fseek(file, 0, SEEK_END);
            long file_size = ftell(file);
            fseek(file, 0, SEEK_SET);

            D64Tracks = 35;
            D64HasErrorMap = false;
            memset(D64Image, 0, sizeof(D64Image));
            memset(D64ErrorMap, 0, sizeof(D64ErrorMap));

            if(file_size >= 205312)      { D64Tracks = 42; D64HasErrorMap = (file_size >= 206114); }
            else if(file_size >= 196608) { D64Tracks = 40; D64HasErrorMap = (file_size >= 197376); }
            else if(file_size >= 174848) { D64Tracks = 35; D64HasErrorMap = (file_size >= 175531); }
            else return false;
        }

        {
            size_t image_size = (size_t)track_index[D64Tracks + 1] * 256;
            reading_elements = fread (D64Image,1,image_size,file);
            if(reading_elements != image_size)
            {
                return false;
            }

            if(D64HasErrorMap)
            {
                size_t sectors = track_index[D64Tracks + 1];
                if(fread(D64ErrorMap,1,sectors,file) != sectors)
                    D64HasErrorMap = false;
            }
        }

        D64ImageToGCRImage();

        UpdateGCRPointer();

        ImageWriteStatus = false;
        ImageDirectoryWriteStatus = false;
        ImageTyp = D64;

        return true;
        break;

    case G64:
        char  kennung[9];
        char  version;
        int8_t  trackanzahl;
        unsigned short  tracksize;
        uint32_t trackpos[84];
        uint32_t trackspeed[84];

        // Diskwechsel simulieren
        StartDiskChange();

        if (file == nullptr)
            return false;

        for(int i=0;i<G64_IMAGE_SIZE;i++) GCRImage[i]=0x00;

        reading_elements = fread (kennung,1,8,file);
        if(reading_elements != 8)
            return false;

        kennung[8]=0;

        if(0 != strcmp("GCR-1541",kennung))
        {
            //MessageBox(0,"Fehlerhaftes G64 Image","Error!",0);
            return false;
        }

        reading_elements = fread (&version,1,1,file);
        if(reading_elements != 1)
            return false;

        if(version != 0)
        {
            //MessageBox(0,"Dies Version unterstützt nur G64 Images Version 1","Error!",0);
            return false;
        }

        reading_elements = fread (&trackanzahl,1,1,file);
        if(reading_elements != 1)
            return false;

        if(trackanzahl > 84)
        {
            //MessageBox(0,"Das Image enthält zuviele Tracks!","Error!",0);
        }

        reading_elements = fread (&tracksize,1,2,file);
        if(reading_elements != 2)
            return false;

        if(tracksize != 7928)
        {
            //MessageBox(0,"Unbekannte Trackgröße","Error!",0);
        }

        reading_elements = fread (&trackpos,4,84,file);
        if(reading_elements != 84)
            return false;

        reading_elements = fread (&trackspeed,4,84,file);
        if(reading_elements != 84)
            return false;

        for(int i=0;i<trackanzahl;i++)
        {
            if(trackpos[i] != 0)
            {
                fseek(file,trackpos[i],SEEK_SET);

                reading_elements = fread(&TrackSize[i],1,2,file);
                if(reading_elements != 2)
                    return false;

                reading_elements = fread(GCRImage+(i*tracksize),1,tracksize-2,file);
                if(reading_elements != size_t(tracksize-2))
                    return false;
            }
        }

        // GCR_PTR = GCRSpurStart = GCRImage + ((AktHalbSpur)) * GCR_TRACK_SIZE;
        // GCRSpurEnde = GCRSpurStart + TrackSize[AktHalbSpur];

        GCRBitTrackSize = TrackSize[AktHalbSpur] * 8;
        GCRBitTrackPos = 0;

        //fclose(file);

        UpdateGCRPointer();

        ImageWriteStatus = false;
        ImageDirectoryWriteStatus = false;
        ImageTyp = G64;

        return true;
        break;
    }
    return false;
}

void Floppy1541::UnLoadDiskImage()
{
    if(image_file == nullptr)
        return;

    CheckImageWrite();

    for(int i=0;i<D64_IMAGE_SIZE_MAX;i++)
        D64Image[i] = 0;
    for(int i=0;i<G64_IMAGE_SIZE;i++)
        GCRImage[i] = 0;
    WriteProtect = false;

    fclose(image_file);
    image_file = nullptr;
}

inline void Floppy1541::CheckImageWrite()
{
    if(image_file == nullptr)
        return;

    if(ImageWriteStatus && !WriteProtectAkt)
    {
        switch(ImageTyp)
        {
        case D64:
            GCRImageToD64Image();
            fseek(image_file, 0, SEEK_SET);
            fwrite(D64Image,1,(size_t)track_index[D64Tracks + 1] * 256, image_file);
            if(D64HasErrorMap)
                fwrite(D64ErrorMap,1,track_index[D64Tracks + 1], image_file);
            break;

        case G64:
            break;
        }
    }
}

inline int Floppy1541::GCRSectorSize(unsigned int spur)
{
    return GCR_SYNC_SIZE + GCR_HEADER_SIZE + GCR_HEADER_GAP_SIZE + GCR_SYNC_SIZE + GCR_DATA_BLOCK_SIZE + d64_sector_gap[d64_track_zone[spur]];
}

inline void Floppy1541::D64ImageToGCRImage()
{
    uint16_t disk_id = GetDiskIDFromBAM();

    for (int track=1; track<=D64Tracks; track++)
    {
        TrackSize[(track-1)*2] = RealTrackSize(track);
        TrackSize[((track-1)*2)+1] = RealTrackSize(track);
        memset(GCRImage + ((track-1)*2) * GCR_TRACK_SIZE, 0x55, TrackSize[(track-1)*2]);   // Lücke bis zum Spurende
        for(int sector=0; sector<num_sectors[track-1];sector++)
        {
            SectorToGCR(track, sector, disk_id);
        }

        // Wie bei einer vom DOS formatierten Disk sind die Spuren gegeneinander versetzt (Skew): Der Spuranfang liegt je Spur 2 Sektoren weiter
        uint8_t *track_data = GCRImage + ((track-1)*2) * GCR_TRACK_SIZE;
        uint16_t track_size = TrackSize[(track-1)*2];
        uint32_t skew = ((track-1) * GCRSectorSize(track) * 2) % track_size;
        if(skew > 0)
        {
            uint8_t tmp[GCR_TRACK_SIZE];
            memcpy(tmp, track_data, track_size);
            memcpy(track_data, tmp + track_size - skew, skew);
            memcpy(track_data + skew, tmp, track_size - skew);
        }
    }

    // Spuren hinter dem Image sind leer
    for (int track=D64Tracks+1; track<=NUM_TRACKS; track++)
    {
        TrackSize[(track-1)*2] = 0;
        TrackSize[((track-1)*2)+1] = 0;
    }

    ImageWriteStatus=false;
    ImageDirectoryWriteStatus=false;
}

inline void Floppy1541::SectorToGCR(unsigned int spur, unsigned int sektor, uint16_t disk_id)
{
    uint8_t id1 = disk_id;
    uint8_t id2 = disk_id >> 8;
    uint8_t block[256];
    uint8_t buffer[4];
    uint8_t *P = GCRImage + ((spur-1)*2) * GCR_TRACK_SIZE + sektor * GCRSectorSize(spur);

    int TEMP;
    TEMP=track_index[spur]+(sektor);
    TEMP*=256;

    for (int z=0;z<256;z++) block[z]=D64Image[TEMP+z];

    // Fehlerinfo des D64 Images (1 = kein Fehler)
    uint8_t error_code = D64HasErrorMap ? D64ErrorMap[track_index[spur]+sektor] : 1;
    if(error_code == 0x0B)  // 29 Disk ID mismatch
    {
        id1 ^= 0xFF;
        id2 ^= 0xFF;
    }

    // Create GCR header (15 Bytes)
    // SYNC
    for(int i=0; i<GCR_SYNC_SIZE; i++)
        *P++ = 0xFF;
    buffer[0] = (error_code == 0x02) ? 0xFF : 0x08;   // Header mark (20 Header descriptor not found)
    buffer[1] = sektor ^ spur ^ id2 ^ id1;		// Checksum
    if(error_code == 0x09) buffer[1] ^= 0xFF;   // 27 Checksum error in header
    buffer[2] = sektor;
    buffer[3] = spur;
    ConvertToGCR(buffer, P);
    buffer[0] = id2;
    buffer[1] = id1;
    buffer[2] = 0x0F;
    buffer[3] = 0x0F;
    ConvertToGCR(buffer, P+5);
    P += GCR_HEADER_SIZE;

    // Lücke zwischen Header und Datenblock
    memset(P, 0x55, GCR_HEADER_GAP_SIZE);
    P += GCR_HEADER_GAP_SIZE;

    // Create GCR data (325 Bytes)
    uint8_t SUM;
    // SYNC
    for(int i=0; i<GCR_SYNC_SIZE; i++)
        *P++ = (error_code == 0x03) ? 0x55 : 0xFF;   // 21 No sync character
    buffer[0] = (error_code == 0x04) ? 0xFF : 0x07;	// Data mark (22 Data descriptor not found)
    SUM = buffer[1] = block[0];
    SUM ^= buffer[2] = block[1];
    SUM ^= buffer[3] = block[2];
    ConvertToGCR(buffer, P);
    P += 5;

    for (int i=3; i<255; i+=4)
    {
        SUM ^= buffer[0] = block[i];
        SUM ^= buffer[1] = block[i+1];
        SUM ^= buffer[2] = block[i+2];
        SUM ^= buffer[3] = block[i+3];
        ConvertToGCR(buffer, P);
        P += 5;
    }

    SUM ^= buffer[0] = block[255];
    buffer[1] = (error_code == 0x05) ? (SUM ^ 0xFF) : SUM;   // Checksum (23 Checksum error in data block)
    buffer[2] = 0;
    buffer[3] = 0;
    ConvertToGCR(buffer, P);
    P += 5;

    uint8_t gap_size = d64_sector_gap[d64_track_zone[spur]];
    memset(P, 0x55, gap_size);							// Gap
}

inline void Floppy1541::ConvertToGCR(uint8_t *source_buffer, uint8_t *destination_buffer)
{
    const unsigned short int GCR_TBL[16] = {0x0a, 0x0b, 0x12, 0x13, 0x0e, 0x0f, 0x16, 0x17,0x09, 0x19, 0x1a, 0x1b, 0x0d, 0x1d, 0x1e, 0x15};
    unsigned short int tmp;

    tmp = (GCR_TBL[*source_buffer >> 4] << 5) | GCR_TBL[*source_buffer & 15];
    *destination_buffer++ = tmp >> 2;
    *destination_buffer = (tmp << 6) & 0xc0;
    source_buffer++;

    tmp = (GCR_TBL[*source_buffer >> 4] << 5) | GCR_TBL[*source_buffer & 15];
    *destination_buffer++ |= (tmp >> 4) & 0x3f;
    *destination_buffer = (tmp << 4) & 0xf0;
    source_buffer++;

    tmp = (GCR_TBL[*source_buffer >> 4] << 5) | GCR_TBL[*source_buffer & 15];
    *destination_buffer++ |= (tmp >> 6) & 0x0f;
    *destination_buffer = (tmp << 2) & 0xfc;
    source_buffer++;

    tmp = (GCR_TBL[*source_buffer >> 4] << 5) | GCR_TBL[*source_buffer & 15];
    *destination_buffer++ |= (tmp >> 8) & 0x03;
    *destination_buffer = (uint8_t)tmp;
}

inline uint16_t Floppy1541::RealTrackSize(unsigned int spur)
{
    // Länge einer Spur in Bytes (eine Umdrehung = 200ms) abhängig von der Zone
    static const uint16_t real_track_size[4] = {7692, 7142, 6666, 6250};
    return real_track_size[d64_track_zone[spur]];
}

inline void Floppy1541::GCRImageToD64Image()
{
    for (int spur=1; spur<=D64Tracks; spur++)
        DecodeTrackToD64(spur);
}

// Sucht in der GCR Spur nach Sektor Headern mit folgendem Datenblock und übernimmt den Datenblock in das D64 Image.
// Die Lage der Sektoren in der Spur spielt keine Rolle (z.B. nach einem FORMAT oder Schreibzugriffen).
// Ein Sektor wird nur übernommen, wenn Header und Datenblock gültig sind (gültige GCR Codes, Marker, Prüfsummen).
// Sonst bleibt der Sektor im D64 unverändert, damit ein Schreibzugriff das Image nicht zerstören kann.
inline void Floppy1541::DecodeTrackToD64(unsigned int spur)
{
    static const bool valid_code[32] = {false,false,false,false,false,false,false,false,false,true,true,true,false,true,true,true,
                                        false,false,true,true,false,true,true,true,false,true,true,true,false,true,true,false};

    const uint8_t *base = GCRImage + ((spur-1)*2) * GCR_TRACK_SIZE;
    const uint32_t nbits = TrackSize[(spur-1)*2] * 8;
    if(nbits == 0)
        return;

    auto get_bit = [&](uint32_t pos) -> unsigned int {
        pos %= nbits;
        return (base[pos >> 3] >> (7 - (pos & 7))) & 1;
    };

    // Liest count Bytes ab Bitposition pos (beliebige Bitlage) und prüft ob alle 5 Bit GCR Codes gültig sind
    auto get_bytes = [&](uint32_t pos, uint8_t *dst, int count) -> bool {
        unsigned int code = 0;
        int code_bits = 0;
        bool ok = true;
        for(int i=0; i<count; i++)
        {
            uint8_t v = 0;
            for(int j=0; j<8; j++)
            {
                unsigned int b = get_bit(pos++);
                v = (v << 1) | b;
                code = ((code << 1) | b) & 31;
                if(++code_bits == 5)
                {
                    code_bits = 0;
                    if(!valid_code[code])
                        ok = false;
                }
            }
            dst[i] = v;
        }
        return ok;
    };

    int pending_sector = -1;
    uint32_t ones = 0;

    // Zwei Durchläufe, damit auch Header und Datenblock über den Spuranfang hinweg gefunden werden
    for(uint32_t pos=0; pos<2*nbits; pos++)
    {
        if(get_bit(pos))
        {
            ones++;
            continue;
        }

        if(ones < SYNC_BIT_THRESHOLD)
        {
            ones = 0;
            continue;
        }
        ones = 0;

        // Hier endet eine Sync Markierung, ab pos beginnen die Daten
        uint8_t mark;
        get_bytes(pos, &mark, 1);

        if(mark == 0x52)
        {
            uint8_t g[10], b1[4], b2[4];
            pending_sector = -1;
            if(!get_bytes(pos, g, 10))
                continue;

            ConvertToD64(g, b1);
            ConvertToD64(g+5, b2);

            if(b1[0] == 0x08 && b1[3] == spur && b1[2] < num_sectors[spur-1] && b1[1] == (uint8_t)(b1[2] ^ b1[3] ^ b2[0] ^ b2[1]))
                pending_sector = b1[2];
        }
        else if(mark == 0x55 && pending_sector >= 0)
        {
            uint8_t g[GCR_DATA_BLOCK_SIZE], b[4], block[256];
            int sector = pending_sector;
            pending_sector = -1;

            if(!get_bytes(pos, g, GCR_DATA_BLOCK_SIZE))
                continue;

            uint8_t *dst = block;
            uint8_t sum = 0;

            ConvertToD64(g, b);
            if(b[0] != 0x07)
                continue;
            *dst++ = b[1];
            *dst++ = b[2];
            *dst++ = b[3];
            sum ^= b[1] ^ b[2] ^ b[3];

            for(int i=3, n=5; i<255; i+=4, n+=5)
            {
                ConvertToD64(g+n, b);
                *dst++ = b[0];
                *dst++ = b[1];
                *dst++ = b[2];
                *dst++ = b[3];
                sum ^= b[0] ^ b[1] ^ b[2] ^ b[3];
            }

            ConvertToD64(g + 5*64, b);
            *dst = b[0];
            sum ^= b[0];
            if(sum != b[1])
                continue;

            memcpy(D64Image + ((track_index[spur]+sector)*256), block, 256);
        }
    }
}

inline void Floppy1541::ConvertToD64(uint8_t *source_buffer, uint8_t *destination_buffer)
{
    static uint8_t CONV_TBL[32]={32,32,32,32,32,32,32,32,32,8,0,1,32,12,4,5,32,32,2,3,32,15,6,7,32,9,10,11,32,13,14,32};
    uint8_t GCR5;
    uint8_t TMP1;

    GCR5=source_buffer[0]>>3;
    TMP1=CONV_TBL[GCR5&31]<<4;
    GCR5=(source_buffer[0]<<2)|(source_buffer[1]>>6);
    TMP1|=CONV_TBL[GCR5&31];
    destination_buffer[0]=TMP1;

    GCR5=source_buffer[1]>>1;
    TMP1=CONV_TBL[GCR5&31]<<4;
    GCR5=(source_buffer[1]<<4)|(source_buffer[2]>>4);
    TMP1|=CONV_TBL[GCR5&31];
    destination_buffer[1]=TMP1;

    GCR5=(source_buffer[2]<<1)|(source_buffer[3]>>7);
    TMP1=CONV_TBL[GCR5&31]<<4;
    GCR5=source_buffer[3]>>2;
    TMP1|=CONV_TBL[GCR5&31];
    destination_buffer[2]=TMP1;

    GCR5=(source_buffer[3]<<3)|(source_buffer[4]>>5);
    TMP1=CONV_TBL[GCR5&31]<<4;
    GCR5=source_buffer[4];
    TMP1|=CONV_TBL[GCR5&31];
    destination_buffer[3]=TMP1;
}

void Floppy1541::SetC64IEC(uint8_t *iec)
{
    via1->C64IEC = iec;
}

void Floppy1541::SetDeviceNumber(uint8_t number)
{
    if((number < 8) || (number > 11)) return;
    if(Jumper == (number-8)) return;

    Jumper = number - 8;
    cpu->Reset();
}

void Floppy1541::SetWriteProtect(bool status)
{
    WriteProtect = WriteProtectAkt = status;
}

void Floppy1541::GetFloppyInfo(FLOPPY_INFO *fi)
{
    fi->Sektor = RAM[0x19];
    fi->Spur = (AktHalbSpur+1)>>1;

    uint8_t tmp = via2->GetIO_Zero();
    fi->Motor = !!(tmp&4);
    fi->Data = !!(tmp&8);
    fi->Data_RMS = via2->GetIOPB3_RMS();

    fi->ErrorFlag = RAM[0x26D];
    for(int i=0; i<36; i++)
        fi->ErrorMsg[i] = RAM[0x2D5 + i];
}

bool Floppy1541::LoadDosRom(const char *filename)
{
    FILE *file;
    file = fopen(filename, "rb");
    if (file == NULL)
    {
        return false;
    }

    if(0x4000 != fread (ROM,1,0x4000,file))
    {
        fclose(file);
        return false;
    }

    fclose(file);
    return true;
}

int Floppy1541::LoadFloppySounds(const char *motor_sound_filename, const char *motor_on_sound_filename, const char *motor_off_sound_filename, const char *anschlag_sound_filename, const char *stepper_dec_sound_filename, const char *Stepper_inc_sound_filename)
{
    FILE* File;
    size_t reading_elements;

    FloppySoundsLoaded = false;

    File = fopen(motor_on_sound_filename, "rb");
    if (File == NULL)
    {
        return 0x01;
    }

    fseek(File,0,SEEK_END);
    FloppySound00Size = ftell(File)>>1;
    fseek(File,0,SEEK_SET);

    FloppySound00 = new unsigned short[FloppySound00Size];
    reading_elements = fread(FloppySound00,2,FloppySound00Size,File);
    fclose(File);

    if(reading_elements != (unsigned int)FloppySound00Size)
    {
        delete[] FloppySound00;
        return 0x01;
    }

    File = fopen(motor_sound_filename, "rb");
    if (File == NULL)
    {
        delete[] FloppySound00;
        return 0x02;
    }

    fseek(File,0,SEEK_END);
    FloppySound01Size = ftell(File)>>1;
    fseek(File,0,SEEK_SET);

    FloppySound01 = new unsigned short[FloppySound01Size];
    reading_elements = fread(FloppySound01,2,FloppySound01Size,File);
    fclose(File);

    if(reading_elements != (unsigned int)FloppySound01Size)
    {
        delete[] FloppySound00;
        delete[] FloppySound01;
        return 0x02;
    }

    File = fopen(motor_off_sound_filename, "rb");
    if (File == NULL)
    {
        delete[] FloppySound00;
        delete[] FloppySound01;
        return 0x03;
    }

    fseek(File,0,SEEK_END);
    FloppySound02Size = ftell(File)>>1;
    fseek(File,0,SEEK_SET);

    FloppySound02 = new unsigned short[FloppySound02Size];
    reading_elements = fread(FloppySound02,2,FloppySound02Size,File);
    fclose(File);
    if(reading_elements != (unsigned int)FloppySound02Size)
    {
        delete[] FloppySound00;
        delete[] FloppySound01;
        delete[] FloppySound02;
        return 0x03;
    }

    File = fopen(Stepper_inc_sound_filename, "rb");
    if (File == NULL)
    {
        delete[] FloppySound00;
        delete[] FloppySound01;
        delete[] FloppySound02;
        return 0x04;
    }

    fseek(File,0,SEEK_END);
    FloppySound03Size = ftell(File)>>1;
    fseek(File,0,SEEK_SET);

    FloppySound03 = new unsigned short[FloppySound03Size];
    reading_elements = fread(FloppySound03,2,FloppySound03Size,File);
    fclose(File);
    if(reading_elements != (unsigned int)FloppySound03Size)
    {
        delete[] FloppySound00;
        delete[] FloppySound01;
        delete[] FloppySound02;
        delete[] FloppySound03;
        return 0x04;
    }

    File = fopen(stepper_dec_sound_filename, "rb");
    if (File == NULL)
    {
        delete[] FloppySound00;
        delete[] FloppySound01;
        delete[] FloppySound02;
        delete[] FloppySound03;
        return 0x05;
    }

    fseek(File,0,SEEK_END);
    FloppySound04Size = ftell(File)>>1;
    fseek(File,0,SEEK_SET);

    FloppySound04 = new unsigned short[FloppySound04Size];
    reading_elements = fread(FloppySound04,2,FloppySound04Size,File);
    fclose(File);
    if(reading_elements != (unsigned int)FloppySound04Size)
    {
        delete[] FloppySound00;
        delete[] FloppySound01;
        delete[] FloppySound02;
        delete[] FloppySound03;
        delete[] FloppySound04;
        return 0x05;
    }

    File = fopen(anschlag_sound_filename, "rb");
    if (File == NULL)
    {
        delete[] FloppySound00;
        delete[] FloppySound01;
        delete[] FloppySound02;
        delete[] FloppySound03;
        delete[] FloppySound04;
        return 0x06;
    }

    fseek(File,0,SEEK_END);
    FloppySound05Size = ftell(File)>>1;
    fseek(File,0,SEEK_SET);

    FloppySound05 = new unsigned short[FloppySound05Size];
    reading_elements = fread(FloppySound05,2,FloppySound05Size,File);
    fclose(File);
    if(reading_elements != (unsigned int)FloppySound05Size)
    {
        delete[] FloppySound00;
        delete[] FloppySound01;
        delete[] FloppySound02;
        delete[] FloppySound03;
        delete[] FloppySound04;
        delete[] FloppySound05;
        return 0x06;
    }

    FloppySoundsLoaded = true;
    return 0;
}

void Floppy1541::ResetCycleCounter()
{
    CycleCounter = 0;
}

uint8_t *Floppy1541::GetRamPointer()
{
    return RAM;
}

bool Floppy1541::OneCycle()
{
    if(FloppySoundEnabled)
    {
        if(DiskMotorOn != MotorStatusOld)
        {
            if(DiskMotorOn)
            {
                MotorLoopStatus = 1;
                FloppySound00Pos = 0;
            }
            else
            {
                MotorLoopStatus = 3;
                FloppySound02Pos = 0;
            }
        }
        MotorStatusOld = DiskMotorOn;

        if(StepperInc)
        {
            StepperInc = false;
            StepperLoopStatus = 1;
            FloppySound03Pos = 0;
        }

        if(StepperDec)
        {
            StepperDec = false;
            StepperLoopStatus = 2;
            FloppySound04Pos = 0;
        }

        if(StepperAnschlag)
        {
            StepperAnschlag = false;
            StepperLoopStatus = 3;
            FloppySound05Pos = 0;
        }

        FreqConvCounter+=FreqConvAddWert;
        if(FreqConvCounter>=1.0f)
        {
            FreqConvCounter-=1.0f;

            if(FloppySoundsLoaded && FloppyEnabled)
                RenderFloppySound();
            else {
                SoundBuffer[SoundBufferPos] = SoundBuffer[SoundBufferPos+1] = 0;
                SoundBufferPos += 2;
                if(SoundBufferPos >= SoundBufferSize*2) SoundBufferPos = 0;
            }
        }
    }

    if(!FloppyEnabled) return true;

    // Disk Wechsel Simulieren
    if(DiskChangeSimState != 0)
    {
        DiskChangeSimCycleCounter--;
        if(DiskChangeSimCycleCounter == 0)
        {
            DiskChangeSimState--;
            if(DiskChangeSimState != 0)
            {
                WriteProtect = !WriteProtect;
                DiskChangeSimCycleCounter = DISK_CHANGE_STATE_CYCLES;
            }
            else WriteProtect = WriteProtectAkt;
        }
    }

    CycleCounter++;

    // PHI1
    via1->OneZyklus();
    via2->OneZyklus();

    if(DiskMotorOn)
        RotateDisk();

    if((VIA1_IRQ == true) || (VIA2_IRQ == true)) IRQ = true;
    else IRQ = false;

    cpu->Phi1();

    if(!*RESET)
    {
        StepperIncWait = true;
        AktHalbSpur = 33; //-1
        UpdateGCRPointer();
    }

    // PHI2
    bool ret = cpu->OneZyklus();
    if(BreakStatus != 0) *FoundBreakpoint = true;
    return ret;
}

uint8_t Floppy1541::ReadByte(uint16_t address)
{
    return ReadProcTbl[(address)>>8](address);
}

void Floppy1541::WriteByte(uint16_t address, uint8_t value)
{
    WriteProcTbl[(address)>>8](address,value);
}

void Floppy1541::GetCpuReg(REG_STRUCT *reg, IREG_STRUCT *ireg)
{
    if(!FloppyEnabled) return;

    if(ireg != nullptr)
    {
        cpu->GetInterneRegister(ireg);
        ireg->cycle_counter = CycleCounter;
    }

    if(reg != nullptr)
        cpu->GetRegister(reg);
}

void Floppy1541::SetCpuReg(REG_STRUCT *reg)
{
    cpu->SetRegister(reg);
}

void Floppy1541::SetResetReady(bool* ResetReady, uint16_t ResetReadyAdr)
{
    cpu->ResetReady = ResetReady;
    cpu->ResetReadyAdr = ResetReadyAdr;
}

void Floppy1541::WriteNoMem(uint16_t /*address*/, uint8_t /*value*/)
{
}

uint8_t Floppy1541::ReadNoMem(uint16_t /*address*/)
{
    return 0x0;
}

void Floppy1541::WriteRam(uint16_t address, uint8_t value)
{
    RAM[address] = value;
}

uint8_t Floppy1541::ReadRam(uint16_t address)
{
    return RAM[address];
}

uint8_t Floppy1541::ReadRom(uint16_t address)
{
    return ROM[address-0xC000];
}

bool Floppy1541::SyncFound()
{
    // Sync Signal (VIA2 PB7 ist low solange mindestens 10 aufeinander folgende 1er Bits unter dem Kopf sind)
    if ((AktHalbSpur >= ((NUM_TRACKS-1) * 2)) || (GCR_PTR == nullptr) || (DiskMotorOn == false)) return false;

    return SyncActive;
}

uint8_t Floppy1541::ReadGCRByte()
{
    return ReadLatch;
}

void Floppy1541::WriteGCRByte(uint8_t value)
{
    // Der Schreibkopf schreibt das Datenbyte aus dem Latch von VIA2 Port A im Takt der Disk (siehe RotateDisk).
    // Ein Schreibzugriff des Prozessors auf $1C01 setzt nur das Latch.
    (void)value;
}

// Dreht die Disk um ein Bit weiter, wenn die Zeit für ein Bit abgelaufen ist (3,25 bis 4 Zyklen je nach Zone).
// Beim Lesen wird das Byte Ready (SO) nach jedem vollständigen Byte gesetzt, außer während des Sync Signals.
// Im Schreibmodus (PCR CB2 = low) wird das Latch von VIA2 Port A Bit für Bit auf die Disk geschrieben.
inline void Floppy1541::RotateDisk()
{
    if ((AktHalbSpur >= ((NUM_TRACKS-1) * 2)) || (GCR_PTR == nullptr) || (GCRBitTrackSize == 0))
        return;

    unsigned int spur = (AktHalbSpur >> 1) + 1;
    int cycles_per_byte = 26 + 2 * d64_track_zone[spur <= 40 ? spur : 40];

    BitAccu += 8;
    if(BitAccu < cycles_per_byte)
        return;
    BitAccu -= cycles_per_byte;

    bool byte_ready = false;

    if((via2->GetPCR() & 0xE0) == 0xC0)
    {
        // Schreibmodus
        if(BitCounter == 0)
            WriteShift = via2->GetPA();

        if(!WriteProtect)
        {
            ImageWriteStatus = true;
            if(AktHalbSpur == (DIRECTORY_TRACK-1) * 2)
                ImageDirectoryWriteStatus = true;
            PokeGCRBit(GCRBitTrackPos, (WriteShift & 0x80) != 0);
        }
        WriteShift <<= 1;

        OnesCount = 0;
        SyncActive = false;

        if(++BitCounter == 8)
        {
            BitCounter = 0;
            byte_ready = true;
        }
    }
    else
    {
        // Lesemodus
        bool bit = PeekGCRBit(GCRBitTrackPos);
        ReadShift = (ReadShift << 1) | (bit ? 1 : 0);

        if(bit)
        {
            if(OnesCount < 255)
                OnesCount++;

            if(OnesCount >= SYNC_BIT_THRESHOLD)
            {
                // Während des Sync Signals ist der Bitzähler angehalten
                SyncActive = true;
                BitCounter = 0;
            }
            else if(++BitCounter == 8)
            {
                BitCounter = 0;
                byte_ready = true;
            }
        }
        else
        {
            OnesCount = 0;
            SyncActive = false;

            if(++BitCounter == 8)
            {
                BitCounter = 0;
                byte_ready = true;
            }
        }

        if(byte_ready)
            ReadLatch = ReadShift;
    }

    GCRBitTrackPos++;
    if(GCRBitTrackPos >= GCRBitTrackSize)
        GCRBitTrackPos = 0;

    // Byte Ready (SO) nur wenn CA2 high ist (Byte Ready Enable)
    if(byte_ready && ((via2->GetPCR() & 0x0E) == 0x0E))
        cpu->SET_SR_BIT6();
}

void Floppy1541::SpurInc()
{
    if (AktHalbSpur == ((NUM_TRACKS-1) * 2)) return;

    AktHalbSpur++;
    UpdateGCRPointer();

    if(StepperIncWait)
        StepperIncWait = false;
    else StepperInc = true;
}

void Floppy1541::SpurDec()
{
    static uint8_t stepper_bump = 0;

    if (AktHalbSpur  == 0)
    {
        UpdateGCRPointer();

        if(stepper_bump != 2)
            stepper_bump++;
        else
        {
            stepper_bump = 0;
            StepperAnschlag = true;
        }

        return;
    }

    AktHalbSpur--;
    UpdateGCRPointer();

    StepperDec = true;
}

void Floppy1541::RenderFloppySound()
{
    switch(MotorLoopStatus)
    {
    case 0:
        SoundBuffer[SoundBufferPos] = 0;
        break;
    case 1:
        SoundBuffer[SoundBufferPos] = FloppySound00[FloppySound00Pos];
        FloppySound00Pos++;
        if(FloppySound00Pos>=FloppySound00Size)
        {
            FloppySound01Pos = 0;
            MotorLoopStatus = 2;
        }
        break;
    case 2:
        SoundBuffer[SoundBufferPos] = FloppySound01[FloppySound01Pos];
        FloppySound01Pos++;
        if(FloppySound01Pos>=FloppySound01Size) FloppySound01Pos = 0;
        break;
    case 3:
        SoundBuffer[SoundBufferPos] = FloppySound02[FloppySound02Pos];
        FloppySound02Pos++;
        if(FloppySound02Pos>=FloppySound02Size)
        {
            MotorLoopStatus = 0;
        }
        break;
    }

    switch(StepperLoopStatus)
    {
    case 0:
        SoundBuffer[SoundBufferPos] += 0;
        break;
    case 1:
        SoundBuffer[SoundBufferPos] += FloppySound03[FloppySound03Pos];
        FloppySound03Pos++;
        if(FloppySound03Pos>=FloppySound03Size)
        {
            StepperLoopStatus = 0;
        }
        break;
    case 2:
        SoundBuffer[SoundBufferPos] += FloppySound04[FloppySound04Pos];
        FloppySound04Pos++;
        if(FloppySound04Pos>=FloppySound04Size)
        {
            StepperLoopStatus = 0;
        }
        break;
    case 3:
        SoundBuffer[SoundBufferPos] += FloppySound05[FloppySound05Pos];
        FloppySound05Pos++;
        if(FloppySound05Pos>=FloppySound05Size)
        {
            StepperLoopStatus = 0;
        }
        break;
    }

    SoundBuffer[SoundBufferPos] =  (short)(SoundBuffer[SoundBufferPos] * Volume);
    SoundBuffer[SoundBufferPos+1] = SoundBuffer[SoundBufferPos];
    SoundBufferPos += 2;

    if(SoundBufferPos >= SoundBufferSize*2) SoundBufferPos = 0;
}

void Floppy1541::StartDiskChange()
{
    DiskChangeSimState = DISK_CHANGE_STATE_COUNTS;
    DiskChangeSimCycleCounter = DISK_CHANGE_STATE_CYCLES;
    WriteProtect = !WriteProtect;
}

void Floppy1541::UpdateGCRPointer()
{
    GCR_PTR = GCRSpurStart = GCRImage + ((AktHalbSpur)) * GCR_TRACK_SIZE;
    GCRSpurEnde = GCRSpurStart + TrackSize[AktHalbSpur];

    // Die Winkelposition der Disk bleibt beim Spurwechsel erhalten
    uint32_t old_size = GCRBitTrackSize;
    uint32_t old_pos = GCRBitTrackPos;
    GCRBitTrackSize = TrackSize[AktHalbSpur] * 8;
    if(old_size > 0 && GCRBitTrackSize > 0)
        GCRBitTrackPos = (uint32_t)(((uint64_t)old_pos * GCRBitTrackSize) / old_size);
    else
        GCRBitTrackPos = 0;
    SyncBitCounter = 0;
    SyncBitsFound = false;
}

inline bool Floppy1541::PeekGCRBit(int pos)
{
    if((TrackSize[AktHalbSpur] * 8) <= pos)
        if(TrackSize[AktHalbSpur] > 0) pos = pos % (TrackSize[AktHalbSpur] * 8);    // Sicherstellen, dass die Position innerhalb der Spur bleibt, wenn sie größer ist als die Spurgröße (Zusätzliches Prüfung auf Spurgröße > 0, um Division durch Null zu vermeiden)

    int byte_pos = pos / 8;
    int bit_pos = pos % 8;

    return GCRSpurStart[byte_pos] & (0X80 >> bit_pos);
}

inline void Floppy1541::PokeGCRBit(int pos, bool bit)
{
    if((TrackSize[AktHalbSpur] * 8) <= pos)
        if(TrackSize[AktHalbSpur] > 0) pos = pos % (TrackSize[AktHalbSpur] * 8);    // Sicherstellen, dass die Position innerhalb der Spur bleibt, wenn sie größer ist als die Spurgröße (Zusätzliches Prüfung auf Spurgröße > 0, um Division durch Null zu vermeiden)

    int byte_pos = pos / 8;
    int bit_pos = pos % 8;

    if(bit)
        GCRSpurStart[byte_pos] |= (0X80 >> bit_pos);
    else
        GCRSpurStart[byte_pos] &= ~(0X80 >> bit_pos);
}

inline uint8_t Floppy1541::PeekGCRByte(uint32_t pos)
{
    if(pos >= GCRBitTrackSize)
        return 0;

    uint8_t gcr_byte = 0;

    for(int i=0; i<8; i++)
    {
        if(PeekGCRBit(pos++))
        {
            if(pos == GCRBitTrackSize)
                pos = 0;
            gcr_byte |= (0x80 >> i);
        }
    }
    return gcr_byte;
}

inline void Floppy1541::PokeGCRByte(uint32_t pos, uint8_t value)
{
    for(int i=0; i<8; i++)
    {
        bool bit = (value & (0x80 >> i)) != 0;
        PokeGCRBit(pos++, bit);
        if(pos == GCRBitTrackSize)
            pos = 0;
    }
}

uint16_t Floppy1541::GetDiskIDFromBAM()
{
    // position for BAM
    uint8_t track = 18;
    uint8_t sector = 0;

    uint16_t disk_id;

    int index;

    index = track_index[track] + sector;
    index *= 256;

    disk_id = D64Image[index + 162];
    disk_id |= D64Image[index + 163] << 8;

    return disk_id;
}

int16_t Floppy1541::AddBreakGroup()
{
    if(breakgroup_count == MAX_BREAK_GROUP_NUM) return -1;

    BreakGroup[breakgroup_count] = new BREAK_GROUP;
    memset(BreakGroup[breakgroup_count],0,sizeof(BREAK_GROUP));
    BreakGroup[breakgroup_count]->iRAddressCount = 1;
    BreakGroup[breakgroup_count]->iWAddressCount = 1;
    breakgroup_count ++;
    return breakgroup_count - 1;
}

void Floppy1541::DelBreakGroup(int index)
{
    if(index >= breakgroup_count) return;
    delete BreakGroup[index];
    breakgroup_count--;
    for(int i = index; i < breakgroup_count; i++) BreakGroup[i] = BreakGroup[i+1];
    UpdateBreakGroup();
}

BREAK_GROUP* Floppy1541::GetBreakGroup(int index)
{
    if(index >= breakgroup_count) return nullptr;
    return BreakGroup[index];
}

void Floppy1541::UpdateBreakGroup()
{
    for(int i=0; i<= 0xffff;i++) Breakpoints[i] = 0;
    for(int i=0;i<breakgroup_count;i++)
    {
        BREAK_GROUP* bg = BreakGroup[i];
        if(bg->Enable)
        {
            if(bg->bPC) Breakpoints[bg->iPC] |= 1;
            if(bg->bAC) Breakpoints[bg->iAC] |= 2;
            if(bg->bXR) Breakpoints[bg->iXR] |= 4;
            if(bg->bYR) Breakpoints[bg->iYR] |= 8;

            if(bg->bRAddress)
            {
                uint16_t address = bg->iRAddress;
                for(int j=0; j<bg->iRAddressCount; j++)
                    Breakpoints[address++] |= 16;
            }

            if(bg->bWAddress)
            {
                uint16_t address = bg->iWAddress;
                for(int j=0; j<bg->iWAddressCount; j++)
                    Breakpoints[address++] |= 32;
            }

            if(bg->bRWert) Breakpoints[bg->iRWert] |= 64;
            if(bg->bWWert) Breakpoints[bg->iWWert] |= 128;
            if(bg->bRZ) Breakpoints[bg->iRZ] |= 256;
            if(bg->bRZZyklus) Breakpoints[bg->iRZZyklus] |= 512;
        }
    }
}

void Floppy1541::DeleteAllBreakGroups()
{
    for(int i=0;i<breakgroup_count;i++)
    {
        delete BreakGroup[i];
    }
    breakgroup_count = 0;
    UpdateBreakGroup();
}

int Floppy1541::GetBreakGroupCount()
{
    return breakgroup_count;
}

#define READ_OR_RETURN_ERROR(variable) \
reading_elements = fread(&variable,sizeof(variable),1,file); \
    if(reading_elements != 1) \
{ \
        fclose(file); \
        return -3; \
}

int Floppy1541::LoadBreakGroups(const char *filename)
{
    FILE *file;
    char Kennung[10];
    char Version;
    unsigned char Groupanzahl;
    size_t reading_elements;

    DeleteAllBreakGroups();

    file = fopen (filename, "rb");
    if (file == NULL)
    {
        /// Datei konnte nicht geöffnet werden ///
        return -1;
    }

    /// Kennung ///
    reading_elements = fread(Kennung,sizeof(Kennung),1,file);
    if(reading_elements != 1)
    {
        /// Fehler beim Lesen der Kennung ///
        fclose(file);
        return -3;
    }

    if(0 != strcmp("EMU64_BPT",Kennung))
    {
        /// Kein Emu64 Format ///
        fclose(file);
        return -2;
    }

    /// Version ///
    reading_elements = fread(&Version,sizeof(Version),1,file);
    if(reading_elements != 1)
    {
        /// Fehler beim Lesen der Version ///
        fclose(file);
        return -6;
    }

    switch(Version)
    {
    case 1:
        /// Groupanzahl ///
        reading_elements = fread(&Groupanzahl,sizeof(Groupanzahl),1,file);
        if(reading_elements != 1)
        {
            fclose(file);
            return -5;
        }

        if(Groupanzahl == 0)
        {
            fclose(file);
            return -4;
        }

        /// Groups ///
        for(int ii=0;ii<Groupanzahl;ii++)
        {
            int i = AddBreakGroup();
            if(i < 0)
            {
                /// Fehler beim Hinzufügen einer Breakgruppe ///
                fclose(file);
                return -7;
            }

            READ_OR_RETURN_ERROR(BreakGroup[i]->Name);
            READ_OR_RETURN_ERROR(BreakGroup[i]->Enable);
            READ_OR_RETURN_ERROR(BreakGroup[i]->bPC);
            READ_OR_RETURN_ERROR(BreakGroup[i]->iPC);
            READ_OR_RETURN_ERROR(BreakGroup[i]->bAC);
            READ_OR_RETURN_ERROR(BreakGroup[i]->iAC);
            READ_OR_RETURN_ERROR(BreakGroup[i]->bXR);
            READ_OR_RETURN_ERROR(BreakGroup[i]->iXR);
            READ_OR_RETURN_ERROR(BreakGroup[i]->bYR);
            READ_OR_RETURN_ERROR(BreakGroup[i]->iYR);
            READ_OR_RETURN_ERROR(BreakGroup[i]->bRAddress);
            READ_OR_RETURN_ERROR(BreakGroup[i]->iRAddress);
            READ_OR_RETURN_ERROR(BreakGroup[i]->bWAddress);
            READ_OR_RETURN_ERROR(BreakGroup[i]->iWAddress);
            READ_OR_RETURN_ERROR(BreakGroup[i]->bRWert);
            READ_OR_RETURN_ERROR(BreakGroup[i]->iRWert);
            READ_OR_RETURN_ERROR(BreakGroup[i]->bWWert);
            READ_OR_RETURN_ERROR(BreakGroup[i]->iWWert);
            READ_OR_RETURN_ERROR(BreakGroup[i]->bRZ);
            READ_OR_RETURN_ERROR(BreakGroup[i]->iRZ);
            READ_OR_RETURN_ERROR(BreakGroup[i]->bRZZyklus);
            READ_OR_RETURN_ERROR(BreakGroup[i]->iRZZyklus);

            // version 2 compatiblity
            BreakGroup[i]->iRAddressCount = 1;
            BreakGroup[i]->iWAddressCount = 1;
        }
        break;

    case 2:
        /// ChangeLog
        /// added iRAddressCount
        /// added iWAddressCount

        /// Groupanzahl ///
        reading_elements = fread(&Groupanzahl,sizeof(Groupanzahl),1,file);
        if(reading_elements != 1)
        {
            /// Fehler beim Lesen der Groupanzahl ///
            fclose(file);
            return -5;
        }

        if(Groupanzahl == 0)
        {
            fclose(file);
            return -4;
        }

        /// Groups ///
        for(int ii=0;ii<Groupanzahl;ii++)
        {
            int i = AddBreakGroup();
            if(i < 0)
            {
                /// Fehler beim Hinzufügen einer Breakgruppe ///
                fclose(file);
                return -7;
            }

            READ_OR_RETURN_ERROR(BreakGroup[i]->Name);
            READ_OR_RETURN_ERROR(BreakGroup[i]->Enable);
            READ_OR_RETURN_ERROR(BreakGroup[i]->bPC);
            READ_OR_RETURN_ERROR(BreakGroup[i]->iPC);
            READ_OR_RETURN_ERROR(BreakGroup[i]->bAC);
            READ_OR_RETURN_ERROR(BreakGroup[i]->iAC);
            READ_OR_RETURN_ERROR(BreakGroup[i]->bXR);
            READ_OR_RETURN_ERROR(BreakGroup[i]->iXR);
            READ_OR_RETURN_ERROR(BreakGroup[i]->bYR);
            READ_OR_RETURN_ERROR(BreakGroup[i]->iYR);
            READ_OR_RETURN_ERROR(BreakGroup[i]->bRAddress);
            READ_OR_RETURN_ERROR(BreakGroup[i]->iRAddress);
            READ_OR_RETURN_ERROR(BreakGroup[i]->iRAddressCount);
            READ_OR_RETURN_ERROR(BreakGroup[i]->bWAddress);
            READ_OR_RETURN_ERROR(BreakGroup[i]->iWAddress);
            READ_OR_RETURN_ERROR(BreakGroup[i]->iWAddressCount);
            READ_OR_RETURN_ERROR(BreakGroup[i]->bRWert);
            READ_OR_RETURN_ERROR(BreakGroup[i]->iRWert);
            READ_OR_RETURN_ERROR(BreakGroup[i]->bWWert);
            READ_OR_RETURN_ERROR(BreakGroup[i]->iWWert);
            READ_OR_RETURN_ERROR(BreakGroup[i]->bRZ);
            READ_OR_RETURN_ERROR(BreakGroup[i]->iRZ);
            READ_OR_RETURN_ERROR(BreakGroup[i]->bRZZyklus);
            READ_OR_RETURN_ERROR(BreakGroup[i]->iRZZyklus);
        }
        break;

    default:
        /// Version nicht unterstützt ///
        fclose(file);
        return -3;
    }

    fclose(file);
    return 0;
}

bool Floppy1541::SaveBreakGroups(const char *filename)
{
    FILE *file;
    char Kennung[]  = "EMU64_BPT";
    char Version    = 2;

    file = fopen (filename, "wb");
    if (file == NULL)
    {
        return false;
    }

    /// Kennung ///
    fwrite(Kennung,sizeof(Kennung),1,file);

    /// Version ///
    fwrite(&Version,sizeof(Version),1,file);

    /// Groupanzahl ///
    fwrite(&breakgroup_count,sizeof(breakgroup_count),1,file);

    /// Groups ///
    for(int i=0;i<breakgroup_count;i++)
    {
        fwrite(BreakGroup[i]->Name,sizeof(BreakGroup[i]->Name),1,file);
        fwrite(&BreakGroup[i]->Enable,sizeof(BreakGroup[i]->Enable),1,file);
        fwrite(&BreakGroup[i]->bPC,sizeof(BreakGroup[i]->bPC),1,file);
        fwrite(&BreakGroup[i]->iPC,sizeof(BreakGroup[i]->iPC),1,file);
        fwrite(&BreakGroup[i]->bAC,sizeof(BreakGroup[i]->bAC),1,file);
        fwrite(&BreakGroup[i]->iAC,sizeof(BreakGroup[i]->iAC),1,file);
        fwrite(&BreakGroup[i]->bXR,sizeof(BreakGroup[i]->bXR),1,file);
        fwrite(&BreakGroup[i]->iXR,sizeof(BreakGroup[i]->iXR),1,file);
        fwrite(&BreakGroup[i]->bYR,sizeof(BreakGroup[i]->bYR),1,file);
        fwrite(&BreakGroup[i]->iYR,sizeof(BreakGroup[i]->iYR),1,file);
        fwrite(&BreakGroup[i]->bRAddress,sizeof(BreakGroup[i]->bRAddress),1,file);
        fwrite(&BreakGroup[i]->iRAddress,sizeof(BreakGroup[i]->iRAddress),1,file);
        fwrite(&BreakGroup[i]->iRAddressCount,sizeof(BreakGroup[i]->iRAddressCount),1,file);
        fwrite(&BreakGroup[i]->bWAddress,sizeof(BreakGroup[i]->bWAddress),1,file);
        fwrite(&BreakGroup[i]->iWAddress,sizeof(BreakGroup[i]->iWAddress),1,file);
        fwrite(&BreakGroup[i]->iWAddressCount,sizeof(BreakGroup[i]->iWAddressCount),1,file);
        fwrite(&BreakGroup[i]->bRWert,sizeof(BreakGroup[i]->bRWert),1,file);
        fwrite(&BreakGroup[i]->iRWert,sizeof(BreakGroup[i]->iRWert),1,file);
        fwrite(&BreakGroup[i]->bWWert,sizeof(BreakGroup[i]->bWWert),1,file);
        fwrite(&BreakGroup[i]->iWWert,sizeof(BreakGroup[i]->iWWert),1,file);
        fwrite(&BreakGroup[i]->bRZ,sizeof(BreakGroup[i]->bRZ),1,file);
        fwrite(&BreakGroup[i]->iRZ,sizeof(BreakGroup[i]->iRZ),1,file);
        fwrite(&BreakGroup[i]->bRZZyklus,sizeof(BreakGroup[i]->bRZZyklus),1,file);
        fwrite(&BreakGroup[i]->iRZZyklus,sizeof(BreakGroup[i]->iRZZyklus),1,file);
    }

    fclose(file);
    return true;
}

bool Floppy1541::CheckBreakpoints()
{
    int BreaksIO = 0;

    for (int i=0;i<breakgroup_count;i++)
    {
        BREAK_GROUP* bg = BreakGroup[i];
        int count1 = 0;
        int count2 = 0;

        if(bg->Enable)
        {
            if(bg->bPC)
            {
                count1++;
                if((BreakStatus&1) && (BreakWerte[0] == bg->iPC)) count2++;
            }
            if(bg->bAC)
            {
                count1++;
                if((BreakStatus&2) && (BreakWerte[1] == bg->iAC)) count2++;
            }
            if(bg->bXR)
            {
                count1++;
                if((BreakStatus&4) && (BreakWerte[2] == bg->iXR)) count2++;
            }
            if(bg->bYR)
            {
                count1++;
                if((BreakStatus&8) && (BreakWerte[3] == bg->iYR)) count2++;
            }
            if(bg->bRAddress)
            {
                count1++;
                if((BreakStatus&16) && (BreakWerte[4] == bg->iRAddress)) count2++;
            }
            if(bg->bWAddress)
            {
                count1++;
                if((BreakStatus&32) && (BreakWerte[5] == bg->iWAddress)) count2++;
            }
            if(bg->bRWert)
            {
                count1++;
                if((BreakStatus&64) && (BreakWerte[6] == bg->iRWert)) count2++;
            }
            if(bg->bWWert)
            {
                count1++;
                if((BreakStatus&128) && (BreakWerte[7] == bg->iWWert)) count2++;
            }
        }
        if((count1 == count2) && (count1 > 0))
        {
            BreakGroup[i]->bTrue = true;
            BreaksIO++;
        }
        else BreakGroup[i]->bTrue = false;
    }
    BreakStatus = 0;

    if(BreaksIO > 0)
    {
        return true;
    }
    else return false;
}

bool Floppy1541::CheckImageDirectoryWrite()
{
//#define CHECK_ONLY_MOTOR_OFF

#ifdef CHECK_ONLY_MOTOR_OFF
    bool ret = ImageDirectoryWriteStatus;

    if(!DiskMotorOn)
    {
        ImageDirectoryWriteStatus = false;
        return  ret;
    }
    return false;
#else
    bool ret = ImageDirectoryWriteStatus;
    ImageDirectoryWriteStatus = false;

    return  ret;
#endif
}

uint8_t *Floppy1541::GetCurrentD64ImageBuffer()
{
    GCRImageToD64Image();
    return  D64Image;
}
