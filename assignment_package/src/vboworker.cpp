#include "vboworker.h"
#include "iostream"
VBOWorker::VBOWorker(Terrain *terrain,
                    std::vector<ChunkVBOData>* mp_chunksWithVBOData,
                     Chunk *c,
                     QMutex *mutex)
    :mp_terrain(terrain),
      mp_chunksWithVBOData(mp_chunksWithVBOData ),
      mp_mutex(mutex),
      mp_chunk(c)
{
}
void VBOWorker::run() {
    std::vector<glm::vec4> pos;
    std::vector<glm::vec4> col;
    std::vector<glm::vec4> nor;
    std::vector<GLuint> idx;
    mp_chunk->createVBO(&pos, &col, &nor, &idx);



    ChunkVBOData vboData;
    vboData.associated_chunk = mp_chunk;
    vboData.idx_data = idx;
    for (unsigned int i = 0; i < pos.size(); ++i) {
        vboData.vertex_data.push_back(pos.at(i));
        vboData.vertex_data.push_back(nor.at(i));
        vboData.vertex_data.push_back(col.at(i));
    }

    mp_mutex->lock();
    mp_chunksWithVBOData->push_back(vboData);
    mp_mutex->unlock();
}
