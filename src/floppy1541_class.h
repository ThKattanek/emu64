//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: floppy1541_class.h                    //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 18.09.2019        		//
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef FLOPPY1541_CLASS_H
#define FLOPPY1541_CLASS_H

#include <math.h>
#include <cstring>
#include <fstream>
#include <functional>

#include "./mos6502_class.h"
#include "./mos6522_class.h"
#include "./structs.h"

#define D64 0
#define G64 1
#define MAX_BREAK_GROUP_NUM 255

#define D64_IMAGE_SIZE 174848
#define G64_IMAGE_SIZE 665952

#define DISK_CHANGE_STATE_COUNTS 4
#define DISK_CHANGE_STATE_CYCLES 1000

#define DIRECTORY_TRACK 18

#define HEADER_GAP_BYTES 9

class Floppy1541
{    
public:

    /// Funktionen ///

    Floppy1541(bool *reset, int samplerate = 0, int buffersize = 0, bool *floppy_found_breakpoint = nullptr);
    ~Floppy1541();

    void SetEnableFloppy(bool status);
    bool GetEnableFloppy(void);
    void SetEnableFloppySound(bool status);
    bool GetEnableFloppySound();
    void* GetSoundBuffer();
    void ZeroSoundBufferPos();
    void SetFloppySoundVolume(float_t volume);
    bool LoadDiskImage(const char* filename);
    void UnLoadDiskImage();
    void SetC64IEC(uint8_t* port);
    void SetDeviceNumber(uint8_t number);
    void SetWriteProtect(bool status);
    bool LoadDosRom(const char* filename);
    bool OneCycle();
    void GetCpuReg(REG_STRUCT *reg, IREG_STRUCT *ireg);
    void SetCpuReg(REG_STRUCT *reg);
    void SetResetReady(bool* ResetReady, uint16_t ResetReadyAdr);
    void GetFloppyInfo(FLOPPY_INFO *fi);
    uint8_t ReadByte(uint16_t address);
    void WriteByte(uint16_t address, uint8_t value);
    int LoadFloppySounds(const char* motor_sound_filename, const char* motor_on_sound_filename, const char* motor_off_sound_filename, const char* anschlag_sound_filename, const char* stepper_dec_sound_filename, const char* Stepper_inc_sound_filename);
    void ResetCycleCounter();
    uint8_t* GetRamPointer();
    int16_t AddBreakGroup();
    void DelBreakGroup(int index);
    BREAK_GROUP* GetBreakGroup(int index);
    void UpdateBreakGroup();
    void DeleteAllBreakGroups();
    int GetBreakGroupCount();
    int LoadBreakGroups(const char *filename);
    bool SaveBreakGroups(const char *filename);
    bool CheckBreakpoints();
    bool CheckImageDirectoryWrite();
    uint8_t *GetCurrentD64ImageBuffer();

    /// Wird mit den einzelnen Chips verbunden via "bind" ///

    void WriteNoMem(uint16_t address, uint8_t value);
    uint8_t ReadNoMem(uint16_t address);
    void WriteRam(uint16_t address, uint8_t value);
    uint8_t ReadRam(uint16_t address);
    uint8_t ReadRom(uint16_t address);
    bool SyncFound();
    uint8_t ReadGCRByte();
    void WriteGCRByte(uint8_t value);
    void SpurInc();
    void SpurDec();

    /// Variablen ///

    bool            *RESET;
    unsigned char   FloppyIECLocal;
    unsigned char   Jumper;
    bool            StepperInc;
    bool            StepperDec;
    bool            StepperAnschlag;
    bool            DiskMotorOn;

    unsigned short  History[256];
    unsigned char   HistoryPointer;



private:

    /// Funktionen ///

    void CheckImageWrite();
    void D64ImageToGCRImage();
    int SectorToGCR(unsigned int track, unsigned int sector);
    void ConvertToGCR(unsigned char *source_buffer, unsigned char *destination_buffer);
    void GCRImageToD64Image();
    void GCRToSector(unsigned int spur, unsigned int sektor);
    void ConvertToD64(unsigned char *source_buffer, unsigned char *destination_buffer);
    void RenderFloppySound();
    void StartDiskChange();

    /// Variablen ///

    static const uint8_t num_sectors[42];
    static const uint8_t d64_track_zone[41];
    static const uint8_t d64_sector_gap[4];
    static const uint8_t motor_speed[4];

    uint8_t motor_rotate_speed;
    uint8_t motor_rotate_speed_counter;

    bool    FloppyEnabled;
    bool    WriteProtect;
    bool    WriteProtectAkt;            // Wird zusätzlich für Diskwechselsimaltion benötigt

