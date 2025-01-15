/**
* \file
* \brief Файл с реализацией методов класса PolynomGenerator
*/


#include <fstream>
#include <cstdlib>
#include <cassert>
#include <chrono>
#include <functional>

#include "polynom_generator.h"

using namespace std;

int intRand(const int& min, const int& max) {
    static thread_local std::random_device rd;
    static thread_local std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(generator);
}

/**
 * \brief Конструктор класса
*/
PolynomGenerator::PolynomGenerator() {}

/**
 * \brief Деструктор класса
 * Освобождает память контейнера list<Polinom>
 */ 
PolynomGenerator::~PolynomGenerator()
{}

std::vector<PolynomExt> PolynomGenerator::generatePolynoms(const int minDim, const int maxDim, const int minDegree, const int maxDegree, const int polyCountForEach)
{
    std::vector<PolynomExt> polynoms;

    for (auto dim = minDim; dim <= maxDim; dim++)
    {
        if (!isPrime(dim))
            continue;

        for (auto degree = minDegree; degree <= maxDegree; degree++)
        {
            std::cout << dim << " " << degree << std::endl;

            PolynomExt base(FieldExt::GetInstance(dim), std::vector<int>(degree + 1, 1));

            int nIrreducibleCount = 0;
            int nReducibleCount = 0;

            while (nIrreducibleCount + nReducibleCount < polyCountForEach)
            {
                PolynomExt p = GenRandPolynom(base);

                auto polyState = PolynomChecker::BerlekampTest(p);
//                polynoms.push_back(p);
//                nIrreducibleCount++;

                if (polyState == IRREDUCIBLE && nIrreducibleCount < (polyCountForEach / 2))
                {
                    nIrreducibleCount++;
                    polynoms.push_back(p);
                }

                if (polyState == REDUCIBLE && nReducibleCount < (polyCountForEach / 2))
                {
                    nReducibleCount++;
                    polynoms.push_back(p);
                }
            }
        }
    }

    return polynoms;
}

PolynomExt PolynomGenerator::GenRandPolynom(const PolynomExt &base)
{
    PolynomExt pRes = base;
    int nEnd = base.getRefCoef().size() - 1;

    for (int i = 0; i < nEnd; i++)
    {
        pRes[i] = intRand(0, base.GetPrime() - 1) * intRand(0, 1);
    }

    return pRes;
}

PolynomExt PolynomGenerator::GenRandPolynom(const FieldExt* field, int nDeg)
{
    if (field->IsPrimeField())
    {
        PolynomExt base(field, std::vector<int>(nDeg, intRand(1, base.GetPrime() - 1)));
        return GenRandPolynom(base);
    }
    else
    {
        ExtCoeffs extCoeffs;

        extCoeffs.emplace(nDeg, GenNonZeroCoef(field));

        for (int i = 0; i < nDeg; i++)
        {
            if (intRand(0, 1))
            {
                extCoeffs.emplace(i, GenNonZeroCoef(field));
            }
        }

        return PolynomExt(field, extCoeffs);
    }
}

/// Generates non zero coefficient for given field
PolynomExt PolynomGenerator::GenNonZeroCoef(const FieldExt *field)
{
    auto subField = field->GetSubfield();

    if (subField->IsPrimeField())
    {
        int nDeg = intRand(0, field->GetPolynom()->GetDegree() - 1);
        int initialCoef = intRand(1, field->GetPrime() - 1);

        PolynomExt base(subField, std::vector<int>(nDeg + 1, initialCoef));
        auto poly = GenRandPolynom(base);

        return poly;
    }
    else
    {
        int nDeg = intRand(0, field->GetPolynom()->GetDegree() - 1);
        return GenRandPolynom(subField, nDeg);
    }
}
