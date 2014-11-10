#include "movement_strategy.h"
#include "../StateParser/gamestate.h"


std::queue<Command*> MovementStrategy::DetermineActions(GameState &state) {
    // For now simply move in a circle!
    // TODO: Check for terrain we cant move through
    std::queue<Command* > moves;
    if(!state.player.alive) return moves;
    if(state.player.TankSlow.alive) {
        moves.push(new MoveCommand(1.0, state.player.TankSlow.id));
        moves.push(new RotateCommand(0.1, state.player.TankSlow.id));
    }
    if(state.player.TankFast.alive) {
        moves.push(new MoveCommand(1.0, state.player.TankFast.id));
        moves.push(new RotateCommand(0.1, state.player.TankFast.id));
    }

    return moves;
}