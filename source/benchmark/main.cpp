#include "benchmark.h"
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

    Benchmark benchmark(lib_path);

    benchmark.loadLibrary();
    benchmark.runBenchmark(1, 7, 1, 10, 1000);
    
    return 0;
}