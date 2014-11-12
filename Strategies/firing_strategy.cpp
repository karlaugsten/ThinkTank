#include "firing_strategy.h"
#include <cmath>
#include <string>
#include <iostream>

using namespace std;

/*
* returns true if a ray intersects a circle
* direction is in radians from the (1,0) vector.
* start is the start of the ray.
* outIntersection will have the closest intersection point.
 */
bool intersectLineCircle(Position &circle, double radius, Position &start, double direction, Position &outIntersection){

    Position rayDir = Position(cos(direction), sin(direction));
    Position originNew = circle - start;

    double b = originNew.x*rayDir.x + rayDir.y*originNew.y;
    double c = radius*radius - (originNew.Norm2());
    double a = rayDir.Norm2();

    double diva = 1.0/a;

    // Negative doesnt have square root
    if( (b*b - 4.0*a*c) <= 0.0 ) return false;

    double b4ac = sqrt( (b*b - 4.0*a*c) );

    double intersect1 = (b-b4ac)*diva;
    double intersect2 = (b + b4ac)*diva;

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

/*
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
        if(terrain.Type==TerrainType::IMPASSABLE) continue; // If the current terrain type is impassible, look for solid
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
} */

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

bool FiringStrategy::getClosestTarget(GameState& state, Position& thisTank, Position& target){
    double distanceBetweenTanksFast = 100000;
    double distanceBetweenTanksSlow = 100000;
    bool FastIsAlive = true;
    bool SlowIsAlive = true;
    bool FastInSight = true;
    bool SlowInSight = true;

    if(state.opponent.TankFast.alive) {//Check if fast tank is alive and calculate its relative distance
        Position enemy1 = state.opponent.TankFast.position;
        distanceBetweenTanksFast=thisTank.Distance(enemy1);
        if(!inSight(state.map.terrain, thisTank, enemy1)) FastInSight = false;// Alive but hiding
    } else {
        FastIsAlive = false; //dead
    }
    if(state.opponent.TankSlow.alive) {//Check if fast tank is alive and calculate its relative distance
        Position enemy2 = state.opponent.TankSlow.position;
        distanceBetweenTanksSlow=thisTank.Distance(enemy2);
        if(!inSight(state.map.terrain, thisTank, enemy2)) SlowInSight = false;// Alive but hiding
    } else {
        SlowIsAlive = false; //dead
    }

    if(!SlowInSight && !FastInSight){ // Both are alive and not in sight
        if(distanceBetweenTanksFast>distanceBetweenTanksSlow){ // Aim at the nearest but hold your fire
            target.x = state.opponent.TankSlow.position.x;
            target.y = state.opponent.TankSlow.position.y;
        }else{
            target.x = state.opponent.TankFast.position.x;
            target.y = state.opponent.TankFast.position.y;
        }
        return false; // Hold your fire captain!!!
    }

    if(!FastInSight&&!SlowIsAlive){ //Check if there is only fast tank alive but not in sight
        target.x = state.opponent.TankFast.position.x;
        target.y = state.opponent.TankFast.position.y;
        return false;
    }else if(!SlowInSight&&!FastIsAlive){ //Check if there is only fast tank alive but not in sight
        target.x = state.opponent.TankSlow.position.x;
        target.y = state.opponent.TankSlow.position.y;
        return false;
    }else if(!FastInSight&&SlowIsAlive){ //Only slow in sight
        target.x = state.opponent.TankSlow.position.x;
        target.y = state.opponent.TankSlow.position.y;
        return true;
    }else if(!SlowInSight&&FastIsAlive) { //Only fast in sight
        target.x = state.opponent.TankFast.position.x;
        target.y = state.opponent.TankFast.position.y;
        return true;

    }else if(!SlowIsAlive&&!FastIsAlive){// If both are dead, target is irrelevant, just don't shoot
        return false;
    }
    else{// If both are alive and in sight
        if(distanceBetweenTanksFast>distanceBetweenTanksSlow){
            target.x = state.opponent.TankSlow.position.x;
            target.y = state.opponent.TankSlow.position.y;
        }else{
            target.x = state.opponent.TankFast.position.x;
            target.y = state.opponent.TankFast.position.y;
        }
        return true;
    }
}


std::queue<Command*> FiringStrategy::DetermineActions(GameState &state) {
    std::queue<Command* > moves;
    if(!state.player.alive) return moves;
    if(state.player.TankFast.alive) {// Do slow tank firing strategy
        Position thisTank = state.player.TankFast.position;
        Position closestEnemy;
        bool canShoot = getClosestTarget(state, thisTank, closestEnemy);


        double angle = state.player.TankFast.turret;
        angle = thisTank.GetAngle(closestEnemy) - angle;
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
        Position thisTank = state.player.TankSlow.position;
        Position closestEnemy;
        bool canShoot = getClosestTarget(state, thisTank, closestEnemy);
        double angle = state.player.TankSlow.turret;
        angle = thisTank.GetAngle(closestEnemy) - angle;
        moves.push(new RotateTurretCommand(angle, state.player.TankSlow.id));
        if(canShoot
                && closestEnemy == state.opponent.TankSlow.position
                && aiming(state.player.TankSlow, state.opponent.TankSlow)
                && !aiming(state.player.TankSlow, state.player.TankFast)
                ){

            moves.push(new FireCommand(state.player.TankSlow.id));

        } else if (canShoot
                && closestEnemy == state.opponent.TankFast.position
                && aiming(state.player.TankSlow, state.opponent.TankFast)
                && !aiming(state.player.TankSlow, state.player.TankFast)
                ) {

            moves.push(new FireCommand(state.player.TankSlow.id));

        } else {
            moves.push(new StopFireCommand(state.player.TankSlow.id));
        }
    }

    return moves;
}

