#include "firing_strategy.h"
#include <cmath>
#include <string>
#include <iostream>
#include "../util.h"

using namespace std;

/*
* returns true if tank is aiming at target tank
 */
bool aiming(Tank &tank, Tank &target){
    Position intersect;
    return Util::intersectLineCircle(target.position, target.hitRadius, tank.position, tank.turret, intersect);
}

Position getPointOnLineWithDistanceFromCurrent(Position current, Position previous, double distance){
    //http://math.stackexchange.com/questions/175896/finding-a-point-along-a-line-a-certain-distance-away-from-another-point
    Position v= Position(current.x-previous.x,current.y-previous.y);
    double vMagnitude = sqrt(v.x*v.x+v.y*v.y);
    Position unitVector = Position(v.x/vMagnitude,v.y/vMagnitude);
    return Position(current.x+(unitVector.x*distance), current.y+(unitVector.y*distance));
}

Position getCurrentTargetFromTankAngle(Tank &ourTank, double angle){
    Position toReturn;
    double x= cos(angle);
    double y= sin(angle);
    toReturn.x=ourTank.position.x+x*10000;
    toReturn.y=ourTank.position.y+y*10000;
    return toReturn;
}

Position getTargetWithVariance(GameState& state, Tank ourTank, Tank enemyTank, Position previousTankPosition, bool &isInPredictionRange){
    //if the tank hasn't moved return current position
    if(enemyTank.position.x == previousTankPosition.x && enemyTank.position.y == previousTankPosition.y) return enemyTank.position;
    double ratioForGoingForward=1.0;
    double ratioForGoingBackward=0.0;
    double distanceFromTanks = ourTank.position.Distance(enemyTank.position);
    double timeBetweenMissileCollision = distanceFromTanks/30.0; //constant missile speed
    double distanceTravelableByEnemy=enemyTank.speed*timeBetweenMissileCollision;
    // Distances are given as a percentage of direction likelihood

    Position Forward = getPointOnLineWithDistanceFromCurrent(enemyTank.position, previousTankPosition, ratioForGoingForward*distanceTravelableByEnemy);
    Position Backward = getPointOnLineWithDistanceFromCurrent(enemyTank.position, previousTankPosition, -(ratioForGoingBackward*distanceTravelableByEnemy));
    isInPredictionRange = Util::doIntersect(Forward, Backward, ourTank.position, getCurrentTargetFromTankAngle(ourTank, ourTank.turret)) || aiming(ourTank, enemyTank);
    if(distanceFromTanks<=20) return enemyTank.position;
    return Util::randPointInRange(Forward, Backward);
}


bool inSight(vector<Terrain> &vecTerrain, Position &tankPosition, Position &enemyPosition) {
    for(int i=0;i<vecTerrain.size();i++) {
        Terrain terrain = vecTerrain[i];
        if(terrain.Type==TerrainType::IMPASSABLE) continue; // If the current terrain type is impassible, look for solids
        Position upperLeft = Position(terrain.position.x, terrain.position.y+terrain.size.y);
        Position upperRight = Position(terrain.position.x+terrain.size.x, terrain.position.y+terrain.size.y);
        Position lowerRight = Position(terrain.position.x+terrain.size.x, terrain.position.y);
        Position lowerLeft = terrain.position;

        if(Util::doIntersect(tankPosition,enemyPosition,upperLeft,upperRight)){
            return false;
        }else if(Util::doIntersect(tankPosition,enemyPosition,upperRight,lowerRight)){
            return false;
        }else if(Util::doIntersect(tankPosition,enemyPosition,lowerRight,lowerLeft)){
            return false;
        }else if(Util::doIntersect(tankPosition,enemyPosition,lowerLeft,upperLeft)){
            return false;
        }
    }

    return true;
}

