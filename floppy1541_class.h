//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: floppy1541_class.h                    //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 28.08.2011		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#ifndef FLOPPY1541_CLASS_H
#define FLOPPY1541_CLASS_H

#include "cstring"
#include <fstream>
using namespace std;

#include "mos6502_class.h"
#include "mos6522_class.h"
#include "structs.h"

#include "tr1/functional"
using namespace std::tr1;
using namespace std::tr1::placeholders;

static const int NUM_SECTORS[42] = {21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,19,19,19,19,19,19,19,18,18,18,18,18,18,17,17,17,17,17,17,17,17,17,17,17,17};

#define D64 0
#define G64 1
#define MAX_BREAK_GROUPS 255

class Floppy1541
{    
public:

    /// Funktionen ///

    Floppy1541(bool *reset, int samplerate = 0,int puffersize = 0, bool *floppy_found_breakpoint = 0);
    ~Floppy1541();

    void SetEnableFloppy(bool status);
    bool GetEnableFloppy(void);
    void SetEnableFloppySound(bool status);
    bool GetEnableFloppySound(void);
    void* GetSoundBuffer(void);
    void ZeroSoundBufferPos(void);
    void SetFloppySoundVolume(double volume);
    bool LoadDiskImage(char* filename);
    void UnLoadDiskImage(void);
    void SetC64IEC(unsigned char* port);
    void SetDeviceNummer(unsigned char nummer);
    bool LoadDosRom(char* filename);
    bool OneZyklus(void);
    void GetCpuReg(REG_STRUCT *reg,IREG_STRUCT *ireg);
    void SetCpuReg(REG_STRUCT *reg);
    void SetResetReady(bool* ResetReady,unsigned short ResetReadyAdr);
    void GetFloppyInfo(FLOPPY_INFO *fi);
    unsigned char ReadByte(unsigned short adresse);
    void WriteByte(unsigned short adresse,unsigned char wert);
    int LoadFloppySounds(char* motor_sound,char* motor_on_sound,char* motor_off_sound,char* anschlag_sound,char* stepper_dec_sound,char* Stepper_inc_sound);
    void ResetCycleCounter(void);
    unsigned char* GetRamPointer(void);
    int AddBreakGroup(void);
    void DelBreakGroup(int index);
    BREAK_GROUP* GetBreakGroup(int index);
    void UpdateBreakGroup(void);
    void DeleteAllBreakGroups(void);
    int GetBreakGroupAnz(void);
    int LoadBreakGroups(char *filename);
    bool SaveBreakGroups(char *filename);
    bool CheckBreakpoints(void);

    /// Wird mit den einzelnen Chips verbunden via "bind" ///

    void WriteNoMem(unsigned short adresse,unsigned char wert);
    unsigned char ReadNoMem(unsigned short adresse);
    void WriteRam(unsigned short adresse,unsigned char wert);
    unsigned char ReadRam(unsigned short adresse);
    unsigned char ReadRom(unsigned short adresse);
    bool SyncFound(void);
    unsigned char ReadGCRByte(void);
    void WriteGCRByte(unsigned char);
    void SpurInc(void);
    void SpurDec(void);

    /// Variablen ///

    bool            *RESET;
    unsigned char   FloppyIECLocal;
    unsigned char   Jumper;
    bool            WriteProtected;
    bool            StepperInc;
    bool            StepperDec;
    bool            StepperAnschlag;
    bool            DiskMotorOn;

    unsigned short  History[256];
    unsigned char   HistoryPointer;

private:

    /// Funktionen ///

    void CheckImageWrite(void);
    void D64ImageToGCRImage(void);
    void SectorToGCR(unsigned int spur, unsigned int sektor);
    void ConvertToGCR(unsigned char *source_buffer, unsigned char *destination_buffer);
    void GCRImageToD64Image(void);
    void GCRToSector(unsigned int spur, unsigned int sektor);
    void ConvertToD64(unsigned char *source_buffer, unsigned char *destination_buffer);
    void RenderFloppySound(void);

    /// Variablen ///

    bool                FloppyEnabled;

    REG_STRUCT rs;

    function<unsigned char(unsigned short)>  ReadProcTbl[256];
    function<void(unsigned short, unsigned char)> WriteProcTbl[256];

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
    static const int	NUM_TRACKS = 42;
    static const int	GCR_SECTOR_SIZE = 364;  // SYNC Header Gap SYNC Data Gap (should be 5 SYNC bytes each) ///  ALF Sector in Byte
    static const int	GCR_TRACK_SIZE = 7928;  // Each track in gcr_data has 21 sectors
    int                 AktHalbSpur;		// Aktuelle Halbspur Nummer (2..70)
    unsigned char *     GCR_PTR;                // Zeiger auf GCR Daten Unter R/W Kopf
    unsigned char *     GCRSpurStart;		// Zeiger auf Start der GCR Daten auf Aktuellen Track
    unsigned char *     GCRSpurEnde;		// Zeiger auf Ende der GCR Daten auf Aktuellen Track
    bool                ImageWriteStatus;	// Sowie in Image geschrieben wird gehts auf true
    unsigned char       D64Image[174848];	// Aktuelles D64 Image
    unsigned char       GCRImage[665952];	// Aktuelles GCR Image
    unsigned short      TrackSize[84];

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

    unsigned char   BreakGroupAnz;
    BREAK_GROUP     *BreakGroup[MAX_BREAK_GROUPS];

    ////////////////////////////////////////////////////////////
};

#endif // FLOPPY1541_CLASS_H
