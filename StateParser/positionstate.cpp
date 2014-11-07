#include "positionstate.h"
#include <iostream>
#include "rapidjson/document.h"
#include <cmath>

Position::Position(const rapidjson::Value &dom)
{

    assert(dom.IsArray());
    assert(dom.Size() == 2);
    x = dom[0].GetDouble();
    y = dom[1].GetDouble();

}

Position::~Position(){
}

Position operator-(const Position &p1, const Position &p2){
    return Position(p1.x - p2.x, p1.y - p2.y);
}

/**
* Gets the angle of this position, relative to another position.
*/
double Position::GetAngle(const Position& other){
    // subtract one from the other, get the angle of that
    Position tmp = other - *this;
    return tmp.GetAngle();
}

/**
* Returns the angle of the vector this position makes with the 1, 0 vector.
*/
double Position::GetAngle(){
    return std::atan(y/x);
}
