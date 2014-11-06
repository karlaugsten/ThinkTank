#include "gamestate.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <string>

GameState::GameState(rapidjson::Document &dom)
{
    rapidjson::Value& tr = dom["timeRemaining"];
    timeRemaining = tr.GetDouble();
    tr = dom["timestamp"];
    timestamp = tr.GetDouble();
    mapState = MapState(dom["map"]);

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

        us = firstPlayer;
        opponent = secondPlayer;
    } else if(secondPlayer.name == "Think Tank"){
        us = secondPlayer;
        opponent = firstPlayer;
    }
    else {
        // Could not find our team in the map state!
        assert(false);
    }
}

GameState::~GameState(){
}

