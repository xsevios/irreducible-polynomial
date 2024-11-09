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
    out << "isirreducible=" << p.isIrreducible() << " dim=" << p.getPrime();

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
    testBigint();

    assert(msb(0) == 0);
    assert(msb(1) == 0);
    assert(msb(2) == 1);
    assert(msb(3) == 1);
    assert(msb(4) == 2);
    assert(msb(9) == 3);

    auto pFieldExt = FieldExt::GetInstance(7);

    assert(pFieldExt->GetMultInverse(1) == 1);
    assert(pFieldExt->GetMultInverse(2) == 4);

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
        PolynomExt p1(FieldExt::GetInstance(7), {6, 4, 5});
        PolynomExt p2(FieldExt::GetInstance(7), std::vector<int>{1, 2});

        PolynomExt p(FieldExt::GetInstance(7), std::vector<int>{});

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
        PolynomExt f(FieldExt::GetInstance(5), std::vector<int>{3, 2, 0, 2, 0, 1, 2, 0, 3, 0, 3, 0, 0, 1});
        PolynomExt g(FieldExt::GetInstance(5), std::vector<int>{2, 0, 1, 0, 0, 2, 0, 4, 0, 0, 0, 0, 3});

        PolynomExt pGcd = gcd(f, g);

        assert(pGcd == PolynomExt(FieldExt::GetInstance(5), std::vector<int>{1, 0, 4, 1, 0, 1, 0, 4, 1}));
    }

    {
        PolynomExt f(FieldExt::GetInstance(5), std::vector<int>{3, 0, 3, 4, 0, 1, 1, 3, 1});
        PolynomExt g(FieldExt::GetInstance(5), std::vector<int>{2, 4, 0, 3, 1, 1});

        PolynomExt pGcd = gcd(f, g);

        assert(pGcd == PolynomExt(FieldExt::GetInstance(5), std::vector<int>{3, 0, 3, 1}));
    }

    // -----------------------------------

    {
        PolynomExt p1(FieldExt::GetInstance(3), std::vector<int>{1, 0, 2, 2, 0, 1, 1, 0, 2, 2, 0, 1});
        Factors SffFactors = PolynomChecker::SquareFreeFactorization(p1);

        Factors factors;
        factors.insert({1, PolynomExt(FieldExt::GetInstance(3), std::vector<int>{1, 1})});
        factors.insert({3, PolynomExt(FieldExt::GetInstance(3), std::vector<int>{1, 0, 1})});
        factors.insert({4, PolynomExt(FieldExt::GetInstance(3), std::vector<int>{2, 1})});

        assert(factors == SffFactors);
    }

    {
        PolynomExt p2(FieldExt::GetInstance(5), std::vector<int>{3, 2, 0, 2, 0, 1, 2, 0, 3, 0, 3, 0, 0, 1});
        Factors SffFactors = PolynomChecker::SquareFreeFactorization(p2);

        Factors factors;
        factors.insert({1, PolynomExt(FieldExt::GetInstance(5), std::vector<int>{3, 2, 1})});
        factors.insert({2, PolynomExt(FieldExt::GetInstance(5), std::vector<int>{1, 0, 4, 1})});
        factors.insert({5, PolynomExt(FieldExt::GetInstance(5), std::vector<int>{1, 1})});

        assert(factors == SffFactors);
    }

    {
        PolynomExt p(FieldExt::GetInstance(2), std::vector<int>{1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1});
        Factors SffFactors = PolynomChecker::SquareFreeFactorization(p);

        Factors factors;
        factors.insert({1, PolynomExt(FieldExt::GetInstance(2), std::vector<int>{1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1})});
        assert(factors == SffFactors);
    }

    {
        PolynomExt p(FieldExt::GetInstance(2), std::vector<int>{0, 1, 0, 1, 1});
        Factors SffFactors = PolynomChecker::SquareFreeFactorization(p);

        Factors factors;
        factors.insert({1, PolynomExt(FieldExt::GetInstance(2), std::vector<int>{0, 1, 0, 1, 1})});
        assert(factors == SffFactors);
    }

    // -----------------------------------

    {
        PolynomExt p(FieldExt::GetInstance(3), std::vector<int>{1, 1, 2, 1, 1, 1, 1});
        PolynomExt x(FieldExt::GetInstance(3), std::vector<int>{0, 1});

        assert(x.BinExp(27, p) == x);
    }

    {
        PolynomExt f(FieldExt::GetInstance(5), std::vector<int>{1, 2, 3, 4, 0, 0, 1});
        PolynomExt p(FieldExt::GetInstance(5), std::vector<int>{2, 0, 0, 1});
        PolynomExt expected(FieldExt::GetInstance(5), std::vector<int>{4, 0, 3, 4, 0, 3});

        assert(((p.BinExp(12, f) - PolynomExt(FieldExt::GetInstance(5), std::vector<int>{1})) % f) == expected);
    }

    // -----------------------------------

    {
        PolynomExt p1(FieldExt::GetInstance(3), std::vector<int>{1, 0, 1, 2, 2, 0, 2, 0, 0, 0, 0, 1});
        Factors DdfFactors = PolynomChecker::DistinctDegreeFactorization(p1);

        Factors factors;
        factors.insert({1, PolynomExt(FieldExt::GetInstance(3), std::vector<int>{2, 0, 1})});
        factors.insert({1, PolynomExt(FieldExt::GetInstance(3), std::vector<int>{2, 1})});
        factors.insert({2, PolynomExt(FieldExt::GetInstance(3), std::vector<int>{1, 0, 1})});
        factors.insert({3, PolynomExt(FieldExt::GetInstance(3), std::vector<int>{1, 1, 2, 1, 1, 1, 1})});

        assert(factors == DdfFactors);
    }

    // -----------------------------------

    {
        Factors factors = {{2, PolynomExt(FieldExt::GetInstance(5), std::vector<int>{1, 2, 3, 4, 0, 0, 1})} };
        Factors edfFactors = PolynomChecker::EqualDegreeFactorization(factors);

        Factors irreducibles = {
                {2, PolynomExt(FieldExt::GetInstance(5), std::vector<int>{3, 0, 1})},
                {2, PolynomExt(FieldExt::GetInstance(5), std::vector<int>{1, 4, 1})},
                {2, PolynomExt(FieldExt::GetInstance(5), std::vector<int>{2, 1, 1})}
        };

        assert(irreducibles == edfFactors);
    }

    {
        Factors factors = {{4, PolynomExt(FieldExt::GetInstance(2), std::vector<int>{1, 1, 0, 1, 1, 1, 0, 1, 1})} };
        Factors edfFactors = PolynomChecker::EqualDegreeFactorization(factors);

        Factors irreducibles = {
                {4, PolynomExt(FieldExt::GetInstance(2), std::vector<int>{1, 0, 0, 1, 1})},
                {4, PolynomExt(FieldExt::GetInstance(2), std::vector<int>{1, 1, 0, 0, 1})}
        };

        assert(irreducibles == edfFactors);
    }

    // -----------------------------------

    {
        PolynomExt p(FieldExt::GetInstance(3), std::vector<int>{2, 1, 1, 1, 0, 0, 2, 1});
        assert(PolynomChecker::CantorZassenhausTest(p) == IRREDUCIBLE);
    }

    {
        PolynomExt p(FieldExt::GetInstance(2), std::vector<int>{1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1});
        assert(PolynomChecker::CantorZassenhausTest(p) == IRREDUCIBLE);
    }

    {
        PolynomExt p(FieldExt::GetInstance(2), std::vector<int>{0, 1, 0, 1, 1});
        assert(PolynomChecker::CantorZassenhausFactorization(p).size() == 2);
        assert(PolynomChecker::CantorZassenhausTest(p) == REDUCIBLE);
    }

    // -----------------------------------

    {
        PolynomExt p(FieldExt::GetInstance(3), std::vector<int>{2, 1, 1, 1, 0, 0, 2, 1});
        assert(PolynomChecker::RabinsTest(p) == IRREDUCIBLE);
    }

    {
        PolynomExt p(FieldExt::GetInstance(2), std::vector<int>{0, 1, 1});
        assert(PolynomChecker::RabinsTest(p) == REDUCIBLE);
    }

    {
        PolynomExt p(FieldExt::GetInstance(2), std::vector<int>{1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1});
        assert(PolynomChecker::RabinsTest(p) == IRREDUCIBLE);
    }

    {
        PolynomExt p(FieldExt::GetInstance(2), std::vector<int>{0, 1, 0, 1, 1});
        assert(PolynomChecker::RabinsTest(p) == REDUCIBLE);
    }

    {
        PolynomExt p(FieldExt::GetInstance(7), std::vector<int>{6, 2, 0, 1, 2, 0, 6, 5, 0, 6, 0, 0, 0, 3});
        assert(PolynomChecker::RabinsTest(p) == IRREDUCIBLE);
    }

    {
        PolynomExt p(FieldExt::GetInstance(7), std::vector<int>{5, 4, 0, 0, 4, 3, 1, 0, 0, 0, 4, 0, 0, 0, 0, 5, 0, 0, 6});
        assert(PolynomChecker::RabinsTest(p) == REDUCIBLE);
    }

    // ----------------- BerlekampTest ------------------

    {
        PolynomExt p(FieldExt::GetInstance(2), std::vector<int>{1, 0, 0, 1, 1});
        assert(PolynomChecker::BerlekampTest(p) == IRREDUCIBLE);
    }

    {
        PolynomExt p(FieldExt::GetInstance(3), std::vector<int>{1, 2, 2, 2, 1, 0, 0, 1, 2, 1});
        assert(PolynomChecker::BerlekampTest(p) == REDUCIBLE);
    }

    {
        PolynomExt p(FieldExt::GetInstance(2), std::vector<int>{1, 1, 1, 1});
        assert(PolynomChecker::BerlekampTest(p) == REDUCIBLE);
    }

    {
        PolynomExt p(FieldExt::GetInstance(2), std::vector<int>{0, 1, 0, 1, 1});
        assert(PolynomChecker::BerlekampTest(p) == REDUCIBLE);
    }

    {
        PolynomExt p(FieldExt::GetInstance(3), std::vector<int>{1, 1, 2, 0, 0, 1});
        assert(PolynomChecker::BerlekampTest(p) == IRREDUCIBLE);
    }

    testFieldExtensions();
    testPolynomGenerator();
}

