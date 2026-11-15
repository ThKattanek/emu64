//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: state_stream.h                        //
//                                              //
// Serialisierung des Emulatorzustands          //
// (Quick C64 Snapshot, Issue #200)             //
//                                              //
//////////////////////////////////////////////////

#ifndef STATE_STREAM_H
#define STATE_STREAM_H

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <string>
#include <vector>

// Little-Endian Schreiber in einen Speicherpuffer.
// Daten sind in Chunks (4 Zeichen Tag, 16 Bit Version, 32 Bit Laenge) gegliedert.
class StateWriter
{
public:
    StateWriter() { buffer.reserve(1 << 20); }

    void U8(uint8_t v) { buffer.push_back(v); }
    void U16(uint16_t v) { U8(v & 0xff); U8(v >> 8); }
    void U32(uint32_t v) { U16(v & 0xffff); U16(v >> 16); }
    void U64(uint64_t v) { U32(v & 0xffffffffu); U32(v >> 32); }
    void I32(int32_t v) { U32(static_cast<uint32_t>(v)); }
    // Per Referenz und ueber das rohe Byte: Auch nicht initialisierte bool Member (Wert != 0/1) werden so sicher als 0/1 geschrieben
    void Bool(const bool &v) { U8(*reinterpret_cast<const uint8_t*>(&v) != 0 ? 1 : 0); }
    void F32(float v) { uint32_t t; std::memcpy(&t, &v, 4); U32(t); }
    void F64(double v) { uint64_t t; std::memcpy(&t, &v, 8); U64(t); }
    void Bytes(const void *data, size_t size);
    void String(const std::string &s);

    void BeginChunk(const char tag[4], uint16_t version);
    void EndChunk();

    const std::vector<uint8_t>& Data() const { return buffer; }

private:
    std::vector<uint8_t> buffer;
    std::vector<size_t> chunk_stack;
};

// Lesender Zugriff. Fehler werden nur gemerkt (Ok()), es wird nie ausserhalb gelesen.
class StateReader
{
public:
    StateReader(const uint8_t *data, size_t size) : base(data), p(data), end(data + size), chunk_end(data + size) {}

    uint8_t U8();
    uint16_t U16() { uint16_t a = U8(); return a | (static_cast<uint16_t>(U8()) << 8); }
    uint32_t U32() { uint32_t a = U16(); return a | (static_cast<uint32_t>(U16()) << 16); }
    uint64_t U64() { uint64_t a = U32(); return a | (static_cast<uint64_t>(U32()) << 32); }
    int32_t I32() { return static_cast<int32_t>(U32()); }
    bool Bool() { return U8() != 0; }
    float F32() { uint32_t t = U32(); float v; std::memcpy(&v, &t, 4); return v; }
    double F64() { uint64_t t = U64(); double v; std::memcpy(&v, &t, 8); return v; }
    bool Bytes(void *data, size_t size);
    std::string String();
    bool Skip(size_t size);

    // Sucht den naechsten Chunk. Gibt false zurueck wenn keiner mehr da ist.
    // Nach dem Lesen EndChunk() aufrufen (springt hinter den Chunk).
    bool NextChunk(char tag[5], uint16_t &version);
    // Springt zu einem Chunk mit bestimmtem Tag (ab Beginn). Gibt false zurueck wenn nicht vorhanden.
    bool FindChunk(const char tag[4], uint16_t &version);
    void EndChunk();

    bool Ok() const { return !error; }
    void SetError() { error = true; }
    size_t Remaining() const { return static_cast<size_t>(chunk_end - p); }

private:
    const uint8_t *base;
    const uint8_t *p;
    const uint8_t *end;
    const uint8_t *chunk_end;
    const uint8_t *chunk_next = nullptr;
    bool error = false;
};

uint32_t StateCRC32(const uint8_t *data, size_t size);

// Hilfsmakros fuer einfache Member
#define SS_W(w, var) (w).Bytes(&(var), sizeof(var))
#define SS_R(r, var) (r).Bytes(&(var), sizeof(var))

#endif // STATE_STREAM_H
