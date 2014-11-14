#include "differential_movement_strategy.h"
#include "../StateParser/gamestate.h"
#include <iostream>
#include <cmath>
#include "../util.h"

double optimalDistance(Position from, Position to, double dist, double amplitude){
    return amplitude*cos(acos(-1)*from.Distance(to)/dist) * exp(-(from.Distance(to)*from.Distance(to))/(dist*dist));
}

double degradingGaussian(Position inFrontOf, double angle, double x, double y, double amplitude, double degredation){
    double goodness = 0.0;
    double theta1 = angle - acos(-1)/2.0;
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

double DifferentialMovementStrategy::CalculateGoodness(GameState &state, GameState& previousState, Tank &tank, Tank &otherTank, double x, double y){
    double goodness = 0.0;
    Position current = Position(x,y);
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
            goodness -= (10.0/((y - t.position.y)*(y - t.position.y)));
            goodness -= (10.0/((y - (t.position.y + t.size.y))*(y - (t.position.y + t.size.y))));
        }

        if(y > t.position.y && y < t.position.y + t.size.y){
            goodness -= (10.0/((x - t.position.x)*(x - t.position.x)));
            goodness -= (10.0/((x - (t.position.x + t.size.x))*(x - (t.position.x + t.size.x))));
        }
        if(y > t.position.y && y < t.position.y + t.size.y && x > t.position.x && x < t.position.x + t.size.x) return -1E30;
        goodness -= (10.0/(current.Distance(t.position)*current.Distance(t.position)));
        goodness -= 10.0/(current.Distance(Position(t.position.x, t.position.y + t.size.y))* current.Distance(Position(t.position.x, t.position.y + t.size.y)));
        goodness -= 10.0/(current.Distance(Position(t.position.x + t.size.x, t.position.y))* current.Distance(Position(t.position.x + t.size.x, t.position.y)));
        goodness -= 10.0/(current.Distance(Position(t.position.x + t.size.x, t.position.y + t.size.y))* current.Distance(Position(t.position.x + t.size.x, t.position.y + t.size.y)));

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
        // TODO: make it really bad if you are super close to them
        goodness -= optimalDistance(otherTank.position, Position(x,y), dist, 10.0);
    }

    if(state.opponent.alive) {
        if(state.opponent.TankSlow.alive) {
            // if tank is fast, we can get closer to enemy
            double dist;
            if(tank.Type == TankType::FAST){
                dist = 25.0;
            } else {
                dist = 60.0;
            }
            goodness -= optimalDistance(state.opponent.TankSlow.position, Position(x,y), dist, 5.0);

            // reduce according to gaussian infront of turret
            goodness += degradingGaussian(state.opponent.TankSlow.position, state.opponent.TankSlow.turret, x, y, 8.0, 8.0/std::max(state.map.height, state.map.width));

        }
        if(state.opponent.TankFast.alive){

            // if tank is fast, we can get closer to enemy
            double dist;
            if(tank.Type == TankType::FAST){
                dist = 25.0;
            } else {
                dist = 60.0;
            }
            goodness -= optimalDistance(state.opponent.TankFast.position, Position(x,y), dist, 5.0);

            // reduce according to gaussian infront of turret
            goodness += degradingGaussian(state.opponent.TankFast.position, state.opponent.TankFast.turret, x, y, 8.0, 8.0/std::max(state.map.height, state.map.width));

        }
    }

    // For every line of sight, subtract some function
    // find the minimum distance to the line of sight polygon.
    Position cur(x,y);
    double minDist = 1E30;
    for(int i = 0; i < state.fastLineOfSight.size(); i++){


        Position p = state.fastLineOfSight[i];
        Position q = state.fastLineOfSight[(i+1)%state.fastLineOfSight.size()];
        Position c;
        minDist = std::min(Util::distToLineSegment(cur, p, q, c), fabs(minDist));
        /*
        // point lies on some line of sight
        if(curLine.dir.x*(x-curLine.from.x) + curLine.dir.y*(y-curLine.from.y) > 0.0 && curLine.dir.x*(x-curLine.to.x) + curLine.dir.y*(y-curLine.to.y) < 0.0){
            double c = crossProduct(curLine.from - curLine.onSide, curLine.from - curLine.to);
            Position curPos = Position(x,y);
            double b = crossProduct(curLine.from - curPos, curLine.from - curLine.to);
            // they are on the same side
            // we should do checking if our guy has fired recently or not.
            if(c < 0.0){
                goodness -= 8.0/(1.0 + exp(-0.1*(-curLine.dir.y*(x-curLine.from.x) + curLine.dir.x*(y-curLine.from.y))));
            }
            else {
                // we should do checking for
                goodness -= (8.0 - (8.0/(1.0 + exp(-0.1*(-curLine.dir.y*(x-curLine.from.x) + curLine.dir.x*(y-curLine.from.y))))));
            }

        }*/
    }
    double time = 3.0;
    if(tank.Type == TankType::FAST){
        time = 5.0;
    }
    if(tank.lastTimeFired - state.timeRemaining > time) {
        if (Util::insidePolygon(cur, state.fastLineOfSight)) {
            goodness += 0.1 * minDist;

        } else {
            goodness -= 0.1 * minDist;
        }
    } else {
        if (Util::insidePolygon(cur, state.fastLineOfSight)) {
            goodness -= 0.1 * minDist;

        } else {
            goodness += 0.1 * minDist;
        }
    }

    // if the new position is in the current direction of travel, give it some kind of boost.
    /*Position dPos;
    if(tank.Type == TankType::SLOW){
        dPos = previousState.player.TankSlow.position - tank.position;
    } else {
        dPos = previousState.player.TankFast.position - tank.position;
    }*/
    // it has actually moved some amount

    /*if(dPos.Norm2() > 0.1) {
        Position cPos = tank.position - Position(x,y);
        double dAngle = dPos.GetAngle();
        double cAngle = cPos.GetAngle();

        if (fabs(dAngle - cAngle) < acos(-1)/2.0){
            // add goodness to try and go the current direction over switching.
            goodness += 0.001;
        }
    }*/

    return goodness;

}

