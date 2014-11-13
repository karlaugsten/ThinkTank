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
    bool getClosestTarget(GameState& state, GameState& previous, Tank& thisTank, Position& target);
public:
    FiringStrategy(){
    }

    std::queue<Command*> DetermineActions(GameState &state, GameState &previousState);
};


#endif