#include "terrain.h"
#include <iostream>
#include "rapidjson/document.h"

Terrain::Terrain(const rapidjson::Value &dom)
{

    const rapidjson::Value& type = dom["type"];
    assert(type.IsString());
    std::string t = type.GetString();
    if(t == "SOLID")
    {
        Type = TerrainType::SOLID;
    } else if(t == "IMPASSABLE")
    {
        Type = TerrainType::IMPASSABLE;
    } else
    {
        // Should never get here!
        assert(false);
    }
    position = Position(dom["boundingBox"]["corner"]);
    size = Position(dom["boundingBox"]["size"]);
    // TODO: Finish implementing this
}

Terrain::~Terrain(){
}
