#ifndef POSITIONSTATE_H
#define POSITIONSTATE_H

#include <string>
#include "rapidjson/document.h"

class Position
{
private:

public:

    double x;
    double y;

    Position(){ x = 0.0; y = 0.0; }
    Position(double x, double y){
        x = x;
        y = y;
    }
    Position(const rapidjson::Value &dom); // Constructor that parses the game state response message
    ~Position();

    double GetAngle(const Position& other);
    double GetAngle();

    friend Position operator-(const Position &p1, const Position &p2);
};

#endif