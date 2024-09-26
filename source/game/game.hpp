#pragma once
#include "game_object.hpp"
#include "kb_chunk_archive.hpp"
#include "kb_chunk_mesh.hpp"

class Game
{
private:
    GO           mChunkHolder = GO();
    KbChunkMesh* mChMesh;

public:
    Game()
    {
        auto chArc = new KbChunkArchive("C:/Users/Made/Documents/src/cs/cross/srtm/workdir/chunks.kchk");

        mChMesh = new KbChunkMesh(chArc->provideChunk(0, 0), chArc->verticesPerChunk);
        mChMesh->bake();
    }

    void gameOnStart()
    {
        mChunkHolder = GO(VEC3_ZERO, VEC3_ZERO);
    }

    void gameOnGfxUpdate()
    {
        mChMesh->draw();
    }
};

Game* gGame;