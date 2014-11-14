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
        this->x = x;
        this->y = y;
    }
    Position(const rapidjson::Value &dom); // Constructor that parses the game state response message
    ~Position();

    double GetAngle(const Position& other);
    double GetAngle();
    double Distance(const Position& other) const;
    double Norm2();
    double Length();
    Position Normalize();

    friend Position operator-(const Position &p1, const Position &p2);
    bool operator==(const Position &other) const;
    bool operator!=(const Position &other) const;
};

#endif