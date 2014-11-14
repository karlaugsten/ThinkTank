#include "gamestate.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "../util.h"
#include <iostream>
#include <string>
#include <Python/Python.h>

GameState::GameState(rapidjson::Document &dom)
{
    rapidjson::Value& tr = dom["timeRemaining"];
    timeRemaining = tr.GetDouble();
    tr = dom["timestamp"];
    timestamp = tr.GetDouble();
    map = MapState(dom["map"]);

    // Parse players array
    const rapidjson::Value& players = dom["players"];
    assert(players.IsArray());
    Player firstPlayer;
    Player secondPlayer;
    if(players.Size() == 1){
        firstPlayer = Player(players[0]);
    }else{
        firstPlayer = Player(players[0]);
        secondPlayer = Player(players[1]);
    }

    if(firstPlayer.name == "Think Tank"){

        player = firstPlayer;
        opponent = secondPlayer;
    } else if(secondPlayer.name == "Think Tank"){
        player = secondPlayer;
        opponent = firstPlayer;
    }
    else {
        // Could not find our team in the map state!
        assert(false);
    }

    // make additional array of projectiles for easy access to all projectiles.
    if(player.alive) {
        projectiles.insert(projectiles.end(), player.TankFast.projectiles.begin(), player.TankFast.projectiles.end());
        projectiles.insert(projectiles.end(), player.TankSlow.projectiles.begin(), player.TankSlow.projectiles.end());

    }
    if(opponent.alive) {
        projectiles.insert(projectiles.end(), opponent.TankFast.projectiles.begin(), opponent.TankFast.projectiles.end());
        projectiles.insert(projectiles.end(), opponent.TankSlow.projectiles.begin(), opponent.TankSlow.projectiles.end());
    }

    this->CalculateLineOfSight();
}

GameState::~GameState(){
}

void GameState::CalculateLineOfSight(){
    // Go through and construct line of site polygons for fast and slow tanks

    // firstly construct vector of segments
    typedef std::pair<Position, Position> segment;
    std::vector<std::pair<Position, Position> > segs;
    segs.push_back(segment(Position(0,0), Position(0, map.height)));
    segs.push_back(segment(Position(0,0), Position(map.width, 0)));
    segs.push_back(segment(Position(0,map.height), Position(map.width, map.height)));
    segs.push_back(segment(Position(map.width,0), Position(map.width, map.height)));

    for(int i = 0; i < map.terrain.size(); i++) if(map.terrain[i].Type == TerrainType::SOLID){
            Terrain t = map.terrain[i];

            segs.push_back(segment(t.position, Position(t.position.x + t.size.x, t.position.y)));
            segs.push_back(segment(t.position, Position(t.position.x, t.position.y + t.size.y)));
            segs.push_back(segment(Position(t.position.x + t.size.x, t.position.y), Position(t.position.x + t.size.x, t.position.y + t.size.y)));
            segs.push_back(segment(Position(t.position.x, t.position.y + t.size.y), Position(t.position.x + t.size.x, t.position.y + t.size.y)));
        }
    // now for every point in the segments, construct a new segment and find the closest intersection with another segment
    std::vector<Position> sight;

    Position tank = opponent.TankFast.position;

    for(int k = 0; k < 1000; k++){
        double r = 0.5;
        double dx = r*cos(double(k)*2*acos(-1)/1000.0);
        double dy = r*sin(double(k)*2*acos(-1)/1000.0);
        Position curPos(tank.x + dx, tank.y + dy);
        Position closest;
        double minDist = 1E30;
        // far is some point far away along the line from tank->pos[k]
        Position tmp = curPos - tank;
        tmp = tmp.Normalize();
        Position far = Position(tank.x + tmp.x*10000.0, tank.y + tmp.y*10000.0);
        //std::cout << far.x << " " << far.y << std::endl;
        for(int i = 0; i < segs.size(); i++){
            segment cur = segs[i];
            Position intersect1;
            if(Util::lineIntersectSeg(cur.first, cur.second, tank, far, intersect1)){
                // if we find an intersection which is closer than the current pos, it cant be in the line of sight polygon.
                if(intersect1.Distance(tank) < minDist){
                    minDist = intersect1.Distance(tank);
                    closest = intersect1;
                }
            }
        }
        //std::cout << closest.x << " " << closest.y << std::endl;
        sight.push_back(closest);


    }
    //Util::pivot = tank;
    //std::sort(sight.begin(), sight.end(), Util::angleCmp);
    this->fastLineOfSight.clear();
    int p, q, r;
    p = 0; q = 1; r = 2;
    this->fastLineOfSight.push_back(sight[p]);
    while(r < sight.size()){
        if(Util::collinear(sight[p], sight[q], sight[r])){
            q++;
            r++;
        } else {
            //std::cout << p << " " << q << " " << r << std::endl;

            this->fastLineOfSight.push_back(sight[q]);
            this->fastLineOfSight.push_back(sight[r]);
            int tmp = r;
            q+= 2;
            r+= 2;
            p = tmp;
        }
    }
    //std::cout << this->fastLineOfSight.size() << std::endl;
}

