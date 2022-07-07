/**
* \file
* \brief Файл с реализацией методов класса Benchmark
*/


#include <fstream>
#include <cstdlib>
#include <cassert>
#include <chrono>

#include "LibraryInterface/LibraryInterface.h"
#include "benchmark.h"

using namespace std;
/**
 * Перегрузка оператора << для вывода полинома в формате
 * "ПРИЗНАК_ПРИВОДИМОСТИ РАЗМЕРНОСТЬ_ПОЛЯ КОЭФФИЦИЕНТ КОЭФФИЦИЕНТ ... КОЭФФИЦИЕНТ"
 */
ostream& operator<<(ostream& out, const Polynom& p)
{
    out << p.isIrreducible() << " " << p.getDim();
    
    vector<int> coef = p.getCoef();
    for(unsigned i = 0; i < coef.size(); i++)
        out << " " << coef[i];

    return out;
}
/**
 * \brief Конструктор класса
 * \param[in] libPath путь к библиотеки
 * \param[in] confPath путь к конфигурационному файлу
*/
Benchmark::Benchmark(string libPath) : libPath(libPath) {}
/**
 * \brief Деструктор класса
 * Освобождает память контейнера list<Polinom>
 */ 
Benchmark::~Benchmark()
{}

/**
 * \brief Загружает библиотеку по средствам класса LibraryInterface
 */
void Benchmark::loadLibrary()
{
    lib = new LibraryInterface(libPath);
    
    if(!(*lib))
    {
        std::cerr << lib->GetErrorMessage();
        exit(ERROR_LIB);
    }
}

template<class T>
bool isPrime(T n)
{
    T n_sqrt = sqrt(n);

    if (n_sqrt * n_sqrt == n)
        return false;

    if (n % 2 == 0 && n != 2)
        return false;

    for (T i = 3; i <= n_sqrt; i += 2)
    {
        if (n % i == 0)
        {
            return false;
        }
    }

    return true;
}

void Benchmark::runBenchmark(const int maxDim, const int maxDegree, const int polyCountForEach)
{
    std::vector<Polynom> polynoms;
    for (auto dim = 2; dim <= maxDim; dim++)
    {
        if (!isPrime(dim))
            continue;

        std::cout << "Polynom dim=" << dim << std::endl;

        for (auto degree = 1; degree < maxDegree; degree++)
        {
            Polynom base(dim, std::vector<int>(degree, 1));

            for (auto i = 0; i < polyCountForEach; i++)
            {
                Polynom p = GenRandPolynom(base);
                polynoms.push_back(p);
                std::cout << p << std::endl;
            }
        }
    }

    std::vector<PolynomState> berlekampRes;
    std::vector<PolynomState> cantorzassenhausRes;
    int i = 0;
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;

    i = 0;
    std::cout << "--- Berlekamp benchmark started ---" << std::endl;
    begin = std::chrono::steady_clock::now();
    for (auto& polynom : polynoms)
    {
        berlekampRes.push_back(PolynomChecker::BerlekampTest(polynom));
        std::cout << "    " << ++i << "/" << polynoms.size();
    }
    end = std::chrono::steady_clock::now();
    std::cout << "--- Berlekamp benchmark completed ---" << std::endl;
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;

    i = 0;
    std::cout << "--- Cantor-Cassenhausen benchmark started ---" << std::endl;
    begin = std::chrono::steady_clock::now();
    for (auto& polynom : polynoms)
    {
        cantorzassenhausRes.push_back(PolynomChecker::CantorZassenhausTest(polynom));
        std::cout << "    " << ++i << "/" << polynoms.size();
    }
    end = std::chrono::steady_clock::now();
    std::cout << "--- Cantor-Cassenhausen benchmark completed ---" << std::endl;
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;

    for (int i = 0; i < berlekampRes.size(); i++)
    {
        if (berlekampRes[i] != cantorzassenhausRes[i])
            std::cout << "Test results mismatch (" << berlekampRes[i] << "/" << cantorzassenhausRes[i] << ") for polynom №" << i + 1 << " " << polynoms[i] << std::endl;
    }
}

Polynom Benchmark::GenRandPolynom(const Polynom &p)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib1(0, p.getPrime() - 1);
    std::uniform_int_distribution<> distrib2(0, 1);

    Polynom pRes(p.getDim(), {});

    while (pRes.getDegree() == 0)
    {
        for (int i = 0; i < p.getCoef().size(); i++)
        {
            pRes.getRefCoef().push_back(distrib1(gen) * distrib2(gen) % p.getPrime());
        }

        pRes.RemoveLeadingZeros();
    }

    return pRes;
}
