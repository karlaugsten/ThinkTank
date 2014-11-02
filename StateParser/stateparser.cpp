#include "stateparser.h"
#include <iostream>

StateParser::StateParser(zmq::socket_t* s){
    stateChannel = s;

    allocator = new rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>(buffer, sizeof(buffer));

}

StateParser::~StateParser() {
    // Do nothing for now;
}

void StateParser::Run() {
    zmq::message_t msg;

    while(stateChannel->recv(&msg)){

        // TODO: This static_cast needs to be optimized!!
        std::string tmp = std::string(static_cast<char*>(msg.data()), msg.size());
        std::cout << "Recieved message: " << std::endl << tmp << std::endl;
        state = GameState::ParseState((*allocator), tmp);

        allocator->Clear();
    }
}