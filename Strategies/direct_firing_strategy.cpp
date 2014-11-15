#include "direct_firing_strategy.h"
#include <cmath>
#include <string>
#include <iostream>
#include "../util.h"


std::queue<Command*> DirectFiringStrategy::DetermineActions(GameState &state, GameState &previousState) {
    std::queue<Command* > moves;
    if(!state.player.alive) return moves;
    if(state.player.TankFast.alive) {// Do slow tank firing strategy
        Tank thisTank = state.player.TankFast;
        Position closestEnemy;
        bool canShoot = Util::getClosestTarget(state, previousState, thisTank, closestEnemy);


        double angle = state.player.TankFast.turret;
        angle = thisTank.position.GetAngle(closestEnemy) - angle;
        moves.push(new RotateTurretCommand(angle, state.player.TankFast.id));
        if(canShoot
                && closestEnemy == state.opponent.TankSlow.position
                && Util::aiming(state.player.TankFast, state.opponent.TankSlow)
                && !Util::aiming(state.player.TankFast, state.player.TankSlow)
                ){

            moves.push(new FireCommand(state.player.TankFast.id));

        } else if (canShoot
                && closestEnemy == state.opponent.TankFast.position
                && Util::aiming(state.player.TankFast, state.opponent.TankFast)
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
        bool canShoot = Util::getClosestTarget(state, previousState, thisTank, closestEnemy);
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

