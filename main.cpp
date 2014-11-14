#include <iostream>
#include "StateParser/gamestate.h"
#include "StateParser/stateparser.h"
#include "command.h"
#include "commandchannel.h"
#include "control.h"
#include "Strategies/movement_strategy.h"
#include "Strategies/differential_movement_strategy.h"


using namespace std;


int main(int argc, char* argv[]) {
    if(argc != 4) {
        cerr << "Program must be run with arguments: ./ThinkTank server_ip match_token password" << endl;
        return -1;
    }

    std::string server_ip = argv[1];
    std::string match_token = argv[2];
    std::string password = argv[3];

    zmq::context_t ctx (1);
    CommandChannel* cmdChannel = new CommandChannel(ctx, server_ip, match_token, password);

    zmq::socket_t sub (ctx, ZMQ_SUB);

    std::string state_channel_connection = "tcp://" + server_ip + ":5556";
    sub.connect (state_channel_connection.c_str());
    sub.setsockopt(ZMQ_SUBSCRIBE, match_token.c_str(), strlen(match_token.c_str()));

    // Spawn new thread for state parsing.
    StateParser* parser = new StateParser(&sub);


    Control* control = new Control(parser, cmdChannel);

    std::thread parserThread = parser->Start();
    std::thread controlThread = control->Start();
    // constantly wait for input
    string command;
    while(cin >> command){
        // dynamically change strategies based on command line input!
        if(command == "exit" || command == "quit"){
            exit(0);
        }
        if(command == "-c" || command == "circle"){
            Strategy* circleMovement = new MovementStrategy();
            control->movementStrategy = circleMovement;
        }
        if(command == "-d" || command == "differential"){
            // defaults to ternary search
            Strategy* diffMovement = new DifferentialMovementStrategy(true);
            control->movementStrategy = diffMovement;
        }
        if(command == "-dl" || command == "differential-linear"){
            // do a linear search instead of ternary
            Strategy* diffMovement = new DifferentialMovementStrategy(false);
            control->movementStrategy = diffMovement;
        }
        if(command == "-dt" || command == "differential-ternary"){
            // do a ternary search
            Strategy* diffMovement = new DifferentialMovementStrategy(true);
            control->movementStrategy = diffMovement;
        }
    }
    // Join and wait for threads to finish
    parserThread.join();
    controlThread.join();

    delete control;
    delete parser;

    return 0;
}
