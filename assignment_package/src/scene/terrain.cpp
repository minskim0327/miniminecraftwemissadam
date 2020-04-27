#include "terrain.h"
#include "cube.h"
#include <stdexcept>
#include <iostream>
#include "river.h"

Terrain::Terrain(OpenGLContext *context)
    : m_chunks(), m_generatedTerrain(), mp_context(context)
{}

Terrain::~Terrain() {
    //m_geomCube.destroy();
}

// Combine two 32-bit ints into one 64-bit int
// where the upper 32 bits are X and the lower 32 bits are Z
int64_t toKey(int x, int z) {
    int64_t xz = 0xffffffffffffffff;
    int64_t x64 = x;
    int64_t z64 = z;

    // Set all lower 32 bits to 1 so we can & with Z later
    xz = (xz & (x64 << 32)) | 0x00000000ffffffff;

    // Set all upper 32 bits to 1 so we can & with XZ
    z64 = z64 | 0xffffffff00000000;

    // Combine
    xz = xz & z64;
    return xz;
}

glm::ivec2 toCoords(int64_t k) {
    // Z is lower 32 bits
    int64_t z = k & 0x00000000ffffffff;
    // If the most significant bit of Z is 1, then it's a negative number
    // so we have to set all the upper 32 bits to 1.
    // Note the 8    V
    if(z & 0x0000000080000000) {
        z = z | 0xffffffff00000000;
    }
    int64_t x = (k >> 32);

    return glm::ivec2(x, z);
}

// Surround calls to this with try-catch if you don't know whether
// the coordinates at x, y, z have a corresponding Chunk
BlockType Terrain::getBlockAt(int x, int y, int z) const
{
    if(hasChunkAt(x, z)) {
        // Just disallow action below or above min/max height,
        // but don't crash the game over it.
        if(y < 0 || y >= 256) {
            return EMPTY;
        }
        const uPtr<Chunk> &c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        return c->getBlockAt(static_cast<unsigned int>(x - chunkOrigin.x),
                             static_cast<unsigned int>(y),
                             static_cast<unsigned int>(z - chunkOrigin.y));
    }
    else {
        throw std::out_of_range("Coordinates " + std::to_string(x) +
                                " " + std::to_string(y) + " " +
                                std::to_string(z) + " have no Chunk!");
    }
}

BlockType Terrain::getBlockAt(glm::vec3 p) const {
    return getBlockAt(p.x, p.y, p.z);
}

bool Terrain::hasChunkAt(int x, int z) const {
    // Map x and z to their nearest Chunk corner
    // By flooring x and z, then multiplying by 16,
    // we clamp (x, z) to its nearest Chunk-space corner,
    // then scale back to a world-space location.
    // Note that floor() lets us handle negative numbers
    // correctly, as floor(-1 / 16.f) gives us -1, as
    // opposed to (int)(-1 / 16.f) giving us 0 (incorrect!).
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks.find(toKey(16 * xFloor, 16 * zFloor)) != m_chunks.end();
}


uPtr<Chunk>& Terrain::getChunkAt(int x, int z) {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
//    return m_chunks[toKey(16 * xFloor, 16 * zFloor)];
    int64_t key = toKey(16 * xFloor, 16 * zFloor);
    uPtr<Chunk> &ref = m_chunks[key];
    if(ref == nullptr) {
        std::cout << "Got null at " << xFloor << ", " << zFloor << std::endl;
    }
    return ref;
}


const uPtr<Chunk>& Terrain::getChunkAt(int x, int z) const {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    const uPtr<Chunk> &ref = m_chunks.at(toKey(16 * xFloor, 16 * zFloor));
    if(ref == nullptr) {
        std::cout << "Got null at " << xFloor << ", " << zFloor << std::endl;
    }
    return ref;
}

void Terrain::setBlockAt(int x, int y, int z, BlockType t)
{
    if(hasChunkAt(x, z)) {
        uPtr<Chunk> &c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        c->setBlockAt(static_cast<unsigned int>(x - chunkOrigin.x),
                      static_cast<unsigned int>(y),
                      static_cast<unsigned int>(z - chunkOrigin.y),
                      t);
//        c->destroy();
//        c->create();
    }
    else {
//        std::cout<<"has no chunk when called setBlockAt" << std::endl;
        hasChunkAt(x, z);
        throw std::out_of_range("Coordinates " + std::to_string(floor(x / 16.f) * 16) +
                                " " + std::to_string(y) + " " +
                                std::to_string(floor(z / 16.f) * 16) + " have no Chunk!");
    }
}


