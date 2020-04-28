#pragma once
#include <iostream>
#include <random>
#include "terrain.h"

class Terrain;

class Cave
{
public:
    Cave(Terrain*, int, int);
    Terrain *m_terrain;
    int posx;
    int posz;
    int startx;
    int startz;
    int radius;
    glm::vec3 currpos;
    float perlinNoise3D(glm::vec3 p);
    float surflet3D(glm::vec3 p, glm::vec3 gridPoint);
    glm::vec3 random3(glm::vec3);
    void carveOpening();
    void draw();
    void carveSphere();
    void drawLava();
};

