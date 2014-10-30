#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "mapstate.h"
#include "playerstate.h"
#include <string>
#include "rapidjson/document.h"

class GameState
{
private:
    long timeRemaining;
    long timestamp;
    MapState* mapState;
    Player* opponent;
    Player* us;
    //GameState() { } // private default constructor
public:

    GameState(rapidjson::Document &dom); // Constructor that parses the game state response message

    MapState* GetMapState() { return mapState; }
    Player* GetOpponent() { return opponent; }
    Player* GetPlayer() { return us; }
    long GetTimeRemaining() { return timeRemaining; }
    long GetTimestamp() { return timestamp; }

    static GameState* ParseState(std::string state);

};

#endif