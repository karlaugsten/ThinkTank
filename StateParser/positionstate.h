#ifndef POSITIONSTATE_H
#define POSITIONSTATE_H

#include <string>
#include "rapidjson/document.h"

class Position
{
private:

public:

    double x;
    double y;

    Position(){ x = 0.0; y = 0.0; }
    Position(const rapidjson::Value &dom); // Constructor that parses the game state response message
    ~Position();

};

#endif