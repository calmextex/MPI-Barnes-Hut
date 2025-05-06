#include <bodies.h>
#include "helpers.h"
#include <iostream>

using namespace std;

// Bodies.cpp contains the implementation of the bodies class. 
// Sets the intial values for the bodies, and calculate the new positions and velocities for the bodies
// LeapFrogVerlet is taken from assignment page

void bodies::resetForce() {
    forces.fx = 0;
    forces.fy = 0;
}

void bodies::leapFrogVerlet(double dt) {
    if (mass == -1) {
        return;
    }
    //debugging, noticed that bodies.vx and vy were zero
    //cout << "After update - Body " << id << " velocities: " << vx << ", " << vy << endl;
    

    // Calculate acceleration using accumulated forces
    double ax = forces.fx / mass;
    double ay = forces.fy / mass;

    // Store current velocities
    double old_vx = vx;
    double old_vy = vy;

    // Calculate new velocities first
    vx = old_vx + (ax * dt);
    vy = old_vy + (ay * dt);

    // Calculate new positions using average of old and new velocities
    x = x + (0.5 * (old_vx + vx) * dt);
    y = y + (0.5 * (old_vy + vy) * dt);
    
    // Check boundaries
    if (x < 0 || x > 4 || y < 0 || y > 4) {
        mass = -1;
    }
}