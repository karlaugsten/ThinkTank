#include "playerstate.h"
#include <iostream>
#include "rapidjson/document.h"

Player::Player(const rapidjson::Value &dom)
{
    alive = true;
    const rapidjson::Value& sc = dom["score"];
    assert(sc.IsInt());
    score = sc.GetDouble();

    const rapidjson::Value& nm = dom["name"];
    assert(nm.IsString());
    name = nm.GetString();

    const rapidjson::Value& tanks = dom["tanks"];
    assert(tanks.IsArray());
    // TODO: If tank is not there handle this better (i.e if its dead)
    Tank firstTank;
    Tank secondTank;
    if(tanks.Size() == 2) {
        firstTank = Tank(tanks[0]);
        secondTank = Tank(tanks[1]);
    } else if(tanks.Size() == 1){
        firstTank = Tank(tanks[0]);
        secondTank.alive = false;
    }else{
        firstTank.alive = false;
        secondTank.alive = false;
    }
    if (firstTank.Type == TankType::FAST) {

        TankFast = firstTank;
        TankSlow = secondTank;
    } else if (firstTank.Type == TankType::SLOW) {

        TankFast = secondTank;
        TankSlow = firstTank;
    } else {
        // Could not find fast and slow tanks!
        assert(false);
    }

    // TODO: Finish implementing this
}

Player::~Player(){
}

