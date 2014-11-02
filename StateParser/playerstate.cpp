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
    Tank* firstTank = new Tank(tanks[0]);
    Tank* secondTank = new Tank(tanks[1]);

    if(firstTank->Type == TankType::FAST)
    {

        TankFast = firstTank;
        TankSlow = secondTank;
    } else if(firstTank->Type == TankType::SLOW)
    {

        TankFast = secondTank;
        TankSlow = firstTank;
    } else
    {
        // Could not find fast and slow tanks!
        assert(false);
    }

    // TODO: Finish implementing this
}

Player::~Player(){
    delete TankFast;
    delete TankSlow;
}