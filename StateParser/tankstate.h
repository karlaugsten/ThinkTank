#ifndef TANKSTATE_H
#define TANKSTATE_H

#include <vector>
#include "rapidjson/document.h"
#include "projectilestate.h"
#include "positionstate.h"

enum TankType { FAST, SLOW};

class Tank
{
private:
    Tank(){}
public:
    double health;
    double hitRadius;
    double collisionRadius;
    double tracks;
    double turret;
    double speed;

    Position* position;

    bool alive;

    std::string name;
    std::string id;

    TankType Type;

    std::vector<Projectile*> projectiles;

    Tank(const rapidjson::Value &dom); // Constructor that parses the map state message
    ~Tank();
    Tank* Clone();

};

#endif