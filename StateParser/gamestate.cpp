#include "gamestate.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "../util.h"
#include <iostream>
#include <string>
#include <Python/Python.h>

GameState::GameState(rapidjson::Document &dom)
{
    rapidjson::Value& tr = dom["timeRemaining"];
    timeRemaining = tr.GetDouble();
    tr = dom["timestamp"];
    timestamp = tr.GetDouble();
    map = MapState(dom["map"]);

    // Parse players array
    const rapidjson::Value& players = dom["players"];
    assert(players.IsArray());
    Player firstPlayer;
    Player secondPlayer;
    if(players.Size() == 1){
        firstPlayer = Player(players[0]);
    }else{
        firstPlayer = Player(players[0]);
        secondPlayer = Player(players[1]);
    }

    if(firstPlayer.name == "Think Tank"){

        player = firstPlayer;
        opponent = secondPlayer;
    } else if(secondPlayer.name == "Think Tank"){
        player = secondPlayer;
        opponent = firstPlayer;
    }
    else {
        // Could not find our team in the map state!
        assert(false);
    }

    // make additional array of projectiles for easy access to all projectiles.
    if(player.alive) {
        projectiles.insert(projectiles.end(), player.TankFast.projectiles.begin(), player.TankFast.projectiles.end());
        projectiles.insert(projectiles.end(), player.TankSlow.projectiles.begin(), player.TankSlow.projectiles.end());

    }
    if(opponent.alive) {
        projectiles.insert(projectiles.end(), opponent.TankFast.projectiles.begin(), opponent.TankFast.projectiles.end());
        projectiles.insert(projectiles.end(), opponent.TankSlow.projectiles.begin(), opponent.TankSlow.projectiles.end());
    }

}

GameState::~GameState(){
}