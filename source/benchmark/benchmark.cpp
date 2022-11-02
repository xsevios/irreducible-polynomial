/**
* \file
* \brief Файл с реализацией методов класса Benchmark
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
ostream& operator<<(ostream& out, const Polynom& p)
{
    vector<int> coef = p.getCoef();
    for(int i = coef.size() - 1; i >= 0; i--)
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

std::vector<Polynom> Benchmark::generatePolynoms(const int minDim, const int maxDim, const int minDegree, const int maxDegree, const int polyCountForEach)
{
    std::vector<Polynom> polynoms;

    for (auto dim = minDim; dim <= maxDim; dim++)
    {
        if (!isPrime(dim))
            continue;

        for (auto degree = minDegree; degree <= maxDegree; degree++)
        {
            Polynom base(dim, std::vector<int>(degree, 1));

            for (auto i = 0; i < polyCountForEach; i++)
            {
                Polynom p = GenRandPolynom(base);
                polynoms.push_back(p);
            }
        }
    }

    return polynoms;
}

std::chrono::nanoseconds Benchmark::runBenchmark(std::vector<Polynom> &polynoms, std::function<PolynomState(const Polynom&)> method, std::vector<PolynomState> &checkRes)
{
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;

    begin = std::chrono::steady_clock::now();
    for (auto& polynom : polynoms)
    {
        checkRes.push_back(method(polynom));
    }
    end = std::chrono::steady_clock::now();

    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
}

void Benchmark::runBenchmark(const int minDim, const int maxDim, const int minDegree, const int maxDegree, const int polyCountForEach)
{
    std::cout << "--- Generation polynoms started ---" << std::endl;
    std::cout << "Min dim: " << minDim << std::endl;
    std::cout << "Max dim: " << maxDim << std::endl;
    std::cout << "Min degree: " << minDegree << std::endl;
    std::cout << "Max degree: " << maxDegree << std::endl;
    std::cout << "Number of polynoms for each dim and degree: " << polyCountForEach << std::endl;
    std::vector<Polynom> polynoms = generatePolynoms(minDim, maxDim, minDegree, maxDegree, polyCountForEach);
    std::cout << "--- Generation polynoms completed ---" << std::endl;
    std::cout << "Number of generated polynoms: " << polynoms.size() << std::endl;

    std::cout << std::endl;

    using benchmarkData = std::tuple<std::string, std::function<PolynomState(const Polynom& f)>, std::vector<PolynomState>, std::chrono::nanoseconds>;

    std::chrono::nanoseconds berlekampElapsed;
    std::chrono::nanoseconds cantorzassenhausElapsed;
    std::chrono::nanoseconds rabinsElapsed;

    std::vector<PolynomState> berlekampRes;
    std::vector<PolynomState> cantorzassenhausRes;
    std::vector<PolynomState> rabinsRes;

    std::vector<benchmarkData> toRun = {
            {"Berlekamp",           PolynomChecker::BerlekampTest,          berlekampRes,           berlekampElapsed},
            {"Cantor-Zassenhaus",   PolynomChecker::CantorZassenhausTest,   cantorzassenhausRes,    cantorzassenhausElapsed},
            {"Rabin",               PolynomChecker::RabinsTest,             rabinsRes,              rabinsElapsed},
    };

    for (auto run : toRun)
    {
        std::cout << "--- " << std::get<0>(run) << " benchmark started ---" << std::endl;
        std::get<3>(run) = runBenchmark(polynoms, std::get<1>(run), std::get<2>(run));
        std::cout << "--- " << std::get<0>(run) << " benchmark completed ---" << std::endl;
        std::cout << "Elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::get<3>(run)).count() << "[ms]" << std::endl;
        std::cout << std::endl;
    }

    for (int i = 0; i < berlekampRes.size(); i++)
    {
        if (berlekampRes[i]  != cantorzassenhausRes[i] || cantorzassenhausRes[i]  != rabinsRes[i])
        {
            std::cout << "Test results mismatch (" << berlekampRes[i] << "/" << cantorzassenhausRes[i] << "/" << rabinsRes[i] << ") for polynom №" << i + 1 << " " << polynoms[i]  << std::endl;
            assert(0);
        }
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
