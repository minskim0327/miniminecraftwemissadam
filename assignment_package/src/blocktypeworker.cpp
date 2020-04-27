#include "blocktypeworker.h"
#include "iostream"

BlockTypeWorker::BlockTypeWorker(Terrain * terrain,
                                 int64_t hashCoord,
                                 std::vector<Chunk*> terrainsChunk,
                                 std::vector<Chunk *> *mp_chunksWithOnlyBlockData,
                                 QMutex* mutex)
    :mp_terrain(terrain), coord(hashCoord), terrainsChunk(terrainsChunk), mp_chunksWithOnlyBlockData(mp_chunksWithOnlyBlockData), mp_mutex(mutex)
{
}

void BlockTypeWorker::run() {
    // create 4 by 4 chunks and set its neighbors
    createChunksInTerrain();
}

void BlockTypeWorker::createChunksInTerrain() {
     int x = toCoords(coord).x;
     int z = toCoords(coord).y;

//     if (mp_terrain->hasChunkAt(x, z)) {
//         return;
//     }

     // fill chunk with block
     for (int i = x; i < x + 64; i++) {
         for (int j = z; j < z + 64; j++) {
             try {
             mp_terrain->fillBlock(i, j);
             }
             catch(std::out_of_range &e) {
                 std::cout << "asdfafasdf";
             }
         }
     }

    mp_mutex->lock();


    std::cout<<"reac" << std::endl;
    for (Chunk *c : terrainsChunk) {
         mp_terrain->chunksWithOnlyBlockData.push_back(c);
    }

    mp_mutex->unlock();
}
