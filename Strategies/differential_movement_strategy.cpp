#include "differential_movement_strategy.h"
#include "../StateParser/gamestate.h"
#include <iostream>
#include <cmath>
#include "../util.h"

double optimalDistance(Position from, Position to, double dist, double amplitude){
    return amplitude*cos(Util::PI*from.Distance(to)/dist) * exp(-(from.Distance(to)*from.Distance(to))/(dist*dist));
}

double degradingGaussian(Position inFrontOf, double angle, double x, double y, double amplitude, double degredation){
    double goodness = 0.0;
    double theta1 = angle - Util::PI/2.0;
    Position A = Position(inFrontOf.x + cos(theta1), inFrontOf.y + sin(theta1));
    Position B = Position(inFrontOf.x - cos(theta1), inFrontOf.y - sin(theta1));
    // Determine cross product for direction
    double test = (B.x - A.x)*(y-A.y) - (B.y - A.y)*(x - A.x);
    // if test is positive point x,y is in front of projectile
    if(test < 0.0){
        double a = cos(theta1);
        double c = sin(theta1);
        double b = inFrontOf.x;
        double d = inFrontOf.y;
        goodness -= (-degredation*(-c*(x-b) + a*(y-b)) + amplitude)*exp(-(a*(x-b) + c*(y-d))*(a*(x-b) + c*(y-d))/15.0);
    }
    return goodness;
}

double DifferentialMovementStrategy::CalculateGoodness(const GameState &state, const GameState& previousState, const Tank &tank, const Tank &otherTank, const double x, const double y){
    double goodness = 0.0;
    Position current = Position(x,y);
    // Add subtract 1/r^2 for the outer walls
    if(y < 0 || y > state.map.height || x < 0 || x > state.map.width) return -1E30;
    goodness -= (20.0/(x*x));
    goodness -= (20.0/(y*y));
    goodness -= (20.0/((x - state.map.width)*(x - state.map.width)));
    goodness -= (20.0/((y - state.map.height)*(y - state.map.height)));

    if(state.timeRemaining < 5.0){
        goodness -= ((100 - state.timeRemaining*state.timeRemaining*state.timeRemaining)/(y*y));
        goodness -= ((100 - state.timeRemaining*state.timeRemaining*state.timeRemaining)/((y - state.map.height)*(y - state.map.height)));
    }

    // subtract 1/d^2 for distance to terrain objects.
    for(int i = 0; i < state.map.terrain.size(); i++){
        Terrain t = state.map.terrain[i];
        if(x > t.position.x && x < t.position.x + t.size.x){
            goodness -= (10.0/((y - t.position.y)*(y - t.position.y)));
            goodness -= (10.0/((y - (t.position.y + t.size.y))*(y - (t.position.y + t.size.y))));
        }

        if(y > t.position.y && y < t.position.y + t.size.y){
            goodness -= (10.0/((x - t.position.x)*(x - t.position.x)));
            goodness -= (10.0/((x - (t.position.x + t.size.x))*(x - (t.position.x + t.size.x))));
        }
        if(y > t.position.y && y < t.position.y + t.size.y && x > t.position.x && x < t.position.x + t.size.x) return -1E30;

        if(x < t.position.x && y < t.position.y){
            goodness -= (10.0/(current.Distance(t.position)*current.Distance(t.position)));
        } else if( y > t.position.y + t.size.y && x < t.position.x) {
            goodness -= 10.0 / (current.Distance(Position(t.position.x, t.position.y + t.size.y)) * current.Distance(Position(t.position.x, t.position.y + t.size.y)));
        } else if(x > t.position.x + t.size.x && y < t.position.y) {
            goodness -= 10.0 / (current.Distance(Position(t.position.x + t.size.x, t.position.y)) * current.Distance(Position(t.position.x + t.size.x, t.position.y)));
        } else if(x > t.position.x + t.size.x && y > t.position.y + t.size.y) {
            goodness -= 10.0 / (current.Distance(Position(t.position.x + t.size.x, t.position.y + t.size.y)) * current.Distance(Position(t.position.x + t.size.x, t.position.y + t.size.y)));
        }
    }

    // subtract -Aexp(-(a*(x-b) + c*(y-d))^2/B) function if tank is in front of projectile
    for(int i = 0; i < state.projectiles.size(); i++){
        // determine if position is in front of projectile
        Projectile p = state.projectiles[i];
        goodness += degradingGaussian(p.position, p.direction, x, y, 20.0, 20.0/std::max(state.map.height, state.map.width));

    }

    // Try to stay away from other tank!
    if(otherTank.alive){
        double dist = 70.0;
        goodness -= 3.0*exp(-(tank.position.Distance(otherTank.position)*tank.position.Distance(otherTank.position))/(8.0*8.0));
        goodness -= optimalDistance(otherTank.position, Position(x,y), dist, 10.0);
    }

    if(state.opponent.alive) {
        if(state.opponent.TankSlow.alive) {
            // if tank is fast, we can get closer to enemy
            double dist;
            if(tank.Type == TankType::FAST){
                dist = 40.0;
            } else {
                dist = 80.0;
            }

            goodness -= 3.0*exp(-(tank.position.Distance(state.opponent.TankSlow.position)*tank.position.Distance(state.opponent.TankSlow.position))/(8.0*8.0));

            goodness -= optimalDistance(state.opponent.TankSlow.position, Position(x,y), dist, 5.0);

            // reduce according to gaussian infront of turret
            goodness += degradingGaussian(state.opponent.TankSlow.position, state.opponent.TankSlow.turret, x, y, 8.0, 8.0/std::max(state.map.height, state.map.width));

        }
        if(state.opponent.TankFast.alive){

            // if tank is fast, we can get closer to enemy
            double dist;
            if(tank.Type == TankType::FAST){
                dist = 50.0;
            } else {
                dist = 80.0;
            }

            goodness -= 3.0*exp(-(tank.position.Distance(state.opponent.TankFast.position)*tank.position.Distance(state.opponent.TankFast.position))/(8.0*8.0));

            goodness -= optimalDistance(state.opponent.TankFast.position, Position(x,y), dist, 5.0);

            // reduce according to gaussian infront of turret
            goodness += degradingGaussian(state.opponent.TankFast.position, state.opponent.TankFast.turret, x, y, 8.0, 8.0/std::max(state.map.height, state.map.width));
        }
    }

    return goodness;

}

