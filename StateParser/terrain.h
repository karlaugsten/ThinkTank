#ifndef TERRAIN_H
#define TERRAIN_H

#include "rapidjson/document.h"

enum TerrainType { IMPASSABLE, SOLID};

class Terrain {
private:



public:

    Terrain(const rapidjson::Value &dom);
    ~Terrain();

    TerrainType Type;



};

#endif