#ifndef PROJECTILESTATE_H
#define PROJECTILESTATE_H

#include <string>
#include "rapidjson/document.h"
#include "positionstate.h"

class Projectile
{
private:
public:
    std::string id;
    double direction;
    double speed;
    double damage;

    Position position;

    Projectile() { } // private default constructor
    Projectile(const rapidjson::Value &dom); // Constructor that parses the game state response message
    ~Projectile();
};

#endif