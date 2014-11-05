#include "playerstate.h"
#include <iostream>
#include "rapidjson/document.h"

Player::Player(const rapidjson::Value &dom)
{

    const rapidjson::Value& sc = dom["score"];
    assert(sc.IsInt());
    score = sc.GetDouble();

    const rapidjson::Value& nm = dom["name"];
    assert(nm.IsString());
    name = nm.GetString();

    const rapidjson::Value& tanks = dom["tanks"];
    assert(tanks.IsArray());
    // TODO: If tank is not there handle this better (i.e if its dead)
    Tank *firstTank = NULL;
    Tank *secondTank = NULL;
    if(tanks.Size() == 2) {
        firstTank = new Tank(tanks[0]);
        secondTank = new Tank(tanks[1]);
    } else if(tanks.Size() == 1){
        firstTank = new Tank(tanks[0]);

    }
    if(firstTank != NULL){
        if (firstTank->Type == TankType::FAST) {

            TankFast = firstTank;
            TankSlow = secondTank;
        } else if (firstTank->Type == TankType::SLOW) {

            TankFast = secondTank;
            TankSlow = firstTank;
        } else {
            // Could not find fast and slow tanks!
            assert(false);
        }
    }

    // TODO: Finish implementing this
}

Player::~Player(){
    delete TankFast;
    delete TankSlow;
}

Player* Player::Clone(){
    Player* clone = new Player();
    clone->name = this->name;
    clone->score = this->score;
    if(this->TankFast != NULL) {
        clone->TankFast = this->TankFast->Clone();
    }
    if(this->TankSlow != NULL){
        clone->TankSlow = this->TankSlow->Clone();
    }
    return clone;
}