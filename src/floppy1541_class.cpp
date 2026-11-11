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

const uint8_t Floppy1541::d64_track_zone[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                              1,1,1,1,1,1,1,
                                              2,2,2,2,2,2,
                                              3,3,3,3,3,3,3,3,3,3,3,3};

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
    SectorMapDirty = true;
    memset(&SectorMap, 0, sizeof(SectorMap));
    BitAccu = 0;
    BitCounter = 0;
    OnesCount = 0;
    SyncActive = false;
    ReadShift = 0;
    ReadLatch = 0;
    ReadLatched = false;
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
        ReadProcTbl[i] = BindDelegate<&Floppy1541::ReadNoMem>(this);
        WriteProcTbl[i] = BindDelegate<&Floppy1541::WriteNoMem>(this);
    }

    // Adressdekodierung wie beim echten 1541 (und VICE): A13/A14 sind nicht angeschlossen, der Bereich $0000-$1FFF
    // wiederholt sich also 4x bis $7FFF. Darin liegen RAM ($0000-$07FF, 2 KiB), VIA1 ($1800-$1BFF) und VIA2 ($1C00-$1FFF).
    // Die VIAs spiegeln ihre 16 Register jeweils im ganzen 1 KiB Bereich. Das ROM (16 KiB) liegt bei $C000 und ist bei $8000 gespiegelt.
    // Schnelllader nutzen diese Spiegel (z.B. LDA ($xx,X) auf $1F05 = Timer 1 von VIA2).
    for(int block=0; block<4; block++)
    {
        const int base = block * 0x20;

        for(int i=0;i<8;i++)
        {
            ReadProcTbl[base+i] = BindDelegate<&Floppy1541::ReadRam>(this);
            WriteProcTbl[base+i] = BindDelegate<&Floppy1541::WriteRam>(this);
        }

        for(int i=0;i<4;i++)
        {
            ReadProcTbl[base+0x18+i] = BindDelegate<&MOS6522::ReadIO>(via1);
            WriteProcTbl[base+0x18+i] = BindDelegate<&MOS6522::WriteIO>(via1);

            ReadProcTbl[base+0x1C+i] = BindDelegate<&MOS6522::ReadIO>(via2);
            WriteProcTbl[base+0x1C+i] = BindDelegate<&MOS6522::WriteIO>(via2);
        }
    }

    for(int i=0;i<128;i++)
    {
        ReadProcTbl[i+0x80] = BindDelegate<&Floppy1541::ReadRom>(this);
    }

    via2->SyncFound = BindDelegate<&Floppy1541::SyncFound>(this);
    via2->ReadGCRByte = BindDelegate<&Floppy1541::ReadGCRByte>(this);
    via2->WriteGCRByte = BindDelegate<&Floppy1541::WriteGCRByte>(this);
    via2->SpurDec = BindDelegate<&Floppy1541::SpurDec>(this);
    via2->SpurInc = BindDelegate<&Floppy1541::SpurInc>(this);

    via1->TriggerInterrupt = BindDelegate<&MOS6502::TriggerInterrupt>(cpu);
    via1->ClearInterrupt = BindDelegate<&MOS6502::ClearInterrupt>(cpu);

    via2->TriggerInterrupt = BindDelegate<&MOS6502::TriggerInterrupt>(cpu);
    via2->ClearInterrupt = BindDelegate<&MOS6502::ClearInterrupt>(cpu);

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
    ReadLatched = false;
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

            if(!D64DetectLayout(file_size, &D64Tracks, &D64HasErrorMap))
                return false;
        }

        {
            size_t image_size = (size_t)D64_TRACK_INDEX[D64Tracks + 1] * 256;
            reading_elements = fread (D64Image,1,image_size,file);
            if(reading_elements != image_size)
            {
                return false;
            }

            if(D64HasErrorMap)
            {
                size_t sectors = D64_TRACK_INDEX[D64Tracks + 1];
                if(fread(D64ErrorMap,1,sectors,file) != sectors)
                    D64HasErrorMap = false;
            }
        }

        D64ImageToGCRImage();

        UpdateGCRPointer();
        SectorMapDirty = true;

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

        for(int i=0;i<NUM_TRACKS*2;i++)
            TrackSize[i] = 0;

        for(int i=0;i<trackanzahl;i++)
        {
            if(trackpos[i] != 0)
            {
                fseek(file,trackpos[i],SEEK_SET);

                reading_elements = fread(&TrackSize[i],1,2,file);
                if(reading_elements != 2)
                    return false;

                // Die letzte Spur im G64 kann kuerzer sein als die maximale Spurgroesse (Dateiende), sie muss nur ihre eigene Laenge enthalten
                if(TrackSize[i] > tracksize-2)
                    return false;

                reading_elements = fread(GCRImage+(i*tracksize),1,tracksize-2,file);
                if(reading_elements < size_t(TrackSize[i]))
                    return false;
            }
        }

        InitEmptyTracks();

        // GCR_PTR = GCRSpurStart = GCRImage + ((AktHalbSpur)) * GCR_TRACK_SIZE;
        // GCRSpurEnde = GCRSpurStart + TrackSize[AktHalbSpur];

        GCRBitTrackSize = TrackSize[AktHalbSpur] * 8;
        GCRBitTrackPos = 0;

        //fclose(file);

        UpdateGCRPointer();
        SectorMapDirty = true;

        ImageWriteStatus = false;
        ImageDirectoryWriteStatus = false;
        ImageTyp = G64;

        return true;
        break;

    case NIB:
        // Diskwechsel simulieren
        StartDiskChange();

        if (file == nullptr)
            return false;

        for(int i=0;i<G64_IMAGE_SIZE;i++) GCRImage[i]=0x00;
        for(int i=0;i<NUM_TRACKS*2;i++)
            TrackSize[i] = 0;

        if(!NIBImageToGCRImage(file))
            return false;

        InitEmptyTracks();

        GCRBitTrackSize = TrackSize[AktHalbSpur] * 8;
        GCRBitTrackPos = 0;

        UpdateGCRPointer();
        SectorMapDirty = true;

        ImageWriteStatus = false;
        ImageDirectoryWriteStatus = false;
        ImageTyp = NIB;

        return true;
        break;
    }
    return false;
}