Chunk* Terrain::createChunkAt(int x, int z) {
//    std::cout << "Initializing chunk at " << x << ", " << z << std::endl;
    uPtr<Chunk> chunk = mkU<Chunk>(mp_context);
    Chunk *cPtr = chunk.get();
    chunk->setWorldPos(x, z);

//   std::cout << x << ", " << z << std::endl;
    m_chunks[toKey(x, z)] = std::move(chunk);
    // Set the neighbor pointers of itself and its neighbors
    if(hasChunkAt(x, z + 16)) {
        auto &chunkNorth = m_chunks[toKey(x, z + 16)];
        cPtr->linkNeighbor(chunkNorth, ZPOS);
    }
    if(hasChunkAt(x, z - 16)) {
        auto &chunkSouth = m_chunks[toKey(x, z - 16)];
        cPtr->linkNeighbor(chunkSouth, ZNEG);
    }
    if(hasChunkAt(x + 16, z)) {
        auto &chunkEast = m_chunks[toKey(x + 16, z)];
        cPtr->linkNeighbor(chunkEast, XPOS);
    }
    if(hasChunkAt(x - 16, z)) {
        auto &chunkWest = m_chunks[toKey(x - 16, z)];
        cPtr->linkNeighbor(chunkWest, XNEG);
    }
    return cPtr;
}



//depending on the player's new position, decide whether a new chunk
//should be added to the terrain(Elaine 1st)
void Terrain::updateScene(glm::vec3 pos, ShaderProgram *shaderProgram) {
    int xFloor = static_cast<int>(glm::floor(pos.x / 16.f));
    int zFloor = static_cast<int>(glm::floor(pos.z / 16.f));
    int x = 16 * xFloor;
    int z = 16 * zFloor; //x and z represent the chunk's position (lower left corner)

    Chunk* forward = nullptr;
    Chunk* backward  = nullptr;
    Chunk* right  = nullptr;
    Chunk* left  = nullptr;
    River river = River(this, (int) glm::floor(pos.x / 64.f), (int) glm::floor(pos.z / 64.f));
    int xpos = (int) glm::floor(pos.x / 64.f);
    int zpos = (int) glm::floor(pos.z / 64.f);
    if (!hasChunkAt(x, z + 16)) {
        forward = createChunkAt(x, z + 16);
    }
    if (!hasChunkAt(x, z - 16)) {
        backward = createChunkAt(x, z - 16);
    }
    if (!hasChunkAt(x + 16, z)) {
        right = createChunkAt(x + 16, z);
    }
    if (!hasChunkAt(x - 16, z)) {
        left = createChunkAt(x - 16, z);
    }

    if (forward != nullptr) {
        for(int i = x; i < x + 16; ++i) {
            for(int j = z + 16; j < z + 32; ++j) {
//                if((i + j) % 2 == 0) {
//                    setBlockAt(i, 128, j, STONE);
//                }
//                else {
//                    setBlockAt(i, 128, j, DIRT);
//                }
                fillBlock(i, j);
                if (xpos * zpos % 2 == 0)
                    river.draw();
            }
        }
        forward->setWorldPos(x, z +16);
        forward->create();
    }
    if (backward != nullptr) {
        for(int i = x; i < x + 16; ++i) {
            for(int j = z -16; j < z; ++j) {
//                if((i + j) % 2 == 0) {
//                    setBlockAt(i, 128, j, STONE);
//                }
//                else {
//                    setBlockAt(i, 128, j, DIRT);
//                }
                fillBlock(i, j);
                if (xpos * zpos % 2 == 0)
                    river.draw();
            }
        }
        backward->setWorldPos(x, z -16);
        backward->create();
    }
    if (right != nullptr) {
        for(int i = x +16; i < x + 32; ++i) {
            for(int j = z; j < z + 16; ++j) {
                fillBlock(i, j);
                if (xpos * zpos % 2 == 0)
                    river.draw();
            }
        }
        right->setWorldPos(x +16, z);
        right->create();
    }
    if (left != nullptr) {
        for(int i = x -16; i < x; ++i) {
            for(int j = z; j < z + 16; ++j) {
                fillBlock(i, j);
                if (xpos * zpos % 2 == 0)
                    river.draw();
            }
        }
        left->setWorldPos(x -16, z);
        left->create();
    }

}

