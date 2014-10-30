#ifndef PLAYERSTATE_H
#define PLAYERSTATE_H

#include "rapidjson/document.h"
#include "tankstate.h"

class Player
{
public:
    int score;
    std::string name;
    Tank* TankSlow;
    Tank* TankFast;

    Player(const rapidjson::Value &dom); // Constructor that parses the map state message

};

#endif