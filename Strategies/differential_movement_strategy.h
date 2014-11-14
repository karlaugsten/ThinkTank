#ifndef DIFFERENTIAL_MOVEMENT_STRATEGY_H
#define DIFFERENTIAL_MOVEMENT_STRATEGY_H

#include "strategy.h"
#include "../StateParser/gamestate.h"

/**
* A Strategy that models the playing field with a "goodness"
* equation, and moves the tanks always to areas of increasing goodness.
*/
class DifferentialMovementStrategy : public Strategy
{
protected:

private:
    bool ternary = true;
    /*
    * Calculates the goodness at the given position
     */
    double CalculateGoodness(const GameState &state, const GameState &previousState, const Tank &tank, const Tank &otherTank, const Position& position);
    Position linearSearch(const GameState &state, const GameState &previousState, const Tank &tank, const Tank &otherTank, const int iterations, const double r);
    Position ternarySearch(const GameState &state, const GameState &previousState, const Tank &tank, const Tank &otherTank, const int maxIterations, const double r);

public:
    DifferentialMovementStrategy(){
        ternary = true;
    }

    DifferentialMovementStrategy(bool t){
        ternary = t;
    }
    double CalculateGoodness(const GameState &state, const GameState &previousState, const Tank &tank, const Tank &otherTank, const double x, const double y);
    std::queue<Command*> DetermineActions(GameState &state, GameState &previousState);
};


#endif