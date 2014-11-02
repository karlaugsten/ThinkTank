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
    if(strcmp(firstPlayer->name.c_str(), "Think Tank")){

        us = firstPlayer;
        opponent = secondPlayer;
    } else if(strcmp(secondPlayer->name.c_str(), "Think Tank")){
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

GameState* GameState::ParseState(rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator, std::string &json){
    rapidjson::Document dom(&allocator);
    dom.Parse(json.c_str());
    if(dom.HasParseError()){
        return NULL;
    }
    return new GameState(dom);

}

GameState* GameState::ParseState(std::string &json){
    rapidjson::Document dom;
    dom.Parse(json.c_str());
    if(dom.HasParseError()){
        return NULL;
    }
    return new GameState(dom);

}
