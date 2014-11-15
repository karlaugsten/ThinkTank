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
    bool getClosestTarget(GameState& state, GameState& previous, Tank& thisTank, Tank& target);
    void sendTankCommands(std::queue<Command*> &moves, GameState &state, GameState &previousState, Tank &thisTank);
public:
    FiringStrategy(){

    }

    std::queue<Command*> DetermineActions(GameState &state, GameState &previousState);
};


#endif