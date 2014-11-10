#ifndef MOVEMENT_STRATEGY_H
#define MOVEMENT_STRATEGY_H

#include "strategy.h"

/**
* A class to represent movement strategies.
*/
class MovementStrategy : public Strategy
{
protected:
public:
    MovementStrategy(){
    }

    std::queue<Command*> DetermineActions(GameState &state);
};


#endif