#include "firing_strategy.h"
#include <cmath>
#include <string>
#include <iostream>

using namespace std;


Position randPointInRange(Position a, Position b){
    Position c;
    if(a == b){
        return a;
    }
    c.x = a.x + (b.x - a.x) * rand() / (1.0 + RAND_MAX);
    c.y = a.y + (c.x - a.x) * (a.y - b.y) / (a.x - b.x);
    return c;
}

Position getPointOnLineWithDistanceFromCurrent(Position current, Position previous, double distance){
    //http://math.stackexchange.com/questions/175896/finding-a-point-along-a-line-a-certain-distance-away-from-another-point
    Position v= Position(current.x-previous.x,current.y-previous.y);
    double vMagnitude = sqrt(v.x*v.x+v.y*v.y);
    Position unitVector = Position(v.x/vMagnitude,v.y/vMagnitude);
    return Position(current.x+(unitVector.x*distance), current.y+(unitVector.y*distance));
}

Position getTargetWithVariance(Tank ourTank, Tank enemyTank, Position previousTankPosition){
    //if the tank hasn't moved return current position
    if(enemyTank.position.x == previousTankPosition.x && enemyTank.position.y == previousTankPosition.y) return enemyTank.position;
    double ratioForGoingForward=0.8;
    double ratioForGoingBackward=0.2;
    double distanceFromTanks = ourTank.position.Distance(enemyTank.position);
    double timeBetweenMissileCollision = distanceFromTanks/30.0; //constant missile speed
    double distanceTravelableByEnemy=enemyTank.speed*timeBetweenMissileCollision;
    // Distances are given as a percentage of direction likelihood
    Position Forward = getPointOnLineWithDistanceFromCurrent(enemyTank.position, previousTankPosition, ratioForGoingForward*distanceTravelableByEnemy);
    Position Backward = getPointOnLineWithDistanceFromCurrent(enemyTank.position, previousTankPosition, -(ratioForGoingBackward*distanceTravelableByEnemy));
    return randPointInRange(Forward, Backward);
}
/*
* returns true if a ray intersects a circle
* direction is in radians from the (1,0) vector.
* start is the start of the ray.
* outIntersection will have the closest intersection point.
 */
bool intersectLineCircle(Position &circle, double radius, Position &start, double direction, Position &outIntersection){

    Position rayDir = Position(cos(direction), sin(direction));
    Position originNew = circle - start;

    double b = 2.0*((originNew.x*rayDir.x) + (rayDir.y*originNew.y));
    double c = (originNew.Norm2()) - radius*radius;
    double a = rayDir.Norm2();

    double delta = b * b - (4.0 * a * c);

    // Negative doesnt have square root
    if( delta <= 0.0 ) return false;
    // if delta is 0.0 it is tangent to circle
    // TODO: handle when delta is zero


    double intersect1 = (-b-sqrt(delta))/(2.0*a);
    double intersect2 = (-b + sqrt(delta))/(2.0*a);

    double intersect = min(intersect1, intersect2);

    outIntersection.x = rayDir.x*intersect + start.x;
    outIntersection.y = rayDir.y*intersect + start.y;

    return true;
}

/*
* returns true if tank is aiming at target tank
 */
bool aiming(Tank &tank, Tank &target){
    Position intersect;
    return intersectLineCircle(target.position, target.hitRadius, tank.position, tank.turret, intersect);
}


// http://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
bool onSegment(Position &p, Position &q, Position &r)
{
    if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) && q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
        return true;
    return false;
}

int orientation(Position &p, Position &q, Position &r)
{
    int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
    if (fabs(val) < 1E-7) return 0;
    return (val > 0)? 1: 2;
}

bool doIntersect(Position &p1, Position &q1, Position &p2, Position &q2)
{
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);
    if (o1 != o2 && o3 != o4)
        return true;
    if (fabs(o1) < 1E-7 && onSegment(p1, p2, q1)) return true;
    if (fabs(o2) < 1E-7 && onSegment(p1, q2, q1)) return true;
    if (fabs(o3) < 1E-7 && onSegment(p2, p1, q2)) return true;
    if (fabs(o4) < 1E-7 && onSegment(p2, q1, q2)) return true;

    return false;
}

bool inSight(vector<Terrain> &vecTerrain, Position &tankPosition, Position &enemyPosition) {
    for(int i=0;i<vecTerrain.size();i++) {
        Terrain terrain = vecTerrain[i];
        if(terrain.Type==TerrainType::IMPASSABLE) continue; // If the current terrain type is impassible, look for solids
        Position upperLeft = Position(terrain.position.x, terrain.position.y+terrain.size.y);
        Position upperRight = Position(terrain.position.x+terrain.size.x, terrain.position.y+terrain.size.y);
        Position lowerRight = Position(terrain.position.x+terrain.size.x, terrain.position.y);
        Position lowerLeft = terrain.position;

        if(doIntersect(tankPosition,enemyPosition,upperLeft,upperRight)){
            return false;
        }else if(doIntersect(tankPosition,enemyPosition,upperRight,lowerRight)){
            return false;
        }else if(doIntersect(tankPosition,enemyPosition,lowerRight,lowerLeft)){
            return false;
        }else if(doIntersect(tankPosition,enemyPosition,lowerLeft,upperLeft)){
            return false;
        }
    }

    return true;
}

