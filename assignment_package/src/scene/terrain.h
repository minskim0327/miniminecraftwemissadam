#pragma once
#include "src/smartpointerhelp.h"
#include "src/glm_includes.h"
#include "chunk.h"
#include <array>
#include <unordered_map>
#include <unordered_set>
#include "src/shaderprogram.h"
#include "cube.h"
#include "river.h"
#include "QMutex"
#include "cave.h"
class River;
class Cave;

//using namespace std;
using namespace std;

// Helper functions to convert (x, z) to and from hash map key
int64_t toKey(int x, int z);
glm::ivec2 toCoords(int64_t k);


struct ChunkVBOData {
    vector<float> vertex_opq_data;
    vector<float> vertex_tran_data;
    vector<GLuint> idx_opq_data;
    vector<GLuint> idx_tran_data;
    Chunk *associated_chunk;
};

// The container class for all of the Chunks in the game.
// Ultimately, while Terrain will always store all Chunks,
// not all Chunks will be drawn at any given time as the world
// expands.
class Terrain {
private:


    // Stores every Chunk according to the location of its lower-left corner
    // in world space.
    // We combine the X and Z coordinates of the Chunk's corner into one 64-bit int
    // so that we can use them as a key for the map, as objects like std::pairs or
    // glm::ivec2s are not hashable by default, so they cannot be used as keys.
    std::unordered_map<int64_t, uPtr<Chunk>> m_chunks;

    // We will designate every 64 x 64 area of the world's x-z plane
    // as one "terrain generation zone". Every time the player moves
    // near a portion of the world that has not yet been generated
    // (i.e. its lower-left coordinates are not in this set), a new
    // 4 x 4 collection of Chunks is created to represent that area
    // of the world.
    // The world that exists when the base code is run consists of exactly
    // one 64 x 64 area with its lower-left corner at (0, 0).
    // When milestone 1 has been implemented, the Player can move around the
    // world to add more "terrain generation zone" IDs to this set.
    // While only the 3 x 3 collection of terrain generation zones
    // surrounding the Player should be rendered, the Chunks
    // in the Terrain will never be deleted until the program is terminated.
    std::unordered_set<int64_t> m_generatedTerrain;

    // TODO: DELETE ALL REFERENCES TO m_geomCube AS YOU WILL NOT USE
    // IT IN YOUR FINAL PROGRAM!
    // The instance of a unit cube we can use to render any cube.
    // Presently, Terrain::draw renders one instance of this cube
    // for every non-EMPTY block within its Chunks. This is horribly
    // inefficient, and will cause your game to run very slowly until
    // milestone 1's Chunk VBO setup is completed.
    //Cube m_geomCube;

    OpenGLContext* mp_context;

    int time;

public:
    Terrain(OpenGLContext *context);
    ~Terrain();

    // Minseok Kim MS2
    std::vector<Chunk*> chunksWithOnlyBlockData;
    QMutex mutexWithOnlyBlockData;

    std::vector<ChunkVBOData> chunksWithVBOData;
    QMutex mutexChunksWithVBOData;

    // Instantiates a new Chunk and stores it in
    // our chunk map at the given coordinates.
    // Returns a pointer to the created Chunk.
    Chunk* createChunkAt(int x, int z);
    // Do these world-space coordinates lie within
    // a Chunk that exists?
    bool hasChunkAt(int x, int z) const;
    // Assuming a Chunk exists at these coords,
    // return a mutable reference to it
    uPtr<Chunk>& getChunkAt(int x, int z);
    // Assuming a Chunk exists at these coords,
    // return a const reference to it
    const uPtr<Chunk>& getChunkAt(int x, int z) const;
    // Given a world-space coordinate (which may have negative
    // values) return the block stored at that point in space.
    BlockType getBlockAt(int x, int y, int z) const;
    BlockType getBlockAt(glm::vec3 p) const;
    // Given a world-space coordinate (which may have negative
    // values) set the block at that point in space to the
    // given type.
    void setBlockAt(int x, int y, int z, BlockType t);

    // Draws every Chunk that falls within the bounding box
    // described by the min and max coords, using the provided
    // ShaderProgram
    void draw(int minX, int maxX, int minZ, int maxZ, ShaderProgram *shaderProgram);

    // Initializes the Chunks that store the 64 x 256 x 64 block scene you
    // see when the base code is run.
    void CreateTestScene();
    //create chunk vbo
    void createChunks(int minx, int maxx, int minz, int maxz);
    //expand the terrain
    void updateScene(const glm::vec3 pos, ShaderProgram *shaderProgram);

    //settime (Elaine2)
    void setTime(int t);

    //chang
    int getGrasslandHeight(int x, int z);
    int getMountainHeight(int x, int z);
    int getSandHeight(int x, int z);
    float perlinNoise(glm::vec2 uv);
    float surflet(glm::vec2 p, glm::vec2 gridPoint);
    float worleyNoise(glm::vec2 uv);
    float interpNoise1D(float);
    float fbm(float);
    float noise1D(int);
    float remap(float, float, float, float, float);
    void fillBlock(int x, int z);

    // Min MS2
    std::vector<int64_t> checkExpansion(glm::vec3 position);
};
