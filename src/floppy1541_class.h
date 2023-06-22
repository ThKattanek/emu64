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
// Letzte Änderung am 22.06.2023	       		//
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

#define MAX_BREAK_GROUP_NUM 255

#define D64_IMAGE_SIZE 174848
#define G64_IMAGE_SIZE 665952

#define DISK_CHANGE_STATE_COUNTS 4
#define DISK_CHANGE_STATE_CYCLES 1000

#define DIRECTORY_TRACK 18

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
	bool LoadDiskImage(FILE *file, int typ);		// 0=D64 , 1=G64
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
    uint8_t         FloppyIECLocal;
    uint8_t         Jumper;
    bool            StepperInc;
    bool            StepperDec;
    bool            StepperAnschlag;
    bool            DiskMotorOn;

    uint16_t        History[256];
    uint8_t         HistoryPointer;

    int SyncFoundCount;

private:

    /// Funktionen ///

    void CheckImageWrite();
    void D64ImageToGCRImage();
	void SectorToGCR(unsigned int spur, unsigned int sektor, uint16_t disk_id);
    void ConvertToGCR(uint8_t *source_buffer, uint8_t *destination_buffer);
    void GCRImageToD64Image();
    void GCRToSector(unsigned int spur, unsigned int sektor);
    void ConvertToD64(uint8_t *source_buffer, uint8_t *destination_buffer);
    void RenderFloppySound();
    void StartDiskChange();

	uint16_t GetDiskIDFromBAM();

    /// Variablen ///

    static const uint8_t num_sectors[42];
    static const uint8_t d64_track_zone[41];
    static const uint8_t d64_sector_gap[4];

    bool    FloppyEnabled;
    bool    WriteProtect;
    bool    WriteProtectAkt;            // Wird zusätzlich für Diskwechselsimaltion benötigt

    int     DiskChangeSimState;         // 0 = momentan kein Diskwechsel
    int     DiskChangeSimCycleCounter;

    REG_STRUCT rs;

    std::function<uint8_t(uint16_t)>  ReadProcTbl[256];
    std::function<void(uint16_t, uint8_t)> WriteProcTbl[256];

    uint8_t RAM[0x800];   // 2KB
    uint8_t ROM[0x4000];  // 16KB

    bool VIA1_IRQ;
    bool VIA2_IRQ;
    bool IRQ;

    int CycleCounter;

    MOS6502 *cpu;
    MOS6522 *via1;
    MOS6522 *via2;

    ////////// Fürs Disk Image //////////

    #define             FileNameSize 1024

	FILE *              image_file;
    int                 ImageTyp;
    uint8_t             AktGCRWert;
    static const int	NUM_TRACKS = 42;
    static const int	GCR_SECTOR_SIZE = 364;      // SYNC Header Gap SYNC Data Gap (should be 5 SYNC bytes each) ///  ALF Sector in Byte
    static const int	GCR_TRACK_SIZE = 7928;      // Each track in gcr_data has 21 sectors
    uint8_t             AktHalbSpur;                // Aktuelle Halbspur Nummer (2..70)
    uint8_t *           GCR_PTR;                    // Zeiger auf GCR Daten Unter R/W Kopf
    uint8_t *           GCRSpurStart;               // Zeiger auf Start der GCR Daten auf Aktuellen Track
    uint8_t *           GCRSpurEnde;                // Zeiger auf Ende der GCR Daten auf Aktuellen Track
    bool                ImageWriteStatus;           // Sowie in Image geschrieben wird gehts auf true
    bool                ImageDirectoryWriteStatus;  // Sowie in das Image auf Spur 18 geschrieben wird wird es true
    uint8_t             D64Image[D64_IMAGE_SIZE];   // Aktuelles D64 Image
    uint8_t             GCRImage[G64_IMAGE_SIZE];   // Aktuelles GCR Image
    uint16_t            TrackSize[256];

    /// Für Floppy Sound ///

    float               Volume;
    bool                StepperIncWait;
    bool                FloppySoundsLoaded;	// zeigt an ob alle Floppy Sound geladen sind
    bool                FloppySoundEnabled;
    float               Samplerate;
    float               FreqConvCounter;
    float               FreqConvAddWert;
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

    uint16_t        Breakpoints[0x10000];
    uint16_t        BreakWerte[16];
    uint16_t        BreakStatus;
    bool            *FoundBreakpoint;

    uint8_t         breakgroup_count;
    BREAK_GROUP     *BreakGroup[MAX_BREAK_GROUP_NUM];

    ////////////////////////////////////////////////////////////
};

#endif // FLOPPY1541_CLASS_H
