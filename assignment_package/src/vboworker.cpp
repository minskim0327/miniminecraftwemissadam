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
//    std::vector<glm::vec4> pos;
//    std::vector<glm::vec4> col;
//    std::vector<glm::vec4> nor;
//    std::vector<GLuint> idx;
//    mp_chunk->createVBO(&pos, &col, &nor, &idx);
    std::vector<glm::vec4> posOpq;
    std::vector<glm::vec4> norOpq;
    std::vector<glm::vec2> uvOpq;
    std::vector<glm::vec4> posTran;
    std::vector<glm::vec4> norTran;
    std::vector<glm::vec2> uvTran;
    std::vector<float> animOpq;
    std::vector<float> animTran;
    std::vector<float> allOpq; //interleaved
    std::vector<float> allTran;
    std::vector<GLuint> idxOpq;
    std::vector<GLuint> idxTran;
    mp_chunk->createVBO(&posOpq,
                        &norOpq,
                        &uvOpq,
                        &animOpq,
                        &idxOpq,
                        &posTran,
                        &norTran,
                        &uvTran,
                        &animTran,
                        &idxTran);

    ChunkVBOData vboData;
    vboData.associated_chunk = mp_chunk;
    vboData.idx_opq_data = idxOpq;
    vboData.idx_tran_data = idxTran;

    // interleave opq
    for (unsigned int i = 0; i < posOpq.size(); ++i) {
        vboData.vertex_opq_data.push_back(posOpq.at(i)[0]);
        vboData.vertex_opq_data.push_back(posOpq.at(i)[1]);
        vboData.vertex_opq_data.push_back(posOpq.at(i)[2]);
        vboData.vertex_opq_data.push_back(posOpq.at(i)[3]);
        vboData.vertex_opq_data.push_back(norOpq.at(i)[0]);
        vboData.vertex_opq_data.push_back(norOpq.at(i)[1]);
        vboData.vertex_opq_data.push_back(norOpq.at(i)[2]);
        vboData.vertex_opq_data.push_back(norOpq.at(i)[3]);
        vboData.vertex_opq_data.push_back(uvOpq.at(i)[0]);
        vboData.vertex_opq_data.push_back(uvOpq.at(i)[1]);
        vboData.vertex_opq_data.push_back(animOpq.at(i));
    }

    for (unsigned int i = 0; i < posTran.size(); ++i) {
        vboData.vertex_tran_data.push_back(posTran.at(i)[0]);
        vboData.vertex_tran_data.push_back(posTran.at(i)[1]);
        vboData.vertex_tran_data.push_back(posTran.at(i)[2]);
        vboData.vertex_tran_data.push_back(posTran.at(i)[3]);
        vboData.vertex_tran_data.push_back(norTran.at(i)[0]);
        vboData.vertex_tran_data.push_back(norTran.at(i)[1]);
        vboData.vertex_tran_data.push_back(norTran.at(i)[2]);
        vboData.vertex_tran_data.push_back(norTran.at(i)[3]);
        vboData.vertex_tran_data.push_back(uvTran.at(i)[0]);
        vboData.vertex_tran_data.push_back(uvTran.at(i)[1]);
        vboData.vertex_tran_data.push_back(animTran.at(i));
    }

    mp_mutex->lock();
    mp_chunksWithVBOData->push_back(vboData);
    mp_mutex->unlock();
}
