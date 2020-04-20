#include "chunk.h"
#include "drawable.h"
#include "iostream"


//Chunk::Chunk() :m_blocks(), m_neighbors{{XPOS, nullptr}, {XNEG, nullptr}, {ZPOS, nullptr}, {ZNEG, nullptr}}
//{
//    std::fill_n(m_blocks.begin(), 65536, EMPTY);
//}

Chunk::Chunk(OpenGLContext* context) : Drawable(context), m_blocks(), m_neighbors{{XPOS, nullptr}, {XNEG, nullptr}, {ZPOS, nullptr}, {ZNEG, nullptr}}
{
    std::fill_n(m_blocks.begin(), 65536, EMPTY);
}


const static std::unordered_map<Direction, Direction, EnumHash> oppositeDirection {
    {XPOS, XNEG},
    {XNEG, XPOS},
    {YPOS, YNEG},
    {YNEG, YPOS},
    {ZPOS, ZNEG},
    {ZNEG, ZPOS}
};


void Chunk::linkNeighbor(uPtr<Chunk> &neighbor, Direction dir) {
    if(neighbor != nullptr) {
        this->m_neighbors[dir] = neighbor.get();
        neighbor->m_neighbors[oppositeDirection.at(dir)] = this;
    }
}

Chunk::~Chunk() {}

