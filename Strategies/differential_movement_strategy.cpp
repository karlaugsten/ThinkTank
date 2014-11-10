#include "differential_movement_strategy.h"
#include "../StateParser/gamestate.h"

double DifferentialMovementStrategy::CalculateGoodness(GameState &state, double x, double y){
    double goodness = 0.0;
    // Add subtract 1/r^2 for the outer walls
    goodness -= 1.0/(x*x);
    goodness -= 1.0/(y*y);
    goodness -= 1.0/((x - state.map.width)*(x - state.map.width));
    goodness -= 1.0/((y - state.map.height)*(y - state.map.height));

    for(int i = 0; i < state.map.terrain.size(); i++){
        Terrain t = state.map.terrain[i];
        if(x > t.position.x && x < t.position.x + t.size.x){
            goodness -= 1.0/((y - t.position.y)*(y - t.position.y));
            goodness -= 1.0/((y - (t.position.y + t.size.y))*(y - (t.position.y + t.size.y)));
        }

        if(y > t.position.x && y < t.position.x + t.size.x){
            goodness -= 1.0/((x - t.position.x)*(x - t.position.x));
            goodness -= 1.0/((x - (t.position.x + t.size.x))*(x - (t.position.x + t.size.x)));
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
    // TODO: Add in calculation for terrain object walls
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
        double r = 0.1;
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
        double tmp = curPos.GetAngle(bestPos) - angle;
        if (tmp > acos(-1)) {
            tmp -= 2 * acos(-1);
        }
        if (tmp < -acos(-1)) {
            tmp += 2 * acos(-1);
        }
        angle = tmp;
        // TODO: Check for unbreakable terrain
        moves.push(new RotateCommand(angle, state.player.TankSlow.id));
        moves.push(new MoveCommand(1.0, state.player.TankSlow.id));
    }
    if(state.player.TankFast.alive) {
        // for now do a linear search for where the goodness is increasing with radius 1.
        double r = 0.1;
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
        double tmp = curPos.GetAngle(bestPos) - angle;
        if (tmp > acos(-1)) {
            tmp -= 2 * acos(-1);
        }
        if (tmp < -acos(-1)) {
            tmp += 2 * acos(-1);
        }
        angle = tmp;
        // TODO: Check for unbreakable terrain
        moves.push(new RotateCommand(angle, state.player.TankFast.id));
        moves.push(new MoveCommand(1.0, state.player.TankFast.id));
    }

    return moves;
}