bool FiringStrategy::getClosestTarget(GameState& state, GameState& previous, Tank& thisTank, Position& target){
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
            target.x = getTargetWithVariance(thisTank, state.opponent.TankSlow, previous.opponent.TankSlow.position).x;
            target.y = getTargetWithVariance(thisTank, state.opponent.TankSlow, previous.opponent.TankSlow.position).y;
        }else{
            //target.x = state.opponent.TankFast.position.x;
            //target.y = state.opponent.TankFast.position.y;
            target.x = getTargetWithVariance(thisTank, state.opponent.TankFast, previous.opponent.TankFast.position).x;
            target.y = getTargetWithVariance(thisTank, state.opponent.TankFast, previous.opponent.TankFast.position).y;
        }
        return false; // Hold your fire captain!!!
    }

    if(!FastInSight&&!SlowIsAlive){ //Check if there is only fast tank alive but not in sight
        //target.x = state.opponent.TankFast.position.x;
        //target.y = state.opponent.TankFast.position.y;
        target.x = getTargetWithVariance(thisTank, state.opponent.TankFast, previous.opponent.TankFast.position).x;
        target.y = getTargetWithVariance(thisTank, state.opponent.TankFast, previous.opponent.TankFast.position).y;
        return false;
    }else if(!SlowInSight&&!FastIsAlive){ //Check if there is only fast tank alive but not in sight
        //target.x = state.opponent.TankSlow.position.x;
        //target.y = state.opponent.TankSlow.position.y;
        target.x = getTargetWithVariance(thisTank, state.opponent.TankSlow, previous.opponent.TankSlow.position).x;
        target.y = getTargetWithVariance(thisTank, state.opponent.TankSlow, previous.opponent.TankSlow.position).y;
        return false;
    }else if(!FastInSight&&SlowIsAlive){ //Only slow in sight
        //target.x = state.opponent.TankSlow.position.x;
        //target.y = state.opponent.TankSlow.position.y;
        target.x = getTargetWithVariance(thisTank, state.opponent.TankSlow, previous.opponent.TankSlow.position).x;
        target.y = getTargetWithVariance(thisTank, state.opponent.TankSlow, previous.opponent.TankSlow.position).y;
        return true;
    }else if(!SlowInSight&&FastIsAlive) { //Only fast in sight
        //target.x = state.opponent.TankFast.position.x;
        //target.y = state.opponent.TankFast.position.y;
        target.x = getTargetWithVariance(thisTank, state.opponent.TankFast, previous.opponent.TankFast.position).x;
        target.y = getTargetWithVariance(thisTank, state.opponent.TankFast, previous.opponent.TankFast.position).y;
        return true;

    }else if(!SlowIsAlive&&!FastIsAlive){// If both are dead, target is irrelevant, just don't shoot
        return false;
    }
    else{// If both are alive and in sight
        if(distanceBetweenTanksFast>distanceBetweenTanksSlow){
            //target.x = state.opponent.TankSlow.position.x;
            //target.y = state.opponent.TankSlow.position.y;
            target.x = getTargetWithVariance(thisTank, state.opponent.TankSlow, previous.opponent.TankSlow.position).x;
            target.y = getTargetWithVariance(thisTank, state.opponent.TankSlow, previous.opponent.TankSlow.position).y;
        }else{
            //target.x = state.opponent.TankFast.position.x;
            //target.y = state.opponent.TankFast.position.y;
            target.x = getTargetWithVariance(thisTank, state.opponent.TankFast, previous.opponent.TankFast.position).x;
            target.y = getTargetWithVariance(thisTank, state.opponent.TankFast, previous.opponent.TankFast.position).y;
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
        bool canShoot = getClosestTarget(state, previousState, thisTank, closestEnemy);


        double angle = state.player.TankFast.turret;
        angle = thisTank.position.GetAngle(closestEnemy) - angle;
        moves.push(new RotateTurretCommand(angle, state.player.TankFast.id));
        if(canShoot
                && closestEnemy == state.opponent.TankSlow.position
                && aiming(state.player.TankFast, state.opponent.TankSlow)
                && !aiming(state.player.TankFast, state.player.TankSlow)
                ){

            moves.push(new FireCommand(state.player.TankFast.id));

        } else if (canShoot
                && closestEnemy == state.opponent.TankFast.position
                && aiming(state.player.TankFast, state.opponent.TankFast)
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
        bool canShoot = getClosestTarget(state, previousState, thisTank, closestEnemy);
        cout<< "slightly backward X: "<< closestEnemy.x << " -- Y :" << closestEnemy.y<<endl;
        double angle = state.player.TankSlow.turret;
        angle = thisTank.position.GetAngle(closestEnemy) - angle;
        cout << "DAT ANGLE DOE: " << angle<< endl;
        moves.push(new RotateTurretCommand(angle, state.player.TankSlow.id));
        if(canShoot
                && !aiming(state.player.TankSlow, state.player.TankFast)
                ){

            moves.push(new FireCommand(state.player.TankSlow.id));

        } else if (canShoot
                && !aiming(state.player.TankSlow, state.player.TankFast)
                ) {

            moves.push(new FireCommand(state.player.TankSlow.id));

        } else {
            moves.push(new StopFireCommand(state.player.TankSlow.id));
        }
    }

    return moves;
}