    int     DiskChangeSimState;         // 0 = momentan kein Diskwechsel
    int     DiskChangeSimCycleCounter;

    REG_STRUCT rs;

    std::function<unsigned char(unsigned short)>  ReadProcTbl[256];
    std::function<void(unsigned short, unsigned char)> WriteProcTbl[256];

    unsigned char RAM[0x800];   // 2KB
    unsigned char ROM[0x4000];  // 16KB

    bool VIA1_IRQ;
    bool VIA2_IRQ;
    bool IRQ;

    int CycleCounter;

    MOS6502 *cpu;
    MOS6522 *via1;
    MOS6522 *via2;

    ////////// Fürs Disk Image //////////

    #define             FileNameSize 1024

    char                ImageFileName[FileNameSize];
    int                 ImageTyp;
    unsigned char       AktGCRWert;
    uint8_t             *AktGCRWrite;
    static const int	NUM_TRACKS = 42;
    static const int	GCR_SECTOR_SIZE = 364;      // SYNC Header Gap SYNC Data Gap (should be 5 SYNC bytes each) ///  ALF Sector in Byte
    static const int	GCR_TRACK_SIZE = 7928;      // Each track in gcr_data has 21 sectors
    unsigned char       AktHalbSpur;                // Aktuelle Halbspur Nummer (2..70)
    unsigned char *     GCR_PTR;                    // Zeiger auf GCR Daten Unter R/W Kopf
    unsigned char *     GCRSpurStart;               // Zeiger auf Start der GCR Daten auf Aktuellen Track
    unsigned char *     GCRSpurEnde;                // Zeiger auf Ende der GCR Daten auf Aktuellen Track
    bool                ImageWriteStatus;           // Sowie in Image geschrieben wird gehts auf true
    bool                ImageDirectoryWriteStatus;  // Sowie in das Image auf Spur 18 geschrieben wird wird es true
    unsigned char       D64Image[D64_IMAGE_SIZE];   // Aktuelles D64 Image
    unsigned char       GCRImage[G64_IMAGE_SIZE];   // Aktuelles GCR Image
    unsigned short      TrackSize[256];

    uint8_t             gcr_byte;
    uint8_t             gcr_byte_old;

    bool                sync_found;
    bool                sync_found_old;
    bool                soe;
    bool                soe_old;
    int                 soe_time;                   // Counter in Zyklen wann das Signal wieder zurückgenommen wird

    /// Für Floppy Sound ///

    double              Volume;
    bool                StepperIncWait;
    bool                FloppySoundsLoaded;	// zeigt an ob alle Floppy Sound geladen sind
    bool                FloppySoundEnabled;
    double              Samplerate;
    double              FreqConvCounter;
    double              FreqConvAddWert;
    int                 SoundBufferPos;
    int                 SoundBufferSize;
    short               *SoundBuffer;
    unsigned short*	FloppySound00;
    int			FloppySound00Size;
    int                 FloppySound00Pos;
    unsigned short*	FloppySound01;
    int			FloppySound01Size;
    int                 FloppySound01Pos;
    unsigned short*	FloppySound02;
    int			FloppySound02Size;
    int                 FloppySound02Pos;
    unsigned short*	FloppySound03;
    int			FloppySound03Size;
    int                 FloppySound03Pos;
    unsigned short*	FloppySound04;
    int			FloppySound04Size;
    int                 FloppySound04Pos;
    unsigned short*	FloppySound05;
    int			FloppySound05Size;
    int                 FloppySound05Pos;
    bool                MotorStatusOld;
    int                 MotorLoopStatus;
    int                 StepperLoopStatus;

    /////////////////////// BREAKPOINTS ////////////////////////

    // Bit 0 = PC Adresse
    // Bit 1 = AC
    // Bit 2 = XR
    // Bit 3 = YR
    // Bit 4 = Lesen von einer Adresse
    // Bit 5 = Schreiben in einer Adresse
    // Bit 6 = Lesen eines Wertes
    // Bit 7 = Schreiben eines Wertes
    // Bit 8 = Beim erreichen einer bestommten Raster Zeile
    // Bit 9 = Beim erreichen eines Zyklus in einer Rasterzeile

    unsigned short  Breakpoints[0x10000];
    unsigned short  BreakWerte[16];
    unsigned short  BreakStatus;
    bool            *FoundBreakpoint;

    uint8_t         breakgroup_count;
    BREAK_GROUP     *BreakGroup[MAX_BREAK_GROUP_NUM];

    ////////////////////////////////////////////////////////////
};

#endif // FLOPPY1541_CLASS_H
