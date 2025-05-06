#include "argparse.h"
#include <iostream>
#include <chrono>

using namespace std;
// modeled from the argparse example in Lab 1 from class

void get_opts(int argc, char **argv, struct options_t *opts) {
    if (argc == 1) {
        std::cout << "Usage:" << std::endl;
        std::cout << "\t[Required] --inputfilename or -i <input file name (char *)>" << std::endl;
        std::cout << "\t[Required]--outputfilename or -o <output file name (char *)>" << std::endl;
        std::cout << "\t[Required]--steps or -s <number of iterations (int)>" << std::endl;
        std::cout << "\t[Required]--theta or -t <threshold for MAC (double)>" << std::endl;
        std::cout << "\t[Required]--dt or -d <timestep (double)>" << std::endl;
    }

    struct option l_opts[] = {
        {"inputfilename", required_argument, 0, 'i'},
        {"outputfilename", required_argument, 0, 'o'},
        {"steps", required_argument, 0, 's'},
        {"theta", required_argument, 0, 't'},
        {"dt", required_argument, 0, 'd'},
    };

    int ind, c;
    while ((c = getopt_long(argc, argv, "i:o:s:t:d:", l_opts, &ind)) != -1) {
        switch (c) {
            case 0:
                break;
            case 'i':
                opts->inputfilename = (char *)optarg;
                break;
            case 'o':
                opts->outputfilename = (char *)optarg;
                break;
            case 's':
                opts->steps = atoi((char *)(optarg));
                break;
            case 't':
                opts->theta = atof((char *)(optarg));
                break;
            case 'd':
                opts->dt = atof((char *)(optarg));
                break;
            case ':':
                std::cerr << argv[0] << ": option '-" << (char)optopt << "' requires an argument" << std::endl;
                exit(1);
        }
    }
}