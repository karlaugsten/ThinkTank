#ifndef PROJECTILESTATE_H
#define PROJECTILESTATE_H

#include <string>
#include "rapidjson/document.h"

class Projectile
{
private:
    std::string id;
    double direction;
    double speed;
    double damage;
    Projectile() { } // private default constructor
public:

    Projectile(const rapidjson::Value &dom); // Constructor that parses the game state response message
    ~Projectile();
};

#endif