#ifndef PREDICTIVE_FIRING_STRATEGY_H
#define PREDICTIVE_FIRING_STRATEGY_H

#include "strategy.h"
#include "../StateParser/gamestate.h"
#include <cmath>
#include <string>

/**
* A class to represent movement strategies.
*/
class PredictiveFiringStrategy : public Strategy
{
protected:

private:
    bool getClosestTarget(GameState& state, GameState& previous, Tank& thisTank, Position& target);
public:
    PredictiveFiringStrategy(){
    }

    std::queue<Command*> DetermineActions(GameState &state, GameState &previousState);
};


#endif