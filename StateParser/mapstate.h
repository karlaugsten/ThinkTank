#ifndef MAPSTATE_H
#define MAPSTATE_H

#include "rapidjson/document.h"
#include "terrain.h"
#include <vector>

class MapState
{
private:


public:
    MapState(){ } //private constructor
    MapState(const rapidjson::Value &dom); // Constructor that parses the map state message
    ~MapState();

    double width;
    double height;
    std::vector<Terrain> terrain;

    int GetWidth() { return width; }
    int GetHeight() { return height; }
    std::vector<Terrain> GetTerrain() { return terrain; }

};

#endif