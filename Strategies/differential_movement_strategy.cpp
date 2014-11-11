#include "differential_movement_strategy.h"
#include "../StateParser/gamestate.h"
#include <iostream>

double DifferentialMovementStrategy::CalculateGoodness(GameState &state, double x, double y){
    double goodness = 0.0;
    // Add subtract 1/r^2 for the outer walls
    if(y < 0 || y > state.map.height || x < 0 || x > state.map.width) return -1E30;
    goodness -= (20.0/(x*x));
    goodness -= (20.0/(y*y));
    goodness -= (20.0/((x - state.map.width)*(x - state.map.width)));
    goodness -= (20.0/((y - state.map.height)*(y - state.map.height)));

    // subtract 1/d^2 for distance to terrain objects.
    for(int i = 0; i < state.map.terrain.size(); i++){
        Terrain t = state.map.terrain[i];
        if(x > t.position.x && x < t.position.x + t.size.x){
            goodness -= (1.0/((y - t.position.y)*(y - t.position.y)));
            goodness -= (1.0/((y - (t.position.y + t.size.y))*(y - (t.position.y + t.size.y))));
        }

        if(y > t.position.y && y < t.position.y + t.size.y){
            goodness -= (1.0/((x - t.position.x)*(x - t.position.x)));
            goodness -= (1.0/((x - (t.position.x + t.size.x))*(x - (t.position.x + t.size.x))));
        }
        if(y > t.position.y && y < t.position.y + t.size.y && x > t.position.x && x < t.position.x + t.size.x) return -1E30;
        // TODO: for every corner of the terrain object, subtract 1/(distancetocorner)^2
    }

    // subtract -Aexp(-(a*(x-b) + c*(y-d))^2/B) function if tank is in front of projectile
    for(int i = 0; i < state.projectiles.size(); i++){
        // determine if position is in front of projectile
        Projectile p = state.projectiles[i];
        double theta1 = p.direction - acos(-1)/2.0;
        Position A = Position(p.position.x - cos(theta1), p.position.y + sin(theta1));
        Position B = Position(p.position.x + cos(theta1), p.position.y - sin(theta1));
        // Determine cross product for direction
        double test = (B.x - A.x)*(y-A.y) - (B.y - A.y)*(x - A.x);
        // if test is positive point x,y is in front of projectile
        if(test > 0.0){
            double a = -cos(theta1);
            double c = sin(theta1);
            double b = p.position.x;
            double d = p.position.y;
            // TODO: make this function degrade the farther x,y is from projectile.
            goodness -= 5.0*exp(-(a*(x-b) + c*(y-d))*(a*(x-b) + c*(y-d)));
        }
    }

    if(state.opponent.alive) {
        if(state.opponent.TankSlow.alive) {
            goodness -= (1.0 / (state.opponent.TankSlow.position.Distance(Position(x, y))));
        }
        if(state.opponent.TankFast.alive){
            goodness -= (1.0 / (state.opponent.TankFast.position.Distance(Position(x, y))));
        }
    }
    // TODO: Add in calculation for projectiles

    return goodness;

}

double DifferentialMovementStrategy::CalculateGoodness(GameState &state, Position &position){
    return CalculateGoodness(state, position.x, position.y);
}

std::queue<Command*> DifferentialMovementStrategy::DetermineActions(GameState &state) {
    // For now simply move in a circle!

    std::queue<Command* > moves;
    if(!state.player.alive) return moves;
    if(state.player.TankSlow.alive) {
        // for now do a linear search for where the goodness is increasing with radius 1.
        double r = 0.5;
        Position curPos = state.player.TankSlow.position;
        double mx = 1E30;
        double my = 1E30;
        double maxgoodness = -1E30;
        double currentgoodness = CalculateGoodness(state, curPos);
        for(int t = 0; t < 1000; t++){
            double dx = r*cos(2*acos(-1)/double(t));
            double dy = r*sin(2*acos(-1)/double(t));
            double goodness = CalculateGoodness(state, curPos.x+dx, curPos.y+dy);
            if(goodness - currentgoodness > maxgoodness){
                maxgoodness = goodness - currentgoodness;
                mx = dx;
                my = dy;
            }
        }
        // best position is now in mx, my;
        Position bestPos = Position(curPos.x + mx, curPos.y + my);
        double angle = state.player.TankSlow.tracks;
        angle = curPos.GetAngle(bestPos) - angle;

        moves.push(new RotateCommand(angle, state.player.TankSlow.id));
        // check if tracks are pointing in proper direction
        angle = state.player.TankSlow.tracks;
        Position dir = Position(state.player.TankSlow.position.x + r*cos(angle), state.player.TankSlow.position.y + r*sin(angle));
        double goodness = CalculateGoodness(state, dir);

        moves.push(new MoveCommand(10.0, state.player.TankSlow.id));

    }
    if(state.player.TankFast.alive) {
        // for now do a linear search for where the goodness is increasing with radius 1.
        double r = 0.5;
        Position curPos = state.player.TankFast.position;
        double mx = 1E30;
        double my = 1E30;
        double maxgoodness = -1E30;
        double currentgoodness = CalculateGoodness(state, curPos);
        for(int t = 0; t < 1000; t++){
            double dx = r*cos(2*acos(-1)/double(t));
            double dy = r*sin(2*acos(-1)/double(t));
            double goodness = CalculateGoodness(state, curPos.x+dx, curPos.y+dy);
            if(goodness - currentgoodness > maxgoodness){
                maxgoodness = goodness - currentgoodness;
                mx = dx;
                my = dy;
            }
        }
        // best position is now in mx, my;
        Position bestPos = Position(curPos.x + mx, curPos.y + my);
        double angle = state.player.TankFast.tracks;
        angle = curPos.GetAngle(bestPos) - angle;

        moves.push(new RotateCommand(angle, state.player.TankFast.id));

        std::cout << CalculateGoodness(state, state.player.TankFast.position) << std::endl;

        Position dir = Position(state.player.TankFast.position.x + r*cos(angle), state.player.TankFast.position.y + r*sin(angle));
        double goodness = CalculateGoodness(state, dir);

        moves.push(new MoveCommand(10.0, state.player.TankFast.id));

    }

    return moves;
}