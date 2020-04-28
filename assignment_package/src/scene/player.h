#pragma once
#include "entity.h"
#include "camera.h"
#include "terrain.h"
#include "QSound"

class Player : public Entity {
private:
    glm::vec3 m_velocity, m_acceleration;
    Camera m_camera;
    const Terrain &mcr_terrain;
    int ifAxis;
    void processInputs(InputBundle &inputs);
    void computePhysics(float dT, const Terrain &terrain, InputBundle &inputs);



public:
    // Readonly public reference to our camera
    // for easy access from MyGL
    const Camera& mcr_camera;
    QSound* footstep;
    QSound* jump;
    QSound* flying;

    Player(glm::vec3 pos, const Terrain &terrain);
    virtual ~Player() override;

    void setCameraWidthHeight(unsigned int w, unsigned int h);

    void tick(float dT, InputBundle &input) override;

    // get current players position
    glm::vec3 getPosition();
    // check collision for a body
    void detectCollision(glm::vec3 *displacement, const Terrain &terrain);

    // check collision for a ray
    bool gridMarch(glm::vec3 rayOrigin,
                   glm::vec3 rayDirection,
                   const Terrain &terrain,
                   float *out_dist,
                   glm::ivec3 *out_blockHit);

    // check whether the player is at ground
    bool isOnGroundLevel(const Terrain &terrain, InputBundle &input);
    bool isInWater(const Terrain &terrain, InputBundle &input);

    void destroyBlock(Terrain *terrain);
    void createBlock(Terrain *terrain);
    // Player overrides all of Entity's movement
    // functions so that it transforms its camera
    // by the same amount as it transforms itself.
    void moveAlongVector(glm::vec3 dir) override;
    void moveForwardLocal(float amount) override;
    void moveRightLocal(float amount) override;
    void moveUpLocal(float amount) override;
    void moveForwardGlobal(float amount) override;
    void moveRightGlobal(float amount) override;
    void moveUpGlobal(float amount) override;
    void rotateOnForwardLocal(float degrees) override;
    void rotateOnRightLocal(float degrees) override;
    void rotateOnUpLocal(float degrees) override;
    void rotateOnForwardGlobal(float degrees) override;
    void rotateOnRightGlobal(float degrees) override;
    void rotateOnUpGlobal(float degrees) override;

    // For sending the Player's data to the GUI
    // for display
    QString posAsQString() const;
    QString velAsQString() const;
    QString accAsQString() const;
    QString lookAsQString() const;
};
