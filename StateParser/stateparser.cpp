#include "stateparser.h"
#include <iostream>

StateParser::StateParser(zmq::socket_t* s){
    stateChannel = s;

    allocator = new rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>(buffer, sizeof(buffer));

}

StateParser::~StateParser() {
    // Do nothing for now;
    delete allocator;
}

void StateParser::Run() {


    while(ReceiveAndParse());
}

bool StateParser::ReceiveAndParse() {
    zmq::message_t msg;
    if(stateChannel->recv(&msg)) {
        // TODO: This static_cast needs to be optimized!!
        std::string json = std::string(static_cast<char *>(msg.data()), msg.size());

        rapidjson::Document dom(allocator);
        dom.Parse(json.c_str());
        if (dom.HasParseError()) {
            std::cout << "ERROR: Parsing json. " << json << std::endl;
            return false;
        }
        const rapidjson::Value &d_comm_type = dom["comm_type"];
        assert(d_comm_type.IsString());
        std::string comm_type = d_comm_type.GetString();
        if (comm_type == "GAMESTATE") {
            SetState(new GameState(dom));
        } else if (comm_type == "GAME_START") {
            // TODO: implement this somehow
            std::cout << "Recieved Game Start message: " << std::endl << json << std::endl;
        } else if (comm_type == "GAME_END") {
            // TODO: implement this somehow
            std::cout << "Recieved Game End message: " << std::endl << json << std::endl;
        } else if (comm_type == "MatchEnd") {
            // TODO: implement this somehow
            std::cout << "Recieved Match End message: " << std::endl << json << std::endl;
        }
        allocator->Clear();
        return true;
    }
    return false;
}

// Returns a copy of the game state
GameState* StateParser::GetState(){
    GameState* ret;
    stateLock.lock();
    ret = state->Clone();
    stateLock.unlock();
    return ret;
}

void StateParser::SetState(GameState* newState){
    stateLock.lock();
    state = newState;
    stateLock.unlock();
}