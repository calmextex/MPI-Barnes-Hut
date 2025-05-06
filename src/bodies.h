#ifndef BODIES_H
#define BODIES_H

#include <vector>
#include <argparse.h>
#include "helpers.h"

struct bodies {
    int id; //index of the body
    double x; //x position
    double y; //y position
    double vx; //x velocity
    double vy; //y velocity
    double mass; //mass
    ForceAccumulator forces; //forces

    bodies(int id=0, double x=0, double y=0, double vx=0, double vy=0,
           double mass=0, double fx=0, double fy=0) {
        this->id = id;
        this->x = x;
        this->y = y;
        this->vx = vx;
        this->vy = vy;
        this->mass = mass;
        this->forces.fx = fx;
        this->forces.fy = fy;
    }

    void addForce(double dfx, double dfy) {
        forces.fx += dfx;
        forces.fy += dfy;
    }

    void resetForce();
    void leapFrogVerlet(double dt);
};

#endif // BODIES_H