double DifferentialMovementStrategy::CalculateGoodness(GameState &state, GameState &previousState, Tank &tank, Tank &otherTank, Position &position){
    return CalculateGoodness(state, previousState, tank, otherTank, position.x, position.y);
}

std::queue<Command*> DifferentialMovementStrategy::DetermineActions(GameState &state, GameState &previousState) {
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
        double currentgoodness = CalculateGoodness(state, previousState, state.player.TankSlow, state.player.TankFast, curPos);
        for(int t = 0; t < 1000; t++){
            double dx = r*cos(double(t)*2*acos(-1)/1000.0);
            double dy = r*sin(double(t)*2*acos(-1)/1000.0);
            double goodness = CalculateGoodness(state, previousState, state.player.TankSlow, state.player.TankFast, curPos.x+dx, curPos.y+dy);
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

        // check if tracks are pointing in proper direction
        Position dir = Position(state.player.TankSlow.position.x + r*cos(angle), state.player.TankSlow.position.y + r*sin(angle));

        // it might be better to go backwards instead of forwards!
        if(fabs(angle) > acos(-1)/2.0){
            moves.push(new RotateCommand(angle - acos(-1), state.player.TankSlow.id));
            moves.push(new MoveCommand(10.0, state.player.TankSlow.id, true));
        } else {
            moves.push(new RotateCommand(angle, state.player.TankSlow.id));
            moves.push(new MoveCommand(10.0, state.player.TankSlow.id));
        }

    }
    if(state.player.TankFast.alive) {
        // for now do a linear search for where the goodness is increasing with radius 1.
        double r = 0.5;
        Position curPos = state.player.TankFast.position;
        double mx = 1E30;
        double my = 1E30;
        double maxgoodness = -1E30;
        double currentgoodness = CalculateGoodness(state, previousState, state.player.TankFast, state.player.TankSlow, curPos);
        for(int t = 0; t < 1000; t++){
            double dx = r*cos(double(t)*2*acos(-1)/1000.0);
            double dy = r*sin(double(t)*2*acos(-1)/1000.0);
            double goodness = CalculateGoodness(state, previousState, state.player.TankFast, state.player.TankSlow, curPos.x+dx, curPos.y+dy);
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

        // it might be better to go backwards instead of forwards!
        if(fabs(angle) > acos(-1)/2.0){
            moves.push(new RotateCommand(angle - acos(-1), state.player.TankFast.id));
            moves.push(new MoveCommand(10.0, state.player.TankFast.id, true));
        } else {
            moves.push(new RotateCommand(angle, state.player.TankFast.id));
            moves.push(new MoveCommand(10.0, state.player.TankFast.id));
        }


    }

    return moves;
}