// NIB Image (MNIB-1541-RAW, nibtools): 256 Byte Header, danach je Halbspur 8192 Bytes Rohdaten (GCR Bytes wie sie der Lesekopf liefert).
// Header: Offset 0 Kennung "MNIB-1541-RAW", ab Offset 16 je Halbspur 2 Bytes (Halbspurnummer 2=Spur 1, 3=Spur 1.5, 4=Spur 2 ... und Dichte in Bit 0-1), Ende bei Halbspurnummer 0.
// Die Spurdaten folgen in der Reihenfolge der Header Eintraege.
bool Floppy1541::NIBImageToGCRImage(FILE *file)
{
    static const uint32_t NIB_HEADER_SIZE = 256;
    static const uint32_t NIB_TRACK_SIZE = 8192;
    static const uint32_t NIB_MAX_ENTRIES = (NIB_HEADER_SIZE - 16) / 2;

    uint8_t header[NIB_HEADER_SIZE];
    uint8_t raw[NIB_TRACK_SIZE];

    if(fseek(file, 0, SEEK_SET) != 0)
        return false;

    if(fread(header, 1, NIB_HEADER_SIZE, file) != NIB_HEADER_SIZE)
        return false;

    if(0 != memcmp(header, "MNIB-1541-RAW", 13))
        return false;

    int loaded_tracks = 0;
    bool entry_present[NUM_TRACKS*2] = {false};

    for(uint32_t entry=0; entry<NIB_MAX_ENTRIES; entry++)
    {
        const unsigned int half_track_nr = header[16 + entry*2];
        const unsigned int density = header[17 + entry*2];

        if(half_track_nr == 0)
            break;

        // Unvollstaendige Datei: Es zaehlen nur komplette Spuren
        if(fread(raw, 1, NIB_TRACK_SIZE, file) != NIB_TRACK_SIZE)
            break;

        // Halbspurnummer 2 = Spur 1 = Halbspur 0 im Emulator
        if(half_track_nr < 2 || half_track_nr - 2 >= (unsigned int)(NUM_TRACKS*2))
            continue;

        const unsigned int half_track = half_track_nr - 2;
        const uint16_t size = NIBTrackToGCR(raw, density, GCRImage + half_track * GCR_TRACK_SIZE);
        TrackSize[half_track] = size;
        entry_present[half_track] = true;
        loaded_tracks++;
    }

    // Fehlende Halbspuren zwischen zwei Spuren mit gleichem Inhalt: Die Disk wurde dann auf mehreren Kopfpositionen gleich beschrieben (z.B. Predator,
    // Spur 35 + 36 mit Sonder-ID). Ein Kopf zwischen beiden liest dieselben Daten. NIB Images enthalten Halbspuren nur auf Wunsch, die Daten fehlen dann einfach.
    // Nur bei gleichem Inhalt der Nachbarn ergaenzen, sonst bleibt die Halbspur unformatiert (viele Kopierschutzabfragen erwarten das).
    for(int half_track=1; half_track<NUM_TRACKS*2-1; half_track+=2)
    {
        if(entry_present[half_track] || TrackSize[half_track] != 0)
            continue;
        if(TrackSize[half_track-1] == 0 || TrackSize[half_track+1] == 0)
            continue;

        uint8_t *lower = GCRImage + (half_track-1) * GCR_TRACK_SIZE;
        uint8_t *upper = GCRImage + (half_track+1) * GCR_TRACK_SIZE;
        if(NIBTracksEquivalent(lower, TrackSize[half_track-1], upper, TrackSize[half_track+1]))
        {
            memcpy(GCRImage + half_track * GCR_TRACK_SIZE, lower, TrackSize[half_track-1]);
            TrackSize[half_track] = TrackSize[half_track-1];
        }
    }

    return loaded_tracks > 0;
}

