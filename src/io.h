#ifndef _IO_H
#define _IO_H

#include <argparse.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include "bodies.h"

using namespace std;

// Add function declarations
void read_file(struct options_t* args, vector<bodies>* bodies);
void write_file(struct options_t* args, vector<bodies>* bodies);

#endif