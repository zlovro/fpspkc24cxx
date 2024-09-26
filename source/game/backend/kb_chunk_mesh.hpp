#pragma once

#include <types.hpp>
#include <glad/glad.h>

#include "variable_bitset.hpp"
#include "shaders/shader.hpp"

class KbChunkMesh
{
private:
    u32 mVtxCount, mTriCount;

    ShaderRaw* mShader = gShaderBasic;

    VariableBitset* mData;
    VariableBitset* mBakedData;

    u16* mIndices;

    GLuint mVao, mVbo, mEbo;

    bool mBaked = false;

public:
    u8  heightBitCount;
    u16 size;

    KbChunkMesh(VariableBitset* pData, u16 pSize)
    {
        mData = pData;
        size  = pSize;

        auto vertices = new u8[size * size];
        glGenBuffers(1, &mVao);
        glBindVertexArray(mVao);

        glGenBuffers(1, &mVbo);

        glBindBuffer(GL_ARRAY_BUFFER, mVbo);
        glBufferData(GL_ARRAY_BUFFER, size * size, vertices, GL_STATIC_DRAW);

        delete[] vertices;
    }

    ~KbChunkMesh()
    {
        delete mBakedData;
        delete[] mIndices;
    }

    void bake()
    {
        if (mBaked)
        {
            return;
        }

        mBaked = true;

        mVtxCount = size * size;
        mTriCount = (size - 1) * (size - 1) * 6;

        mIndices = new u16[mTriCount];

        heightBitCount = mData->bitsPerElement;
        mBakedData     = new VariableBitset(mVtxCount, heightBitCount);

        for (int y = 0, i = 0; y < size; y++)
        {
            for (auto x = 0; x < size; x++)
            {
                mBakedData->set(i, mData->get(i));

                i++;
            }
        }

        for (int y = 0, triIdx = 0, i = 0; y < size - 1; y++)
        {
            for (auto x = 0; x < size - 1; x++)
            {
                mIndices[triIdx++] = i;
                mIndices[triIdx++] = i + size;
                mIndices[triIdx++] = i + 1;

                mIndices[triIdx++] = i + 1;
                mIndices[triIdx++] = i + size;
                mIndices[triIdx++] = i + 1 + size;

                i++;
            }

            i++;
        }


        glBindVertexArray(mVao);
        glBindBuffer(GL_ARRAY_BUFFER, mVbo);

        glGenBuffers(1, &mEbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * size * sizeof(mIndices[0]), mIndices, GL_STATIC_DRAW);
    }

    void draw()
    {
        mShader->use();

        glBindVertexArray(mVao);

        glBindBuffer(GL_ARRAY_BUFFER, mVbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);

        glDrawElements(GL_TRIANGLES, mVtxCount * sizeof(mIndices[0]), GL_UNSIGNED_SHORT, mIndices);

        glBindVertexArray(0);
    }
};
