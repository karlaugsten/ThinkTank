#include <iostream>
#include <fstream>
#include <string>
#include "StateParser/gamestate.h"
#include "StateParser/stateparser.h"
#include "command.h"
#include <zmq.hpp>
#include <thread>
#include <unistd.h>
#include "commandchannel.h"
#include <cmath>
#include "Strategies/movement_strategy.h"
#include "Strategies/differential_movement_strategy.h"
#include "Strategies/strategy.h"
#include "Strategies/firing_strategy.h"

using namespace std;


// Plots the gamestate using the goodness calculator for testing.
void plot_test(GameState &state) {
    DifferentialMovementStrategy strategy = DifferentialMovementStrategy();
    ofstream myfile;
    cout << state.map.width << " " << state.map.height << endl;
    myfile.open("/Users/karlaugsten/plot_test.out");
    for(int i = 1; i < 1000; i++){
        for(int j = 1; j < 1000; j++){
            double x = double(i)*state.map.width/1000.0;
            double y = double(j)*state.map.height/1000.0;
            double goodness = strategy.CalculateGoodness(state, state.player.TankSlow, x, y);
            if(goodness < -1E29) goodness = 10.0;
            myfile << x << "\t" << y << "\t" << goodness << endl;
        }
    }
    myfile.close();
}

int main(int argc, char* argv[]) {

    std::string map = "{\n"
            "\"comm_type\": \"GAMESTATE\",\n"
            "\"timeRemaining\" : 300000.00,\n"
            "\"timestamp\" : 145433123.00,\n"
            "\"map\" : {\n"
            "\"size\" : [100, 100],\n"
            "\"terrain\" : [\n"
            "{\n"
            "\"type\" : \"SOLID\",\n"
            "\"boundingBox\" : {\n"
            "\"corner\" : [5, 10],\n"
            "\"size\" : [6, 8]\n"
            "}\n"
            "},\n"
            "{\n"
            "\"type\" : \"IMPASSABLE\",\n"
            "\"boundingBox\" : {\n"
            "\"corner\" : [80, 80],\n"
            "\"size\" : [50, 80]\n"
            "}\n"
            "}\n"
            "]\n"
            "},\n" // End of Map
            "\"players\" : [\n"
            "{\n"
            "\"score\" : 1,\n"
            "\"name\" : \"Think Tank\",\n"
            "\"tanks\" : [\n"
            "{\n"
            "\"id\" : \"eb49f487-92dc-4f66-ac83-91ae04a4cc16\",\n"
            "\"health\" : 0,\n"
            "\"hitRadius\" : 2.0,\n"
            "\"collisionRadius\" : 1.0,\n"
            "\"type\" : \"TankSlow\",\n"
            "\"alive\" : true,\n"
            "\"position\" : [63, 33.2],\n"
            "\"tracks\" : 3.14,\n"
            "\"turret\": 0.0,\n"
            "\"speed\" : 5,\n"
            "\"projectiles\" : [\n"
            "{\n"
            "\"id\" : \"blahblah\",\n"
            "\"position\" : [50, 50],\n"
            "\"direction\" : 4.7123,\n"
            "\"speed\" : 30,\n"
            "\"damage\" : 100\n"
            "}\n"
            "]\n"
            "},\n"
            "{\n"
            "\"id\" : \"eb49f487-92dc-4f66-ac83-91ae04a4cc18\",\n"
            "\"health\" : 2,\n"
            "\"hitRadius\" : 1.5,\n"
            "\"collisionRadius\" : 0.75,\n"
            "\"type\" : \"TankFast\",\n"
            "\"alive\" : true,\n"
            "\"position\" : [53, 33.2],\n"
            "\"tracks\" : 3.14,\n"
            "\"turret\" : 0.0,\n"
            "\"speed\" : 10,\n"
            "\"projectiles\" : []\n"
            "}\n"
            "]\n"
            "},\n" // end of player 1
            "{\n"
            "\"score\" : 3,\n"
            "\"name\" : \"Terrible Team BCA\",\n"
            "\"tanks\" : [\n"
            "{\n"
            "\"id\" : \"eb49f487-92dc-4f66-ac83-91ae04a4cc19\",\n"
            "\"health\" : 0,\n"
            "\"hitRadius\" : 2.0,\n"
            "\"collisionRadius\" : 1.0,\n"
            "\"type\" : \"TankSlow\",\n"
            "\"alive\" : true,\n"
            "\"position\" : [43, 33.2],\n"
            "\"tracks\" : 3.14,\n"
            "\"turret\": 0.0,\n"
            "\"speed\" : 5,\n"
            "\"projectiles\" : []\n"
            "},\n"
            "{\n"
            "\"id\" : \"eb49f487-92dc-4f66-ac83-91ae04a4cc15\",\n"
            "\"health\" : 2,\n"
            "\"hitRadius\" : 1.5,\n"
            "\"collisionRadius\" : 0.75,\n"
            "\"type\" : \"TankFast\",\n"
            "\"alive\" : true,\n"
            "\"position\" : [23, 33.2],\n"
            "\"tracks\" : 3.14,\n"
            "\"turret\" : 0.0,\n"
            "\"speed\" : 10,\n"
            "\"projectiles\" : []\n"
            "} ] } ]\n" // end of players array
            "}";
    // simply parse test state
    StateParser parser = StateParser(NULL);
    parser.ParseState(map);
    GameState& state = parser.game;

    plot_test(state);
    return 0;
}