bool FiringStrategy::getClosestTarget(GameState& state, GameState& previous, Tank& thisTank, Position& target, bool &isInPredictionRange){
    double distanceBetweenTanksFast = 100000;
    double distanceBetweenTanksSlow = 100000;
    bool FastIsAlive = true;
    bool SlowIsAlive = true;
    bool FastInSight = true;
    bool SlowInSight = true;

    if(state.opponent.TankFast.alive) {//Check if fast tank is alive and calculate its relative distance
        Position enemy1 = state.opponent.TankFast.position;
        distanceBetweenTanksFast=thisTank.position.Distance(enemy1);
        if(!inSight(state.map.terrain, thisTank.position, enemy1)) FastInSight = false;// Alive but hiding
    } else {
        FastIsAlive = false; //dead
    }
    if(state.opponent.TankSlow.alive) {//Check if fast tank is alive and calculate its relative distance
        Position enemy2 = state.opponent.TankSlow.position;
        distanceBetweenTanksSlow=thisTank.position.Distance(enemy2);
        if(!inSight(state.map.terrain, thisTank.position, enemy2)) SlowInSight = false;// Alive but hiding
    } else {
        SlowIsAlive = false; //dead
    }

    if(!SlowInSight && !FastInSight){ // Both are alive and not in sight
        if(distanceBetweenTanksFast>distanceBetweenTanksSlow){ // Aim at the nearest but hold your fire
            //target.x = state.opponent.TankSlow.position.x;
            //target.y = state.opponent.TankSlow.position.y;
            target.x = getTargetWithVariance(state,thisTank, state.opponent.TankSlow, previous.opponent.TankSlow.position,isInPredictionRange).x;
            target.y = getTargetWithVariance(state, thisTank, state.opponent.TankSlow, previous.opponent.TankSlow.position,isInPredictionRange).y;
        }else{
            //target.x = state.opponent.TankFast.position.x;
            //target.y = state.opponent.TankFast.position.y;
            target.x = getTargetWithVariance(state, thisTank, state.opponent.TankFast, previous.opponent.TankFast.position,isInPredictionRange).x;
            target.y = getTargetWithVariance(state, thisTank, state.opponent.TankFast, previous.opponent.TankFast.position,isInPredictionRange).y;
        }
        return false; // Hold your fire captain!!!
    }

    if(!FastInSight&&!SlowIsAlive){ //Check if there is only fast tank alive but not in sight
        //target.x = state.opponent.TankFast.position.x;
        //target.y = state.opponent.TankFast.position.y;
        target.x = getTargetWithVariance(state, thisTank, state.opponent.TankFast, previous.opponent.TankFast.position,isInPredictionRange).x;
        target.y = getTargetWithVariance(state, thisTank, state.opponent.TankFast, previous.opponent.TankFast.position,isInPredictionRange).y;
        return false;
    }else if(!SlowInSight&&!FastIsAlive){ //Check if there is only fast tank alive but not in sight
        target.x = getTargetWithVariance(state, thisTank, state.opponent.TankSlow, previous.opponent.TankSlow.position,isInPredictionRange).x;
        target.y = getTargetWithVariance(state, thisTank, state.opponent.TankSlow, previous.opponent.TankSlow.position,isInPredictionRange).y;
        return false;
    }else if(!FastInSight&&SlowIsAlive){ //Only slow in sight
        target.x = getTargetWithVariance(state, thisTank, state.opponent.TankSlow, previous.opponent.TankSlow.position,isInPredictionRange).x;
        target.y = getTargetWithVariance(state, thisTank, state.opponent.TankSlow, previous.opponent.TankSlow.position,isInPredictionRange).y;
        return true;
    }else if(!SlowInSight&&FastIsAlive) { //Only fast in sight
        target.x = getTargetWithVariance(state, thisTank, state.opponent.TankFast, previous.opponent.TankFast.position,isInPredictionRange).x;
        target.y = getTargetWithVariance(state, thisTank, state.opponent.TankFast, previous.opponent.TankFast.position,isInPredictionRange).y;
        return true;

    }else if(!SlowIsAlive&&!FastIsAlive){// If both are dead, target is irrelevant, just don't shoot
        return false;
    }
    else{// If both are alive and in sight
        if(distanceBetweenTanksFast>distanceBetweenTanksSlow){
            target.x = getTargetWithVariance(state, thisTank, state.opponent.TankSlow, previous.opponent.TankSlow.position,isInPredictionRange).x;
            target.y = getTargetWithVariance(state, thisTank, state.opponent.TankSlow, previous.opponent.TankSlow.position,isInPredictionRange).y;
        }else{
            target.x = getTargetWithVariance(state, thisTank, state.opponent.TankFast, previous.opponent.TankFast.position,isInPredictionRange).x;
            target.y = getTargetWithVariance(state, thisTank, state.opponent.TankFast, previous.opponent.TankFast.position,isInPredictionRange).y;
        }
        return true;
    }
}


std::queue<Command*> FiringStrategy::DetermineActions(GameState &state, GameState &previousState) {
    std::queue<Command* > moves;
    if(!state.player.alive) return moves;
    if(state.player.TankFast.alive) {// Do slow tank firing strategy
        Tank thisTank = state.player.TankFast;
        Position closestEnemy;
        bool isInPredictionRange;
        bool canShoot = getClosestTarget(state, previousState, thisTank, closestEnemy, isInPredictionRange);


        double angle = state.player.TankFast.turret;
        angle = thisTank.position.GetAngle(closestEnemy) - angle;
        moves.push(new RotateTurretCommand(angle, state.player.TankFast.id));
        if(canShoot
                && isInPredictionRange
                && !aiming(state.player.TankFast, state.player.TankSlow)
                ){

            moves.push(new FireCommand(state.player.TankFast.id));

        } else if (canShoot
                && isInPredictionRange
                && !aiming(state.player.TankFast, state.player.TankSlow)
                ) {

            moves.push(new FireCommand(state.player.TankFast.id));

        } else {
            moves.push(new StopFireCommand(state.player.TankFast.id));
        }

    }
    if(state.player.TankSlow.alive) {
        // Do fast tank firing strategy
        Tank thisTank = state.player.TankSlow;
        Position closestEnemy;
        bool isInPredictionRange;
        bool canShoot = getClosestTarget(state, previousState, thisTank, closestEnemy, isInPredictionRange);
        double angle = state.player.TankSlow.turret;
        angle = thisTank.position.GetAngle(closestEnemy) - angle;
        moves.push(new RotateTurretCommand(angle, state.player.TankSlow.id));
        if(canShoot
                && isInPredictionRange
                && !aiming(state.player.TankSlow, state.player.TankFast)
                ){

            moves.push(new FireCommand(state.player.TankSlow.id));

        } else if (canShoot
                && isInPredictionRange
                && !aiming(state.player.TankSlow, state.player.TankFast)
                ) {

            moves.push(new FireCommand(state.player.TankSlow.id));

        } else {
            moves.push(new StopFireCommand(state.player.TankSlow.id));
        }
    }

    return moves;
}

