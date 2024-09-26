#pragma once
#include <filesystem>
#include <fstream>

#include <gzip/decompress.hpp>

#include "binary_reader.hpp"
#include "kb_util.hpp"
#include "types.hpp"
#include "variable_bitset.hpp"

typedef class KbChunkArchiveStruct
{
private:
    BinaryReaderWriter mStream = {nullptr};
    std::fstream       mFileStream;

    // array of sets of variable bitset pointers
    VariableBitset*** mCachedChunks;

public:
    u32 chunkCount;
    u16 chunkCountX, chunkCountZ;
    u16 verticesPerChunk;
    u16 maxValue;

    u32* offsetTable;

    u64 fileSize;
    u8  lodLevels;

    KbChunkArchiveStruct(std::string pFileName)
    {
        mFileStream = std::fstream(pFileName, std::ios_base::in | std::ios_base::binary);
        mStream     = BinaryReaderWriter(&mFileStream);

        fileSize = std::filesystem::file_size(pFileName);

        chunkCountX      = mStream.read<u16>();
        chunkCountZ      = mStream.read<u16>();
        verticesPerChunk = mStream.read<u16>();
        maxValue         = mStream.read<u16>();

        lodLevels  = bitSize(verticesPerChunk);
        chunkCount = chunkCountX * chunkCountZ;

        offsetTable = new u32[chunkCount];
        for (auto i = 0; i < chunkCount; ++i)
        {
            offsetTable[i] = mStream.read<u32>();
        }

        mCachedChunks = new VariableBitset**[chunkCount];
        for (int i = 0; i < chunkCount; ++i)
        {
            mCachedChunks[i] = nullptr;
        }
    }

    u32 getChunkSize(const int pChunkIdx) const
    {
        return pChunkIdx == chunkCount - 1 ? fileSize - offsetTable[pChunkIdx] : offsetTable[pChunkIdx + 1] - offsetTable[pChunkIdx];
    }

    // not null
    VariableBitset* provideChunk(const int pX, const int pZ, const int pLod = 0) const
    {
        auto lodCollection = mCachedChunks[pX + pZ * chunkCountX];
        if (!lodCollection)
        {
            loadChunk(pX, pZ);
            lodCollection = mCachedChunks[pX + pZ * chunkCountX];
        }

        return lodCollection[pLod];
    }

    // nullable
    VariableBitset* getCachedChunk(const int pX, const int pZ, const int pLod = 0) const
    {
        const auto lodCollection = mCachedChunks[pX + pZ * chunkCountX];
        if (!lodCollection)
        {
            return nullptr;
        }

        return lodCollection[pLod];
    }

    void unloadChunk(const int pX, const int pZ) const
    {
        for (int i = 0; i < lodLevels; ++i)
        {
            delete mCachedChunks[pX + pZ * chunkCountX][i];
        }
        delete[] mCachedChunks[pX + pZ * chunkCountX];
    }

    void loadChunk(const int pX, const int pZ) const
    {
        const auto chunkIdx = pX + chunkCountX * pZ;
        if (mCachedChunks[chunkIdx])
        {
            unloadChunk(pX, pZ);
        }

        auto chunkData = new VariableBitset*[lodLevels];

        mStream.seekRead(offsetTable[chunkIdx], std::ios::beg);
        u8 elementSizeBits = mStream.read<u8>();

        const auto chunkSize    = getChunkSize(chunkIdx);
        const auto tmpChunkData = new u8[chunkSize];
        mStream.getBaseStream()->read((char*)tmpChunkData, chunkSize);

        auto decompressed = gzip::decompress((const char*)tmpChunkData, chunkSize);
        delete[] tmpChunkData;

        const auto lod0 = new VariableBitset((u8*)decompressed.data(), elementSizeBits);
        chunkData[0] = lod0;

        for (auto lod = 1; lod < lodLevels; lod++)
        {
            const auto sizeLod   = verticesPerChunk >> lod;
            const auto lodBitset = new VariableBitset(sizeLod * sizeLod, elementSizeBits);

            for (int y = 0, i = 0; y < sizeLod; y++)
            {
                for (auto x = 0; x < sizeLod; x++)
                {
                    auto yLod0 = y << lod;
                    auto xLod0 = x << lod;

                    if (x == sizeLod - 1)
                    {
                        xLod0 = verticesPerChunk - 1;
                    }
                    if (y == sizeLod - 1)
                    {
                        yLod0 = verticesPerChunk - 1;
                    }

                    lodBitset->set(i, chunkData[0]->get(yLod0 * verticesPerChunk + xLod0));

                    i++;
                }
            }

            chunkData[lod] = lodBitset;
        }

        mCachedChunks[chunkIdx] = chunkData;
    }

    ~KbChunkArchiveStruct()
    {
        for (int i = 0; i < chunkCount; ++i)
        {
            auto chunk = mCachedChunks[i];
            delete[] chunk;
        }
        delete[] offsetTable;
        mFileStream.close();
    }
} KbChunkArchive;
