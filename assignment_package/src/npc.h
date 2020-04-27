#pragma once
#include "drawable.h"
#include "scene/terrain.h"

class NPC : public Drawable
{

private:
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;

    Terrain *mp_terrain;
    glm::vec3 prev_direction;
    int ifAxis;

    bool isCollision;
public:
    NPC(Terrain *terrain, OpenGLContext *context);

    // update the physics related matters of the NPC
    void updatePhysics();

    void detectCollision(glm::vec3 *rayDirection, const Terrain &terrain);
    bool gridMarch(glm::vec3 rayOrigin,
                   glm::vec3 rayDirection,
                   const Terrain &terrain,
                   float *out_dist,
                   glm::ivec3 *out_blockHit);

    //glm::vec3 getPosition();

    void create() override;
};


