#pragma once
#include <QRunnable>
#include <QMutex>
#include <scene/chunk.h>
#include <scene/terrain.h>
using namespace std;

class VBOWorker : public QRunnable
{
private:
    Terrain *mp_terrain;
    std::vector<ChunkVBOData>* mp_chunksWithVBOData;
    QMutex *mp_mutex;
    Chunk *mp_chunk;

public:

    VBOWorker(Terrain *terrain,
              std::vector<ChunkVBOData>* mp_chunksWithVBOData,
              Chunk *c,
              QMutex *mutex);
    void run() override;
};

