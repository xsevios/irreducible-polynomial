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

using Benchmark = std::tuple<std::string, std::function<PolynomState(const PolynomExt& f)>, std::vector<PolynomState>, std::chrono::nanoseconds>;

ostream& operator<<(ostream& out, const PolynomExt& p);
/**
 * \brief Основной класс оболочки библиотеки
 */ 
class BenchmarkRunner
{
    LibraryInterface  * lib         = nullptr;
    string              libPath;                ///<Путь к библиотеки, требующей подключения

public:
    BenchmarkRunner(string libPath);
    ~BenchmarkRunner();
    
    void loadLibrary();

    std::vector<Benchmark> runAll(std::vector<PolynomExt>& polynoms);

    static PolynomExt GenRandPolynom(const PolynomExt& p);

    static std::vector<PolynomExt> generatePolynoms(const int minDim, const int maxDim, const int minDegree, const int maxDegree, const int polyCountForEach);

    void run(std::vector<PolynomExt>& polynoms, Benchmark& benchmark);
};

#endif
