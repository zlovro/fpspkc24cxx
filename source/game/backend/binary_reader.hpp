#pragma once
#include <istream>

#include "types.hpp"

typedef struct BinaryReaderWriterStruct
{
private:
    std::iostream* mStream;

public:
    BinaryReaderWriterStruct(std::iostream* pStream)
    {
        mStream = pStream;
    }

    template <typename T>
    T read() const
    {
        T dst;
        mStream->read((char*)&dst, sizeof(T));
        return dst;
    }

    template <typename T>
    void write(T pValue) const
    {
        mStream->write(&pValue, sizeof(pValue));
    }

    void seekRead(const u64 pPos, const std::ios_base::seekdir pOffType) const
    {
        mStream->seekg(pPos, pOffType);
    }

    void seekWrite(const u64 pPos, const std::ios_base::seekdir pOffType) const
    {
        mStream->seekp(pPos, pOffType);
    }

    std::iostream* getBaseStream() const
    {
        return mStream;
    }
} BinaryReaderWriter;
