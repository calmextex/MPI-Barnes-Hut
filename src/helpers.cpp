#include "helpers.h"
#include <math.h>

const double rlimit = 0.03;

DistanceComponents calcDistance(double x1, double y1, double x2, double y2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    double rawDistance = sqrt(dx * dx + dy * dy);

    // Apply safety limit to distance
    double safeDistance = (rawDistance < rlimit) ? rlimit : rawDistance;

    return {safeDistance, dx, dy};
}