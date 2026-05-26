//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: crt_class.h                           //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef CRT_CLASS_H
#define CRT_CLASS_H

#include <cstdint>
#include <cstdio>

#define MAX_CHIPS 128

// Makro für plattformübergreifendes Packing (1-Byte-Alignment)
#if defined(_MSC_VER)
// Für MSVC (Windows) nutzen wir pragma pack
#define PACKED_STRUCT_START __pragma(pack(push, 1))
#define PACKED_STRUCT_END   __pragma(pack(pop))
#define PACKED_ATTR
#elif defined(__GNUC__) || defined(__clang__)
// Für GCC (Linux, MinGW) und Clang (macOS) nutzen wir das Attribut
#define PACKED_STRUCT_START
#define PACKED_STRUCT_END
#define PACKED_ATTR __attribute__((packed))
#else
#define PACKED_STRUCT_START
#define PACKED_STRUCT_END
#define PACKED_ATTR
#warning "Compiler unterstützt Packing eventuell nicht standardmäßig!"
#endif

PACKED_STRUCT_START
    struct CRT_HEADER
{
    char signature[16];
    uint32_t header_length;
    uint16_t version;
    uint16_t cartridge_type;
    uint8_t exrom;
    uint8_t game;
    uint8_t reserved0[6];
    char cartridge_name[33];    // 32 Zeichen + Nullterminator (Im gespeicherten Format sind es 32 Bytes ohne Nullterminator !!)
} PACKED_ATTR;
PACKED_STRUCT_END

        PACKED_STRUCT_START
    struct CHIP_HEADER
{
    char signature[4];
    uint32_t packet_length;
    uint16_t chip_type;
    uint16_t bank_number;
    uint16_t load_address;
    uint16_t rom_size;
    uint8_t  *chip_rom;
} PACKED_ATTR;
PACKED_STRUCT_END

    class CRTClass
{
public:
    CRTClass(FILE *file);
    ~CRTClass();

    int GetErrorCode() const { return error_code; }

    int LoadCRT(FILE *file);
    int SaveCRT(FILE *file);

    char*GetCartridgeName()             // Gibt den Namen des Cartridges zurück (Nullterminiert)
    { return header->cartridge_name; }
    uint32_t GetHeaderLength();         // Gibt die Länge des Headers zurück (Defaultmäßig 0x40, aber es könnte auch länger sein, wenn zusätzliche Informationen im Header gespeichert sind)
    uint16_t GetImageVersion();         // High Byte = Major Version, Low Byte = Minor Version
    uint16_t GetCartridgeType();        // Gibt den Cartridge Type zurück (siehe CRT Dokumentation für die möglichen Werte)
    uint8_t GetExrom()                  // Gibt die Werte der Exrom und Game Leitungen zurück (0 = Leitung auf High, 1 = Leitung auf Low)
    { return header->exrom; }
    uint8_t GetGame()                   // Gibt die Werte der Exrom und Game Leitungen zurück (0 = Leitung auf High, 1 = Leitung auf Low)
    { return header->game; }
    int GetChipCount()                  // Gibt die Anzahl der Chips zurück, die im CRT Image enthalten sind
    { return chip_count; }
    int GetChipType(int index);         // Gibt den Chip Type für den Chip mit dem angegebenen Index zurück (siehe CRT Dokumentation für die möglichen Werte)
    uint16_t GetChipBankNumber(int index);  // Gibt die Bank Nummer für den Chip mit dem angegebenen Index zurück
    uint16_t GetChipLoadAddress(int index); // Gibt die Load Adresse für den Chip mit dem angegebenen Index zurück
    uint16_t GetChipRomSize(int index);     // Gibt die Größe der ROM Daten für den Chip mit dem angegebenen Index zurück
    void CopyChipRomData(int index, uint8_t *destination, size_t offset); // Kopiert die ROM Daten für den Chip mit dem angegebenen Index in den angegebenen Zielpuffer (Der Zielpuffer muss groß genug sein, um die ROM Daten aufzunehmen, siehe GetChipRomSize)

private:
    uint32_t SwapEndianness(uint32_t value);
    void Cleanup();

    int error_code;

    CRT_HEADER *header = nullptr;
    CHIP_HEADER *chip_header[MAX_CHIPS] = {nullptr};

    FILE *file;

    int chip_count;

};

#endif // CRT_CLASS_H