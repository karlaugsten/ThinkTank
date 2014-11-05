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
        Terrain* tmpTerrain = new Terrain(terrainArray[i]);
        terrain.push_back(tmpTerrain);
    }
}

MapState::~MapState(){
    for(int i = 0; i < terrain.size(); i++){
        delete terrain[i];
    }
}

MapState* MapState::Clone() {
    MapState* clone = new MapState();
    clone->width = this->width;
    clone->height = this->height;
    for(int i = 0; i < this->terrain.size(); i++){
        clone->terrain.push_back(terrain[i]->Clone());
    }
    return clone;

}