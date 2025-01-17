#ifndef _CHECK_IRREDUCIBLE_H
#define _CHECK_IRREDUCIBLE_H

#include "polynom.h"
#include "polynom_ext.h"
#include "../Scheduler/scheduler.h"

#include <unordered_map>
#include <random>

using Factors = std::unordered_multimap<int, PolynomExt>;

PolynomExt gcd(const PolynomExt& a, const PolynomExt& b);
vector<vector<int>> gauss(vector<vector<int>> matrix, int dimGF);

class PolynomChecker
{
public:
                                    PolynomChecker();
    
    bool                            isBusy                          ();
    void                            free                            ();
    void                            init                            (Method method);
    void                            setPoly                         (PolynomExt* polynom, pthread_mutex_t* mutex, pthread_cond_t* cond);
    static void*                    check                           (void *arg);

    // Tools
    static std::vector<int>         GetPrimeDivisors                (int n, bool distinct = false);

    // General irreducibility methods
    void                            matlab();
    void                            berlekamp();
    void                            cantorzassenhaus();

    // Berlekemp tools
    static PolynomState             BerlekampTest                   (const PolynomExt& f);
    static vector<vector<int>>      generateMatrix                  (const PolynomExt& p);
    static int                      getRank                         (vector<vector<int>> m, int dimGF);
    static PolynomState             checkMatrix                     (const PolynomExt& p);

    // Cantor-Zassenhaus tools
    static PolynomState             CantorZassenhausTest            (const PolynomExt& f);
    static Factors                  CantorZassenhausFactorization   (const PolynomExt& f);
    static PolynomState             KaltofenShoupTest               (const PolynomExt& f);
    static Factors                  KaltofenShoupFactorization      (const PolynomExt& f);
    static bool                     isSquareFree                    (const PolynomExt& f);
    static Factors                  SquareFreeFactorization         (const PolynomExt& p);
    static Factors                  DistinctDegreeFactorization     (const PolynomExt& p);
    static Factors                  DistinctDegreeShoupFactorization(const PolynomExt& p, bool test = false);
    static PolynomExt               GenRandPolynom                  (const PolynomExt& p);
    static Factors                  EqualDegreeFactorization        (const PolynomExt& p, int d);
    static Factors                  EqualDegreeFactorization        (Factors factors);
    static std::vector<PolynomExt>  hiPowers                     (const PolynomExt& XP, long bo, const PolynomExt& T, long p);
    static PolynomExt               DDFShoupEval                  (const PolynomExt& Q, const std::vector<PolynomExt>& x, const PolynomExt& T, long p);

    // Rabin's Irreducibility Test
    static PolynomState             RabinsTest                      (const PolynomExt& f);

private:
    PolynomExt*                     polynom;
    pthread_mutex_t*                mutex;
    pthread_cond_t*                 cond;

    bool                            busy;
    Method                          method;
};

#endif