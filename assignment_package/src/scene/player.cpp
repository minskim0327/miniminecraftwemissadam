#include "player.h"
#include <QString>
#include "iostream"
Player::Player(glm::vec3 pos, const Terrain &terrain)
    : Entity(pos), m_velocity(0,0,0), m_acceleration(0,0,0),
      m_camera(pos + glm::vec3(0, 1.5f, 0)), mcr_terrain(terrain),
      mcr_camera(m_camera)
{}

Player::~Player()
{}

void Player::tick(float dT, InputBundle &input) {
    processInputs(input);
    computePhysics(dT, mcr_terrain, input);
}

void Player::processInputs(InputBundle &inputs) {
    float accRate = 1.0f;

    if (isOnGroundLevel(mcr_terrain, inputs)) {
        std::cout << "isOnGroundLevel" << std::endl;
    }
    if (inputs.wPressed) {
        //m_velocity = this->m_forward;
        m_acceleration = accRate * this->m_forward;
    } else if (inputs.sPressed) {
        m_acceleration = -accRate * this->m_forward;
    } else if (inputs.dPressed) {
        m_acceleration = accRate * this->m_right;
    } else if (inputs.aPressed) {
        m_acceleration = -accRate * this->m_right;
    } else if (inputs.ePressed) {
        m_acceleration = accRate * this->m_up;
    } else if (inputs.qPressed) {
        m_acceleration = -accRate * this->m_up;
    } else {
        if (inputs.isFlightMode || inputs.isOnGround) {
            if (inputs.isFlightMode) {
                std::cout << "flightmode" << std::endl;

            }
//            if (inputs.isOnGround) {
//                std::cout << "is on ground" << std::endl;
//            }
            m_velocity = glm::vec3(0.f, 0.f, 0.f);
            m_acceleration = glm::vec3(0.f, 0.f, 0.f);
        }
    }

    // prevent multiple jumps while in the mid-air
    if (inputs.spacePressed && inputs.isOnGround) {
        m_velocity = 10.0f * this->m_up;
        m_acceleration = glm::vec3();
        //m_acceleration = -10.0f * this->m_up;
        inputs.isOnGround = false;
        inputs.spacePressed = false;
    }


    // also need to handle mouse move event?
}

void Player::computePhysics(float dT, const Terrain &terrain, InputBundle &inputs) {
    // TODO: Update the Player's position based on its acceleration
    // and velocity, and also perform collision detection.

    m_velocity = m_velocity * 0.90f + dT * m_acceleration;
    glm::vec3 rayDirection = m_velocity * dT;
    //glm::vec3 displacement = glm::vec3();
    glm::vec3 gravity = glm::vec3(0.0f, -10.0f, 0.0f);

    if (inputs.isFlightMode) {
        ;//displacement = m_velocity * dT;
    } else {
        if (!isOnGroundLevel(terrain, inputs)) {
            std::cout<<"isFlying"<<std::endl;
            m_velocity += gravity * dT;
            rayDirection = m_velocity * dT;
        }
        // detect collision (shoot 12 rays)

        //detectCollision(&rayDirection, terrain);
        // set displacement accordingly with the collision

    }
    this->moveAlongVector(rayDirection);
}

void Player::setCameraWidthHeight(unsigned int w, unsigned int h) {
    m_camera.setWidthHeight(w, h);
}

void Player::detectCollision(glm::vec3 *rayDirection, const Terrain &terrain) {
    glm::vec3 bottomLeftVertex = this->m_position - glm::vec3(0.5f, 0.f, 0.f);
    //glm::vec3 rayOrigin = this->m_position;
    glm::ivec3 out_blockHit = glm::ivec3();
    float out_dist = 0.f;


    for (int x = 0; x <= 1; x++) {
        for (int z = 0; z >= -1; z--) {
            for (int y = 0; y <= 2; y++) {
                glm::vec3 rayOrigin = bottomLeftVertex + glm::vec3(x, y, z);
                if (gridMarch(rayOrigin, *rayDirection, terrain, &out_dist, &out_blockHit)) {
                    std::cout << "collision!" << std::endl;
                    glm::vec3 finalPosition = glm::vec3(out_blockHit);
                    *rayDirection = finalPosition - rayOrigin;
                }
            }
        }
    }

}

