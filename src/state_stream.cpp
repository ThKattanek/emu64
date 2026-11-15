//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: state_stream.cpp                      //
//                                              //
//////////////////////////////////////////////////

#include "state_stream.h"

void StateWriter::Bytes(const void *data, size_t size)
{
    const uint8_t *d = static_cast<const uint8_t*>(data);
    buffer.insert(buffer.end(), d, d + size);
}

void StateWriter::String(const std::string &s)
{
    U32(static_cast<uint32_t>(s.size()));
    Bytes(s.data(), s.size());
}

void StateWriter::BeginChunk(const char tag[4], uint16_t version)
{
    Bytes(tag, 4);
    U16(version);
    chunk_stack.push_back(buffer.size());
    U32(0); // Laenge, wird in EndChunk gesetzt
}

void StateWriter::EndChunk()
{
    if(chunk_stack.empty()) return;
    size_t pos = chunk_stack.back();
    chunk_stack.pop_back();
    uint32_t len = static_cast<uint32_t>(buffer.size() - pos - 4);
    for(int i = 0; i < 4; i++)
        buffer[pos + i] = static_cast<uint8_t>(len >> (8 * i));
}

uint8_t StateReader::U8()
{
    if(p >= chunk_end)
    {
        error = true;
        return 0;
    }
    return *p++;
}

bool StateReader::Bytes(void *data, size_t size)
{
    if(size > static_cast<size_t>(chunk_end - p))
    {
        error = true;
        std::memset(data, 0, size);
        return false;
    }
    std::memcpy(data, p, size);
    p += size;
    return true;
}

bool StateReader::Skip(size_t size)
{
    if(size > static_cast<size_t>(chunk_end - p))
    {
        error = true;
        return false;
    }
    p += size;
    return true;
}

std::string StateReader::String()
{
    uint32_t len = U32();
    if(error || len > static_cast<size_t>(chunk_end - p))
    {
        error = true;
        return std::string();
    }
    std::string s(reinterpret_cast<const char*>(p), len);
    p += len;
    return s;
}

bool StateReader::NextChunk(char tag[5], uint16_t &version)
{
    chunk_end = end;
    if(error || end - p < 10) return false;
    std::memcpy(tag, p, 4);
    tag[4] = 0;
    p += 4;
    version = U16();
    uint32_t len = U32();
    if(len > static_cast<size_t>(end - p))
    {
        error = true;
        return false;
    }
    chunk_next = p + len;
    chunk_end = chunk_next;
    return true;
}

bool StateReader::FindChunk(const char tag[4], uint16_t &version)
{
    p = base;
    chunk_end = end;
    char t[5];
    while(NextChunk(t, version))
    {
        if(std::memcmp(t, tag, 4) == 0) return true;
        EndChunk();
    }
    return false;
}

void StateReader::EndChunk()
{
    if(chunk_next != nullptr) p = chunk_next;
    chunk_next = nullptr;
    chunk_end = end;
}

uint32_t StateCRC32(const uint8_t *data, size_t size)
{
    uint32_t crc = 0xffffffffu;
    for(size_t i = 0; i < size; i++)
    {
        crc ^= data[i];
        for(int b = 0; b < 8; b++)
            crc = (crc >> 1) ^ (0xedb88320u & (0u - (crc & 1u)));
    }
    return ~crc;
}
