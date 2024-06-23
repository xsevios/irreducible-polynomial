#include "utils.h"
#include <iostream>
#include <cstring>

using namespace std;

#define _CONFIG "environ.conf"
#define _LIB "libirredpoly.so"

void usage(const char *prog)
{
    cout << "Usage:" << endl << prog << " [<options>]" << endl <<
    "    -c config_file           use config_file as configuration file" << endl <<
    "    -l library_file          use library_file as library file" << endl;
}

int main(int argc, char **argv)
{
    char const* conf_path = _CONFIG;
    char const* lib_path = _LIB;
    
    int c = 1;
    while ( c < argc )
    {
        if (strcmp(argv[c], "-c") == 0)
        {
            if (++c >= argc)
            {
                cout << "Runtime-Error: -c option requires an input argument" << endl;
                usage(argv[0]);
                return 1;
            }
            else
                conf_path = argv[c];
        }
        
        if (strcmp(argv[c], "-l") == 0)
        {
            if (++c >= argc)
            {
                cout << "Runtime-Error: -l option requires an input argument" << endl;
                usage(argv[0]);
                return 1;
            }
            else
                lib_path = argv[c];
        }
        
        c++;
    }

    int minDim              = 1;
    int maxDim              = 400;
    int minDegree           = 2;
    int maxDegree           = 50;
    int polyCountForEach    = 1000;

    std::vector<Polynom> polynoms = PolyGenerator::generatePolynoms(minDim, maxDim, minDegree, maxDegree, polyCountForEach);

    ofstream polysFile("polynoms.txt", ios::trunc);

    polysFile << polyCountForEach;
    polysFile << std::endl;

    for (auto &p : polynoms)
    {
        polysFile << p;
        polysFile << std::endl;
    }
    
    return 0;
}