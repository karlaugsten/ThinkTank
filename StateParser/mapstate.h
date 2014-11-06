#ifndef MAPSTATE_H
#define MAPSTATE_H

#include "rapidjson/document.h"
#include "terrain.h"
#include <vector>

class MapState
{
private:
    int width;
    int height;
    std::vector<Terrain> terrain;

public:
    MapState(){ } //private constructor
    MapState(const rapidjson::Value &dom); // Constructor that parses the map state message
    ~MapState();


    int GetWidth() { return width; }
    int GetHeight() { return height; }
    std::vector<Terrain> GetTerrain() { return terrain; }

};

#endif