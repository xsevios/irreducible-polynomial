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

ostream& operator<<(ostream& out, const PolynomExt& p);

/**
 * \brief Основной класс оболочки библиотеки
 */ 
class PolynomGenerator
{
public:
    PolynomGenerator();
    ~PolynomGenerator();

    static PolynomExt GenRandPolynom(const PolynomExt& base);
    static PolynomExt GenRandPolynom(const FieldExt* field, int nDeg);
    static PolynomExt GenNonZeroCoef(const FieldExt* field);

    static std::vector<PolynomExt> generatePolynoms(const int minDim, const int maxDim, const int minDegree, const int maxDegree, const int polyCountForEach);
};

#endif