// Prueft, ob zwei Spuren (je eine Umdrehung) denselben Inhalt haben, auch wenn sie an unterschiedlicher Stelle beginnen.
// Wie beim Spurschnitt werden die Syncs auf ein Byte zusammengefasst, weil ihre Laenge je Spur um ein Byte schwankt.
bool Floppy1541::NIBTracksEquivalent(const uint8_t *a, uint16_t size_a, const uint8_t *b, uint16_t size_b)
{
    static const uint32_t PATTERN_MAX = 24;
    static const uint32_t PATTERN_MIN = 8;
    static const int MIN_SCORE = 950;                   // Mindest-Uebereinstimmung in Promille

    // Laengen duerfen nur geringfuegig abweichen (Sync-Laengen)
    const int diff = (int)size_a - (int)size_b;
    if(diff > 64 || diff < -64)
        return false;

    auto collapse = [](const uint8_t *src, uint16_t size, uint8_t *dst) -> uint32_t {
        uint32_t n = 0;
        for(uint32_t i=0; i<size; i++)
        {
            if(src[i] == 0xFF && n > 0 && dst[n-1] == 0xFF)
                continue;
            dst[n++] = src[i];
        }
        return n;
    };

    uint8_t ca[GCR_TRACK_SIZE];
    uint8_t cb[GCR_TRACK_SIZE];
    const uint32_t na = collapse(a, size_a, ca);
    const uint32_t nb = collapse(b, size_b, cb);

    // Anker: erstes Byte hinter einem Sync in a, mit den folgenden Bytes bis zum naechsten Sync
    uint32_t anchor = 0;
    for(uint32_t k=1; k<na; k++)
    {
        if(ca[k] != 0xFF && ca[k-1] == 0xFF)
        {
            anchor = k;
            break;
        }
    }
    if(anchor == 0)
        return false;

    uint32_t plen = 0;
    while(plen < PATTERN_MAX && anchor + plen < na && ca[anchor + plen] != 0xFF)
        plen++;
    if(plen < PATTERN_MIN)
        return false;

    // Anker in b suchen (ringfoermig)
    for(uint32_t o=1; o<nb; o++)
    {
        if(cb[o] == 0xFF || cb[o-1] != 0xFF)
            continue;

        bool match = true;
        for(uint32_t i=0; i<plen && match; i++)
            match = (cb[(o + i) % nb] == ca[anchor + i]);
        if(!match)
            continue;

        const uint32_t n = (na < nb) ? na : nb;
        uint32_t matches = 0;
        for(uint32_t j=0; j<n; j++)
        {
            if(ca[(anchor + j) % na] == cb[(o + j) % nb])
                matches++;
        }
        if((int)(matches * 1000 / n) >= MIN_SCORE)
            return true;
    }
    return false;
}

