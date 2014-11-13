#ifndef STRATEGY_H
#define STRATEGY_H

#include <string>
#include <zmq.hpp>
#include <queue>
#include "../command.h"
#include "../StateParser/gamestate.h"

/**
* Base class that defines a strategies behaviour.
*/
class Strategy
{
protected:
public:
    Strategy(){
    }

    virtual std::queue<Command*> DetermineActions(GameState& state, GameState& previousState)=0;
};

#endif