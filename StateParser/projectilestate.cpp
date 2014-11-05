#include "projectilestate.h"

Projectile::Projectile(const rapidjson::Value &dom){
    const rapidjson::Value& d_id = dom["id"];
    assert(d_id.IsString());
    id = d_id.GetString();

    const rapidjson::Value& d_direction = dom["direction"];
    assert(d_direction.IsNumber());
    direction = d_direction.GetDouble();

    const rapidjson::Value& d_speed = dom["speed"];
    assert(d_speed.IsNumber());
    speed = d_speed.GetDouble();

    const rapidjson::Value& d_damage = dom["damage"];
    assert(d_damage.IsNumber());
    damage = d_damage.GetDouble();

    position = new Position(dom["position"]);
}

Projectile::~Projectile() {
    // Do nothing for now;
    delete position;
}

Projectile* Projectile::Clone(){
    Projectile* clone = new Projectile();
    clone->speed = this->speed;
    clone->position = this->position->Clone();
    clone->damage = this->damage;
    clone->direction = this->direction;
    clone->id = this->id;
    return clone;
}