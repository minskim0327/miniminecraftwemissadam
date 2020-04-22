#include "river.h"

River::River(Terrain *m_terrain, int terrainx, int terrainz) :
    m_terrain(m_terrain), terrainx(terrainx), terrainz(terrainz), turtles(std::stack<Turtle>()),
    grammer("FX"), currTurtle(nullptr), iteration(2), length(10), depth(0), drawingRules()
{
    for (int i = 0; i < iteration; i++) {
        expand();
    }
}

void River::expand() {
    std::string temp = "";
    for (int i = 0; i < grammer.length(); i++) {

        if (grammer[i] == 'X') {
            double random = ((double) rand() / (RAND_MAX));
            if (random < 0.5) {
                temp.append("[+FX]-FX");
            } else {
                temp.append("[+FX][FX]-FX");
            }
        } else if (grammer[i] == 'F') {
            temp.append("FF");
        } else {
            temp.push_back(grammer[i]);
        }
    }
    grammer = temp;
}



void River::goForward() {
    int riverLength = std::max((int) length * depth, 12);
    float newx, newz;
    int rotatedx, rotatedz;
    for (int i = 0; i < riverLength; i++) {
        float step = i * 2 * PI / riverLength;
        float offset = sin(step) * depth;
        newx = currTurtle->posx + offset;
        newz = currTurtle->posz + i;



        rotatedx = int(cos(currTurtle->orientation) * (newx - currTurtle->posx) - sin(currTurtle->orientation) *
                (newz - currTurtle->posz) + currTurtle->posx);
        rotatedz = int(sin(currTurtle->orientation) * (newx - currTurtle->posx) + cos(currTurtle->orientation) *
                (newz - currTurtle->posz) + currTurtle->posz);
        colorNeighbors(terrainx + rotatedx, terrainz + rotatedz, depth + 3, depth);

    }
    currTurtle->posx = rotatedx;
    currTurtle->posz = rotatedz;

    
}

void River::colorNeighbors(int x, int z, int radius, int depth) {
    for (int i = -radius; i <= radius; i++) { //x
        for (int k = -radius; k <= radius; k++) { //z
            for (int j = -radius; j <= radius; j++) { //y
                if (terrainx <= x + i && x + i < terrainx + 64 && terrainz <= z + k && z + k < terrainz + 64 &&
                        i*i + j*j + k*k <= radius*radius) {
                    if (j <= -depth) {
                        if (m_terrain->hasChunkAt(x + i, z + k)) {
                            if (m_terrain->hasChunkAt(x + i, z + k))
                                m_terrain->setBlockAt(x + i, 128 + radius + j, z + k, WATER);
                        }

                    } else {
                        if (m_terrain->hasChunkAt(x + i, z + k)) {
                            if (m_terrain->hasChunkAt(x + i, z + k))
                                m_terrain->setBlockAt(x + i, 128 + radius + j, z + k, EMPTY);
                        }
                    }
                }
            }
            if (terrainx <= x + i && x + i < terrainx + 64 && terrainz <= z + k && z + k < terrainz + 64) {
                if (m_terrain->hasChunkAt(x + i, z + k)) {
                    for (int m = 128 + radius; m < 255; m++) {
                        if (m <= 128 + radius * 2) {
                            if (m_terrain->hasChunkAt(x + i, z + k))
                                m_terrain->setBlockAt(x + i, m, z + k, EMPTY);
                        } else {
                            if (m_terrain->getBlockAt(x + i, m, z + k) == EMPTY) {
                                break;
                            } else {
                                if (m_terrain->hasChunkAt(x + i, z + k))
                                    m_terrain->setBlockAt(x + i, m, z + k, EMPTY);
                            }
                        }

                    }
                }

            }


        }
    }
}

void River::draw() {
    Turtle t = Turtle(32, 1, 0);
    turtles.push(t);
    currTurtle = &turtles.top();
    for (int i = 0; i < grammer.length(); i++) {
        switch(grammer[i]) {
        case 'F':
            depth++;
            if (i == grammer.length() - 1 || grammer[i+1] != 'F') {
                goForward();
                depth = 0;
            }
            break;
        case '[':
            {
            Turtle turtle = Turtle(currTurtle->posx, currTurtle->posz, currTurtle->orientation);
            turtles.push(turtle);
            currTurtle = &turtles.top();
            break;
            }
        case ']':
        {
            turtles.pop();
            currTurtle = &turtles.top();
            break;
        }
        case 'X':
            break;
        case '+':
        {
            float randNum = 30 + rand() % (60 - 40 + 1);
            currTurtle->orientation += randNum * PI / 180.f;
            break;
        }
        case '-':
        {
            //currTurtle->depth++;
            float randNum = 30 + rand() % (60 - 40 + 1);
            currTurtle->orientation -= randNum * PI / 180.f;
            break;
        }
        }
    }
}

