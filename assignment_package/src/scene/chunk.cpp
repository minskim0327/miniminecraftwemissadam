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

    int num_count = 0; //num increased by 4 every time for index vbo
    std::vector<glm::vec4> pos;
    std::vector<glm::vec4> col;
    std::vector<glm::vec4> nor;
    std::vector<glm::vec4> all; //interleaved
    std::vector<GLuint> idx;

    for (int x = 0; x < 16; ++x) {
        for (int y = 0; y < 256; ++y) {
            for (int z = 0; z < 16; ++z) {

                int num = 0; //num of faces in each block that we want to save data
                BlockType t = getBlockAt(x, y, z);
                if (t == EMPTY) {
                    continue;
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

                if (topBlock == EMPTY) {
                    num++;
                    glm::vec4 normal(0.f, 1.f, 0.f, 0.f);
                    nor.push_back(normal);
                    nor.push_back(normal);
                    nor.push_back(normal);
                    nor.push_back(normal);

                    pos.push_back(glm::vec4(x + worldP_x, y+1, z + worldP_z, 1.f));
                    pos.push_back(glm::vec4(x + worldP_x + 1, y+1, z + worldP_z, 1.f));
                    pos.push_back(glm::vec4(x + worldP_x + 1, y+1, z + worldP_z -1, 1.f));
                    pos.push_back(glm::vec4(x + worldP_x, y+1, z + worldP_z-1, 1.f));
                }
                if (botBlock == EMPTY) {
                    num++;
                    glm::vec4 normal(0.f, -1.f, 0.f, 0.f);
                    nor.push_back(normal);
                    nor.push_back(normal);
                    nor.push_back(normal);
                    nor.push_back(normal);

                    pos.push_back(glm::vec4(x + worldP_x, y, z + worldP_z - 1, 1.f));
                    pos.push_back(glm::vec4(x + worldP_x + 1, y, z + worldP_z - 1, 1.f));
                    pos.push_back(glm::vec4(x + worldP_x + 1, y, z + worldP_z, 1.f));
                    pos.push_back(glm::vec4(x + worldP_x, y, z + worldP_z, 1.f));
                }
                if (rightBlock == EMPTY) {
                    num++;
                    glm::vec4 normal(1.f, 0.f, 0.f, 0.f);
                    nor.push_back(normal);
                    nor.push_back(normal);
                    nor.push_back(normal);
                    nor.push_back(normal);

                    pos.push_back(glm::vec4(x + worldP_x + 1, y+1, z + worldP_z, 1.f));
                    pos.push_back(glm::vec4(x + worldP_x + 1, y, z + worldP_z, 1.f));
                    pos.push_back(glm::vec4(x + worldP_x + 1, y, z + worldP_z - 1, 1.f));
                    pos.push_back(glm::vec4(x + worldP_x + 1, y+1, z + worldP_z - 1, 1.f));
                }
                if (leftBlock == EMPTY) {
                    num++;
                    glm::vec4 normal(-1.f, 0.f, 0.f, 0.f);
                    nor.push_back(normal);
                    nor.push_back(normal);
                    nor.push_back(normal);
                    nor.push_back(normal);

                    pos.push_back(glm::vec4(x + worldP_x, y, z + worldP_z, 1.f));
                    pos.push_back(glm::vec4(x + worldP_x, y+1, z + worldP_z, 1.f));
                    pos.push_back(glm::vec4(x + worldP_x, y+1, z + worldP_z -1, 1.f));
                    pos.push_back(glm::vec4(x + worldP_x, y, z + worldP_z - 1, 1.f));
                }
                if (frontBlock == EMPTY) {
                    num++;
                    glm::vec4 normal(0.f, 0.f, 1.f, 0.f);
                    nor.push_back(normal);
                    nor.push_back(normal);
                    nor.push_back(normal);
                    nor.push_back(normal);

                    pos.push_back(glm::vec4(x + worldP_x, y, z + worldP_z, 1.f));
                    pos.push_back(glm::vec4(x + worldP_x + 1, y, z + worldP_z, 1.f));
                    pos.push_back(glm::vec4(x + worldP_x + 1, y+1, z + worldP_z, 1.f));
                    pos.push_back(glm::vec4(x + worldP_x, y+1, z + worldP_z, 1.f));
                }
                if (backBlock == EMPTY) {
                    num++;
                    glm::vec4 normal(0.f, 0.f, -1.f, 0.f);
                    nor.push_back(normal);
                    nor.push_back(normal);
                    nor.push_back(normal);
                    nor.push_back(normal);

                    pos.push_back(glm::vec4(x + worldP_x, y+1, z + worldP_z - 1, 1.f));
                    pos.push_back(glm::vec4(x + worldP_x + 1, y+1, z + worldP_z - 1, 1.f));
                    pos.push_back(glm::vec4(x + worldP_x + 1, y, z + worldP_z - 1, 1.f));
                    pos.push_back(glm::vec4(x + worldP_x, y, z + worldP_z - 1, 1.f));
                }

                for (int i = 0; i < num * 4; ++i) {
                    switch(t) {
                    case GRASS:
                        col.push_back(glm::vec4(95.f, 159.f, 53.f, 255.f) / 255.f);
                        break;
                    case DIRT:
                        col.push_back(glm::vec4(121.f, 85.f, 58.f, 255.f) / 255.f);
                        break;
                    case STONE:
                        col.push_back(glm::vec4(0.5f));
                        break;
                    case SNOW:
                        col.push_back(glm::vec4(1.f));
                        break;
                    case WATER:
                        col.push_back(glm::vec4(0.f, 0.f, 255.f, 255.f) / 255.f);
                        break;
                    default:
                        // Other block types are not yet handled, so we default to black
                        col.push_back(glm::vec4(0.f));
                        break;
                    }
                }

                //int num_count = 0;
                for (int i = 0; i < num; i++) {
                    idx.push_back(num_count);
                    idx.push_back(num_count + 1);
                    idx.push_back(num_count + 2);
                    idx.push_back(num_count);
                    idx.push_back(num_count + 2);
                    idx.push_back(num_count + 3);

                    num_count = num_count + 4;
                }
            }
        }
    }
    m_count = idx.size();

    //interleave
    for (int i = 0; i < pos.size(); ++i) {
        all.push_back(pos.at(i));
        all.push_back(nor.at(i));
        all.push_back(col.at(i));
    }

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

//    generatePos();
//    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufPos);
//    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

//    generateNor();
//    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufNor);
//    mp_context->glBufferData(GL_ARRAY_BUFFER, nor.size() * sizeof(glm::vec4), nor.data(), GL_STATIC_DRAW);

//    generateCol();
//    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufCol);
//    mp_context->glBufferData(GL_ARRAY_BUFFER, col.size() * sizeof(glm::vec4), col.data(), GL_STATIC_DRAW);

    generateAll();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufAll);
    mp_context->glBufferData(GL_ARRAY_BUFFER, all.size() * sizeof(glm::vec4), all.data(), GL_STATIC_DRAW);
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

