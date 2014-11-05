#ifndef STATEPARSER_H
#define STATEPARSER_H

#include <zmq.hpp>
#include "gamestate.h"
#include <mutex>

class StateParser
{
private:

    char buffer[20000];
    zmq::socket_t* stateChannel;
    rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>* allocator;
    static GameState* state;
    static std::mutex stateLock;

public:
    static GameState* GetState();
    static void SetState(GameState* state);

    StateParser(zmq::socket_t *s); // Constructor that parses the game state response message
    ~StateParser();

    void ParseState(std::string stateMsg);

    void Run();
};

#endif