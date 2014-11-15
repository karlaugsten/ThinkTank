#ifndef RANDOM_STRATEGY_H
#define RANDOM_STRATEGY_H

#include "strategy.h"
#include "../StateParser/gamestate.h"
#include <cmath>
#include <string>

/**
* A class to represent movement strategies.
*/
class RandomFiringStrategy : public Strategy
{
protected:

private:
    void sendTankCommands(std::queue<Command*> &moves, GameState &state, GameState &previousState, Tank &thisTank);
public:
    RandomFiringStrategy(){
    }

    std::queue<Command*> DetermineActions(GameState &state, GameState &previousState);
};


#endif