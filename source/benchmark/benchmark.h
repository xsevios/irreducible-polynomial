/**
\file
\brief Заголовочный файл класса Tester

Данный файл содержит в себе определение основного класса оболочки библиотеки
*/


#ifndef _BENCHMARK_H
#define _BENCHMARK_H

#include "LibraryInterface/LibraryInterface.h"
#include <chrono>
#include <functional>

using namespace std;
 
enum ErrorCode
{
    ERROR_LIB = 1,
    ERROR_READ_CONF,
    ERROR_READ_POLY,
    ERROR_WRITE_POLY
};

ostream& operator<<(ostream& out, const Polynom& p);
/**
 * \brief Основной класс оболочки библиотеки
 */ 
class Benchmark
{
    LibraryInterface  * lib         = nullptr;
    string              libPath;                ///<Путь к библиотеки, требующей подключения

public:
    Benchmark(string libPath);
    ~Benchmark();
    
    void loadLibrary();
    void runBenchmark(const int minDim, const int maxDim, const int minDegree, const int maxDegree, const int polyCountForEach);


    static Polynom GenRandPolynom(const Polynom& p);

    vector<Polynom> generatePolynoms(const int minDim, const int maxDim, const int minDegree, const int maxDegree, const int polyCountForEach);

    chrono::nanoseconds runBenchmark(vector<Polynom> &polynoms, std::function<PolynomState(const Polynom&)> method, vector<PolynomState> &checkRes);
};

#endif
