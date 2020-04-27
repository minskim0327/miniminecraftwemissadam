#pragma once
#include "src/smartpointerhelp.h"
#include "src/glm_includes.h"
#include <array>
#include <unordered_map>
#include <cstddef>
#include "src/drawable.h"


//using namespace std;

// C++ 11 allows us to define the size of an enum. This lets us use only one byte
// of memory to store our different block types. By default, the size of a C++ enum
// is that of an int (so, usually four bytes). This *does* limit us to only 256 different
// block types, but in the scope of this project we'll never get anywhere near that many.
enum BlockType : unsigned char
{
    EMPTY, GRASS, DIRT, STONE, SNOW, LAVA, WATER, ICE
};

// The six cardinal directions in 3D space
enum Direction : unsigned char
{
    XPOS, XNEG, YPOS, YNEG, ZPOS, ZNEG
};

// Lets us use any enum class as the key of a
// std::unordered_map
struct EnumHash {
    template <typename T>
    size_t operator()(T t) const {
        return static_cast<size_t>(t);
    }
};

// One Chunk is a 16 x 256 x 16 section of the world,
// containing all the Minecraft blocks in that area.
// We divide the world into Chunks in order to make
// recomputing its VBO data faster by not having to
// render all the world at once, while also not having
// to render the world block by block.

// TODO have Chunk inherit from Drawable
class Chunk : public Drawable {
private:
    // All of the blocks contained within this Chunk
    std::array<BlockType, 65536> m_blocks;
    // This Chunk's four neighbors to the north, south, east, and west
    // The third input to this map just lets us use a Direction as
    // a key for this map.
    // These allow us to properly determine
    std::unordered_map<Direction, Chunk*, EnumHash> m_neighbors;

    int worldP_x;
    int worldP_z;

public:
    //Chunk();
    Chunk(OpenGLContext*);
    void virtual create();
//    void createVBO(std::vector<glm::vec4>* pos,
//                   std::vector<glm::vec4>* col,
//                   std::vector<glm::vec4>* nor,
//                   std::vector<GLuint>* idx);
    void createVBO(
            std::vector<glm::vec4>* posOpq,
            std::vector<glm::vec4>* norOpq,
            std::vector<glm::vec2>* uvOpq,
            std::vector<float> *animOpq,
            std::vector<GLuint>* idxOpq,
            std::vector<glm::vec4> *posTran,
            std::vector<glm::vec4> *norTran,
            std::vector<glm::vec2> *uvTran,
            std::vector<float> *animTran,
            std::vector<GLuint>* idxTran);
    void sendToGPU(std::vector<float>* allOpq,
                   std::vector<GLuint>* idxOpq,
                   std::vector<float>* allTran,
                   std::vector<GLuint>* idxTran);
    virtual ~Chunk();
    GLenum virtual drawMode();
    void setWorldPos(int x, int z);

    void linkNeighbor(uPtr<Chunk>& neighbor, Direction dir);
    BlockType getBlockAt(unsigned int x, unsigned int y, unsigned int z) const;
    BlockType getBlockAt(int x, int y, int z) const;
    void setBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t);
};
