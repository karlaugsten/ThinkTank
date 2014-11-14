#include "control.h"
#include <iostream>

Control::Control(StateParser *p, CommandChannel *ch){
    parser = p;
    cmdChannel = ch;
}

/**
* Starts the state parsing thread
*/
std::thread Control::Start() {
    return std::thread(&Control::Run, this);
}

void Control::Run() {
    GameState state;
    GameState previousState;
    movementStrategy = new DifferentialMovementStrategy(true);
    firingStrategy = new FiringStrategy();
    // Algorithm does stuff here!
    try {
        while (!state.over) {
            // TODO: not thread safe
            state = parser->game;
            previousState = parser->lastGame;
            if(state.over) return;
            while(state.paused){
                state = parser->game;
            }
            cmdChannel->SendCommands(firingStrategy->DetermineActions(state, previousState));
            cmdChannel->SendCommands(movementStrategy->DetermineActions(state, previousState));
        }
    } catch(const std::exception e){
        std::cerr << "ERROR: Recieved exception: " << e.what() << std::endl;
    }
}