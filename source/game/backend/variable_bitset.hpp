#pragma once

#include <types.hpp>

typedef struct VariableBitsetStruct
{
private:
    u8* mData;
    int mBitMask;

public:
    int bitsPerElement;

    explicit VariableBitsetStruct(u8* pData, const int pElementSizeBits)
    {
        mData = pData;
        bitsPerElement = pElementSizeBits;
        mBitMask       = (1 << pElementSizeBits) - 1;
    }

    explicit VariableBitsetStruct(const int pCount, const int pElementSizeBits)
    {
        bitsPerElement = pElementSizeBits;
        mBitMask       = (1 << pElementSizeBits) - 1;

        size_t dataSize = pCount * pElementSizeBits >> 3;
        if (pCount * pElementSizeBits - (dataSize << 3) > 0)
        {
            dataSize++;
        }

        mData = new u8[dataSize];
        memset(mData, 0, dataSize);
    }

    void set(const size_t pIdx, const u64 pState) const
    {
        for (int arrIdx = pIdx * bitsPerElement, i = 0; i < bitsPerElement; arrIdx++, i++)
        {
            mData[arrIdx >> 3] |= (pState >> i & 1) << (arrIdx & mBitMask & 7);
        }
    }

    u64 get(const size_t pIdx) const
    {
        u64 ret = 0;
        for (int arrIdx = pIdx * bitsPerElement, i = 0; i < bitsPerElement; arrIdx++, i++)
        {
            ret |= ((u64)(mData[arrIdx >> 3] >> (arrIdx & mBitMask & 7) & 1)) << i;
        }
        return ret;
    }

    ~VariableBitsetStruct()
    {
        delete[] mData;
    }
} VariableBitset;