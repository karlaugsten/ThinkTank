#include "gamestate.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <string>

GameState::GameState(rapidjson::Document &dom)
{
    rapidjson::Value::MemberIterator tr = dom.FindMember("timeRemaining");
    timeRemaining = tr->value.GetDouble();
    tr = dom.FindMember("timestamp");
    timestamp = tr->value.GetDouble();
    mapState = new MapState(dom["map"]);

    // Parse players array
    const rapidjson::Value& players = dom["players"];
    assert(players.IsArray());

    Player* firstPlayer = new Player(players[0]);
    Player* secondPlayer = new Player(players[1]);
    if(firstPlayer->name == "Think Tank"){

        us = firstPlayer;
        opponent = secondPlayer;
    } else if(secondPlayer->name == "Think Tank"){
        us = secondPlayer;
        opponent = firstPlayer;
    }
    else {
        // Could not find our team in the map state!
        assert(false);
    }
}

GameState::~GameState(){
    delete mapState;
    delete us;
    delete opponent;
}

GameState* GameState::Clone(){
    GameState* clone = new GameState();
    if(this->mapState != NULL)
        clone->mapState = this->mapState->Clone();
    clone->timeRemaining = this->timeRemaining;
    clone->timestamp = this->timestamp;
    if(this->opponent != NULL) {
        clone->opponent = this->opponent->Clone();
    }
    if(this->us != NULL) {
        clone->us = this->opponent->Clone();
    }
    return clone;
}