void Terrain::setTime(int t) {
    time = t;
}


// TODO: When you make Chunk inherit from Drawable, change this code so
// it draws each Chunk with the given ShaderProgram, remembering to set the
// model matrix to the proper X and Z translation!(Elaine 1st)
void Terrain::draw(int minX, int maxX, int minZ, int maxZ, ShaderProgram *shaderProgram) {
    for(int x = minX; x < maxX; x += 16) {
        for(int z = minZ; z < maxZ; z += 16) {
            if (hasChunkAt(x, z)) {
                const uPtr<Chunk> &chunk = getChunkAt(x, z);
//<<<<<<< HEAD

                chunk->setWorldPos(x, z);
                shaderProgram->setModelMatrix(glm::mat4());
                shaderProgram->drawInterleaved(*chunk, 0, 0, time);

            }
        }
    }
    for(int x = minX; x < maxX; x += 16) {
        for(int z = minZ; z < maxZ; z += 16) {
            if (hasChunkAt(x, z)) {
                const uPtr<Chunk> &chunk = getChunkAt(x, z);

                chunk->setWorldPos(x, z);
                shaderProgram->setModelMatrix(glm::mat4());
                shaderProgram->drawInterleaved(*chunk, 0, 1, time);
//=======
//                if(chunk->elemCount() != -1) {
//                    shaderProgram->setModelMatrix(glm::mat4());
//                    shaderProgram->drawInterleaved(*chunk);
//                }
////>>>>>>> minskimMS2
            }
        }
    }
}

//calls chunk.create() to make vbo data (Elaine 1st)
void Terrain::createChunks(int minx, int maxx, int minz, int maxz) {
    for(int x = minx; x < maxx; x += 16) {
        for(int z = minz; z < maxz; z += 16) {
            const uPtr<Chunk> &chunk = getChunkAt(x, z);
            chunk->destroy();
            chunk->setWorldPos(x, z);
            chunk->create();
        }
    }
}

void Terrain::CreateTestScene()
{
    // Create the Chunks that will
    // store the blocks for our
    // initial world space
    for(int x = 0; x < 64; x += 16) {
        for(int z = 0; z < 64; z += 16) {
            createChunkAt(x, z);
        }
    }
    // Tell our existing terrain set that
    // the "generated terrain zone" at (0,0)
    // now exists.
    m_generatedTerrain.insert(toKey(0, 0));


    // Create the basic terrain floor
    for(int x = 0; x < 64; ++x) {
        for(int z = 0; z < 64; ++z) {
            fillBlock(x, z);
        }
    }
    River river = River(this, 0, 0);
   // river.draw();

    //create chunk vbo data (Elaine 1st)
    createChunks(0, 64, 0, 64);


}

void Terrain::fillBlock(int x, int z) {
    int mheight = getMountainHeight(x, z);
    int gheight = getGrasslandHeight(x, z);
    float t = perlinNoise(glm::vec2(x / 256.f, z / 256.f));
    float remapped = remap(t, -1, 1, 0, 1);

    remapped = glm::smoothstep(0.4, 0.6, (double) remapped);

    int lerp = int((1 - remapped) * gheight + remapped * mheight);

    if (remapped < 0.5) {
        for (int y = 0; y < lerp; ++y) {
            if (y == lerp - 1) {
                setBlockAt(x, y, z, GRASS);
            } else if (y <= 128) {
                setBlockAt(x, y, z, STONE);
            } else {
                setBlockAt(x, y, z, DIRT);
            }
        }
    } else { // mountain
        for (int y = 0; y < lerp; ++y) {
            if (y == lerp - 1 && lerp > 200) {
                setBlockAt(x, y, z, SNOW);
            } else if (y == lerp - 1) {
                setBlockAt(x, y, z, STONE);
            } else if (y <= 128) {
                setBlockAt(x, y, z, STONE);
            } else {
                setBlockAt(x, y, z, STONE);
            }
        }
    }
}

float Terrain::perlinNoise(glm::vec2 uv) {
    float surfletSum = 0.f;
    for (int x = 0; x <= 1; ++x) {
        for (int y = 0; y <=1; ++y) {
            float sf = surflet(uv, glm::floor(uv) + glm::vec2(x, y));
            surfletSum += sf;
        }
    }
    return surfletSum;
}