void Tester::testBigint()
{
    {
        bigint n("3");
        bigint exp("3");
        bigint expected("27");
        bigint res = n ^ exp;
        assert(res == expected);
    }

    {
        int n = 11;
        bigint bign("11");

        assert(msb(n) == msb(bign));
        for (int i = 0; i < 15; i++)
        {
            assert(checkBit(n, i) == checkBit(bign, i));
        }
    }
}

void Tester::testFieldExtensions()
{
    {
        PolynomExt p1(FieldExt::GetInstance(3), std::vector<int>{2, 2, 0, 0, 1});
        const auto* pField1 = FieldExt::GetInstance(&p1);

        PolynomExt p2(FieldExt::GetInstance(3), std::vector<int>{2, 2, 0, 0, 1});
        const auto* pField2 = FieldExt::GetInstance(&p2);

        assert(pField1 == pField2);
    }

    // -----------------------------------

    {
        PolynomExt p1(FieldExt::GetInstance(3), std::vector<int>{2, 2, 0, 0, 1});
        const auto* pField = FieldExt::GetInstance(&p1);

        PolynomExt k1(FieldExt::GetInstance(3), std::vector<int>{1, 1});
        PolynomExt k2(FieldExt::GetInstance(3), std::vector<int>{2, 0, 0, 1});

        ExtCoeffs coeffs1;
        coeffs1.emplace(0, k1);
        coeffs1.emplace(2, k2);
        PolynomExt p4(pField, coeffs1);

        auto mul = p4 * p4;

        PolynomExt mul_k0(FieldExt::GetInstance(3), std::vector<int>{1, 2, 1});
        PolynomExt mul_k2(FieldExt::GetInstance(3), std::vector<int>{0, 0, 0, 2});
        PolynomExt mul_k4(FieldExt::GetInstance(3), std::vector<int>{1, 0, 1, 2});

        ExtCoeffs coeffs2;
        coeffs2.emplace(0, mul_k0);
        coeffs2.emplace(2, mul_k2);
        coeffs2.emplace(4, mul_k4);
        PolynomExt mulExp(pField, coeffs2);

        assert(mul == mulExp);

        auto qx = mul / p4;
        auto rx = mul % p4;

        assert(qx == p4);
        assert(rx.GetDegree() == 0);
    }

    {
        PolynomExt p1(FieldExt::GetInstance(3), std::vector<int>{2, 2, 0, 0, 1});
        const auto* pField = FieldExt::GetInstance(&p1);

        PolynomExt k1(FieldExt::GetInstance(3), std::vector<int>{1});
        PolynomExt k2(FieldExt::GetInstance(3), std::vector<int>{2});

        ExtCoeffs coeffs1;
        coeffs1.emplace(0, k1);
        PolynomExt p2(pField, coeffs1);

        ExtCoeffs coeffs2;
        coeffs2.emplace(0, k2);
        PolynomExt p3(pField, coeffs2);

        auto p4 = p2 + p3;
        auto p5 = p2 - p2;

        assert(p4.GetRefExtCoef().empty());
        assert(p5.GetRefExtCoef().empty());
    }

    {
        PolynomExt p1(FieldExt::GetInstance(2), std::vector<int>{1, 1, 0, 1, 1, 0, 0, 0, 1});
        const auto* pField = FieldExt::GetInstance(&p1);

        PolynomExt k1(FieldExt::GetInstance(2), std::vector<int>{1, 1, 0, 0, 1, 0, 1});
        PolynomExt expectedInverse(FieldExt::GetInstance(2), std::vector<int>{0, 1, 0, 1, 0, 0, 1, 1});
        auto inversePolynom = pField->GetMultInverse(&k1);

        assert(expectedInverse == inversePolynom);
    }
}

