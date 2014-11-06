#include "mapstate.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>

MapState::MapState(const rapidjson::Value &dom)
{
    // TODO: Implement this
    const rapidjson::Value& size = dom["size"];
    assert(size.IsArray());
    width = size[0].GetDouble();
    height = size[1].GetDouble();
    const rapidjson::Value& terrainArray = dom["terrain"];

    assert(terrainArray.IsArray());

    for(int i = 0; i < terrainArray.Size(); i++) {
        Terrain tmpTerrain = Terrain(terrainArray[i]);
        terrain.push_back(tmpTerrain);
    }
}

MapState::~MapState(){
}