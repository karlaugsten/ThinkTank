#include "stateparser.h"
#include <iostream>


std::mutex StateParser::stateLock;
// initialize to null
//GameState* StateParser::state = NULL;

StateParser::StateParser(zmq::socket_t* s){
    stateChannel = s;
    allocator = new rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>(buffer, sizeof(buffer));
}

StateParser::~StateParser() {
    // Do nothing for now;
    delete allocator;
}

/**
* Starts the state parsing thread
*/
std::thread StateParser::Start() {
    return std::thread(&StateParser::Run, this);
}

void StateParser::Run() {

    zmq::message_t msg;
    while (stateChannel->recv(&msg)) {
        stateChannel->recv(&msg);
        // TODO: This static_cast needs to be optimized!!
        std::string json = std::string(static_cast<char *>(msg.data()), msg.size());
        //std::cout << "Received: " << json << std::endl;
        ParseState(json);
        if(this->game.over) break;
    }

}

void StateParser::ParseState(std::string stateMsg){
    rapidjson::Document dom(allocator);
    dom.Parse(stateMsg.c_str());
    if (dom.HasParseError()) {
        std::cout << "ERROR: Parsing json. " << stateMsg << std::endl;
        return;
    }
    const rapidjson::Value &d_comm_type = dom["comm_type"];
    assert(d_comm_type.IsString());
    std::string comm_type = d_comm_type.GetString();
    if (comm_type == "GAMESTATE") {
        // update last game;
        lastGame = game;
        game = GameState(dom);
        game.paused = false;
        DetermineTimeLastFired();
        //StateParser::SetState(tmp);
    } else if (comm_type == "GAME_START") {
        // TODO: implement this somehow
        std::cout << "Recieved Game Start message: " << std::endl << stateMsg << std::endl;
        game.paused = false;
    } else if (comm_type == "GAME_END") {
        // TODO: implement this somehow
        std::cout << "Recieved Game End message: " << std::endl << stateMsg << std::endl;
        game.paused = true;
    } else if (comm_type == "MatchEnd") {
        // TODO: implement this somehow
        std::cout << "Recieved Match End message: " << std::endl << stateMsg << std::endl;
        game.over = true;
    }
    allocator->Clear();
}

void StateParser::DetermineTimeLastFired() {
    if(game.player.TankFast.minDistanceProjectile < lastGame.player.TankFast.minDistanceProjectile){
        game.player.TankFast.lastTimeFired = (game.timeRemaining) + (game.player.TankFast.minDistanceProjectile-game.player.TankFast.hitRadius)/30.0;
    } else {
        game.player.TankFast.lastTimeFired = lastGame.player.TankFast.lastTimeFired;
    }

    if(game.player.TankSlow.minDistanceProjectile < lastGame.player.TankSlow.minDistanceProjectile){
        game.player.TankSlow.lastTimeFired = (game.timeRemaining) + (game.player.TankSlow.minDistanceProjectile-game.player.TankSlow.hitRadius)/30.0;
    } else {
        game.player.TankSlow.lastTimeFired = lastGame.player.TankSlow.lastTimeFired;
    }

    if(game.opponent.TankFast.minDistanceProjectile < lastGame.opponent.TankFast.minDistanceProjectile){
        game.opponent.TankFast.lastTimeFired = (game.timeRemaining) + (game.opponent.TankFast.minDistanceProjectile-game.opponent.TankFast.hitRadius)/30.0;
    } else {
        game.opponent.TankFast.lastTimeFired = lastGame.opponent.TankFast.lastTimeFired;
    }

    if(game.opponent.TankSlow.minDistanceProjectile < lastGame.opponent.TankSlow.minDistanceProjectile){
        game.opponent.TankSlow.lastTimeFired = (game.timeRemaining) + (game.opponent.TankSlow.minDistanceProjectile-game.opponent.TankSlow.hitRadius)/30.0;
    } else {
        game.opponent.TankSlow.lastTimeFired = lastGame.opponent.TankSlow.lastTimeFired;
    }

    std::cout << "Fast Tank player last time: " << game.player.TankFast.lastTimeFired << std::endl;
}



/*

// Returns a copy of the game state
GameState* StateParser::GetState(){
    GameState* ret;
    stateLock.lock();
    if(StateParser::state == NULL) ret = NULL;
    else ret = state->Clone();
    stateLock.unlock();
    return ret;
}

void StateParser::SetState(GameState* newState){
    stateLock.lock();
    if(StateParser::state != NULL)
        delete StateParser::state;
    StateParser::state = newState;
    stateLock.unlock();
}
*/