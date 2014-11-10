#include "firing_strategy.h"

Position FiringStrategy::getClosestTarget(GameState& state, Position& thisTank){
    double distanceBetweenTanksFast=-1;
    double distanceBetweenTanksSlow=-1;

    //Check if each tank is alive and calculate its relative distance
    if(state.opponent.TankFast.alive) {
        Position enemy1 = state.opponent.TankFast.position;
        distanceBetweenTanksFast=thisTank.Distance(enemy1);
    }
    if(state.opponent.TankSlow.alive) {
        Position enemy2 = state.opponent.TankSlow.position;
        distanceBetweenTanksSlow=thisTank.Distance(enemy2);
    }

    // TODO: CHECK FOR UNBREAKABLE BARRIER BETWEEN
    if(distanceBetweenTanksFast==-1){
        return state.opponent.TankSlow.position;
    }else if(distanceBetweenTanksSlow==-1){
        return state.opponent.TankFast.position;
    }else{
        if(distanceBetweenTanksFast>distanceBetweenTanksSlow){
            return state.opponent.TankSlow.position;
        }else{
            return state.opponent.TankFast.position;
        }
    }
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
        // TODO: Check for unbreakable terrain
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
        moves.push(new RotateTurretCommand(angle, state.player.TankSlow.id));
        moves.push(new FireCommand(state.player.TankSlow.id));
    }

    return moves;
}