// Schneidet aus den 8192 Rohbytes einer Spur genau eine Umdrehung heraus (liefert die Laenge in Bytes, 0 = unformatierte Spur ohne Sync).
// Der Dump enthaelt mehr als eine Umdrehung und beginnt an beliebiger Stelle. Die Umdrehungslaenge steht nicht im Image und wird gesucht:
// Ab einem Spuranfang (erstes Byte hinter einem Sync) wird um die Nominallaenge der Dichte herum der Punkt gesucht, an dem sich die Daten wiederholen.
// Die Syncs werden dabei auf ein Byte zusammengefasst, weil ihre Laenge von Umdrehung zu Umdrehung um ein Byte abweichen kann.
// Ohne eindeutige Wiederholung (z.B. gezielt zerstoerte Spur) wird die Nominallaenge ab dem ersten Sync verwendet.
uint16_t Floppy1541::NIBTrackToGCR(const uint8_t *raw, unsigned int density, uint8_t *destination)
{
    static const uint32_t RAW_SIZE = 8192;
    static const uint32_t SEARCH_RANGE = 500;           // Suchbereich um die Nominallaenge (Laufwerke drehen etwas schneller oder langsamer)
    static const uint32_t MIN_COMPARE = 128;            // Mindestlaenge des Vergleichs (nach dem Zusammenfassen der Syncs)
    static const int MIN_SCORE = 600;                   // Mindest-Uebereinstimmung in Promille
    static const int GOOD_SCORE = 950;                  // Ab hier wird nicht weiter gesucht
    static const unsigned int MAX_STARTS = 8;           // Anzahl Spuranfaenge, die probiert werden
    static const uint16_t nominal_size[4] = {6250, 6666, 7142, 7692};

    const uint32_t nominal = nominal_size[density & 3];

    uint8_t c[RAW_SIZE];            // Spur mit zusammengefassten Syncs
    uint16_t idx[RAW_SIZE];         // Position des Bytes in den Rohdaten
    uint32_t nc = 0;
    uint32_t ff_count = 0;

    for(uint32_t i=0; i<RAW_SIZE; i++)
    {
        if(raw[i] == 0xFF)
        {
            ff_count++;
            if(nc > 0 && c[nc-1] == 0xFF)
                continue;
        }
        c[nc] = raw[i];
        idx[nc] = (uint16_t)i;
        nc++;
    }

    // Spur nur aus Syncs (Killer Track)
    if(ff_count == RAW_SIZE)
    {
        memset(destination, 0xFF, nominal);
        return (uint16_t)nominal;
    }

    // Ende eines Syncs (mindestens 2 Bytes $FF) vor Position k im zusammengefassten Strom
    auto is_sync_end = [&](uint32_t k) -> bool {
        return k >= 1 && c[k] != 0xFF && c[k-1] == 0xFF && (idx[k] - idx[k-1]) >= 2;
    };

    uint32_t first_sync_start = 0;
    bool sync_found = false;
    for(uint32_t k=1; k<nc; k++)
    {
        if(is_sync_end(k))
        {
            first_sync_start = idx[k-1];
            sync_found = true;
            break;
        }
    }

    // Keine Syncs: Daten sind nicht lesbar, die Spur gilt als unformatiert
    if(!sync_found)
        return 0;

    int best_score = -1;
    uint32_t best_start = 0;
    uint32_t best_size = 0;
    unsigned int starts_tried = 0;

    for(uint32_t k=1; k<nc && starts_tried<MAX_STARTS; k++)
    {
        if(!is_sync_end(k))
            continue;

        const uint32_t start = idx[k];
        if(start + nominal - SEARCH_RANGE + MIN_COMPARE > RAW_SIZE)
            break;

        starts_tried++;

        for(uint32_t kk=k+1; kk<nc; kk++)
        {
            if(idx[kk] < start + nominal - SEARCH_RANGE)
                continue;
            if(idx[kk] > start + nominal + SEARCH_RANGE)
                break;
            if(!is_sync_end(kk))
                continue;

            const uint32_t n = nc - kk;
            if(n < MIN_COMPARE)
                break;

            const uint32_t size = idx[kk] - start;
            if(size > (uint32_t)GCR_TRACK_SIZE)
                continue;

            uint32_t matches = 0;
            for(uint32_t j=0; j<n; j++)
            {
                if(c[k+j] == c[kk+j])
                    matches++;
            }

            const int score = (int)(matches * 1000 / n);
            if(score > best_score)
            {
                best_score = score;
                best_start = start;
                best_size = size;
            }
        }

        if(best_score >= GOOD_SCORE)
            break;
    }

    if(best_score >= MIN_SCORE)
    {
        memcpy(destination, raw + best_start, best_size);
        return (uint16_t)best_size;
    }

    // Keine Wiederholung gefunden: Nominallaenge ab dem Anfang des ersten Syncs (ringfoermig), damit der Sync selbst in der Spur bleibt
    for(uint32_t i=0; i<nominal; i++)
        destination[i] = raw[(first_sync_start + i) % RAW_SIZE];
    return (uint16_t)nominal;
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
    SectorMapDirty = true;
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
            fwrite(D64Image,1,(size_t)D64_TRACK_INDEX[D64Tracks + 1] * 256, image_file);
            if(D64HasErrorMap)
                fwrite(D64ErrorMap,1,D64_TRACK_INDEX[D64Tracks + 1], image_file);
            break;

        case G64:
            break;

        case NIB:
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
        for(int sector=0; sector<D64_NUM_SECTORS[track-1];sector++)
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

    // Spuren hinter dem Image sind unformatiert
    for (int track=D64Tracks+1; track<=NUM_TRACKS; track++)
    {
        TrackSize[(track-1)*2] = 0;
        TrackSize[((track-1)*2)+1] = 0;
    }
    InitEmptyTracks();

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
    TEMP=D64_TRACK_INDEX[spur]+(sektor);
    TEMP*=256;

    for (int z=0;z<256;z++) block[z]=D64Image[TEMP+z];

    // Fehlerinfo des D64 Images (1 = kein Fehler)
    uint8_t error_code = D64HasErrorMap ? D64ErrorMap[D64_TRACK_INDEX[spur]+sektor] : 1;
    if(error_code == 0x0B)  // 29 Disk ID mismatch (wie VICE: nur ID1 ist invertiert, die Header Pruefsumme passt dazu)
        id1 ^= 0xFF;

    // Create GCR header (15 Bytes)
    // SYNC (21 No sync character: Header und Datenblock haben keinen Sync)
    for(int i=0; i<GCR_SYNC_SIZE; i++)
        *P++ = (error_code == 0x03) ? 0x55 : 0xFF;
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
    // 22 Data descriptor not found ist ein "weicher" Fehler, die Daten bleiben lesbar. Der Marker muss in GCR mit einer 0 beginnen
    // (wie VICE: 0x00), sonst wird er als Teil des Sync gelesen und der Datenblock verschiebt sich.
    buffer[0] = (error_code == 0x04) ? 0x00 : 0x07;	// Data mark
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

// Spuren ohne Daten (hinter dem D64 Image oder fehlende Spuren im G64) sind eine unformatierte Disk:
// Sie drehen sich mit der normalen Spurlaenge ihrer Zone und liefern nur 0 Bits (kein Sync).
// Ohne diese Spurlaenge bliebe der Kopf dort stehen (kein Byte Ready) und Programme die dort
// eine Spur schreiben (z.B. rpm3 auf Spur 36) haengen. Geschriebene Daten bleiben im Speicher, kommen aber nicht ins D64.
inline void Floppy1541::InitEmptyTracks()
{
    for (int half_track=0; half_track<NUM_TRACKS*2; half_track++)
    {
        if(TrackSize[half_track] == 0)
        {
            // Hinter Spur 35 stellt die ROM PB6:5 = 10 ein (28 Takte/Byte): 7142 Bytes ergeben dort wie bei VICE 200 ms je Umdrehung
            const unsigned int spur = (half_track >> 1) + 1;
            TrackSize[half_track] = (spur > 35) ? 7142 : RealTrackSize(spur);
        }
    }
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

            if(b1[0] == 0x08 && b1[3] == spur && b1[2] < D64_NUM_SECTORS[spur-1] && b1[1] == (uint8_t)(b1[2] ^ b1[3] ^ b2[0] ^ b2[1]))
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

            memcpy(D64Image + ((D64_TRACK_INDEX[spur]+sector)*256), block, 256);
        }
    }
}

