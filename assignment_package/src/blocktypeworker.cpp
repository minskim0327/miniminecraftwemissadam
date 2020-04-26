#include "blocktypeworker.h"
#include "iostream"

BlockTypeWorker::BlockTypeWorker(Terrain * terrain,
                                 int64_t hashCoord,
                                 QMutex* mutex)
    :mp_terrain(terrain), coord(hashCoord), mp_mutex(mutex)
{
}

void BlockTypeWorker::run() {
    // create 4 by 4 chunks and set its neighbors
    createChunksInTerrain();
}

void BlockTypeWorker::createChunksInTerrain() {
     std::vector<Chunk*> terrainsChunk;
     int x = toCoords(coord).x;
     int z = toCoords(coord).y;

     if (mp_terrain->hasChunkAt(x, z)) {
         return;
     }

     for (int i = 0; i < 4; i++) {
         for (int j = 0; j < 4; j++) {
//             mp_mutex->lock();
//             std::cout << x + 16 * i << ", " << z + 16 * j << std::endl;
//             mp_mutex->unlock();
//             mp_mutex->lock();
             Chunk* c = mp_terrain->createChunkAt(x + 16 * i, z + 16 * j);
             terrainsChunk.push_back(c);
             //terrainsChunk.push_back(mp_terrain->createChunkAt(x + 16 * i, z + 16 * j));
             if (!mp_terrain->hasChunkAt(x + 16 * i, z + 16 * j)) {
                 ;
//                    ; std::cout<<"hasnochunk" << x << ", " << z << std::endl;
         }
//             mp_mutex->unlock();
         }
     }

     // fill chunk with block
     for (int i = x; i < x + 64; i++) {
         for (int j = z; j < z + 64; j++) {

             mp_terrain->fillBlock(i, j);
         }
     }

    mp_mutex->lock();

    for (Chunk *c : terrainsChunk) {
         mp_terrain->chunksWithOnlyBlockData.push_back(c);
    }

    mp_mutex->unlock();
}
