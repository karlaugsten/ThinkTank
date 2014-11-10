#ifndef DIFFERENTIAL_MOVEMENT_STRATEGY_H
#define DIFFERENTIAL_MOVEMENT_STRATEGY_H

#include "strategy.h"
#include "../StateParser/gamestate.h"

/**
* A Strategy that models the playing field with a "goodness"
* equation, and moves the tanks always to areas of increasing goodness.
*/
class DifferentialMovementStrategy : public Strategy
{
protected:

private:
    /*
    * Calculates the goodness at the given position
     */
    double CalculateGoodness(GameState &state, Position& position);
    double CalculateGoodness(GameState &state, double x, double y);
public:
    DifferentialMovementStrategy(){
    }

    std::queue<Command*> DetermineActions(GameState &state);
};


#endif