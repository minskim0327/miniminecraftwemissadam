#pragma once
#include <stack>
#include <map>
#include "turtle.h"
#include <random>
#include <iostream>
#include "terrain.h"

class Terrain;

const float PI = 3.141592653589793238463;

class River
{
public:
    River(Terrain*, int, int);
    Terrain *m_terrain;
    std::stack<Turtle> turtles;
    std::string grammer;
    Turtle *currTurtle;
    typedef void (*Rule)(void);
    int terrainx;
    int terrainz;
    int iteration;
    int length;
    int depth;
    std::map<char, Rule> drawingRules;
    void expand();
    void draw();
    void colorNeighbors(int, int, int, int);



    void goForward();
    void tiltLeft();
    void tiltRight();
    void saveState();
    void loadState();

};
