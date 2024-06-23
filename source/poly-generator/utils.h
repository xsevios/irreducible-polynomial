/**
\file
\brief Заголовочный файл класса Tester

Данный файл содержит в себе определение основного класса оболочки библиотеки
*/


#ifndef _GEN_UTILS_H
#define _GEN_UTILS_H

#include "LibraryInterface/LibraryInterface.h"
#include <chrono>
#include <functional>

using Benchmark = std::tuple<std::string, std::function<PolynomState(const Polynom& f)>, std::vector<PolynomState>, std::chrono::nanoseconds>;
 
enum ErrorCode
{
    ERROR_LIB = 1,
    ERROR_READ_CONF,
    ERROR_READ_POLY,
    ERROR_WRITE_POLY
};

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

ostream& operator<<(ostream& out, const Polynom& p);
/**
 * \brief Основной класс оболочки библиотеки
 */ 
class PolyGenerator
{
    LibraryInterface  * lib         = nullptr;
    string              libPath;                ///<Путь к библиотеки, требующей подключения

public:
    PolyGenerator(string libPath);
    ~PolyGenerator();
    
    void loadLibrary();

    std::vector<Benchmark> runAll(std::vector<Polynom>& polynoms);

    static Polynom GenRandPolynom(const Polynom& base);

    static std::vector<Polynom> generatePolynoms(const int minDim, const int maxDim, const int minDegree, const int maxDegree, const int polyCountForEach);

    static void run(std::vector<Polynom>& polynoms, Benchmark& benchmark);
};

#endif
