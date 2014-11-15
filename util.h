#ifndef UTIL_H
#define UTIL_H

#include "StateParser/positionstate.h"
#include <cmath>
#include <vector>
#include <iostream>
#include "StateParser/tankstate.h"
#include "StateParser/terrain.h"


class Util {

public:

    static double PI;

    static Position getPointOnLineWithDistanceFromCurrent(Position current, Position previous, double distance){
        //http://math.stackexchange.com/questions/175896/finding-a-point-along-a-line-a-certain-distance-away-from-another-point
        Position v= Position(current.x-previous.x,current.y-previous.y);
        double vMagnitude = sqrt(v.x*v.x+v.y*v.y);
        Position unitVector = Position(v.x/vMagnitude,v.y/vMagnitude);
        return Position(current.x+(unitVector.x*distance), current.y+(unitVector.y*distance));
    }

    static Position getCurrentTargetFromTankAngle(Tank &ourTank, double angle){
        Position toReturn;
        double x= cos(angle);
        double y= sin(angle);
        toReturn.x=ourTank.position.x+x*10000;
        toReturn.y=ourTank.position.y+y*10000;
        return toReturn;
    }

    static bool inSight(std::vector<Terrain> &vecTerrain, Position &tankPosition, Position &enemyPosition) {
        for(int i=0;i<vecTerrain.size();i++) {
            Terrain terrain = vecTerrain[i];
            if(terrain.Type==TerrainType::IMPASSABLE) continue; // If the current terrain type is impassible, look for solids
            Position upperLeft = Position(terrain.position.x, terrain.position.y+terrain.size.y);
            Position upperRight = Position(terrain.position.x+terrain.size.x, terrain.position.y+terrain.size.y);
            Position lowerRight = Position(terrain.position.x+terrain.size.x, terrain.position.y);
            Position lowerLeft = terrain.position;

            if(Util::doIntersect(tankPosition,enemyPosition,upperLeft,upperRight)){
                return false;
            }else if(Util::doIntersect(tankPosition,enemyPosition,upperRight,lowerRight)){
                return false;
            }else if(Util::doIntersect(tankPosition,enemyPosition,lowerRight,lowerLeft)){
                return false;
            }else if(Util::doIntersect(tankPosition,enemyPosition,lowerLeft,upperLeft)){
                return false;
            }
        }

        return true;
    }

    /*
    * returns true if tank is aiming at target tank
     */
    static bool aiming(Tank &tank, Tank &target){
        Position intersect;
        return Util::intersectLineCircle(target.position, target.hitRadius, tank.position, tank.turret, intersect);
    }

    static Position pivot;

    static bool collinear(const Position &p, const Position &q, const Position &r){
        return std::fabs(Util::cross(p,q,r)) < 1E-7;
    }

    static bool angleCmp(const Position &a, const Position &b){
        if(collinear(pivot, a, b)){
            return pivot.Distance(a) < pivot.Distance(b);
        }
        double d1x = a.x - pivot.x, d1y = a.y - pivot.y;
        double d2x = b.x - pivot.x, d2y = b.y - pivot.y;
        return (atan2(d1y, d1x) - atan2(d2y, d2x)) < 0.0;
    }

    static double distToLine(const Position &p, const Position &A, const Position &B, Position &c) {
        double scale = (double)
                ((p.x - A.x) * (B.x - A.x) + (p.y - A.y) * (B.y - A.y)) /
                ((B.x - A.x) * (B.x - A.x) + (B.y - A.y) * (B.y - A.y));
        c.x = A.x + scale * (B.x - A.x);
        c.y = A.y + scale * (B.y - A.y);
        return p.Distance(c);
    }

    static double distToLineSegment(const Position &p, const Position &A, const Position &B, Position &c) {
        if ((B.x - A.x) * (p.x - A.x) + (B.y - A.y) * (p.y - A.y) < 1E-7) {
            c.x = A.x;
            c.y = A.y;
            return p.Distance(A);
        }
        if ((A.x - B.x) * (p.x - B.x) + (A.y - B.y) * (p.y - B.y) < 1E-7) {
            c.x = B.x;
            c.y = B.y;
            return p.Distance(B);
        }

        return distToLine(p, A, B, c);
    }

