#include "benchmark.h"
#include <iostream>
#include <cstring>

using namespace std;

#define _CONFIG "environ.conf"
#define _LIB "libirredpoly.so"

void usage(const char *prog)
{
    cout << "Usage:" << endl << prog << " [<options>]" << endl <<
    "    -p polynoms_file         use prepared polynoms from polynoms_file" << endl <<
    "    -c config_file           use config_file as configuration file" << endl <<
    "    -l library_file          use library_file as library file" << endl;
}

void printProgress(int currentBenchmark, int totalBenchmarks, int barWidth = 70)
{
    float progress = (float) currentBenchmark / totalBenchmarks;

    std::cout << "[";
    int      pos = barWidth * progress;
    for (int i   = 0; i < barWidth; ++i)
    {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " % (" << currentBenchmark << "/" << totalBenchmarks << ")\r";
    std::cout.flush();
}

int get_result_ms(std::vector<Benchmark>& results, int index)
{
    return (int) std::chrono::duration_cast<std::chrono::milliseconds>(std::get<3>(results[index])).count();
}

// -p /mnt/d/Development/Projects/irreducible-polynomial/pari/polynoms.txt
int main(int argc, char **argv)
{
    char const* conf_path = _CONFIG;
    char const* lib_path = _LIB;
    char const* poly_path = nullptr;
    
    int c = 1;
    while (c < argc)
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

        if (strcmp(argv[c], "-p") == 0)
        {
            if (++c >= argc)
            {
                cout << "Runtime-Error: -p option requires an input argument" << endl;
                usage(argv[0]);
                return 1;
            }
            else
                poly_path = argv[c];
        }
        
        c++;
    }

    BenchmarkRunner benchmark(lib_path);

    benchmark.loadLibrary();

    int minDim              = 1;
    int maxDim              = 1000;
    int minDegree           = 1;
    int maxDegree           = 50;
    int polyCountForEach    = 1000;
    int totalBenchmarks     = (maxDim - minDim) * (maxDegree - minDegree);
    int currentBenchmark    = 0;

    float progress = 0.0;
    int barWidth = 70;

    ofstream berlekampFile("berlekamp.txt", ios::trunc);
    ofstream cantorzassenhaus("cantorzassenhaus.txt", ios::trunc);
    ofstream rabin("rabin.txt", ios::trunc);

    int berlekampTotalTimeMs        = 0;
    int cantorzassenhausTotalTimeMs = 0;
    int rabinTotalTimeMs            = 0;

    if (poly_path == nullptr)
    {
        for (int dim = minDim; dim < maxDim; dim++)
        {
            if (isPrime(dim))
            {
                for (int deg = minDegree; deg < maxDegree; deg += 1)
                {
                    std::vector<Polynom> polynoms = BenchmarkRunner::generatePolynoms(dim, dim, deg, deg,
                                                                                      polyCountForEach);
                    auto                 results  = benchmark.runAll(polynoms);

                    auto berlekampMs        = get_result_ms(results, 0);
                    auto cantorzassenhausMs = get_result_ms(results, 1);
                    auto rabinMs            = get_result_ms(results, 2);

                    berlekampTotalTimeMs += berlekampMs;
                    cantorzassenhausTotalTimeMs += cantorzassenhausMs;
                    rabinTotalTimeMs += rabinMs;

                    berlekampFile << berlekampMs << " ";
                    cantorzassenhaus << cantorzassenhausMs << " ";
                    rabin << rabinMs << " ";

                    currentBenchmark++;
                    printProgress(currentBenchmark, totalBenchmarks);
                }

                berlekampFile << std::endl;
                cantorzassenhaus << std::endl;
                rabin << std::endl;
            }
            else
            {
                currentBenchmark += maxDegree - minDegree;
                printProgress(currentBenchmark, totalBenchmarks);
            }
        }
    }
    else
    {
        ifstream polysFile(poly_path, ios::in);

        std::string line;
        std::getline(polysFile, line);

        std::istringstream iss(line);
        iss >> polyCountForEach;

        std::vector<Polynom> polynoms;
        while (std::getline(polysFile, line))
        {
            Polynom p(line);
            polynoms.push_back(p);
        }
        totalBenchmarks = polynoms.size();

        std::vector<Polynom> polynomsBuf;
        int nCurDim = polynoms[0].GetField()->GetDim();
        for (auto& p : polynoms)
        {
            if (p.GetField()->GetDim() > nCurDim)
            {
                nCurDim = p.GetField()->GetDim();
                berlekampFile << std::endl;
                cantorzassenhaus << std::endl;
                rabin << std::endl;
            }

            polynomsBuf.push_back(p);

            if (polynomsBuf.size() == polyCountForEach)
            {
                auto results  = benchmark.runAll(polynomsBuf);

                auto berlekampMs        = get_result_ms(results, 0);
                auto cantorzassenhausMs = get_result_ms(results, 1);
                auto rabinMs            = get_result_ms(results, 2);

                berlekampTotalTimeMs += berlekampMs;
                cantorzassenhausTotalTimeMs += cantorzassenhausMs;
                rabinTotalTimeMs += rabinMs;

                berlekampFile << berlekampMs << " ";
                cantorzassenhaus << cantorzassenhausMs << " ";
                rabin << rabinMs << " ";

                currentBenchmark += polyCountForEach;
                printProgress(currentBenchmark, totalBenchmarks);

                polynomsBuf.clear();
            }
        }
    }


    std::cout << std::endl;

    std::cout << "Berlakamp total ms: "         << berlekampTotalTimeMs         << std::endl;
    std::cout << "Cantor-Zassenhaus total ms: " << cantorzassenhausTotalTimeMs  << std::endl;
    std::cout << "Rabin total ms: "             << rabinTotalTimeMs             << std::endl;

    std::cout << std::endl;

    berlekampFile.close();
    cantorzassenhaus.close();
    rabin.close();
    
    return 0;
}