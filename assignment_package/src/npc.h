#pragma once
#include "drawable.h"
#include "scene/terrain.h"

enum NPCAction {
    NORTH, WEST, SOUTH, EAST
};

class NPC : public Drawable
{

private:
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;

    const Terrain &mcr_terrain;
    OpenGLContext *mp_context;

    glm::vec3 prev_direction;
    int ifAxis;

    bool isOnGround;
    bool isCollision;
public:
    NPC(const Terrain &terrain, OpenGLContext *context);

    // update the physics related matters of the NPC
    void tick(float dT);
    void updatePhysicsInfo();
    void assigneDirection(int direction);
    void processMovement(float dT);

    bool isOnGroundLevel(const Terrain &terrain);
    void detectCollision(glm::vec3 *rayDirection, const Terrain &terrain);
    bool gridMarch(glm::vec3 rayOrigin,
                   glm::vec3 rayDirection,
                   const Terrain &terrain,
                   float *out_dist,
                   glm::ivec3 *out_blockHit);

    //glm::vec3 getPosition();
    void generatePosition();

    void create() override;
    void createTestVBO(std::vector<GLuint> *idx,
                       std::vector<glm::vec4> *pos,
                       std::vector<glm::vec4> *nor,
                       std::vector<glm::vec2> *uv,
                       std::vector<float> *anim);
    void interleaveVBO(std::vector<glm::vec4> *pos,
                       std::vector<glm::vec4> *nor,
                       std::vector<glm::vec2> *uv,
                       std::vector<float> *anim,
                       std::vector<float> *allOpq);
    void createVBO(std::vector<GLuint> *idx, std::vector<glm::vec4> *pos_nor_uv_anim);
    void sendVBOToGPU(std::vector<GLuint> *idx, std::vector<float> *pos_nor_uv_anim);

    int setRandomMovement();
};



