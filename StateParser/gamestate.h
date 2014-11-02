#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "mapstate.h"
#include "playerstate.h"
#include <string>
#include "rapidjson/document.h"

class GameState
{
private:
    double timeRemaining;
    double timestamp;
    MapState* mapState;
    Player* opponent;
    Player* us;
    //GameState() { } // private default constructor
public:

    GameState(rapidjson::Document &dom); // Constructor that parses the game state response message
    ~GameState();
    MapState* GetMapState() { return mapState; }
    Player* GetOpponent() { return opponent; }
    Player* GetPlayer() { return us; }
    long GetTimeRemaining() { return timeRemaining; }
    long GetTimestamp() { return timestamp; }

    static GameState* ParseState(rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator, std::string &state);
    static GameState* ParseState(std::string &state);
};

#endif