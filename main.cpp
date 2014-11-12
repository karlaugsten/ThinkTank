#include <iostream>
#include "StateParser/gamestate.h"
#include "StateParser/stateparser.h"
#include "command.h"
#include "commandchannel.h"
#include "control.h"


using namespace std;


int main(int argc, char* argv[]) {
    if(argc != 4) {

        cout << "Program must be run with arguments: ./ThinkTank server_ip match_token password" << endl;
        return -1;
    }

    std::string server_ip = argv[1];
    std::string match_token = argv[2];
    std::string password = argv[3];
    cout << "Server IP is: " << server_ip << endl;
    cout << "Match Token is: " << match_token << endl;

    zmq::context_t ctx (1);
    CommandChannel cmdChannel = CommandChannel(ctx, server_ip, match_token, password);

    zmq::socket_t sub (ctx, ZMQ_SUB);

    std::string state_channel_connection = "tcp://" + server_ip + ":5556";
    sub.connect (state_channel_connection.c_str());
    sub.setsockopt(ZMQ_SUBSCRIBE, match_token.c_str(), strlen(match_token.c_str()));

    // Spawn new thread for state parsing.
    StateParser parser = StateParser(&sub);

    Control control = Control(parser, cmdChannel);

    std::thread parserThread = parser.Start();
    std::thread controlThread = control.Start();
    // Join and wait for threads to finish
    parserThread.join();
    controlThread.join();

    return 0;
}
