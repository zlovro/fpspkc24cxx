#pragma once

#include <types.hpp>

typedef struct VariableBitsetStruct
{
private:
    u8* mData;
    int mBitMask;

public:
    int bitsPerElement;

    explicit VariableBitsetStruct(const int pCount, const int pElementSizeBits)
    {
        if (pElementSizeBits)

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
        // printf("SET\n");
        for (int arrIdx = pIdx * bitsPerElement, i = 0; i < bitsPerElement; arrIdx++, i++)
        {
            // mBitMask is both the bit mask and bitsPerElement - 1
            mData[arrIdx >> 3] |= (pState >> i & 1) << (arrIdx & mBitMask & 7);
            // printf("\tmdata[%d] |= %d << %d (arrIdx: %d, pstate: %x)\n ", arrIdx >> 3, bit, arrIdx & mBitMask & 7, arrIdx, pState);
        }
        // printf("\n");
    }

    u64 get(const size_t pIdx) const
    {
        // printf("GET ");
        u64 ret = 0;
        for (int arrIdx = pIdx * bitsPerElement, i = 0; i < bitsPerElement; arrIdx++, i++)
        {
            // int bit = ((mData[arrIdx / bitsPerElement] >> ((arrIdx & mBitMask))) & 1);
            // mBitMask is both the bit mask and bitsPerElement - 1
            ret |= ((u64)(mData[arrIdx >> 3] >> (arrIdx & mBitMask & 7) & 1)) << i;
            // printf("%d ", bit);
        }
        // printf("\n");
        return ret;
    }

    ~VariableBitsetStruct()
    {
        delete[] mData;
    }

    // static void test()
    // {
    //     auto bitset = VariableBitsetStruct(5, 57);
    //     bitset.set(0, 0xDEADBEEF);
    //     bitset.set(1, 0xCAFEBABE);
    //     bitset.set(2, 'RADE');
    //     bitset.set(3, 'KRUN');
    //     bitset.set(4, 'IC--');
    //
    //     for (int i = 0; i < 5; ++i)
    //     {
    //         printf("%llx\n", bitset.get(i));
    //     }
    //
    //     // auto bitset = VariableBitsetStruct(2, 9);
    //     // for (int i = 0; i < 2; ++i)
    //     // {
    //     //     bitset.set(i, (i + 1) * 11);
    //     //     printf("%d\n", bitset.get(i));
    //     // }
    // }
} VariableBitset;
