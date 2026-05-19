//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: t64_class.h                           //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef T64_CLASS_H
#define T64_CLASS_H

#include <cstdint>
#include <cstdio>

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
struct T64_HEADER
{
    char signature[32];
    uint16_t version;
    uint16_t max_file_entries;
    uint16_t total_file_entries;
    uint16_t reserved1;
    char image_name[25];       // 24 Zeichen + Nullterminator (Im gespeicherten Format sind es 24 Bytes ohne Nullterminator !!)
} PACKED_ATTR;
PACKED_STRUCT_END

PACKED_STRUCT_START
struct T64_FILE_ENTRY
{
    uint8_t c64s_file_type;
    uint8_t floppy1541_file_type;
    uint16_t start_address;
    uint16_t end_address;
    uint16_t reserved1;
    uint32_t data_offset;
    uint32_t reserved2;
    char filename[17];      // 16 Zeichen + Nullterminator (Im gespeicherten Format sind es 16 Bytes ohne Nullterminator !!)
} PACKED_ATTR;
PACKED_STRUCT_END

    class T64Class
{
public:
    T64Class(FILE *file);
    ~T64Class();

    int GetErrorCode() const { return error_code; }
    const char* GetImageName() const { return header ? header->image_name : nullptr; }
    uint16_t GetVersion() const { return header ? header->version : 0; }
    uint16_t GetTotalEntries();
    char* GetFileName(int index);
    uint8_t GetC64sFileType(int index);
    uint8_t GetFloppy1541FileType(int index);
    int LoadFileToRAM(int index, uint8_t *c64_ram);
    int GetFileOffset(int index);
    uint16_t GetFileStartAddress(int index);
    uint16_t GetFileEndAddress(int index);

private:
    int LoadT64(FILE *file);
    int error_code;

    T64_HEADER *header = nullptr;
    T64_FILE_ENTRY *file_entries;

    FILE *file;
};

#endif // T64_CLASS_H
