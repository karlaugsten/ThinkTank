#include "firing_strategy.h"
#include <cmath>
#include <string>

using namespace std;

Position FiringStrategy::getClosestTarget(GameState& state, Position& thisTank, string overloadClosest=""){
    double distanceBetweenTanksFast=-1;
    double distanceBetweenTanksSlow=-1;

    if(state.opponent.TankFast.alive) {//Check if each tank is alive and calculate its relative distance
        Position enemy1 = state.opponent.TankFast.position;
        distanceBetweenTanksFast=thisTank.Distance(enemy1);
    }
    if(state.opponent.TankSlow.alive) {
        Position enemy2 = state.opponent.TankSlow.position;
        distanceBetweenTanksSlow=thisTank.Distance(enemy2);
    }
    if(distanceBetweenTanksFast==-1){ //Check if there is only one tank as an option
        return state.opponent.TankSlow.position;
    }else if(distanceBetweenTanksSlow==-1){
        return state.opponent.TankFast.position;
    }else{
        if(distanceBetweenTanksFast>distanceBetweenTanksSlow){
            if(overloadClosest == "UnseeableTarget") return state.opponent.TankFast.position; //Closest tank is behind barricade
                return state.opponent.TankSlow.position;
        }else{
            if(overloadClosest == "UnseeableTarget") return state.opponent.TankSlow.position; //Closest tank is behind barricade
                return state.opponent.TankFast.position;
        }
    }
}

bool isIntercepted(Position tankPosition,double angle,Position a, Position b){
    // ref:http://rootllama.wordpress.com/2014/06/20/ray-line-segment-intersection-test-in-2d/
    // direction vector: http://math.stackexchange.com/questions/180874/convert-angle-radians-to-a-heading-vector
    double t1;
    double t2;
    Position v1 = Position(tankPosition.x-a.x, tankPosition.y-a.y);
    Position v2 = Position(b.x-a.x,b.y-a.y);
    Position v3 = Position(-sin(angle), cos(angle));
    t1 = abs(v2.x*v1.y-v2.y*v1.x)/(v2.x*v3.x+v2.y*v3.y);
    t2 = (v1.x*v3.x+v1.y*v3.y)/(v2.x*v3.x+v2.y*v3.y);
    if(t1 >= 0 && t2 >= 0 && t2 <= 1) {
        return true;
    }else{
        return false;
    }
}


bool isTargetInSight(vector<Terrain> vecTerrain,Position tankPosition,double angle) {
    bool isTargetable = true;

    for(int i=0;i<vecTerrain.size();i++) {
        if (isTargetable == false) break;
        Terrain terrain = vecTerrain.at(i);
        Position upperLeft = Position(terrain.position.x, terrain.position.y+terrain.size.y);
        Position upperRight = Position(terrain.position.x+terrain.size.x, terrain.position.y+terrain.size.y);
        Position lowerRight = Position(terrain.position.x+terrain.size.x, terrain.position.y);
        Position lowerLeft = terrain.position;
        if(terrain.Type==0) continue; // If the current terrain type is impassible, look for solid
            if(angle>0 && angle<=1.571){
                isTargetable =(isIntercepted(tankPosition,angle,lowerLeft,upperLeft) || isIntercepted(tankPosition,angle,upperLeft,upperRight));
            }else if(angle>1.571 && angle <= 3.142){
                isTargetable =(isIntercepted(tankPosition,angle,upperLeft,upperRight) || isIntercepted(tankPosition,angle,upperRight,lowerRight));
            }else if(angle>3.142 && angle <= 4.712){
             isTargetable =(isIntercepted(tankPosition,angle,lowerLeft,lowerRight) || isIntercepted(tankPosition,angle,lowerRight,upperRight));
            }else {
             isTargetable =(isIntercepted(tankPosition,angle,lowerLeft,upperLeft) || isIntercepted(tankPosition,angle,lowerLeft,lowerRight));
        }
    }

    return isTargetable;
}


std::queue<Command*> FiringStrategy::DetermineActions(GameState &state) {
    // For now simply move in a circle!
    // TODO: Check for terrain we cant move through
    std::queue<Command* > moves;
    if(!state.player.alive) return moves;
    if(state.player.TankSlow.alive) {
       // Do slow tank firing strategy
        Position thisTank = state.player.TankFast.position;
        double angle = state.player.TankFast.turret;
        double tmp = thisTank.GetAngle(getClosestTarget(state, thisTank)) - angle;
        if (tmp > acos(-1)) {
            tmp -= 2 * acos(-1);
        }
        if (tmp < -acos(-1)) {
            tmp += 2 * acos(-1);
        }
        angle = tmp;
        //if not in sight change target
        if(!isTargetInSight(state.map.GetTerrain(), thisTank, angle)){
            double tmp = thisTank.GetAngle(getClosestTarget(state, thisTank,"UnseeableTarget")) - angle;
            if (tmp > acos(-1)) {
                tmp -= 2 * acos(-1);
            }
            if (tmp < -acos(-1)) {
                tmp += 2 * acos(-1);
            }
            angle = tmp;
        }
        moves.push(new RotateTurretCommand(angle, state.player.TankFast.id));
        moves.push(new FireCommand(state.player.TankFast.id));
    }
    if(state.player.TankFast.alive) {
        // Do fast tank firing strategy
        Position thisTank = state.player.TankSlow.position;
        double angle = state.player.TankSlow.turret;
        double tmp = thisTank.GetAngle(getClosestTarget(state, thisTank)) - angle;
        if (tmp > acos(-1)) {
            tmp -= 2 * acos(-1);
        }
        if (tmp < -acos(-1)) {
            tmp += 2 * acos(-1);
        }
        angle = tmp;
        if(!isTargetInSight(state.map.GetTerrain(), thisTank, angle)){
            double tmp = thisTank.GetAngle(getClosestTarget(state, thisTank,"UnseeableTarget")) - angle;
            if (tmp > acos(-1)) {
                tmp -= 2 * acos(-1);
            }
            if (tmp < -acos(-1)) {
                tmp += 2 * acos(-1);
            }
            angle = tmp;
        }
        moves.push(new RotateTurretCommand(angle, state.player.TankSlow.id));
        moves.push(new FireCommand(state.player.TankSlow.id));
    }

    return moves;
}

