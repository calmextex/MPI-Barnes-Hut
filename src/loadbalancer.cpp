#include "loadbalancer.h"
#include <numeric>

void LoadBalancer::countParticlesInGrid(const std::vector<bodies>& particles) {
    particleCounts.resize(GRID_SIZE * GRID_SIZE, 0);
    
    for (const auto& p : particles) {
        if (p.mass != -1) {
            int gridX = static_cast<int>(p.x / 4.0 * GRID_SIZE);
            int gridY = static_cast<int>(p.y / 4.0 * GRID_SIZE);
            gridX = std::min(GRID_SIZE - 1, std::max(0, gridX));
            gridY = std::min(GRID_SIZE - 1, std::max(0, gridY));
            particleCounts[gridY * GRID_SIZE + gridX]++;
        }
    }
}

std::vector<WorkDistribution> LoadBalancer::distributeWork(
    const std::vector<bodies>& particles,
    int numProcesses
) {
    countParticlesInGrid(particles);
    
    std::vector<WorkDistribution> distribution(numProcesses);
    int totalParticles = std::accumulate(particleCounts.begin(), 
                                       particleCounts.end(), 0);
    int particlesPerProcess = totalParticles / numProcesses;
    
    int currentProcess = 0;
    int currentCount = 0;
    int startIndex = 0;
    
    // Assign work based on particle density
    for (size_t i = 0; i < particles.size(); ++i) {
        if (particles[i].mass != -1) {
            currentCount++;
            
            if (currentCount >= particlesPerProcess && 
                currentProcess < numProcesses - 1) {
                distribution[currentProcess] = {startIndex, 
                                             static_cast<int>(i - startIndex + 1)};
                startIndex = i + 1;
                currentCount = 0;
                currentProcess++;
            }
        }
    }
    
    // Assign remaining particles to last process
    distribution[currentProcess] = {startIndex, 
                                  static_cast<int>(particles.size() - startIndex)};
    
    return distribution;
}