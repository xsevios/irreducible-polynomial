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
    setbuf(stdout, 0);

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

    BenchmarkRunner benchmark(lib_path);

//    for (auto i = 2; i < 1100; i++)
//    {
//        if (isPrime(i))
//        {
//            bigint two = 2;
//            bigint limit = two ^ (bigint)63;
//
//            bigint cur = i;
//
//            bigint q = 63;
//            while ((cur ^ q) > limit)
//                q = q - 1;
//
//            std::cout << "{" << i << ", " << q << "}," << std::endl;
//        }
//    }

    benchmark.loadLibrary();

    int minDim              = 5;
    int maxDim              = 6;
    int minDegree           = 3;
    int maxDegree           = 30;
    int polyCountForEach    = 1000;
    int totalBenchmarks     = (maxDim - minDim) * (maxDegree - minDegree);
    int currentBenchmark    = 0;

    float progress = 0.0;
    int barWidth = 70;

    ofstream berlekampFile("berlekamp.txt", ios::trunc);
    ofstream cantorzassenhaus("cantorzassenhaus.txt", ios::trunc);
    ofstream rabin("rabin.txt", ios::trunc);

    for (int dim = minDim; dim < maxDim; dim++)
    {
        if (isPrime(dim))
        {
            for (int deg = minDegree; deg < maxDegree; deg += 1)
            {
                std::vector<Polynom> polynoms = BenchmarkRunner::generatePolynoms(dim, dim, deg, deg, polyCountForEach);
                auto results = benchmark.runAll(polynoms);

                for (auto& result : results)
                {
                    auto duration = std::get<3>(result);
                    duration.count();
                }

                berlekampFile << deg << " " << (int)std::chrono::duration_cast<std::chrono::milliseconds>(std::get<3>(results[0])).count() << " ";
                cantorzassenhaus << deg << " " << (int)std::chrono::duration_cast<std::chrono::milliseconds>(std::get<3>(results[1])).count() << " ";
                rabin << deg << " " << (int)std::chrono::duration_cast<std::chrono::milliseconds>(std::get<3>(results[2])).count() << " ";

                berlekampFile << std::endl;
                cantorzassenhaus << std::endl;
                rabin << std::endl;

                currentBenchmark++;
                progress = (float)currentBenchmark / totalBenchmarks;

                std::cout << "[";
                int pos = barWidth * progress;
                for (int i = 0; i < barWidth; ++i)
                {
                    if (i < pos) std::cout << "=";
                    else if (i == pos) std::cout << ">";
                    else std::cout << " ";
                }
                std::cout << "] " << int(progress * 100.0) << " %\r";
                std::cout.flush();
            }
        }
        else
        {
            currentBenchmark += maxDegree - minDegree;
            progress = (float)currentBenchmark / totalBenchmarks;

            std::cout << "[";
            int pos = barWidth * progress;
            for (int i = 0; i < barWidth; ++i)
            {
                if (i < pos) std::cout << "=";
                else if (i == pos) std::cout << ">";
                else std::cout << " ";
            }
            std::cout << "] " << int(progress * 100.0) << " %\r";
            std::cout.flush();
        }
    }
    std::cout << std::endl;

    berlekampFile.close();
    cantorzassenhaus.close();
    rabin.close();
    
    return 0;
}