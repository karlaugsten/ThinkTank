#ifndef PLAYERSTATE_H
#define PLAYERSTATE_H

#include "rapidjson/document.h"
#include "tankstate.h"

class Player
{
private:

public:
    Player(){ alive = false; }
    int score;
    bool alive;
    std::string name;
    Tank TankSlow;
    Tank TankFast;

    Player(const rapidjson::Value &dom); // Constructor that parses the map state message
    ~Player();


};

#endif