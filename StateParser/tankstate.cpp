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

    if(strcmp(type.GetString(), "TankSlow"))
    {
        Type = TankType::SLOW;
    } else if(strcmp(type.GetString(), "TankFast"))
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