// Ermittelt die Lage der Sektoren auf der Spur unter dem Kopf (nur fuer die Anzeige).
// Es zaehlt allein der Inhalt der GCR Spur, nicht das DOS. Deshalb funktioniert es auch mit Fastloadern, G64 und nach Schreibzugriffen.
// Sektoren mit ungueltigem Header werden ignoriert, die Datenpruefsumme wird nicht geprueft.
void Floppy1541::ScanSectorMap()
{
    static const bool valid_code[32] = {false,false,false,false,false,false,false,false,false,true,true,true,false,true,true,true,
                                        false,false,true,true,false,true,true,true,false,true,true,true,false,true,true,false};

    SectorMapDirty = false;

    FLOPPY_TRACK_MAP map;
    memset(&map, 0, sizeof(map));
    map.HalbSpur = AktHalbSpur;

    const uint32_t nbits = (AktHalbSpur <= (NUM_TRACKS-1) * 2) ? (uint32_t)TrackSize[AktHalbSpur] * 8 : 0;

    if(nbits > 0)
    {
        map.Bits = nbits;
        const uint8_t *base = GCRImage + AktHalbSpur * GCR_TRACK_SIZE;

        auto get_bit = [&](uint32_t pos) -> unsigned int {
            pos %= nbits;
            return (base[pos >> 3] >> (7 - (pos & 7))) & 1;
        };

        // Liest count Bytes ab Bitposition pos (beliebige Bitlage) und prueft ob alle 5 Bit GCR Codes gueltig sind
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

        // Dreimal durch die Spur: Im ersten Durchlauf wird nur die Sync Zaehlung aufgebaut, Header werden im zweiten
        // gesucht (so ist auch ein Sync ueber den Spuranfang vollstaendig) und der dritte liefert die Datenbloecke dazu.
        int pending = -1;
        uint32_t pending_start = 0;
        uint32_t ones = 0;

        for(uint32_t pos=0; pos<3*nbits; pos++)
        {
            if(get_bit(pos))
            {
                ones++;
                continue;
            }

            const uint32_t sync_len = ones;
            ones = 0;

            if(sync_len < SYNC_BIT_THRESHOLD || pos < nbits)
                continue;

            uint8_t mark;
            get_bytes(pos, &mark, 1);

            if(mark == 0x52)
            {
                pending = -1;
                if(pos >= 2*nbits || map.Anzahl >= FLOPPY_MAX_SECTORS)
                    continue;

                uint8_t g[10], b1[4], b2[4];
                if(!get_bytes(pos, g, 10))
                    continue;

                ConvertToD64(g, b1);
                ConvertToD64(g+5, b2);

                if(b1[0] != 0x08 || b1[2] >= 32 || b1[1] != (uint8_t)(b1[2] ^ b1[3] ^ b2[0] ^ b2[1]))
                    continue;

                pending_start = pos - sync_len;

                FLOPPY_SECTOR &s = map.Sektor[map.Anzahl];
                s.Nr = b1[2];
                s.Start = pending_start % nbits;
                s.HeaderEnde = sync_len + GCR_HEADER_SIZE * 8;
                s.HatDaten = false;
                s.DatenStart = s.DatenEnde = s.HeaderEnde;
                pending = map.Anzahl++;
            }
            else if(mark == 0x55 && pending >= 0)
            {
                FLOPPY_SECTOR &s = map.Sektor[pending];
                pending = -1;

                s.HatDaten = true;
                s.DatenStart = (pos - sync_len) - pending_start;
                s.DatenEnde = (pos - pending_start) + GCR_DATA_BLOCK_SIZE * 8;
                if(s.DatenEnde > nbits)
                    s.DatenEnde = nbits;
            }
        }
    }

    std::lock_guard<std::mutex> lock(SectorMapMutex);
    SectorMap = map;
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
    fi->HalbSpur = AktHalbSpur & 1;
    fi->Spur = (AktHalbSpur >> 1) + 1;
    fi->Sektor = FLOPPY_SECTOR_NONE;
    fi->Bereich = FLOPPY_AREA_NONE;

    {
        // Sektor unter dem Kopf: Der zuletzt passierte Sektor Header der Spur
        std::lock_guard<std::mutex> lock(SectorMapMutex);

        const uint32_t n = SectorMap.Bits;
        if(n > 0 && n == GCRBitTrackSize && SectorMap.HalbSpur == AktHalbSpur && SectorMap.Anzahl > 0)
        {
            const uint32_t pos = GCRBitTrackPos % n;
            uint32_t best_rel = n;
            int best = -1;
            for(int i=0; i<SectorMap.Anzahl; i++)
            {
                uint32_t rel = (pos + n - SectorMap.Sektor[i].Start) % n;
                if(rel < best_rel)
                {
                    best_rel = rel;
                    best = i;
                }
            }

            if(best >= 0)
            {
                const FLOPPY_SECTOR &s = SectorMap.Sektor[best];
                fi->Sektor = s.Nr;

                if(best_rel < s.HeaderEnde)
                    fi->Bereich = FLOPPY_AREA_HEADER;
                else if(!s.HatDaten)
                    fi->Bereich = FLOPPY_AREA_GAP;
                else if(best_rel < s.DatenStart)
                    fi->Bereich = FLOPPY_AREA_HEADER_GAP;
                else if(best_rel < s.DatenEnde)
                    fi->Bereich = FLOPPY_AREA_DATA;
                else
                    fi->Bereich = FLOPPY_AREA_GAP;
            }
        }
    }

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

// Floppy Sound: wird genau einmal pro C64 Takt aufgerufen (unabhaengig vom Laufwerkstakt)
void Floppy1541::SoundCycle()
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
}

