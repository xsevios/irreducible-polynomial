/**
* \file
* \brief Файл с реализацией методов класса Tester
*/


#include <fstream>
#include <cstdlib>
#include <cassert>
#include "LibraryInterface/LibraryInterface.h"
#include "tester.h"

using namespace std;
/**
 * Перегрузка оператора << для вывода полинома в формате
 * "ПРИЗНАК_ПРИВОДИМОСТИ РАЗМЕРНОСТЬ_ПОЛЯ КОЭФФИЦИЕНТ КОЭФФИЦИЕНТ ... КОЭФФИЦИЕНТ"
 */
ostream& operator<<(ostream& out, const Polynom& p)
{
    out << "isirreducible=" << p.isIrreducible() << " dim=" << p.getDim();
    
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
Tester::Tester(string libPath) : libPath(libPath) {}
/**
 * \brief Деструктор класса
 * Освобождает память контейнера list<Polinom>
 */ 
Tester::~Tester()
{}

/**
 * \brief Загружает библиотеку по средствам класса LibraryInterface
 */
void Tester::loadLibrary()
{
    lib = new LibraryInterface(libPath);
    
    if(!(*lib))
    {
        std::cerr << lib->GetErrorMessage();
        exit(ERROR_LIB);
    }
}

void Tester::runTest()
{
    assert(msb(0) == 0);
    assert(msb(1) == 0);
    assert(msb(2) == 1);
    assert(msb(3) == 1);
    assert(msb(4) == 2);
    assert(msb(9) == 3);

    assert(getMultInverse(1, 7) == 1);
    assert(getMultInverse(2, 7) == 4);

    auto divisors = PolynomChecker::GetPrimeDivisors(2);
    assert(PolynomChecker::GetPrimeDivisors(2)          == (std::vector<int>{2}));
    assert(PolynomChecker::GetPrimeDivisors(3)          == (std::vector<int>{3}));
    assert(PolynomChecker::GetPrimeDivisors(9)          == (std::vector<int>{3, 3}));
    assert(PolynomChecker::GetPrimeDivisors(18)         == (std::vector<int>{2, 3, 3}));
    assert(PolynomChecker::GetPrimeDivisors(36)         == (std::vector<int>{2, 2, 3, 3}));
    assert(PolynomChecker::GetPrimeDivisors(4)          == (std::vector<int>{2, 2}));
    assert(PolynomChecker::GetPrimeDivisors(4, true)    == (std::vector<int>{2}));
    assert(PolynomChecker::GetPrimeDivisors(8)          == (std::vector<int>{2, 2, 2}));
    assert(PolynomChecker::GetPrimeDivisors(8, true)    == (std::vector<int>{2}));
    assert(PolynomChecker::GetPrimeDivisors(1024)       == (std::vector<int>{2, 2, 2, 2, 2, 2, 2, 2, 2, 2}));
    assert(PolynomChecker::GetPrimeDivisors(1024, true) == (std::vector<int>{2}));
    assert(PolynomChecker::GetPrimeDivisors(3072, true) == (std::vector<int>{2, 3}));

    {
        Polynom p1(7, {6, 4, 5});
        Polynom p2(7, {1, 2});

        Polynom p(7, {});

        p = p1 + p2;
        assert(p[0] == 0);
        assert(p[1] == 6);
        assert(p[2] == 5);

        p = p1 - p2;
        assert(p[0] == 5);
        assert(p[1] == 2);
        assert(p[2] == 5);

        p = p1 * p2;
        assert(p[0] == 6);
        assert(p[1] == 2);
        assert(p[2] == 6);
        assert(p[3] == 3);

        p = p1 / 2;
        assert(p[0] == 3);
        assert(p[1] == 2);
        assert(p[2] == 6);

        p = p1 / p2;
        assert(p[0] == 6);
        assert(p[1] == 6);
    }

    // ----------------- Gauss ------------------

    {
        std::vector<vector<int>> matrix ={
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {1, 0, 1, 1},
            {1, 1, 1, 0},
        };

        std::vector<vector<int>> gaussMatrix;

        gaussMatrix = gauss(matrix, 2);
        int gaussRank = PolynomChecker::getRank(gaussMatrix, 2);
        assert(gaussRank == 3);
    }


    {
        std::vector<vector<int>> matrix ={
            {0, 0, 0, 1},
            {0, 0, 0, 1},
            {0, 0, 0, 1},
            {0, 0, 0, 1},
        };

        std::vector<vector<int>> gaussMatrix;

        gaussMatrix = gauss(matrix, 2);
        int gaussRank = PolynomChecker::getRank(gaussMatrix, 2);
        assert(gaussRank == 1);
    }

    {
        std::vector<vector<int>> matrix ={
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {0, 1, 1, 1},
            {0, 1, 1, 1},
        };

        std::vector<vector<int>> gaussMatrix;

        gaussMatrix = gauss(matrix, 2);
        int gaussRank = PolynomChecker::getRank(gaussMatrix, 2);
        assert(gaussRank == 2);
    }

    {
        std::vector<vector<int>> matrix ={
            {0, 0, 1, 0},
            {0, 0, 3, 0},
            {0, 0, 0, 2},
            {0, 0, 0, 1},
        };

        std::vector<vector<int>> gaussMatrix;

        gaussMatrix = gauss(matrix, 5);
        int gaussRank = PolynomChecker::getRank(gaussMatrix, 5);
        assert(gaussRank == 2);
    }

    {
        std::vector<vector<int>> matrix = {
            {3, 1, 0, 0},
            {1, 1, 3, 0},
            {0, 1, 2, 0},
            {0, 0, 1, 1},
        };

        std::vector<vector<int>> gaussMatrix;

        gaussMatrix = gauss(matrix, 5);
        int gaussRank = PolynomChecker::getRank(gaussMatrix, 5);
        assert(gaussRank == 3);
    }

    {
        std::vector<vector<int>> matrix ={
            {0, 0, 0, 0, 0},
            {0, 2, 0, 1, 0},
            {0, 2, 1, 1, 0},
            {1, 0, 1, 0, 2},
            {2, 1, 1, 0, 1},
        };

        std::vector<vector<int>> gaussMatrix;

        gaussMatrix = gauss(matrix, 3);
        int gaussRank = PolynomChecker::getRank(gaussMatrix, 3);
        assert(gaussRank == 4);
    }

    // -----------------------------------

    {
        Polynom p1(16, {1, 1, 0, 0, 1});
        Polynom p2(16, {1, 1, 0, 0, 1});

        Polynom p = p1 % p2;
        Polynom pExp(16, {});

        assert(p == pExp);
    }

    {
        Polynom p1(16, {0, 0, 0, 0, 1});
        Polynom p2(16, {1, 1, 0, 0, 1});

        Polynom p = p1 % p2;
        Polynom pExp(16, {1, 1});

        assert(p == pExp);
    }

    {
        Polynom p1(16, {1, 1, 0, 0, 0, 1});
        Polynom p2(16, {1, 1, 0, 0, 1});

        Polynom p = p1 % p2;
        Polynom pExp(16, {1, 0, 1});

        assert(p == pExp);
    }

    // -----------------------------------

    {
        Polynom f(5, {3, 2, 0, 2, 0, 1, 2, 0, 3, 0, 3, 0, 0, 1});
        Polynom g(5, {2, 0, 1, 0, 0, 2, 0, 4, 0, 0, 0, 0, 3});

        Polynom pGcd = gcd(f, g);

        assert(pGcd == Polynom(5, {1, 0, 4, 1, 0, 1, 0, 4, 1}));
    }

    {
        Polynom f(5, {3, 0, 3, 4, 0, 1, 1, 3, 1});
        Polynom g(5, {2, 4, 0, 3, 1, 1});

        Polynom pGcd = gcd(f, g);

        assert(pGcd == Polynom(5, {3, 0, 3, 1}));
    }

    // -----------------------------------

    {
        Polynom p1(3, {1, 0, 2, 2, 0, 1, 1, 0, 2, 2, 0, 1});
        Factors SffFactors = PolynomChecker::SquareFreeFactorization(p1);

        Factors factors;
        factors.insert({1, Polynom(3, {1, 1})});
        factors.insert({3, Polynom(3, {1, 0, 1})});
        factors.insert({4, Polynom(3, {2, 1})});

        assert(factors == SffFactors);
    }

    {
        Polynom p2(5, {3, 2, 0, 2, 0, 1, 2, 0, 3, 0, 3, 0, 0, 1});
        Factors SffFactors = PolynomChecker::SquareFreeFactorization(p2);

        Factors factors;
        factors.insert({1, Polynom(5, {3, 2, 1})});
        factors.insert({2, Polynom(5, {1, 0, 4, 1})});
        factors.insert({5, Polynom(5, {1, 1})});

        assert(factors == SffFactors);
    }

    {
        Polynom p(2, {1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1});
        Factors SffFactors = PolynomChecker::SquareFreeFactorization(p);

        Factors factors;
        factors.insert({1, Polynom(2, {1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1})});
        assert(factors == SffFactors);
    }

    {
        Polynom p(2, {0, 1, 0, 1, 1});
        Factors SffFactors = PolynomChecker::SquareFreeFactorization(p);

        Factors factors;
        factors.insert({1, Polynom(2, {0, 1, 0, 1, 1})});
        assert(factors == SffFactors);
    }

    // -----------------------------------

    {
        Polynom p(3, {1, 1, 2, 1, 1, 1, 1});
        Polynom x(3, {0, 1});

        assert(x.BinExp(27, p) == x);
    }

    {
        Polynom f(5, {1, 2, 3, 4, 0, 0, 1});
        Polynom p(5, {2, 0, 0, 1});
        Polynom expected(5, {4, 0, 3, 4, 0, 3});

        assert(((p.BinExp(12, f) - Polynom(5, {1})) % f) == expected);
    }

    // -----------------------------------

    {
        Polynom p1(3, {1, 0, 1, 2, 2, 0, 2, 0, 0, 0, 0, 1});
        Factors DdfFactors = PolynomChecker::DistinctDegreeFactorization(p1);

        Factors factors;
        factors.insert({1, Polynom(3, {2, 0, 1})});
        factors.insert({1, Polynom(3, {2, 1})});
        factors.insert({2, Polynom(3, {1, 0, 1})});
        factors.insert({3, Polynom(3, {1, 1, 2, 1, 1, 1, 1})});

        assert(factors == DdfFactors);
    }

    // -----------------------------------

    {
        Factors factors = {{2, Polynom(5, {1, 2, 3, 4, 0, 0, 1})} };
        Factors edfFactors = PolynomChecker::EqualDegreeFactorization(factors);

        Factors irreducibles = {
                {2, Polynom(5, {3, 0, 1})},
                {2, Polynom(5, {1, 4, 1})},
                {2, Polynom(5, {2, 1, 1})}
        };

        assert(irreducibles == edfFactors);
    }

    {
        Factors factors = {{4, Polynom(2, {1, 1, 0, 1, 1, 1, 0, 1, 1})} };
        Factors edfFactors = PolynomChecker::EqualDegreeFactorization(factors);

        Factors irreducibles = {
                {4, Polynom(2, {1, 0, 0, 1, 1})},
                {4, Polynom(2, {1, 1, 0, 0, 1})}
        };

        assert(irreducibles == edfFactors);
    }

    // -----------------------------------

    {
        Polynom p(3, {2, 1, 1, 1, 0, 0, 2, 1});
        assert(PolynomChecker::CantorZassenhausTest(p) == IRREDUCIBLE);
    }

    {
        Polynom p(2, {1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1});
        assert(PolynomChecker::CantorZassenhausTest(p) == IRREDUCIBLE);
    }

    {
        Polynom p(2, {0, 1, 0, 1, 1});
        assert(PolynomChecker::CantorZassenhausFactorization(p).size() == 2);
        assert(PolynomChecker::CantorZassenhausTest(p) == REDUCIBLE);
    }

    // -----------------------------------

    {
        Polynom p(3, {2, 1, 1, 1, 0, 0, 2, 1});
        assert(PolynomChecker::RabinsTest(p) == IRREDUCIBLE);
    }

    {
        Polynom p(2, {0, 1, 1});
        assert(PolynomChecker::RabinsTest(p) == REDUCIBLE);
    }

    {
        Polynom p(2, {1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1});
        assert(PolynomChecker::RabinsTest(p) == IRREDUCIBLE);
    }

    {
        Polynom p(2, {0, 1, 0, 1, 1});
        assert(PolynomChecker::RabinsTest(p) == REDUCIBLE);
    }

    // ----------------- BerlekampTest ------------------

    {
        Polynom p(2, {1, 0, 0, 1, 1});
        assert(PolynomChecker::BerlekampTest(p) == IRREDUCIBLE);
    }

    {
        Polynom p(3, {1, 2, 2, 2, 1, 0, 0, 1, 2, 1});
        assert(PolynomChecker::BerlekampTest(p) == REDUCIBLE);
    }

    {
        Polynom p(2, {1, 1, 1, 1});
        assert(PolynomChecker::BerlekampTest(p) == REDUCIBLE);
    }

    {
        Polynom p(2, {0, 1, 0, 1, 1});
        assert(PolynomChecker::BerlekampTest(p) == REDUCIBLE);
    }

    {
        Polynom p(3, {1, 1, 2, 0, 0, 1});
        assert(PolynomChecker::BerlekampTest(p) == IRREDUCIBLE);
    }
}
