#ifndef TANKSTATE_H
#define TANKSTATE_H

#include "rapidjson/document.h"

enum TankType { FAST, SLOW};

class Tank
{
public:
    double health;
    double hitRadius;
    double collisionRadius;
    double tracks;
    double turret;
    double speed;

    bool alive;

    std::string name;
    std::string id;

    TankType Type;

    Tank(const rapidjson::Value &dom); // Constructor that parses the map state message

};

#endif