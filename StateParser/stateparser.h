#ifndef STATEPARSER_H
#define STATEPARSER_H

#include <zmq.hpp>
#include "gamestate.h"

class StateParser
{
private:
    char buffer[20000];
    zmq::socket_t* stateChannel;
    rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>* allocator;
public:

    GameState* state;

    StateParser(zmq::socket_t *s); // Constructor that parses the game state response message
    ~StateParser();

    void Run();
};

#endif