    static double crossProduct(const Position a, const Position b) {
        return a.x * b.y - a.y * b.x;
    }

    static double cross(const Position &p, const Position &q, const Position &r) {
        return (r.x - q.x) * (p.y - q.y) - (r.y - q.y) * (p.x - q.x);
    }

    static double angle(const Position &a, const Position &b, const Position &c) {
        double ux = b.x - a.x, uy = b.y - a.y;
        double vx = c.x - a.x, vy = c.y - a.y;
        return acos((ux * vx + uy * vy) / sqrt((ux * ux + uy * uy) * (vx * vx + vy * vy)));
    }

    static bool insidePolygon(Position p, std::vector<Position> polygon) {
        if ((int) polygon.size() == 0) return false;
        double sum = 0.0;
        int n = polygon.size();
        for (int i = 0; i < n; i++) {
            if (cross(p, polygon[i], polygon[(i + 1) % n]) < 0.0) {
                sum -= angle(p, polygon[i], polygon[(i + 1) % n]);
            } else {
                sum += angle(p, polygon[i], polygon[(i + 1) % n]);
            }
        }

        return (fabs(sum - 2 * Util::PI) < 1E-7 || fabs(sum + 2 * Util::PI) < 1E-7);
    }

    static Position randPointInRange(Position a, Position b) {
        Position c;
        if (a == b) {
            return a;
        }
        if (a.x == b.x) {
            c.x = a.x;
            c.y = a.y + (b.y - a.y) * double(rand() / (1.0 + RAND_MAX));
            return c;
        }
        c.x = a.x + (b.x - a.x) * rand() / (1.0 + RAND_MAX);
        c.y = a.y + (c.x - a.x) * (a.y - b.y) / (a.x - b.x);
        return c;
    }

    static bool lineIntersectSeg(const Position &p, const Position &q, const Position &A, const Position &B, Position &ret){
        if(!doIntersect(p, q, A, B)) return false;
        double a = B.y - A.y;
        double b = A.x - B.x;
        double c = B.x * A.y - A.x * B.y;
        double u = fabs(a * p.x + b * p.y + c);
        double v = fabs(a * q.x + b * q.y + c);
        ret.x = (p.x * v + q.x * u)/(u+v);
        ret.y = (p.y * v + q.y * u)/(u+v);
        return true;
    }
/*
* returns true if a ray intersects a circle
* direction is in radians from the (1,0) vector.
* start is the start of the ray.
* outIntersection will have the closest intersection point.
 */
    static bool intersectLineCircle(Position &circle, double radius, Position &start, double direction, Position &outIntersection) {

        Position rayDir = Position(cos(direction), sin(direction));
        Position originNew = circle - start;

        double b = 2.0 * ((originNew.x * rayDir.x) + (rayDir.y * originNew.y));
        double c = (originNew.Norm2()) - radius * radius;
        double a = rayDir.Norm2();

        double delta = b * b - (4.0 * a * c);

        // Negative doesnt have square root
        if (delta <= 0.0) return false;
        // if delta is 0.0 it is tangent to circle

        double intersect1 = (-b - sqrt(delta)) / (2.0 * a);
        double intersect2 = (-b + sqrt(delta)) / (2.0 * a);

        double intersect = std::min(intersect1, intersect2);

        outIntersection.x = rayDir.x * intersect + start.x;
        outIntersection.y = rayDir.y * intersect + start.y;

        return true;
    }

// http://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
    static bool onSegment(const Position &p, const Position &q, const Position &r) {
        if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) && q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
            return true;
        return false;
    }

    static int orientation(const Position &p, const Position &q, const Position &r) {
        int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
        if (fabs(val) < 1E-7) return 0;
        return (val > 0) ? 1 : 2;
    }

    static bool doIntersect(const Position &p1, const Position &q1, const Position &p2, const Position &q2) {
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

};


#endif