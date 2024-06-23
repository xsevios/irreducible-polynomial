/**
* \file
* \brief Файл с реализацией методов класса PolyGenerator
*/


#include <fstream>
#include <cstdlib>
#include <cassert>
#include <chrono>
#include <functional>

#include "LibraryInterface/LibraryInterface.h"
#include "utils.h"

using namespace std;
/**
 * Перегрузка оператора << для вывода полинома в формате
 * "ПРИЗНАК_ПРИВОДИМОСТИ РАЗМЕРНОСТЬ_ПОЛЯ КОЭФФИЦИЕНТ КОЭФФИЦИЕНТ ... КОЭФФИЦИЕНТ"
 */
ostream& operator<<(ostream& out, const Polynom& p)
{
    out << p.GetField()->GetDim();

    vector<int> coef = p.getCoef();
    for (int i = coef.size() - 1; i >= 0; i--)
        out << " " << coef[i];

    return out;
}
/**
 * \brief Конструктор класса
 * \param[in] libPath путь к библиотеки
 * \param[in] confPath путь к конфигурационному файлу
*/
PolyGenerator::PolyGenerator(string libPath) : libPath(libPath) {}
/**
 * \brief Деструктор класса
 * Освобождает память контейнера list<Polinom>
 */ 
PolyGenerator::~PolyGenerator()
{}

/**
 * \brief Загружает библиотеку по средствам класса LibraryInterface
 */
void PolyGenerator::loadLibrary()
{
    lib = new LibraryInterface(libPath);
    
    if(!(*lib))
    {
        std::cerr << lib->GetErrorMessage();
        exit(ERROR_LIB);
    }
}

std::vector<Polynom> PolyGenerator::generatePolynoms(const int minDim, const int maxDim, const int minDegree, const int maxDegree, const int polyCountForEach)
{
    std::vector<Polynom> polynoms;

    for (auto dim = minDim; dim <= maxDim; dim++)
    {
        if (!isPrime(dim))
            continue;

        for (auto degree = minDegree; degree <= maxDegree; degree++)
        {
            std::cout << dim << " " << degree << std::endl;

            Polynom base(dim, std::vector<int>(degree, 1));

            int nIrreducibleCount = 0;
            int nReducibleCount = 0;

            while (nIrreducibleCount + nReducibleCount < polyCountForEach)
            {
                Polynom p = GenRandPolynom(base);

                // auto polyState = PolynomChecker::BerlekampTest(p);
                polynoms.push_back(p);
                nIrreducibleCount++;

//                if (polyState == IRREDUCIBLE && nIrreducibleCount < (polyCountForEach / 2))
//                {
//                    nIrreducibleCount++;
//                    polynoms.push_back(p);
//                }
//
//                if (polyState == REDUCIBLE && nReducibleCount < (polyCountForEach / 2))
//                {
//                    nReducibleCount++;
//                    polynoms.push_back(p);
//                }
            }
        }
    }

    return polynoms;
}

void PolyGenerator::run(std::vector<Polynom> &polynoms, Benchmark& benchmark)
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

std::vector<Benchmark> PolyGenerator::runAll(std::vector<Polynom>& polynoms)
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

    std::thread thread[benchmarks.size()];

    for (int i = 0; i < benchmarks.size(); i++)
    {
        thread[i] = std::thread(PolyGenerator::run, std::ref(polynoms), std::ref(benchmarks[i]));
    }

    for (int i = 0; i < benchmarks.size(); i++)
    {
        thread[i].join();
    }

//    for (auto& benchmark : benchmarks)
//    {
//        run(polynoms, benchmark);
////        std::cout << (int)std::chrono::duration_cast<std::chrono::milliseconds>(std::get<3>(benchmark)).count() << std::endl;
//    }

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

Polynom PolyGenerator::GenRandPolynom(const Polynom &base)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib1(0, base.getPrime() - 1);
    std::uniform_int_distribution<> distrib2(0, 1);

    Polynom pRes = base;

    for (int i = 0; i < base.getCoef().size() - 1; i++)
    {
        pRes[i] = distrib1(gen) * distrib2(gen) % base.getPrime();
    }

    return pRes;
}
