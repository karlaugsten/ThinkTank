#ifndef TERRAIN_H
#define TERRAIN_H

#include "rapidjson/document.h"
#include "positionstate.h"

enum TerrainType { IMPASSABLE, SOLID};

class Terrain {
private:

    Position* position;
    Position* size;
    Terrain(){}
public:

    Terrain(const rapidjson::Value &dom);
    ~Terrain();
    Terrain* Clone();

    TerrainType Type;



};

#endif