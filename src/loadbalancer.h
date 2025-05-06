#ifndef LOADBALANCER_H
#define LOADBALANCER_H

// Done for optimization EC
// This should count active particles, consider particle density in different regions, and distribute work more evenly among processes.

#include <vector>
#include "bodies.h"

struct WorkDistribution {
    int startIndex;
    int count;
};

class LoadBalancer {
private:
    static constexpr int GRID_SIZE = 4;  // Divide space into 4x4 grid
    std::vector<int> particleCounts;  // Particles per grid cell

    void countParticlesInGrid(const std::vector<bodies>& particles);
    
public:
    std::vector<WorkDistribution> distributeWork(
        const std::vector<bodies>& particles,
        int numProcesses
    );
};

#endif // LOADBALANCER_H