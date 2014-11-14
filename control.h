#ifndef CONTROL_H
#define CONTROL_H

#include <zmq.hpp>
#include "StateParser/gamestate.h"
#include "StateParser/stateparser.h"
#include "commandchannel.h"
#include "Strategies/differential_movement_strategy.h"
#include "Strategies/firing_strategy.h"
#include <mutex>
#include <thread>

class Control
{
private:

    StateParser* parser;
    CommandChannel* cmdChannel;

public:
    Strategy* movementStrategy;
    Strategy* firingStrategy;
    Control(StateParser *parser, CommandChannel *cmdChannel); // Constructor that parses the game state response message
    ~Control(){
        delete parser;
        delete cmdChannel;
    }

    void Run();

    std::thread Start();
};

#endif