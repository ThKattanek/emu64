//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: nbz_class.cpp                         //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "nbz_class.h"

// Das Format entspricht LZ_Uncompress() aus nibtools (lz.c, Copyright (c) 2003-2006 Marcus Geelnard, zlib Lizenz).
// Anders als dort wird hier jeder Zugriff auf Ein- und Ausgabe geprueft, damit auch beschaedigte Dateien nicht ausserhalb
// der Puffer lesen oder schreiben und keine unbegrenzten Datenmengen entstehen.
bool NBZ::Decompress(const uint8_t *in, size_t in_size, std::vector<uint8_t> &out)
{
    out.clear();

    if(in == nullptr || in_size < 2)
        return false;

    const uint8_t marker = in[0];
    size_t in_pos = 1;

    // Zahl mit 7 Bit je Byte, Bit 7 gesetzt = es folgt noch ein Byte (maximal 5 Bytes)
    auto read_var_size = [&](uint32_t &value) -> bool
    {
        value = 0;
        for(int i=0; i<5; i++)
        {
            if(in_pos >= in_size)
                return false;

            const uint8_t b = in[in_pos++];
            value = (value << 7) | (b & 0x7F);
            if((b & 0x80) == 0)
                return true;
        }
        return false;
    };

    while(in_pos < in_size)
    {
        const uint8_t symbol = in[in_pos++];

        if(symbol != marker)
        {
            if(out.size() >= MAX_UNPACKED_SIZE)
                return false;
            out.push_back(symbol);
            continue;
        }

        if(in_pos >= in_size)
            return false;

        if(in[in_pos] == 0)
        {
            // Einzelnes Markerbyte
            in_pos++;
            if(out.size() >= MAX_UNPACKED_SIZE)
                return false;
            out.push_back(marker);
            continue;
        }

        uint32_t length, offset;
        if(!read_var_size(length) || !read_var_size(offset))
            return false;

        if(offset == 0 || offset > out.size() || length > MAX_UNPACKED_SIZE - out.size())
            return false;

        // Die Quelle kann sich mit dem Ziel ueberlappen (Wiederholung), deshalb Byte fuer Byte kopieren
        const size_t src = out.size() - offset;
        for(uint32_t i=0; i<length; i++)
        {
            const uint8_t b = out[src + i];
            out.push_back(b);
        }
    }

    return !out.empty();
}
