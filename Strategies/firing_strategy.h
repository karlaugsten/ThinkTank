#ifndef FIRING_STRATEGY_H
#define FIRING_STRATEGY_H

#include "strategy.h"
#include "../StateParser/gamestate.h"
#include <cmath>
#include <string>

/**
* A class to represent movement strategies.
*/
class FiringStrategy : public Strategy
{
protected:

private:
    Position getClosestTarget(GameState& state, Position& thisTank);
public:
    FiringStrategy(){
    }

    std::queue<Command*> DetermineActions(GameState &state);
};


#endif