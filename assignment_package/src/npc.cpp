#include "npc.h"

NPC::NPC(Terrain *terrain, OpenGLContext *context)
    : Drawable(context), mp_terrain(terrain), ifAxis(-1)
{
}

void NPC::updatePhysics() {
    // decide the new velocity randomly if a collision happens
    // the npc should be able to randomly jump/change direction
    if (isCollision) {
        //TODO
    }
}

void NPC::detectCollision(glm::vec3 *rayDirection, const Terrain &terrain) {
      glm::vec3 bottomLeftVertex = this->m_position - glm::vec3(0.5f, 0.f, 0.5f);
      //glm::vec3 rayOrigin = this->m_position;
      glm::ivec3 out_blockHit = glm::ivec3();
      float out_dist = 0.f;

      for (int x = 0; x <= 1; x++) {
          for (int z = 0; z >= -1; z--) {
              for (int y = 0; y <= 2; y++) {
                  glm::vec3 rayOrigin = bottomLeftVertex + glm::vec3(x, y, z);
                  for (int i = 0; i < 3; i++) {

                  }
                  if (gridMarch(rayOrigin, *rayDirection, terrain, &out_dist, &out_blockHit)) {
                      //*rayDirection = (out_dist - 0.005f) * glm::normalize(*rayDirection);
                      float distance = glm::min(out_dist - 0.005f, glm::abs(glm::length(this->m_position - glm::vec3(out_blockHit))));
                      *rayDirection = distance * glm::normalize(*rayDirection);
                  }
              }
          }
      }
}

bool NPC::gridMarch(glm::vec3 rayOrigin, glm::vec3 rayDirection, const Terrain &terrain, float *out_dist, glm::ivec3 *out_blockHit) {
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
        ifAxis = interfaceAxis;
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

void NPC::create() {
    ;
}
