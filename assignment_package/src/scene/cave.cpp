#include "cave.h"

Cave::Cave(Terrain* m_terrain, int posx, int posz) :
    m_terrain(m_terrain), posx(posx), posz(posz), radius(15), currpos(0)
{
    int startx = posx + 32;
    int startz = posz;
    currpos = glm::vec3(startx, 128, startz);
}

void Cave::carveOpening() {
    for (int i = -radius; i <= radius; i++) { //x
        for (int k = -radius; k <= radius; k++) { //z

            if (i * i + k * k < radius * radius &&
                    m_terrain->hasChunkAt(currpos.x + i, currpos.z + k)) {
                for (int m = 118; m < 255; m++) {
                    m_terrain->setBlockAt(currpos.x + i, m, currpos.z + k, EMPTY);
                }
            }
        }
    }

    draw();
}

void Cave::carveSphere() {
//  m_terrain->setBlockAt(currpos.x, currpos.y, currpos.z, EMPTY);

    for (int i = -radius; i <= radius; i++) { //x
        for (int k = -radius; k <= radius; k++) { //z

            if (m_terrain->hasChunkAt(currpos.x + i, currpos.z + k)) {
                for (int j = -radius; j < radius; j++) {
                    if (i * i + k * k + j * j < radius * radius) {
                        if (m_terrain->getBlockAt(currpos.x + i, currpos.y + j, currpos.z + k) != LAVA)
                            m_terrain->setBlockAt(currpos.x + i, currpos.y + j, currpos.z + k, EMPTY);
                    } else if (i * i + k * k + j * j < radius * radius + 2) {
                        if (m_terrain->getBlockAt(currpos.x + i, currpos.y + j, currpos.z + k) ==STONE)
                            if (currpos.y > 100) {
                                m_terrain->setBlockAt(currpos.x + i, currpos.y + j, currpos.z + k, DIRT);
                            } else {
                                double random = ((double) rand() / (RAND_MAX));
                                if (random < 0.33) {
                                    m_terrain->setBlockAt(currpos.x + i, currpos.y + j, currpos.z + k, EMERALD);
                                } else if (random < 0.66) {
                                    m_terrain->setBlockAt(currpos.x + i, currpos.y + j, currpos.z + k, SAPPHIRE);
                                } else {
                                    m_terrain->setBlockAt(currpos.x + i, currpos.y + j, currpos.z + k, GOLD);
                                }

                            }
                    }
                }
             }
         }
    }

}

void Cave::drawLava() {
    for (int i = -radius; i <= radius; i++) { //x
        for (int k = -radius; k <= radius; k++) { //z

            if (m_terrain->hasChunkAt(currpos.x + i, currpos.z + k)) {
                for (int j = -radius; j < radius; j++) {
                    if (i * i + k * k + j * j < radius * radius && j < -radius + 2) {
                        m_terrain->setBlockAt(currpos.x + i, currpos.y + j, currpos.z + k, LAVA);
                    }
                }
             }
         }
    }
}

void Cave::draw() {
    for (int i = 0; i < 100; i++) {
        float perlin = perlinNoise3D(currpos / 5.f);
        if (perlin >= 0)
            perlin = -1;
        else
            perlin = 1;
        float perlin2 = perlinNoise3D(currpos / 5.f);
        if (perlin2 >= 0)
            perlin2 = -1;
        else
            perlin2 = 1;

        currpos.x += perlin;
        currpos.z += 1;
        if (currpos.y > 90) {
            currpos.y -= 1;
        }
        if (m_terrain->hasChunkAt(currpos.x, currpos.z)) {
            carveSphere();
        } else {
            break;
        }
        if (i % 30 == 0 && currpos.y <= 90) {
            drawLava();
        }
    }
    return;
}

float Cave::perlinNoise3D(glm::vec3 p) {
    float surfletSum = 0.f;
    // Iterate over the four integer corners surrounding uv
    for(int dx = 0; dx <= 1; ++dx) {
        for(int dy = 0; dy <= 1; ++dy) {
            for(int dz = 0; dz <= 1; ++dz) {
                surfletSum += surflet3D(p, glm::floor(p) + glm::vec3(dx, dy, dz));
            }
        }
    }
    return surfletSum;

}

glm::vec3 Cave::random3(glm::vec3 p) {
    return glm::fract(glm::sin(glm::vec3(glm::dot(p, glm::vec3(127.1, 311.7, 200.0)),
                          glm::dot(p, glm::vec3(269.5, 183.3, 212.9)) * 43758.5453f,
                      glm::dot(p, glm::vec3(269.5, 183.3, 212.9)))));
}

float Cave::surflet3D(glm::vec3 p, glm::vec3 gridPoint) {
    // Compute the distance between p and the grid point along each axis, and warp it with a
    // quintic function so we can smooth our cells
    glm::vec3 t2 = glm::abs(p - gridPoint);
    glm::vec3 t = glm::vec3(1.f) - 6.f * glm::pow(t2, glm::vec3(5.f)) +
            15.f * glm::pow(t2, glm::vec3(4.f)) - 10.f * glm::pow(t2, glm::vec3(3.f));
    // Get the random vector for the grid point (assume we wrote a function random2
    // that returns a vec2 in the range [0, 1])
    glm::vec3 gradient = random3(gridPoint) * 2.f - glm::vec3(1.);
    // Get the vector from the grid point to P
    glm::vec3 diff = p - gridPoint;
    // Get the value of our height field by dotting grid->P with our gradient
    float height = glm::dot(diff, gradient);
    // Scale our height field (i.e. reduce it) by our polynomial falloff function
    return height * t.x * t.y * t.z;

}
