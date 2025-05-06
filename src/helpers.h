#ifndef HELPERS_H
#define HELPERS_H

// Distance components struct
struct DistanceComponents {
    double distance;
    double dx;
    double dy;
};

// Force accumulator struct
struct ForceAccumulator {
    double fx;
    double fy;

    void reset() {
        fx = 0.0;
        fy = 0.0;
    }

    void add(double dfx, double dfy) {
        fx += dfx;
        fy += dfy;
    }

    void addProjected(double totalForce, double dx, double dy, double distance) {
        fx += totalForce * dx / distance;
        fy += totalForce * dy / distance;
    }
};

// Distance calculation function
DistanceComponents calcDistance(double x1, double y1, double x2, double y2);

#endif