#include "direct_firing_strategy.h"
#include <cmath>
#include <string>
#include <iostream>
#include "../util.h"

using namespace std;



bool DirectFiringStrategy::getClosestTarget(GameState& state, GameState& previous, Tank& thisTank, Position& target){
    double distanceBetweenTanksFast = 100000;
    double distanceBetweenTanksSlow = 100000;
    bool FastIsAlive = true;
    bool SlowIsAlive = true;
    bool FastInSight = true;
    bool SlowInSight = true;

    if(state.opponent.TankFast.alive) {//Check if fast tank is alive and calculate its relative distance
        Position enemy1 = state.opponent.TankFast.position;
        distanceBetweenTanksFast=thisTank.position.Distance(enemy1);
        if(!Util::inSight(state.map.terrain, thisTank.position, enemy1)) FastInSight = false;// Alive but hiding
    } else {
        FastIsAlive = false; //dead
    }
    if(state.opponent.TankSlow.alive) {//Check if fast tank is alive and calculate its relative distance
        Position enemy2 = state.opponent.TankSlow.position;
        distanceBetweenTanksSlow=thisTank.position.Distance(enemy2);
        if(!Util::inSight(state.map.terrain, thisTank.position, enemy2)) SlowInSight = false;// Alive but hiding
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


std::queue<Command*> DirectFiringStrategy::DetermineActions(GameState &state, GameState &previousState) {
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
                && !Util::aiming(state.player.TankFast, state.player.TankSlow)
                ){

            moves.push(new FireCommand(state.player.TankFast.id));

        } else if (canShoot
                && !Util::aiming(state.player.TankFast, state.player.TankSlow)
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
        double angle = state.player.TankSlow.turret;
        angle = thisTank.position.GetAngle(closestEnemy) - angle;
        moves.push(new RotateTurretCommand(angle, state.player.TankSlow.id));
        if(canShoot
                && closestEnemy == state.opponent.TankSlow.position
                && Util::aiming(state.player.TankSlow, state.opponent.TankSlow)
                && !Util::aiming(state.player.TankSlow, state.player.TankFast)
                ){

            moves.push(new FireCommand(state.player.TankSlow.id));

        } else if (canShoot
                && closestEnemy == state.opponent.TankFast.position
                && Util::aiming(state.player.TankSlow, state.opponent.TankFast)
                && !Util::aiming(state.player.TankSlow, state.player.TankFast)
                ) {

            moves.push(new FireCommand(state.player.TankSlow.id));

        } else {
            moves.push(new StopFireCommand(state.player.TankSlow.id));
        }
    }

    return moves;
}

