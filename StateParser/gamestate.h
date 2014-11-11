#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "mapstate.h"
#include "playerstate.h"
#include <string>
#include "rapidjson/document.h"

class GameState
{
private:



public:
    double timeRemaining;
    double timestamp;
    MapState map;
    Player opponent;
    Player player;
    bool paused = true;
    bool over = false;
    // for easy access keep a list of all projectiles here
    std::vector<Projectile> projectiles;

    GameState() { } // private default constructor
    GameState(rapidjson::Document &dom); // Constructor that parses the game state response message
    ~GameState();

    long GetTimeRemaining() { return timeRemaining; }
    long GetTimestamp() { return timestamp; }

};

#endif