glm::vec2 random2(glm::vec2 p) {
    return glm::fract(glm::sin(glm::vec2(glm::dot(p, glm::vec2(127.1, 311.7)),
                          glm::dot(p, glm::vec2(269.5, 183.3)))) * 43758.5453f);
}

float Terrain::remap(float val, float from1, float to1, float from2, float to2) {
    return (val - from1) / (to1 - from1) * (to2 - from2) + from2;
}

float Terrain::surflet(glm::vec2 p, glm::vec2 gridPoint) {
    glm::vec2 t2 = glm::abs(p - gridPoint);
    glm::vec2 t = glm::vec2(1.f) - (6.f * glm::pow(t2, glm::vec2(5.f)) - 15.f * glm::pow(t2, glm::vec2(4.f)) +
            10.f * glm::pow(t2, glm::vec2(3.f)));
    glm::vec2 rand = random2(gridPoint);
    rand[0] = remap(rand[0], 0, 1, -1, 1);
    rand[1] = remap(rand[1], 0, 1, -1, 1);
    glm::vec2 gradient = random2(gridPoint) * 2.f - glm::vec2(1, 1);
    glm::vec2 diff = p - gridPoint;
    float height = glm::dot(diff, gradient);
    return height * t[0] * t[1];

}


int Terrain::getGrasslandHeight(int x, int z) {
    float worley = worleyNoise(glm::vec2(x / 64.f, z / 64.f));
    return 129 + (worley) * 127 / 2 + 5;
}

int Terrain::getMountainHeight(int x, int z) {
    float perlin = perlinNoise(glm::vec2(x / 32.f, z / 32.f));
    perlin = remap(perlin, -1, 1, 0, 1);

    perlin = glm::smoothstep(0.25, 0.75, (double) perlin);
    perlin = pow(perlin, 2);
    int height = perlin * (127) + 129;
    return height;

}

float Terrain::worleyNoise(glm::vec2 uv) {
    uv *= 2;
    glm::vec2 uvInt = glm::floor(uv);
    glm::vec2 uvFract = glm::fract(uv);
    float minDist = 1;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            glm::vec2 neighbor = glm::vec2(1.0f * i, 1.0f * j);
            glm::vec2 point = random2(uvInt + neighbor);
            glm::vec2 diff = neighbor + point - uvFract;
            float dist = glm::length(diff);
            minDist = glm::min(minDist, dist);
        }
    }
    float perlin = perlinNoise(uv);
    minDist = abs(perlin) * minDist;
    return minDist;
}

float Terrain::noise1D(int x) {
    x = (x << 13) ^ x;
    return ((int) 1.0 - (x * (x * x * 15731 + 789221)
                    + 1376312589) & 0x7fffffff) / 10737741824.0;
}

float Terrain::fbm(float x) {
    float total = 0.f;
    float persistence = 0.7;
    int octaves = 8;
    for (int i = 0; i < octaves; i++) {
        float freq = pow(2.f, i);
        float amp = pow(persistence, i);
        total += interpNoise1D(x * freq) * amp;
    }

    
    return total;
}

float Terrain::interpNoise1D(float x) {
    float intx = glm::floor(x);
    float fractx = glm::fract(x);
    float v1 = noise1D(intx);
    float v2 = noise1D(intx+1);
    return glm::mix(v1, v2, fractx);

}


/***
 * MileStone 2
 */


// Examines 5 by 5 terrain zone from the current player position
// Returns relative positions of terrains that need to be created
std::vector<int64_t> Terrain::checkExpansion(glm::vec3 position) {
//    std::cout << position.x << ", "
//              <<position.y << ", "
//             <<position.z << endl;
//    std::cout << "<,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,," << std::endl;
    std::vector<int64_t> output;
    int lowerLeftX = glm::floor(position.x / 64.0f);
    int lowerLeftZ = glm::floor(position.z / 64.0f);


    // Check Current
    for (int r = -2; r <= 2; r++) {
        for (int c = -2; c <= 2; c++) {
            int64_t currTerrain = toKey((lowerLeftX + c) * 64, (lowerLeftZ + r) * 64);
//            std::cout << (lowerLeftX + c) * 64 << ", " <<
//                          (lowerLeftZ + r) * 64 << std::endl;
            if (m_generatedTerrain.find(currTerrain) == m_generatedTerrain.end()) {
                m_generatedTerrain.insert(currTerrain);
                output.push_back(currTerrain);
            }
        }
    }

    return output;
}




