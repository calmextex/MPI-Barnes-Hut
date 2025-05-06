#ifndef _ARGPARSE_H
#define _ARGPARSE_H

#include <getopt.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

struct options_t {
    char *inputfilename;
    char *outputfilename;
    int steps;
    double theta;
    double dt;
    int n_bodies;
};

void get_opts(int argc, char **argv, struct options_t *opts);
#endif