// Ein Takt der Laufwerks CPU (1 MHz), wird von C64Class im Verhaeltnis Laufwerkstakt zu C64 Takt aufgerufen
bool Floppy1541::OneCycle()
{
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
    RAM[address & 0x7FF] = value;
}

uint8_t Floppy1541::ReadRam(uint16_t address)
{
    return RAM[address & 0x7FF];
}

uint8_t Floppy1541::ReadRom(uint16_t address)
{
    return ROM[address & 0x3FFF];
}

bool Floppy1541::SyncFound()
{
    // Sync Signal (VIA2 PB7 ist low solange mindestens 10 aufeinander folgende 1er Bits unter dem Kopf sind)
    if ((AktHalbSpur > ((NUM_TRACKS-1) * 2)) || (GCR_PTR == nullptr) || (DiskMotorOn == false)) return false;

    return SyncActive;
}

uint8_t Floppy1541::ReadGCRByte()
{
    // Lesen von VIA2 Port A ($1C01 / $1C0F)
    // Mit Eingangslatch (ACR Bit 0, setzt die ROM) liefert der Port das von Byte Ready (CA1) gelatchte Byte.
    // Das Lesen gibt das Latch wieder frei. Ist nichts gelatcht, liegen die Ausgänge des Schieberegisters direkt an.
    if((via2->GetACR() & 0x01) && ReadLatched)
    {
        ReadLatched = false;
        return ReadLatch;
    }
    return ReadShift;
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
    if ((AktHalbSpur > ((NUM_TRACKS-1) * 2)) || (GCR_PTR == nullptr) || (GCRBitTrackSize == 0))
        return;

    // Die Lesegeschwindigkeit kommt wie bei der echten Hardware aus den Dichte-Bits PB6:5 von VIA2 (11 = schnellste Zone, 26 Takte/Byte)
    int cycles_per_byte = 26 + 2 * (3 - ((via2->GetIO_Zero() >> 5) & 3));

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
            SectorMapDirty = true;
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
        if(SectorMapDirty)
            ScanSectorMap();

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

        // Byte Ready erreicht CA1 nur bei aktivem SOE (CA2 high). Die VIA latcht Port A nur, wenn das Latch
        // seit dem letzten Lesen von Port A frei ist. Bis dahin bleibt das zuerst gelatchte Byte stehen
        // (z.B. Turrican Spur 37: das erste $1C01 nach dem Sync-Warten liefert das $55 aus der Lücke hinter dem letzten gelesenen Sektor).
        if(byte_ready && ((via2->GetPCR() & 0x0E) == 0x0E) && (via2->GetACR() & 0x01) && !ReadLatched)
        {
            ReadLatch = ReadShift;
            ReadLatched = true;
        }
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

    // Wird bei jedem Reset Zyklus aufgerufen: Die Sektorlage nur neu ermitteln, wenn sich die Spur wirklich geaendert hat
    if(SectorMap.HalbSpur != AktHalbSpur || SectorMap.Bits != GCRBitTrackSize)
        SectorMapDirty = true;
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

// Das DOS der 1541 liest die Disk-ID erst beim ersten Diskzugriff (INITIALIZE) nach $12/$13.
// Nachlader, die ohne vorherigen Zugriff direkt per Job (z.B. JSR $D586) lesen, bekommen sonst
// "ID mismatch" (Fehler 11). Beim PRG-Autostart aus dem Floppy-Fenster fehlt dieser Zugriff.
void Floppy1541::InitDosDiskId()
{
    if(!FloppyEnabled || image_file == nullptr || ImageTyp != D64) return;
    if(RAM[0x12] != 0 || RAM[0x13] != 0) return;

    uint16_t disk_id = GetDiskIDFromBAM();
    RAM[0x12] = static_cast<uint8_t>(disk_id);
    RAM[0x13] = static_cast<uint8_t>(disk_id >> 8);
}

uint16_t Floppy1541::GetDiskIDFromBAM()
{
    // position for BAM
    uint8_t track = 18;
    uint8_t sector = 0;

    uint16_t disk_id;

    int index;

    index = D64_TRACK_INDEX[track] + sector;
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

int Floppy1541::GetCurrentD64Tracks()
{
    return D64Tracks;
}

uint8_t *Floppy1541::GetCurrentD64ImageBuffer()
{
    GCRImageToD64Image();
    return  D64Image;
}
