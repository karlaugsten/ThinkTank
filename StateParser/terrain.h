#ifndef TERRAIN_H
#define TERRAIN_H

#include "rapidjson/document.h"
#include "positionstate.h"

enum TerrainType { IMPASSABLE, SOLID};

class Terrain {
private:



public:
    Position position;
    Position size;

    Terrain(){}
    Terrain(const rapidjson::Value &dom);
    ~Terrain();

    TerrainType Type;



};

#endif