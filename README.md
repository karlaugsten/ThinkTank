ThinkTank
=========

Think Tank was our team name for the Pason Coding Contest. This is an AI that controls tanks on a playing field to battle head to head against other AI's. This code took first place! The final battle is available to watch [here](https://codingcontest.pason.com/tanks/index.html?mode=log&matchToken=2326adba-2300-48c6-a373-20e4fcc6698d) (it only works on chrome).

Battles
=======

Some more of our tournament battles are available here (replays only work on google chrome):

1. Think Tank vs. Chicken Wing Wednesdays [Final](https://codingcontest.pason.com/tanks/index.html?mode=log&matchToken=2326adba-2300-48c6-a373-20e4fcc6698d)
2. Think Tank vs. Team Dream [Semi-Finals](https://codingcontest.pason.com/tanks/index.html?mode=log&matchToken=b4a8e38f-edaa-4f2f-8286-0d3bc7df96c7)
3. Think Tank vs. Liberal Arts Majors [Semi-Semi Finals](https://codingcontest.pason.com/tanks/index.html?mode=log&matchToken=ec996560-9b11-4704-a18e-26c87583b585)
4. Think Tank vs. One Lightning Trail [Semi-Semi-Semi finals](https://codingcontest.pason.com/tanks/index.html?mode=log&matchToken=a99256f2-f3c1-4184-96fa-71b291f56eaa)
5. Think Tank vs. Pason Employee jnielson (he beat us!) [Grudge Match](https://codingcontest.pason.com/tanks/index.html?mode=log&matchToken=a6d27e13-f670-4ce8-bfd9-93c5074f83e3)

How It Works
=========

The strategies for moving and firing are completely independent.

Movement
--------
The code for movement can be seen under [Strategies/differential_movement_strategy.cpp](https://github.com/karlaugsten/ThinkTank/blob/master/Strategies/differential_movement_strategy.cpp)

The movement works by modelling the playing field with a 'Goodness' equation. This goodness equation basically rates how good any spot on the playing field is at any point in time, based on the current game state (Position of enemy tanks, position of projectiles, terrain objects). The goal of each tank is simply to move in the direction with the highest goodness. The movement strategy does this by searching with a radius of ~0.2 meters around the tank using a ternary search (The playing field goodness is not unimodal but ternary search worked ~99.9% of the time!) for the maximum goodness.

The goodness equation uses a function similar to a gaussian distribution to model how good it is to be infront of a projectile. This then forms a well infront of the projectile, forcing the tanks to move out of the way of a projectile. The well also gets more deep the closer it is to the projectile. The goodness equation also gets worse the closer you are to a wall (by 1/r^2). It also gets worse the closer you are to an enemy/team mate, although it models this such that there is an optimal distance to be away from enemies/team mates.


Firing
------
The firing strategy works by predicting the enemies movements, and firing at a random spot infront of the tank based on the prediction. Randomness was the key since we could not fully predict the enemies movements! 
We also had multiple firing strategies that would either firing directly at the opponent or directly at where we predict the opponent will be. These strategies are available under '/Strategies/'.


Game State Parsing
------------------
The game state was sent to the client from the server over a state channel, using a json object. We had a 'StateParser' class which would parse the json object into game objects (Terrain, Players, Tanks, Projectiles, etc..). This State Parser class would be running on its own thread, constantly parsing states.

Building
--------
To build the client, you will need to install zeromq and zmq.hpp. Then simply make a build directory `mkdir build` and `cd build` then generate the makefile by running `cmake ..` and then build the ThinkTank executable by running `make ThinkTank`. 

