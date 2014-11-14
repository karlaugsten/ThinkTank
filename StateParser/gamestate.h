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

    /**
    * A vector that contains segments which are the enemies slow tanks line of sights
    */
    std::vector<Position> slowLineOfSight;

    /**
    * A vector that contains segments which are the enemies fast tanks line of sights.
    */
    std::vector<Position> fastLineOfSight;

    GameState() { } // private default constructor
    GameState(rapidjson::Document &dom); // Constructor that parses the game state response message
    ~GameState();

    void CalculateLineOfSight();

};

#endif