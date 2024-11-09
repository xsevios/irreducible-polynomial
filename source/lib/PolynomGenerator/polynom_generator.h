/**
\file
\brief Заголовочный файл класса PolynomGenerator

Данный файл содержит в себе определение основного класса
*/


#ifndef _POLYNOM_GENERATOR_H
#define _POLYNOM_GENERATOR_H

#include "../lib.h"
#include <chrono>
#include <functional>

using Benchmark = std::tuple<std::string, std::function<PolynomState(const PolynomExt& f)>, std::vector<PolynomState>, std::chrono::nanoseconds>;
 
enum ErrorCode
{
    ERROR_LIB = 1,
    ERROR_READ_CONF,
    ERROR_READ_POLY,
    ERROR_WRITE_POLY
};

ostream& operator<<(ostream& out, const PolynomExt& p);

/**
 * \brief Основной класс оболочки библиотеки
 */ 
class PolynomGenerator
{
public:
    PolynomGenerator();
    ~PolynomGenerator();
    
    void loadLibrary();

    std::vector<Benchmark> runAll(std::vector<PolynomExt>& polynoms);

    static PolynomExt GenRandPolynom(const PolynomExt& base);
    static PolynomExt GenRandPolynom(const FieldExt* field, int nDeg);
    static PolynomExt GenNonZeroCoef(const FieldExt* field);

    static std::vector<PolynomExt> generatePolynoms(const int minDim, const int maxDim, const int minDegree, const int maxDegree, const int polyCountForEach);

    static void run(std::vector<PolynomExt>& polynoms, Benchmark& benchmark);
};

#endif
