#include "random_firing_strategy.h"
#include <cmath>
#include <string>
#include <iostream>
#include "../util.h"


Position getTargetWithVariance(GameState& state, Tank ourTank, Tank enemyTank, Position previousTankPosition, bool &isInPredictionRange){
    //if the tank hasn't moved return current position
    if(enemyTank.position.x == previousTankPosition.x && enemyTank.position.y == previousTankPosition.y) return enemyTank.position;
    double ratioForGoingForward=1.0;
    double ratioForGoingBackward=0.0;
    double distanceFromTanks = ourTank.position.Distance(enemyTank.position);
    double timeBetweenMissileCollision = distanceFromTanks/30.0; //constant missile speed
    double distanceTravelableByEnemy=enemyTank.speed*timeBetweenMissileCollision;
    // Distances are given as a percentage of direction likelihood

    Position Forward = Util::getPointOnLineWithDistanceFromCurrent(enemyTank.position, previousTankPosition, ratioForGoingForward*distanceTravelableByEnemy);
    Position Backward = Util::getPointOnLineWithDistanceFromCurrent(enemyTank.position, previousTankPosition, -(ratioForGoingBackward*distanceTravelableByEnemy));
    isInPredictionRange = Util::doIntersect(Forward, Backward, ourTank.position, Util::getCurrentTargetFromTankAngle(ourTank, ourTank.turret)) || Util::aiming(ourTank, enemyTank);
    if(distanceFromTanks<=20) return enemyTank.position;
    return Util::randPointInRange(Forward, Backward);
}


void RandomFiringStrategy::sendTankCommands(std::queue<Command*> &moves, GameState &state, GameState &previousState, Tank &thisTank){
    Tank ally;
    Tank closestEnemy;
    Position closesEnemyWithPrediction;
    bool isInPredictionRange;
    bool shootingAtAlly;
    bool canShoot = Util::getClosestTarget(state, previousState, thisTank, closestEnemy);
    if(closestEnemy.position == state.opponent.TankFast.position){
        closesEnemyWithPrediction = getTargetWithVariance(state, thisTank, closestEnemy, previousState.opponent.TankFast.position,isInPredictionRange);
    } else{
        closesEnemyWithPrediction = getTargetWithVariance(state, thisTank, closestEnemy, previousState.opponent.TankSlow.position,isInPredictionRange);
    }
    if(thisTank.position == state.player.TankFast.position){ // Ensure not firing at friendly tank
        shootingAtAlly = Util::aiming(thisTank, state.player.TankSlow);
    }else{
        shootingAtAlly = Util::aiming(thisTank, state.player.TankFast);
    }
    double angle = thisTank.turret;
    angle = thisTank.position.GetAngle(closesEnemyWithPrediction) - angle;
    moves.push(new RotateTurretCommand(angle, thisTank.id)); // Send move commands
    if(canShoot && isInPredictionRange && !shootingAtAlly){
        moves.push(new FireCommand(thisTank.id));

    }else {
        moves.push(new StopFireCommand(thisTank.id));
    }
}


std::queue<Command*> RandomFiringStrategy::DetermineActions(GameState &state, GameState &previousState) {
    std::queue<Command* > moves;
    if(!state.player.alive) return moves;
    if(state.player.TankFast.alive) {
        sendTankCommands(moves, state, previousState, state.player.TankFast);
    }
    if(state.player.TankSlow.alive) {
        sendTankCommands(moves, state, previousState, state.player.TankSlow);
    }
    return moves;
}

