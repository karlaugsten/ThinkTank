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
    MapState mapState;
    Player opponent;
    Player us;

public:
    GameState() { } // private default constructor
    GameState(rapidjson::Document &dom); // Constructor that parses the game state response message
    ~GameState();
    GameState Clone();
    MapState GetMapState() { return mapState; }
    Player GetOpponent() { return opponent; }
    Player GetPlayer() { return us; }
    long GetTimeRemaining() { return timeRemaining; }
    long GetTimestamp() { return timestamp; }

};

#endif