#include "terrain.h"
#include <iostream>
#include "rapidjson/document.h"

Terrain::Terrain(const rapidjson::Value &dom)
{

    const rapidjson::Value& type = dom["type"];
    assert(type.IsString());
    if(strcmp(type.GetString(), "SOLID"))
    {
        Type = TerrainType::SOLID;
    } else if(strcmp(type.GetString(), "IMPASSABLE"))
    {
        Type = TerrainType::IMPASSABLE;
    } else
    {
        // Should never get here!
        assert(false);
    }
    position = new Position(dom["corner"]);
    size = new Position(dom["size"]);
    // TODO: Finish implementing this
}

Terrain::~Terrain(){
    delete position;
    delete size;
}