#include "positionstate.h"
#include <iostream>
#include "rapidjson/document.h"

Position::Position(const rapidjson::Value &dom)
{

    assert(dom.IsArray());
    assert(dom.Size() == 2);
    x = dom[0].GetDouble();
    y = dom[1].GetDouble();

}

Position::~Position(){
}

Position* Position::Clone(){
    Position* clone = new Position();
    clone->x = this->x;
    clone->y = this->y;
    return clone;
}