bool Player::gridMarch(glm::vec3 rayOrigin, glm::vec3 rayDirection,
                       const Terrain &terrain, float *out_dist,
                       glm::ivec3 *out_blockHit) {
    float maxLen = glm::length(rayDirection); // farthest we search
    glm::ivec3 currCell = glm::ivec3(glm::floor(rayOrigin));
    rayDirection = glm::normalize(rayDirection); // now all t values represent world dist

    float curr_t = 0.f;
    while (curr_t < maxLen) {
        float min_t = glm::sqrt(3.f);
        float interfaceAxis = -1; // Track axis for which t is smallest
        for (int i = 0; i < 3; ++i) { // Iterate over the three axes
            if (rayDirection[i] != 0) { // Is ray parallel to axis i?
                float offset = glm::max(0.f, glm::sign(rayDirection[i]));

                // If the player is "exactly on an interface then
                // they'll never move if they're looking in a negative direction
                if (currCell[i] == rayOrigin[i] && offset == 0.f) {
                    offset = -1.f;
                }
                int nextIntercept = currCell[i] + offset;
                float axis_t = (nextIntercept - rayOrigin[i]) / rayDirection[i];
                axis_t = glm::min(axis_t, maxLen); // Clamp to max len to avoid super out of bound errors
                if (axis_t < min_t) {
                    min_t = axis_t;
                    interfaceAxis = i;
                }
            }
        }
        if (interfaceAxis == -1) {
            throw std::out_of_range("interfaceAxis was -1 after the for loop in gridMarch!");
        }
        curr_t += min_t;
        rayOrigin += rayDirection * min_t;
        glm::ivec3 offset = glm::ivec3(0, 0, 0);
        offset[interfaceAxis] = glm::min(0.f, glm::sign(rayDirection[interfaceAxis]));
        currCell = glm::ivec3(glm::floor(rayOrigin)) + offset;
        // If the currCell contains something other than empty, return curr_t
        BlockType cellType = terrain.getBlockAt(currCell.x, currCell.y, currCell.z);

        if (cellType != EMPTY) {
            *out_blockHit = currCell;
            *out_dist = glm::min(maxLen, curr_t);
            return true;
        }
    }

    *out_dist = glm::min(maxLen, curr_t);
    return false;
}

bool Player::isOnGroundLevel(const Terrain &terrain, InputBundle &input) {
//    glm::vec3 rayOrigin = this->m_position;
//    glm::vec3 rayDirection = glm::vec3(0.f, -0.005f, 0.f);
//    float out_dist = 0.f;
//    glm::ivec3 out_blockHit = glm::ivec3();

//    input.isOnGround = gridMarch(rayOrigin, rayDirection, terrain, &out_dist, &out_blockHit);
//    return input.isOnGround;
    glm::vec3 bottomLeftVertex = this->m_position - glm::vec3(0.5f, 0.f, 0.f);
    for (int x = 0; x <= 1; x++) {
        for (int z = 0; z >= -1; z--) {
            if (terrain.getBlockAt(floor(bottomLeftVertex[0]) + x,
                                   floor(bottomLeftVertex[1]) - 1.f,
                                   floor(bottomLeftVertex[2]) + z) != EMPTY) {
                input.isOnGround = true;
                return true;
            }
        }
    }

    return false;
}

void Player::moveAlongVector(glm::vec3 dir) {
    Entity::moveAlongVector(dir);
    m_camera.moveAlongVector(dir);
}

void Player::moveForwardLocal(float amount) {
    Entity::moveForwardLocal(amount);
    m_camera.moveForwardLocal(amount);
}
void Player::moveRightLocal(float amount) {
    Entity::moveRightLocal(amount);
    m_camera.moveRightLocal(amount);
}
void Player::moveUpLocal(float amount) {
    Entity::moveUpLocal(amount);
    m_camera.moveUpLocal(amount);
}
void Player::moveForwardGlobal(float amount) {
    Entity::moveForwardGlobal(amount);
    m_camera.moveForwardGlobal(amount);
}
void Player::moveRightGlobal(float amount) {
    Entity::moveRightGlobal(amount);
    m_camera.moveRightGlobal(amount);
}
void Player::moveUpGlobal(float amount) {
    Entity::moveUpGlobal(amount);
    m_camera.moveUpGlobal(amount);
}
void Player::rotateOnForwardLocal(float degrees) {
    Entity::rotateOnForwardLocal(degrees);
    m_camera.rotateOnForwardLocal(degrees);
}
void Player::rotateOnRightLocal(float degrees) {
    Entity::rotateOnRightLocal(degrees);
    m_camera.rotateOnRightLocal(degrees);
}
void Player::rotateOnUpLocal(float degrees) {
    Entity::rotateOnUpLocal(degrees);
    m_camera.rotateOnUpLocal(degrees);
}
void Player::rotateOnForwardGlobal(float degrees) {
    Entity::rotateOnForwardGlobal(degrees);
    m_camera.rotateOnForwardGlobal(degrees);
}
void Player::rotateOnRightGlobal(float degrees) {
    Entity::rotateOnRightGlobal(degrees);
    m_camera.rotateOnRightGlobal(degrees);
}
void Player::rotateOnUpGlobal(float degrees) {
    Entity::rotateOnUpGlobal(degrees);
    m_camera.rotateOnUpGlobal(degrees);
}

QString Player::posAsQString() const {
    std::string str("( " + std::to_string(m_position.x) + ", " + std::to_string(m_position.y) + ", " + std::to_string(m_position.z) + ")");
    return QString::fromStdString(str);
}
QString Player::velAsQString() const {
    std::string str("( " + std::to_string(m_velocity.x) + ", " + std::to_string(m_velocity.y) + ", " + std::to_string(m_velocity.z) + ")");
    return QString::fromStdString(str);
}
QString Player::accAsQString() const {
    std::string str("( " + std::to_string(m_acceleration.x) + ", " + std::to_string(m_acceleration.y) + ", " + std::to_string(m_acceleration.z) + ")");
    return QString::fromStdString(str);
}
QString Player::lookAsQString() const {
    std::string str("( " + std::to_string(m_forward.x) + ", " + std::to_string(m_forward.y) + ", " + std::to_string(m_forward.z) + ")");
    return QString::fromStdString(str);
}
