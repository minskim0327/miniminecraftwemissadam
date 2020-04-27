#pragma once
#include <QRunnable>
#include <QMutex>
#include <scene/terrain.h>
using namespace std;

class BlockTypeWorker : public QRunnable
{
private:
   Terrain *mp_terrain;
   int64_t coord;
   std::vector<Chunk*> terrainsChunk;
   std::vector<Chunk*> *mp_chunksWithOnlyBlockData;
   QMutex *mp_mutex;

public:
//    BlockTypeWorker();
    BlockTypeWorker(Terrain * terrain,
                    int64_t hashCoord,
                    std::vector<Chunk*> terrainsChunk,
                    std::vector<Chunk*> *mp_chunksWithOnlyBlockData,
                    QMutex* mutex);
    void run() override;

    // create 4 by 4 chunks and set its neighbors
    void createChunksInTerrain();
};
