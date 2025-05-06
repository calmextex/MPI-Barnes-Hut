#include "io.h"
#include "bodies.h"

// adapted from io.cpp in Lab 1
void read_file(struct options_t* args, std::vector<bodies>* bodies) {
    std::ifstream in;
 
    in.open(args->inputfilename);

    // Check if file opened successfully
    if (!in.is_open()) {
        throw std::runtime_error("Could not open input file: " + std::string(args->inputfilename));
    }

    // Read number of bodies
    if (!(in >> args->n_bodies)) {
        throw std::runtime_error("Failed to read number of bodies from input file");
    }

    // Validate number of bodies
    if (args->n_bodies <= 0) {
        throw std::runtime_error("Invalid number of bodies: " + std::to_string(args->n_bodies));
    }

    // Resize the vector
    bodies->resize(args->n_bodies);

    // Read body data
    for (int i = 0; i < args->n_bodies; i++) {
        if (!(in >> bodies->at(i).id >> bodies->at(i).x >> bodies->at(i).y
                 >> bodies->at(i).mass >> bodies->at(i).vx >> bodies->at(i).vy)) {
            throw std::runtime_error("Failed to read body data at index " + std::to_string(i));
        }
        //cout << "After reading - Body " << bodies->at(i).id 
        // << " velocities: " << bodies->at(i).vx << ", " << bodies->at(i).vy << endl;

        // Validate coordinates are within domain
        if (bodies->at(i).x < 0 || bodies->at(i).x > 4 ||
            bodies->at(i).y < 0 || bodies->at(i).y > 4) {
            throw std::runtime_error("Body " + std::to_string(bodies->at(i).id) +
                                     " coordinates outside valid domain (0,4)x(0,4)");
        }

        // Validate mass is positive
        if (bodies->at(i).mass <= 0) {
            throw std::runtime_error("Body " + std::to_string(bodies->at(i).id) +
                                     " has invalid mass: " + std::to_string(bodies->at(i).mass));
        }

        // Initialize forces
        bodies->at(i).forces.reset();
    }

    in.close();
}

void write_file(options_t* args, std::vector<bodies>* bodies) {
    std::ofstream out;
    out.open(args->outputfilename);

    // Check if file opened successfully
    if (!out.is_open()) {
        throw std::runtime_error("Could not open output file: " + std::string(args->outputfilename));
    }

    // Set precision for floating-point output
    out.precision(6);
    out << std::scientific;

    // Write number of bodies
    out << args->n_bodies << std::endl;

    // Write body data
    for (int i = 0; i < args->n_bodies; i++) {
        
        if (!(out << bodies->at(i).id << " "
                  << bodies->at(i).x << " "
                  << bodies->at(i).y << " "
                  << bodies->at(i).mass << " "
                  << bodies->at(i).vx << " "
                  << bodies->at(i).vy << std::endl)) {
            throw std::runtime_error("Failed to write body data at index " + std::to_string(i));
        }
    }

    // Ensure all data is written
    out.flush();
    if (out.fail()) {
        throw std::runtime_error("Failed to flush output file");
    }

    out.close();
}