void Chunk::create() {

    int num_count_Opq = 0; //num increased by 4 every time for index vbo
    int num_count_Tran = 0;
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



    for (int x = 0; x < 16; ++x) {
        for (int y = 0; y < 256; ++y) {
            for (int z = 0; z < 16; ++z) {

                std::vector<glm::vec4> *pos;
                std::vector<glm::vec4> *nor;
                std::vector<glm::vec2> *uv;
                std::vector<float> *anim;

                int num = 0; //num of faces in each block that we want to save data
                BlockType t = getBlockAt(x, y, z);
                if (t == EMPTY) {
                    continue;
                } else if (t == GRASS || t == DIRT || t == STONE || t == SNOW || t == LAVA) {
                    pos = &posOpq;
                    nor = &norOpq;
                    uv = &uvOpq;
                    anim = &animOpq;
                } else {
                    pos = &posTran;
                    nor = &norTran;
                    uv = &uvTran;
                    anim = &animTran;
                }

                // to create vbo data for 6 faces
                BlockType topBlock = EMPTY;
                if (y == 255) {
                    topBlock = EMPTY;
                } else {
                    topBlock = getBlockAt(x, y+1, z);
                }

                BlockType botBlock = EMPTY;
                if (y == 0) {
                    botBlock = EMPTY;
                } else {
                    botBlock = getBlockAt(x, y-1, z);
                }

                BlockType rightBlock = EMPTY;
                if (x == 15) {
                    Chunk *rightNeighbor = m_neighbors.at(XPOS);
                    if (rightNeighbor != nullptr) {
                        rightBlock = rightNeighbor->getBlockAt((x+1)%16, y, z);
                    }
                } else {
                    rightBlock = getBlockAt(x + 1, y, z);
                }

                BlockType leftBlock = EMPTY;
                if (x == 0) {
                    Chunk *leftNeighbor = m_neighbors.at(XNEG);
                    if (leftNeighbor != nullptr) {
                        leftBlock = leftNeighbor->getBlockAt(15, y, z);
                    }
                } else {
                    leftBlock = getBlockAt(x - 1, y, z);
                }

                BlockType frontBlock = EMPTY;
                if (z == 15) {
                    Chunk *frontNeighbor = m_neighbors.at(ZPOS);
                    if (frontNeighbor != nullptr) {
                        frontBlock = frontNeighbor->getBlockAt(x, y, 0);
                    }
                } else {
                    frontBlock = getBlockAt(x, y, z+1);
                }

                BlockType backBlock = EMPTY;
                if (z == 0) {
                    Chunk *backNeighbor = m_neighbors.at(ZNEG);
                    if (backNeighbor != nullptr) {
                        backBlock = backNeighbor->getBlockAt(x, y, 15);
                    }
                } else {
                    backBlock = getBlockAt(x, y, z-1);
                }

                if (topBlock == EMPTY || topBlock == WATER || topBlock == ICE) {
                    num++;
                    glm::vec4 normal(0.f, 1.f, 0.f, 0.f);
                    nor->push_back(normal);
                    nor->push_back(normal);
                    nor->push_back(normal);
                    nor->push_back(normal);

                    pos->push_back(glm::vec4(x + worldP_x, y+1, z + worldP_z, 1.f));
                    pos->push_back(glm::vec4(x + worldP_x + 1, y+1, z + worldP_z, 1.f));
                    pos->push_back(glm::vec4(x + worldP_x + 1, y+1, z + worldP_z -1, 1.f));
                    pos->push_back(glm::vec4(x + worldP_x, y+1, z + worldP_z-1, 1.f));

                    switch(t) {
                    case GRASS:
                        //col->push_back(glm::vec4(95.f, 159.f, 53.f, 255.f) / 255.f);
                        uv->push_back(glm::vec2(8.f/16.f, 13.f/16.f));
                        uv->push_back(glm::vec2(9.f/16.f, 13.f/16.f));
                        uv->push_back(glm::vec2(9.f/16.f, 14.f/16.f));
                        uv->push_back(glm::vec2(8.f/16.f, 14.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);

                        break;
                    case DIRT:
                        //col->push_back(glm::vec4(121.f, 85.f, 58.f, 255.f) / 255.f);
                        uv->push_back(glm::vec2(2.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 16.f/16.f));
                        uv->push_back(glm::vec2(2.f/16.f, 16.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);

                        break;
                    case STONE:
                        //col->push_back(glm::vec4(0.5f));
                        uv->push_back(glm::vec2(1.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(2.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(2.f/16.f, 16.f/16.f));
                        uv->push_back(glm::vec2(1.f/16.f, 16.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);

                        break;
                    case SNOW:
                        //col->push_back(glm::vec4(1.f));
                        uv->push_back(glm::vec2(2.f/16.f, 11.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 11.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 12.f/16.f));
                        uv->push_back(glm::vec2(2.f/16.f, 12.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);

                        break;
                    case ICE:
                        uv->push_back(glm::vec2(3.f/16.f, 11.f/16.f));
                        uv->push_back(glm::vec2(4.f/16.f, 11.f/16.f));
                        uv->push_back(glm::vec2(4.f/16.f, 12.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 12.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);

                        break;
                    case LAVA:
                        uv->push_back(glm::vec2(13.f/16.f, 1.f/16.f));
                        uv->push_back(glm::vec2(14.f/16.f, 1.f/16.f));
                        uv->push_back(glm::vec2(14.f/16.f, 2.f/16.f));
                        uv->push_back(glm::vec2(13.f/16.f, 2.f/16.f));
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        break;
                    case WATER:
                        uv->push_back(glm::vec2(13.f/16.f, 3.f/16.f));
                        uv->push_back(glm::vec2(14.f/16.f, 3.f/16.f));
                        uv->push_back(glm::vec2(14.f/16.f, 4.f/16.f));
                        uv->push_back(glm::vec2(13.f/16.f, 4.f/16.f));
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        break;
                    default:
                        uv->push_back(glm::vec2(0.f/16.f, 0.f/16.f));
                        uv->push_back(glm::vec2(1.f/16.f, 0.f/16.f));
                        uv->push_back(glm::vec2(1.f/16.f, 1.f/16.f));
                        uv->push_back(glm::vec2(0.f/16.f, 1.f/16.f));
                        break;
                    }
                }
                if (botBlock == EMPTY || topBlock == WATER || topBlock == ICE) {
                    num++;
                    glm::vec4 normal(0.f, -1.f, 0.f, 0.f);
                    nor->push_back(normal);
                    nor->push_back(normal);
                    nor->push_back(normal);
                    nor->push_back(normal);

                    pos->push_back(glm::vec4(x + worldP_x, y, z + worldP_z - 1, 1.f));
                    pos->push_back(glm::vec4(x + worldP_x + 1, y, z + worldP_z - 1, 1.f));
                    pos->push_back(glm::vec4(x + worldP_x + 1, y, z + worldP_z, 1.f));
                    pos->push_back(glm::vec4(x + worldP_x, y, z + worldP_z, 1.f));

                    switch(t) {
                    case GRASS:
                        //col->push_back(glm::vec4(95.f, 159.f, 53.f, 255.f) / 255.f);
                        uv->push_back(glm::vec2(2.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 16.f/16.f));
                        uv->push_back(glm::vec2(2.f/16.f, 16.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        break;
                    case DIRT:
                        //col->push_back(glm::vec4(121.f, 85.f, 58.f, 255.f) / 255.f);
                        uv->push_back(glm::vec2(2.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 16.f/16.f));
                        uv->push_back(glm::vec2(2.f/16.f, 16.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        break;
                    case STONE:
                        //col->push_back(glm::vec4(0.5f));
                        uv->push_back(glm::vec2(1.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(2.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(2.f/16.f, 16.f/16.f));
                        uv->push_back(glm::vec2(1.f/16.f, 16.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        break;
                    case SNOW:
                        //col->push_back(glm::vec4(1.f));
                        uv->push_back(glm::vec2(2.f/16.f, 11.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 11.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 12.f/16.f));
                        uv->push_back(glm::vec2(2.f/16.f, 12.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        break;
                    case ICE:
                        uv->push_back(glm::vec2(3.f/16.f, 11.f/16.f));
                        uv->push_back(glm::vec2(4.f/16.f, 11.f/16.f));
                        uv->push_back(glm::vec2(4.f/16.f, 12.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 12.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        break;
                    case LAVA:
                        uv->push_back(glm::vec2(13.f/16.f, 1.f/16.f));
                        uv->push_back(glm::vec2(14.f/16.f, 1.f/16.f));
                        uv->push_back(glm::vec2(14.f/16.f, 2.f/16.f));
                        uv->push_back(glm::vec2(13.f/16.f, 2.f/16.f));
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        break;
                    case WATER:
                        uv->push_back(glm::vec2(13.f/16.f, 3.f/16.f));
                        uv->push_back(glm::vec2(14.f/16.f, 3.f/16.f));
                        uv->push_back(glm::vec2(14.f/16.f, 4.f/16.f));
                        uv->push_back(glm::vec2(13.f/16.f, 4.f/16.f));
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        break;
                    default:
                        uv->push_back(glm::vec2(0.f/16.f, 0.f/16.f));
                        uv->push_back(glm::vec2(1.f/16.f, 0.f/16.f));
                        uv->push_back(glm::vec2(1.f/16.f, 1.f/16.f));
                        uv->push_back(glm::vec2(0.f/16.f, 1.f/16.f));
                        break;
                    }
                }
                if (rightBlock == EMPTY || topBlock == WATER || topBlock == ICE) {
                    num++;
                    glm::vec4 normal(1.f, 0.f, 0.f, 0.f);
                    nor->push_back(normal);
                    nor->push_back(normal);
                    nor->push_back(normal);
                    nor->push_back(normal);

                    pos->push_back(glm::vec4(x + worldP_x + 1, y+1, z + worldP_z, 1.f));
                    pos->push_back(glm::vec4(x + worldP_x + 1, y, z + worldP_z, 1.f));
                    pos->push_back(glm::vec4(x + worldP_x + 1, y, z + worldP_z - 1, 1.f));
                    pos->push_back(glm::vec4(x + worldP_x + 1, y+1, z + worldP_z - 1, 1.f));

                    switch(t) {
                    case GRASS:
                        //col->push_back(glm::vec4(95.f, 159.f, 53.f, 255.f) / 255.f);
                        uv->push_back(glm::vec2(3.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(4.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(4.f/16.f, 16.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 16.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        break;
                    case DIRT:
                        //col->push_back(glm::vec4(121.f, 85.f, 58.f, 255.f) / 255.f);
                        uv->push_back(glm::vec2(2.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 16.f/16.f));
                        uv->push_back(glm::vec2(2.f/16.f, 16.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        break;
                    case STONE:
                        //col->push_back(glm::vec4(0.5f));
                        uv->push_back(glm::vec2(1.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(2.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(2.f/16.f, 16.f/16.f));
                        uv->push_back(glm::vec2(1.f/16.f, 16.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        break;
                    case SNOW:
                        //col->push_back(glm::vec4(1.f));
                        uv->push_back(glm::vec2(2.f/16.f, 11.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 11.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 12.f/16.f));
                        uv->push_back(glm::vec2(2.f/16.f, 12.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        break;
                    case ICE:
                        uv->push_back(glm::vec2(3.f/16.f, 11.f/16.f));
                        uv->push_back(glm::vec2(4.f/16.f, 11.f/16.f));
                        uv->push_back(glm::vec2(4.f/16.f, 12.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 12.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        break;
                    case LAVA:
                        uv->push_back(glm::vec2(13.f/16.f, 1.f/16.f));
                        uv->push_back(glm::vec2(14.f/16.f, 1.f/16.f));
                        uv->push_back(glm::vec2(14.f/16.f, 2.f/16.f));
                        uv->push_back(glm::vec2(13.f/16.f, 2.f/16.f));
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        break;
                    case WATER:
                        uv->push_back(glm::vec2(13.f/16.f, 3.f/16.f));
                        uv->push_back(glm::vec2(14.f/16.f, 3.f/16.f));
                        uv->push_back(glm::vec2(14.f/16.f, 4.f/16.f));
                        uv->push_back(glm::vec2(13.f/16.f, 4.f/16.f));
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        break;
                    default:
                        uv->push_back(glm::vec2(0.f/16.f, 0.f/16.f));
                        uv->push_back(glm::vec2(1.f/16.f, 0.f/16.f));
                        uv->push_back(glm::vec2(1.f/16.f, 1.f/16.f));
                        uv->push_back(glm::vec2(0.f/16.f, 1.f/16.f));
                        break;
                    }
                }
                if (leftBlock == EMPTY || topBlock == WATER || topBlock == ICE) {
                    num++;
                    glm::vec4 normal(-1.f, 0.f, 0.f, 0.f);
                    nor->push_back(normal);
                    nor->push_back(normal);
                    nor->push_back(normal);
                    nor->push_back(normal);

                    pos->push_back(glm::vec4(x + worldP_x, y, z + worldP_z, 1.f));
                    pos->push_back(glm::vec4(x + worldP_x, y+1, z + worldP_z, 1.f));
                    pos->push_back(glm::vec4(x + worldP_x, y+1, z + worldP_z -1, 1.f));
                    pos->push_back(glm::vec4(x + worldP_x, y, z + worldP_z - 1, 1.f));

                    switch(t) {
                    case GRASS:
                        //col->push_back(glm::vec4(95.f, 159.f, 53.f, 255.f) / 255.f);
                        uv->push_back(glm::vec2(3.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(4.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(4.f/16.f, 16.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 16.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        break;
                    case DIRT:
                        //col->push_back(glm::vec4(121.f, 85.f, 58.f, 255.f) / 255.f);
                        uv->push_back(glm::vec2(2.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 16.f/16.f));
                        uv->push_back(glm::vec2(2.f/16.f, 16.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        break;
                    case STONE:
                        //col->push_back(glm::vec4(0.5f));
                        uv->push_back(glm::vec2(1.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(2.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(2.f/16.f, 16.f/16.f));
                        uv->push_back(glm::vec2(1.f/16.f, 16.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        break;
                    case SNOW:
                        //col->push_back(glm::vec4(1.f));
                        uv->push_back(glm::vec2(2.f/16.f, 11.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 11.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 12.f/16.f));
                        uv->push_back(glm::vec2(2.f/16.f, 12.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        break;
                    case ICE:
                        uv->push_back(glm::vec2(3.f/16.f, 11.f/16.f));
                        uv->push_back(glm::vec2(4.f/16.f, 11.f/16.f));
                        uv->push_back(glm::vec2(4.f/16.f, 12.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 12.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        break;
                    case LAVA:
                        uv->push_back(glm::vec2(13.f/16.f, 1.f/16.f));
                        uv->push_back(glm::vec2(14.f/16.f, 1.f/16.f));
                        uv->push_back(glm::vec2(14.f/16.f, 2.f/16.f));
                        uv->push_back(glm::vec2(13.f/16.f, 2.f/16.f));
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        break;
                    case WATER:
                        uv->push_back(glm::vec2(13.f/16.f, 3.f/16.f));
                        uv->push_back(glm::vec2(14.f/16.f, 3.f/16.f));
                        uv->push_back(glm::vec2(14.f/16.f, 4.f/16.f));
                        uv->push_back(glm::vec2(13.f/16.f, 4.f/16.f));
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        break;
                    default:
                        uv->push_back(glm::vec2(0.f/16.f, 0.f/16.f));
                        uv->push_back(glm::vec2(1.f/16.f, 0.f/16.f));
                        uv->push_back(glm::vec2(1.f/16.f, 1.f/16.f));
                        uv->push_back(glm::vec2(0.f/16.f, 1.f/16.f));
                        break;
                    }
                }
                if (frontBlock == EMPTY || topBlock == WATER || topBlock == ICE) {
                    num++;
                    glm::vec4 normal(0.f, 0.f, 1.f, 0.f);
                    nor->push_back(normal);
                    nor->push_back(normal);
                    nor->push_back(normal);
                    nor->push_back(normal);

                    pos->push_back(glm::vec4(x + worldP_x, y, z + worldP_z, 1.f));
                    pos->push_back(glm::vec4(x + worldP_x + 1, y, z + worldP_z, 1.f));
                    pos->push_back(glm::vec4(x + worldP_x + 1, y+1, z + worldP_z, 1.f));
                    pos->push_back(glm::vec4(x + worldP_x, y+1, z + worldP_z, 1.f));

                    switch(t) {
                    case GRASS:
                        //col->push_back(glm::vec4(95.f, 159.f, 53.f, 255.f) / 255.f);
                        uv->push_back(glm::vec2(3.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(4.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(4.f/16.f, 16.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 16.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        break;
                    case DIRT:
                        //col->push_back(glm::vec4(121.f, 85.f, 58.f, 255.f) / 255.f);
                        uv->push_back(glm::vec2(2.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 16.f/16.f));
                        uv->push_back(glm::vec2(2.f/16.f, 16.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        break;
                    case STONE:
                        //col->push_back(glm::vec4(0.5f));
                        uv->push_back(glm::vec2(1.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(2.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(2.f/16.f, 16.f/16.f));
                        uv->push_back(glm::vec2(1.f/16.f, 16.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        break;
                    case SNOW:
                        //col->push_back(glm::vec4(1.f));
                        uv->push_back(glm::vec2(2.f/16.f, 11.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 11.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 12.f/16.f));
                        uv->push_back(glm::vec2(2.f/16.f, 12.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        break;
                    case ICE:
                        uv->push_back(glm::vec2(3.f/16.f, 11.f/16.f));
                        uv->push_back(glm::vec2(4.f/16.f, 11.f/16.f));
                        uv->push_back(glm::vec2(4.f/16.f, 12.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 12.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        break;
                    case LAVA:
                        uv->push_back(glm::vec2(13.f/16.f, 1.f/16.f));
                        uv->push_back(glm::vec2(14.f/16.f, 1.f/16.f));
                        uv->push_back(glm::vec2(14.f/16.f, 2.f/16.f));
                        uv->push_back(glm::vec2(13.f/16.f, 2.f/16.f));
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        break;
                    case WATER:
                        uv->push_back(glm::vec2(13.f/16.f, 3.f/16.f));
                        uv->push_back(glm::vec2(14.f/16.f, 3.f/16.f));
                        uv->push_back(glm::vec2(14.f/16.f, 4.f/16.f));
                        uv->push_back(glm::vec2(13.f/16.f, 4.f/16.f));
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        break;
                    default:
                        uv->push_back(glm::vec2(0.f/16.f, 0.f/16.f));
                        uv->push_back(glm::vec2(1.f/16.f, 0.f/16.f));
                        uv->push_back(glm::vec2(1.f/16.f, 1.f/16.f));
                        uv->push_back(glm::vec2(0.f/16.f, 1.f/16.f));
                        break;
                    }
                }
                if (backBlock == EMPTY || topBlock == WATER || topBlock == ICE) {
                    num++;
                    glm::vec4 normal(0.f, 0.f, -1.f, 0.f);
                    nor->push_back(normal);
                    nor->push_back(normal);
                    nor->push_back(normal);
                    nor->push_back(normal);

                    pos->push_back(glm::vec4(x + worldP_x, y+1, z + worldP_z - 1, 1.f));
                    pos->push_back(glm::vec4(x + worldP_x + 1, y+1, z + worldP_z - 1, 1.f));
                    pos->push_back(glm::vec4(x + worldP_x + 1, y, z + worldP_z - 1, 1.f));
                    pos->push_back(glm::vec4(x + worldP_x, y, z + worldP_z - 1, 1.f));

                    switch(t) {
                    case GRASS:
                        //col->push_back(glm::vec4(95.f, 159.f, 53.f, 255.f) / 255.f);
                        uv->push_back(glm::vec2(3.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(4.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(4.f/16.f, 16.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 16.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        break;
                    case DIRT:
                        //col->push_back(glm::vec4(121.f, 85.f, 58.f, 255.f) / 255.f);
                        uv->push_back(glm::vec2(2.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 16.f/16.f));
                        uv->push_back(glm::vec2(2.f/16.f, 16.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        break;
                    case STONE:
                        //col->push_back(glm::vec4(0.5f));
                        uv->push_back(glm::vec2(1.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(2.f/16.f, 15.f/16.f));
                        uv->push_back(glm::vec2(2.f/16.f, 16.f/16.f));
                        uv->push_back(glm::vec2(1.f/16.f, 16.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        break;
                    case SNOW:
                        //col->push_back(glm::vec4(1.f));
                        uv->push_back(glm::vec2(2.f/16.f, 11.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 11.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 12.f/16.f));
                        uv->push_back(glm::vec2(2.f/16.f, 12.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        break;
                    case ICE:
                        uv->push_back(glm::vec2(3.f/16.f, 11.f/16.f));
                        uv->push_back(glm::vec2(4.f/16.f, 11.f/16.f));
                        uv->push_back(glm::vec2(4.f/16.f, 12.f/16.f));
                        uv->push_back(glm::vec2(3.f/16.f, 12.f/16.f));
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        anim->push_back(0.f);
                        break;
                    case LAVA:
                        uv->push_back(glm::vec2(13.f/16.f, 1.f/16.f));
                        uv->push_back(glm::vec2(14.f/16.f, 1.f/16.f));
                        uv->push_back(glm::vec2(14.f/16.f, 2.f/16.f));
                        uv->push_back(glm::vec2(13.f/16.f, 2.f/16.f));
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        break;
                    case WATER:
                        uv->push_back(glm::vec2(13.f/16.f, 3.f/16.f));
                        uv->push_back(glm::vec2(14.f/16.f, 3.f/16.f));
                        uv->push_back(glm::vec2(14.f/16.f, 4.f/16.f));
                        uv->push_back(glm::vec2(13.f/16.f, 4.f/16.f));
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        anim->push_back(1.f);
                        break;
                    default:
                        uv->push_back(glm::vec2(0.f/16.f, 0.f/16.f));
                        uv->push_back(glm::vec2(1.f/16.f, 0.f/16.f));
                        uv->push_back(glm::vec2(1.f/16.f, 1.f/16.f));
                        uv->push_back(glm::vec2(0.f/16.f, 1.f/16.f));
                        break;
                    }
                }

                //push back indices
                if (t == WATER || t == ICE) {
                    for (int i = 0; i < num; i++) {
                        idxTran.push_back(num_count_Tran);
                        idxTran.push_back(num_count_Tran + 1);
                        idxTran.push_back(num_count_Tran + 2);
                        idxTran.push_back(num_count_Tran);
                        idxTran.push_back(num_count_Tran + 2);
                        idxTran.push_back(num_count_Tran + 3);

                        num_count_Tran = num_count_Tran + 4;
                    }
                } else {
                    for (int i = 0; i < num; i++) {
                        idxOpq.push_back(num_count_Opq);
                        idxOpq.push_back(num_count_Opq + 1);
                        idxOpq.push_back(num_count_Opq + 2);
                        idxOpq.push_back(num_count_Opq);
                        idxOpq.push_back(num_count_Opq + 2);
                        idxOpq.push_back(num_count_Opq + 3);

                        num_count_Opq = num_count_Opq + 4;
                    }

                }
            }
        }
    }
    m_count_opq = idxOpq.size();
    m_count_tran = idxTran.size();

    //interleave Opaque
    for (int i = 0; i < posOpq.size(); ++i) {
        allOpq.push_back(posOpq.at(i)[0]);
        allOpq.push_back(posOpq.at(i)[1]);
        allOpq.push_back(posOpq.at(i)[2]);
        allOpq.push_back(posOpq.at(i)[3]);
        allOpq.push_back(norOpq.at(i)[0]);
        allOpq.push_back(norOpq.at(i)[1]);
        allOpq.push_back(norOpq.at(i)[2]);
        allOpq.push_back(norOpq.at(i)[3]);
        allOpq.push_back(uvOpq.at(i)[0]);
        allOpq.push_back(uvOpq.at(i)[1]);
        allOpq.push_back(animOpq.at(i));
    }

    //interleave Transparent
    for (int i = 0; i < posTran.size(); ++i) {
        allTran.push_back(posTran.at(i)[0]);
        allTran.push_back(posTran.at(i)[1]);
        allTran.push_back(posTran.at(i)[2]);
        allTran.push_back(posTran.at(i)[3]);
        allTran.push_back(norTran.at(i)[0]);
        allTran.push_back(norTran.at(i)[1]);
        allTran.push_back(norTran.at(i)[2]);
        allTran.push_back(norTran.at(i)[3]);
        allTran.push_back(uvTran.at(i)[0]);
        allTran.push_back(uvTran.at(i)[1]);
        allTran.push_back(animTran.at(i));
    }

    generateIdxOpq();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIdxOpq);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxOpq.size() * sizeof(GLuint), idxOpq.data(), GL_STATIC_DRAW);

    generateIdxTran();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIdxTran);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxTran.size() * sizeof(GLuint), idxTran.data(), GL_STATIC_DRAW);

    generateAllOpaque();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufAllOpaque);
    mp_context->glBufferData(GL_ARRAY_BUFFER, allOpq.size() * sizeof(float), allOpq.data(), GL_STATIC_DRAW);

    generatedAllTransparent();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufAllTransparent);
    mp_context->glBufferData(GL_ARRAY_BUFFER, allTran.size() * sizeof(float), allTran.data(), GL_STATIC_DRAW);

}

GLenum Chunk::drawMode() {
    return GL_TRIANGLES;
}

void Chunk::setWorldPos(int x, int z) {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    worldP_x = 16 * xFloor;
    worldP_z = 16 * zFloor;
}
// Does bounds checking with at()
BlockType Chunk::getBlockAt(unsigned int x, unsigned int y, unsigned int z) const {
    return m_blocks.at(x + 16 * y + 16 * 256 * z);
}

// Exists to get rid of compiler warnings about int -> unsigned int implicit conversion
BlockType Chunk::getBlockAt(int x, int y, int z) const {
    return getBlockAt(static_cast<unsigned int>(x), static_cast<unsigned int>(y), static_cast<unsigned int>(z));
}

// Does bounds checking with at()
void Chunk::setBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t) {
    m_blocks.at(x + 16 * y + 16 * 256 * z) = t;
}

