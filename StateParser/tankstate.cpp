#include "tankstate.h"
#include <iostream>
#include "rapidjson/document.h"

Tank::Tank(const rapidjson::Value &dom)
{

    const rapidjson::Value& strId = dom["id"];
    assert(strId.IsString());
    id = strId.GetString();
    const rapidjson::Value& type = dom["type"];
    assert(type.IsString());
    std::string strType = type.GetString();
    if(strType == "TankSlow")
    {
        Type = TankType::SLOW;
    } else if(strType == "TankFast")
    {
        Type = TankType::FAST;
    } else
    {
        // Should never get here!
        assert(false);
    }

    const rapidjson::Value& d_health = dom["health"];
    assert(d_health.IsNumber());
    health = d_health.GetDouble();

    const rapidjson::Value& d_hitRadius = dom["hitRadius"];
    assert(d_hitRadius.IsNumber());
    hitRadius = d_hitRadius.GetDouble();

    const rapidjson::Value& d_collisionRadius = dom["collisionRadius"];
    assert(d_collisionRadius.IsNumber());
    collisionRadius = d_collisionRadius.GetDouble();

    const rapidjson::Value& d_tracks = dom["tracks"];
    assert(d_tracks.IsNumber());
    tracks = d_tracks.GetDouble();

    const rapidjson::Value& d_turret = dom["turret"];
    assert(d_turret.IsNumber());
    turret = d_turret.GetDouble();

    const rapidjson::Value& d_speed = dom["speed"];
    assert(d_speed.IsNumber());
    speed = d_speed.GetDouble();

    const rapidjson::Value& d_alive = dom["alive"];
    assert(d_alive.IsBool());
    alive = d_alive.GetBool();


    const rapidjson::Value& d_projectiles = dom["projectiles"];
    assert(d_projectiles.IsArray());
    for(int i = 0; i < d_projectiles.Size(); i++){
        Projectile* projectile = new Projectile(d_projectiles[i]);
        projectiles.push_back(projectile);
    }

    // TODO: Finish implementing this
    position = new Position(dom["position"]);
}

Tank::~Tank(){
    for(int i = 0; i < projectiles.size(); i++){
        delete projectiles[i];
    }

    delete position;
}

Tank* Tank::Clone(){
    Tank* clone = new Tank();
    clone->name = this->name;
    clone->Type = this->Type;
    clone->alive = this->alive;
    clone->collisionRadius = this->collisionRadius;
    clone->health = this->health;
    clone->hitRadius = this->hitRadius;
    clone->id = this->id;
    clone->position = this->position->Clone();
    clone->speed = this->speed;
    clone->tracks = this->tracks;
    clone->turret = this->turret;
    for(int i = 0; i < this->projectiles.size(); i++){
        clone->projectiles.push_back(projectiles[i]->Clone());
    }
    return clone;
}