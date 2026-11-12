//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: nbz_class.h                           //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef NBZ_CLASS_H
#define NBZ_CLASS_H

#include <cstddef>
#include <cstdint>
#include <vector>

// NBZ ist eine komplette NIB Datei (MNIB-1541-RAW), komprimiert mit dem LZ77 Verfahren aus nibtools (lz.c von Marcus Geelnard,
// zlib Lizenz). Es ist kein Deflate. Aufbau: Das erste Byte ist ein Markerbyte. Jedes andere Byte wird kopiert. Der Marker gefolgt von
// 0 steht fuer ein einzelnes Markerbyte, sonst folgen Laenge und Offset (je mit 7 Bit pro Byte, Bit 7 = weitere Bytes folgen),
// die aus dem bereits entpackten Bereich kopiert werden.
class NBZ
{
public:
    // Groesste NIB Datei: 256 Byte Header und 120 Eintraege zu je 8192 Byte
    static const size_t MAX_UNPACKED_SIZE = 256 + 120 * 8192;

    // Entpackt die Daten in out. Gibt false zurueck bei fehlerhaften Daten (auch wenn mehr als MAX_UNPACKED_SIZE entstehen wuerde).
    static bool Decompress(const uint8_t *in, size_t in_size, std::vector<uint8_t> &out);
};

#endif // NBZ_CLASS_H