void Tester::testPolynomGenerator()
{
    {
        PolynomExt p1(FieldExt::GetInstance(3), std::vector<int>{2, 2, 0, 0, 1});
        const auto* pField = FieldExt::GetInstance(&p1);

        PolynomExt k0(FieldExt::GetInstance(3), std::vector<int>{0, 2, 0, 2});
        PolynomExt k1(FieldExt::GetInstance(3), std::vector<int>{1, 0, 2});
        PolynomExt k2(FieldExt::GetInstance(3), std::vector<int>{2});
        PolynomExt k5(FieldExt::GetInstance(3), std::vector<int>{0, 2, 0, 2});

        ExtCoeffs coeffs;
        coeffs.emplace(0, k0);
        coeffs.emplace(1, k1);
        coeffs.emplace(2, k2);
        coeffs.emplace(5, k5);
        PolynomExt poly(pField, coeffs);

        auto cantorState = PolynomChecker::CantorZassenhausTest(poly);
        auto rabinState = PolynomChecker::RabinsTest(poly);

        assert(cantorState == rabinState);
    }

    {
        PolynomExt p1(FieldExt::GetInstance(3), std::vector<int>{2, 2, 0, 0, 1});
        const auto* pField = FieldExt::GetInstance(&p1);

        PolynomExt a3(FieldExt::GetInstance(3), std::vector<int>{0, 2});
        PolynomExt a4(FieldExt::GetInstance(3), std::vector<int>{1, 2, 2});
        PolynomExt a5(FieldExt::GetInstance(3), std::vector<int>{2, 0, 0, 2});

        ExtCoeffs coeffs1;
        coeffs1.emplace(3, a3);
        coeffs1.emplace(4, a4);
        coeffs1.emplace(5, a5);
        PolynomExt a(pField, coeffs1);

        PolynomExt d3(FieldExt::GetInstance(3), std::vector<int>{1, 2, 2});
        PolynomExt d4(FieldExt::GetInstance(3), std::vector<int>{1, 0, 0, 1});

        ExtCoeffs coeffs2;
        coeffs2.emplace(3, d3);
        coeffs2.emplace(4, d4);
        PolynomExt d(pField, coeffs2);

        assert(a.Derivative() == d);
    }

    {
        PolynomExt p1(FieldExt::GetInstance(3), std::vector<int>{2, 2, 0, 0, 1});
        const auto* pField = FieldExt::GetInstance(&p1);

        PolynomExt a1(FieldExt::GetInstance(3), std::vector<int>{1});
        PolynomExt a2(FieldExt::GetInstance(3), std::vector<int>{2});

        ExtCoeffs coeffs1;
        coeffs1.emplace(5, a1);
        coeffs1.emplace(4, a2);
        coeffs1.emplace(0, a1);
        PolynomExt a(pField, coeffs1);

        auto cantorState = PolynomChecker::CantorZassenhausTest(a);
        auto rabinState = PolynomChecker::RabinsTest(a);

        assert(cantorState == rabinState);
    }

    {
        PolynomExt p1(FieldExt::GetInstance(3), std::vector<int>{2, 2, 0, 0, 1});
        const auto* pField = FieldExt::GetInstance(&p1);

        PolynomExt a4(FieldExt::GetInstance(3), std::vector<int>{0, 0, 0, 1});
        PolynomExt a3(FieldExt::GetInstance(3), std::vector<int>{0, 2});
        PolynomExt a2(FieldExt::GetInstance(3), std::vector<int>{2, 0, 0, 1});
        PolynomExt a1(FieldExt::GetInstance(3), std::vector<int>{2});
        PolynomExt a0(FieldExt::GetInstance(3), std::vector<int>{1, 0, 2, 1});

        ExtCoeffs coeffs1;
        coeffs1.emplace(4, a4);
        coeffs1.emplace(3, a3);
        coeffs1.emplace(2, a2);
        coeffs1.emplace(1, a1);
        coeffs1.emplace(0, a0);
        PolynomExt a(pField, coeffs1);

        auto b = a * a;

        assert(b.CheckConsistensy());
    }

    {
        PolynomExt p1(FieldExt::GetInstance(3), std::vector<int>{2, 2, 0, 0, 1});
        const auto* pField = FieldExt::GetInstance(&p1);

        PolynomExt a5(FieldExt::GetInstance(3), std::vector<int>{2, 2});
        PolynomExt a4(FieldExt::GetInstance(3), std::vector<int>{0, 0, 2});
        PolynomExt a3(FieldExt::GetInstance(3), std::vector<int>{0, 0, 0, 2});
        PolynomExt a0(FieldExt::GetInstance(3), std::vector<int>{1});

        ExtCoeffs coeffs1;
        coeffs1.emplace(5, a5);
        coeffs1.emplace(4, a4);
        coeffs1.emplace(3, a3);
        coeffs1.emplace(0, a0);
        PolynomExt a(pField, coeffs1);

        PolynomState cantorState = PolynomChecker::CantorZassenhausTest(a);
        PolynomState rabinState = PolynomChecker::RabinsTest(a);

        assert(cantorState == rabinState);
    }
}
