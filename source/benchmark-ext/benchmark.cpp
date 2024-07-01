/**
* \file
* \brief Файл с реализацией методов класса BenchmarkRunner
*/


#include <fstream>
#include <cstdlib>
#include <cassert>
#include <chrono>
#include <functional>

#include "LibraryInterface/LibraryInterface.h"
#include "benchmark.h"

using namespace std;
/**
 * Перегрузка оператора << для вывода полинома в формате
 * "ПРИЗНАК_ПРИВОДИМОСТИ РАЗМЕРНОСТЬ_ПОЛЯ КОЭФФИЦИЕНТ КОЭФФИЦИЕНТ ... КОЭФФИЦИЕНТ"
 */
ostream& operator<<(ostream& out, const PolynomExt& p)
{
    vector<int> coef = p.getRefCoef();
    for(int i = coef.size() - 1; i >= 0; i--)
        out << " " << coef[i];

    return out;
}
/**
 * \brief Конструктор класса
 * \param[in] libPath путь к библиотеки
 * \param[in] confPath путь к конфигурационному файлу
*/
BenchmarkRunner::BenchmarkRunner(string libPath) : libPath(libPath) {}
/**
 * \brief Деструктор класса
 * Освобождает память контейнера list<Polinom>
 */ 
BenchmarkRunner::~BenchmarkRunner()
{}

/**
 * \brief Загружает библиотеку по средствам класса LibraryInterface
 */
void BenchmarkRunner::loadLibrary()
{
    lib = new LibraryInterface(libPath);
    
    if(!(*lib))
    {
        std::cerr << lib->GetErrorMessage();
        exit(ERROR_LIB);
    }
}

std::vector<PolynomExt> BenchmarkRunner::generatePolynoms(const int minDim, const int maxDim, const int minDegree, const int maxDegree, const int polyCountForEach)
{
    std::vector<PolynomExt> polynoms;

    for (auto dim = minDim; dim <= maxDim; dim++)
    {
        if (!isPrime(dim))
            continue;

        for (auto degree = minDegree; degree <= maxDegree; degree++)
        {
            PolynomExt base(FieldExt::GetInstance(dim), std::vector<int>(degree, 1));

            for (auto i = 0; i < polyCountForEach; i++)
            {
                PolynomExt p = GenRandPolynom(base);
                polynoms.push_back(p);
            }
        }
    }

    return polynoms;
}

void BenchmarkRunner::run(std::vector<PolynomExt> &polynoms, Benchmark& benchmark)
{
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;

    auto method = std::get<1>(benchmark);
    auto checkRes = std::get<2>(benchmark);

    begin = std::chrono::steady_clock::now();
    for (auto& polynom : polynoms)
    {
        checkRes.push_back(method(polynom));
    }
    end = std::chrono::steady_clock::now();

    std::get<3>(benchmark) = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
}

std::vector<Benchmark> BenchmarkRunner::runAll(std::vector<PolynomExt>& polynoms)
{
    std::chrono::nanoseconds berlekampElapsed;
    std::chrono::nanoseconds cantorzassenhausElapsed;
    std::chrono::nanoseconds rabinsElapsed;

    std::vector<PolynomState> berlekampRes;
    std::vector<PolynomState> cantorzassenhausRes;
    std::vector<PolynomState> rabinsRes;

    std::vector<Benchmark> benchmarks = {
            {"Berlekamp",           PolynomChecker::BerlekampTest,          berlekampRes,           berlekampElapsed},
            {"Cantor-Zassenhaus",   PolynomChecker::CantorZassenhausTest,   cantorzassenhausRes,    cantorzassenhausElapsed},
            {"Rabin",               PolynomChecker::RabinsTest,             rabinsRes,              rabinsElapsed},
    };

    for (auto& benchmark : benchmarks)
    {
        run(polynoms, benchmark);
//        std::cout << (int)std::chrono::duration_cast<std::chrono::milliseconds>(std::get<3>(benchmark)).count() << std::endl;
    }

    for (int i = 0; i < berlekampRes.size(); i++)
    {
        if (berlekampRes[i]  != cantorzassenhausRes[i] || cantorzassenhausRes[i]  != rabinsRes[i])
        {
            std::cout << "Test results mismatch (" << berlekampRes[i] << "/" << cantorzassenhausRes[i] << "/" << rabinsRes[i] << ") for polynom №" << i + 1 << " " << polynoms[i]  << std::endl;
            assert(0);
        }
    }

    int irreducible = 0;
    for (auto& polynom : polynoms)
    {
        if (polynom.isIrreducible() == IRREDUCIBLE)
            irreducible++;
    }

    return benchmarks;
}

PolynomExt BenchmarkRunner::GenRandPolynom(const PolynomExt &p)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib1(0, p.GetField()->GetPrime() - 1);
    std::uniform_int_distribution<> distrib2(0, 1);

    PolynomExt pRes(p.GetField(), {});

    while (pRes.GetDegree() == 0)
    {
        for (int i = 0; i < p.getRefCoef().size(); i++)
        {
            pRes.getRefCoef().push_back(distrib1(gen) * distrib2(gen) % p.GetPrime());
        }

        pRes.RemoveLeadingZeros();
    }

    return pRes;
}