double DifferentialMovementStrategy::CalculateGoodness(const GameState &state, const GameState &previousState, const Tank &tank, const Tank &otherTank, const Position &position){
    return CalculateGoodness(state, previousState, tank, otherTank, position.x, position.y);
}

/*
 * Searches linearly around the tank for the best goodness
  */
Position DifferentialMovementStrategy::linearSearch(const GameState &state, const GameState &previousState, const Tank &tank, const Tank &otherTank, const int iterations, const double r){
    Position curPos = tank.position;
    double mx = 1E30;
    double my = 1E30;
    double maxgoodness = -1E30;
    double currentgoodness = CalculateGoodness(state, previousState, tank, otherTank, curPos);
    for(int t = 0; t < 1000; t++){
        double dx = r*cos(double(t)*2*Util::PI/1000.0);
        double dy = r*sin(double(t)*2*Util::PI/1000.0);
        double goodness = CalculateGoodness(state, previousState, tank, otherTank, curPos.x+dx, curPos.y+dy);
        if(goodness - currentgoodness > maxgoodness){
            maxgoodness = goodness - currentgoodness;
            mx = dx;
            my = dy;
        }
    }
    return Position(curPos.x + mx, curPos.y + my);
}

/*
 * Does a ternary search on the goodness function to find the max goodness
  */
Position DifferentialMovementStrategy::ternarySearch(const GameState &state, const GameState &previousState, const Tank &tank, const Tank &otherTank, const int maxIterations, const double r){
    Position curPos = tank.position;
    double max = 1E30;
    double left = 1E-30;
    double right = 2*Util::PI;
    int it = 0;
    while(it++ < maxIterations){
        if(fabs(right - left) < 1E-3){
            max = (right + left) / 2.0;
            break;
        }
        double leftThird = left + (right - left)/3.0;
        double rightThird = right - (right - left)/3.0;
        double dx = r*cos(leftThird);
        double dy = r*sin(leftThird);
        double leftThirdGoodness = CalculateGoodness(state, previousState, tank, otherTank, curPos.x+dx, curPos.y+dy);
        dx = r*cos(rightThird);
        dy = r*sin(rightThird);
        double rightThirdGoodness = CalculateGoodness(state, previousState, tank, otherTank, curPos.x+dx, curPos.y+dy);
        if(leftThirdGoodness < rightThirdGoodness){
            left = leftThird;
        } else {
            right = rightThird;
        }
    }

    double dx = r*cos(max);
    double dy = r*sin(max);

    return Position(curPos.x + dx, curPos.y + dy);
}

std::queue<Command*> DifferentialMovementStrategy::DetermineActions(GameState &state, GameState &previousState) {
    // For now simply move in a circle!

    std::queue<Command* > moves;
    if(!state.player.alive) return moves;
    if(state.player.TankSlow.alive) {

        Position curPos = state.player.TankSlow.position;
        double r = 0.2;
        Position bestPos;
        if(ternary)
            bestPos = ternarySearch(state, previousState, state.player.TankSlow, state.player.TankFast, 1000, r);
        else
            bestPos = linearSearch(state, previousState, state.player.TankSlow, state.player.TankFast, 1000, r);
        double angle = state.player.TankSlow.tracks;
        angle = curPos.GetAngle(bestPos) - angle;

        // it might be better to go backwards instead of forwards!
        if(fabs(angle) > Util::PI/2.0){
            moves.push(new RotateCommand(angle - Util::PI, state.player.TankSlow.id));
            moves.push(new MoveCommand(10.0, state.player.TankSlow.id, true));
        } else {
            moves.push(new RotateCommand(angle, state.player.TankSlow.id));
            moves.push(new MoveCommand(10.0, state.player.TankSlow.id));
        }

    }
    if(state.player.TankFast.alive) {
        Position curPos = state.player.TankFast.position;
        double r = 0.2;
        Position bestPos;
        if(ternary)
            bestPos = ternarySearch(state, previousState, state.player.TankFast, state.player.TankSlow, 1000, r);
        else
            bestPos = linearSearch(state, previousState, state.player.TankFast, state.player.TankSlow, 1000, r);
        double angle = state.player.TankFast.tracks;
        angle = curPos.GetAngle(bestPos) - angle;

        // it might be better to go backwards instead of forwards!
        if(fabs(angle) > Util::PI/2.0){
            moves.push(new RotateCommand(angle - Util::PI, state.player.TankFast.id));
            moves.push(new MoveCommand(10.0, state.player.TankFast.id, true));
        } else {
            moves.push(new RotateCommand(angle, state.player.TankFast.id));
            moves.push(new MoveCommand(10.0, state.player.TankFast.id));
        }
    }

    return moves;
}