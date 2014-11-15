#include "predictive_firing_strategy.h"
#include <cmath>
#include <string>
#include <iostream>
#include "../util.h"


Position getPredictedPosition(const Tank &tank, const Tank &enemy, const Tank &previousEnemy, double diffTime){
    // determine point to shoot from velocity tank is travelling at.
    Position uVec = enemy.position - previousEnemy.position;


    double speed = uVec.Length()/diffTime;
    double distance = (tank.position - enemy.position).Length() - tank.hitRadius - enemy.hitRadius;
    // hard code 30m/s for projectile speed.
    double timeToHit = distance/30.0;
    // distance is now distance enemy will travel.
    distance = speed*timeToHit;
    Position dir(cos(enemy.tracks), sin(enemy.tracks));
    Position prediction = Position(enemy.position.x + dir.x*distance, enemy.position.y + dir.y*distance);
    return prediction;
}


std::queue<Command*> PredictiveFiringStrategy::DetermineActions(GameState &state, GameState &previousState) {
    std::queue<Command* > moves;
    if(!state.player.alive) return moves;
    if(fabs(previousState.timeRemaining - state.timeRemaining) < 1E-7){
        return moves;
    }
    if(state.player.TankFast.alive) {// Do slow tank firing strategy
        Tank thisTank = state.player.TankFast;
        Tank closestEnemy;
        bool canShoot = Util::getClosestTarget(state, previousState, thisTank, closestEnemy);
        Position target;

        if(closestEnemy.position == state.opponent.TankSlow.position){
            target = getPredictedPosition(state.player.TankFast, state.opponent.TankSlow, previousState.opponent.TankSlow, previousState.timeRemaining - state.timeRemaining);
        } else if (closestEnemy.position == state.opponent.TankFast.position){
            target = getPredictedPosition(state.player.TankFast, state.opponent.TankFast, previousState.opponent.TankFast, previousState.timeRemaining - state.timeRemaining);
        }

        double angle = state.player.TankFast.turret;
        angle = thisTank.position.GetAngle(target) - angle;
        moves.push(new RotateTurretCommand(angle, state.player.TankFast.id));
        Position useless;
        if(canShoot
                &&  Util::intersectLineCircle(target, 2.0, thisTank.position, thisTank.turret, useless)
                && !Util::aiming(state.player.TankFast, state.player.TankSlow)
                ){

            moves.push(new FireCommand(state.player.TankFast.id));

        } else {
            moves.push(new StopFireCommand(state.player.TankFast.id));
        }

    }
    if(state.player.TankSlow.alive) {
        // Do fast tank firing strategy
        Tank thisTank = state.player.TankSlow;
        Tank closestEnemy;
        bool canShoot = Util::getClosestTarget(state, previousState, thisTank, closestEnemy);

        Position target;
        if(closestEnemy.position == state.opponent.TankSlow.position){
            target = getPredictedPosition(state.player.TankSlow, state.opponent.TankSlow, previousState.opponent.TankSlow, previousState.timeRemaining - state.timeRemaining);
        } else if (closestEnemy.position == state.opponent.TankFast.position){
            target = getPredictedPosition(state.player.TankSlow, state.opponent.TankFast, previousState.opponent.TankFast, previousState.timeRemaining - state.timeRemaining);
        }

        double angle = state.player.TankSlow.turret;
        angle = thisTank.position.GetAngle(target) - angle;
        moves.push(new RotateTurretCommand(angle, state.player.TankSlow.id));
        Position useless;
        if(canShoot
                && Util::intersectLineCircle(target, 2.0, thisTank.position, thisTank.turret, useless)
                && !Util::aiming(state.player.TankSlow, state.player.TankFast)
                ){

            moves.push(new FireCommand(state.player.TankSlow.id));

        } else {
            moves.push(new StopFireCommand(state.player.TankSlow.id));
        }
    }

    return moves;
}

