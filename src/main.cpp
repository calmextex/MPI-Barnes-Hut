#include <mpi.h>
#include <iostream>
#include <vector>
#include "argparse.h"
#include "io.h"
#include "quadtree.h"
#include "bodies.h"
#include "loadbalancer.h"

using namespace std;

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    struct options_t opts;
    if (rank == 0) {
        get_opts(argc, argv, &opts);
    }
    // Broadcast input parameters to all processes
    MPI_Bcast(&opts.n_bodies, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&opts.theta, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&opts.dt, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&opts.steps, 1, MPI_INT, 0, MPI_COMM_WORLD);
    //Root process reads the particle data, and the others allocate the memory
    vector<bodies> particles;
    if (rank == 0) {
        read_file(&opts, &particles); // read to get the number of particles
    }

    MPI_Bcast(&opts.n_bodies, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (rank != 0) {
        particles.resize(opts.n_bodies);
    }
    // initialize the load balancere and work distribution struct
    LoadBalancer loadBalancer;
    vector<WorkDistribution> workDist;
    
    // vectors that will receive the counts and dispalcements for MPI_Gatherv
    vector<int> recvcounts(size);
    vector<int> displs(size);


    // Root process distributes the work and broadcasts the distribution
    if (rank == 0) {
      workDist = loadBalancer.distributeWork(particles, size);
    }
    workDist.resize(size);  // Ensure all processes have space allocated
    MPI_Bcast(workDist.data(), size * sizeof(WorkDistribution), MPI_BYTE, 0, MPI_COMM_WORLD);

    // Calculate dynamic update frequency
    int update_freq;
    if (opts.steps <= 1) {
        update_freq = opts.steps + 1;
    } else if (opts.steps <= 100) {
        update_freq = opts.steps / 2;
    } else {
        update_freq = opts.steps / 10;
    }

    double start_time = 0.0;
    if (rank == 0) {
        start_time = MPI_Wtime();
    }

    // Simulation loop for the number of steps
    for (int step = 0; step < opts.steps; step++) {
      // Broadcast particle data to all processes
      MPI_Bcast(particles.data(), opts.n_bodies * sizeof(bodies), MPI_BYTE, 0, MPI_COMM_WORLD);
      // Perform dynamic load balancing at specified intervals
      if (step == 0 || (update_freq > 0 && step % update_freq == 0)) {
        if (rank == 0) {
          workDist = loadBalancer.distributeWork(particles, size);
        }
        MPI_Bcast(workDist.data(), size * sizeof(WorkDistribution), MPI_BYTE, 0, MPI_COMM_WORLD);
        // Calculate receive counts and displacements for gathering data
        int pos = 0;
        for (int i = 0; i < size; i++) {
          recvcounts[i] = workDist[i].count * sizeof(bodies);
          displs[i] = pos * sizeof(bodies);
          pos += workDist[i].count;
        }
      }
      // Construct a quadtree for force calculation
      QuadTree tree(0, 4.0, 0.0, 0.0);
      for (bodies& p : particles) {
        if (p.mass != -1) {
          tree.insertBody(p);
        }
      }
      // Each process calculates forces for its assigned particles
      int start = workDist[rank].startIndex;
      int count = workDist[rank].count;

      for (int i = start; i < start + count; i++) {
        if (particles[i].mass != -1) {
          particles[i].resetForce();
          tree.calculateNetForce(particles[i], opts.theta);
          particles[i].leapFrogVerlet(opts.dt);
        }
      }
      // Gather updated particle data at the root process
      if (rank == 0) {
        MPI_Gatherv(MPI_IN_PLACE, 0, MPI_BYTE, particles.data(), recvcounts.data(), displs.data(), MPI_BYTE, 0, MPI_COMM_WORLD);
      } else {
        MPI_Gatherv(&particles[start], count * sizeof(bodies), MPI_BYTE, nullptr, nullptr, nullptr, MPI_BYTE, 0, MPI_COMM_WORLD);
      }
    }
    // Measure and print execution time, and write results to file
    if (rank == 0) {
      double end_time = MPI_Wtime();
      printf("%f\n", end_time - start_time);
      write_file(&opts, &particles);
    }

    MPI_Finalize();
    return 0;
}