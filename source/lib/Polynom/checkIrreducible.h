#ifndef _CHECK_IRREDUCIBLE_H
#define _CHECK_IRREDUCIBLE_H

#include "polynom.h"
#include "../Scheduler/scheduler.h"

#include <unordered_map>
#include <random>

using Factors = std::unordered_multimap<int, Polynom>;

Polynom gcd(Polynom a, Polynom b);

class PolynomChecker
{
public:
                                    PolynomChecker();
    
    bool                            isBusy                          ();
    void                            free                            ();
    void                            init                            (Method method);
    void                            setPoly                         (Polynom* polynom, pthread_mutex_t* mutex, pthread_cond_t* cond);
    static void*                    check                           (void *arg);

    // General irreducibility methods
    void                            matlab();
    void                            berlekamp();
    void                            cantorzassenhaus();

    // Berlekemp tools
    static PolynomState             BerlekampTest                   (const Polynom& f);
    static vector<vector<int>>      generateMatrix                  (const Polynom& p);
    static int                      getRank                         (vector<vector<int>> m, int dimGF);
    static PolynomState             checkMatrix                     (const Polynom& p);

    // Cantor-Zassenhaus tools
    static PolynomState             CantorZassenhausTest            (const Polynom& f);
    static Factors                  CantorZassenhausFactorization   (const Polynom& f);
    static bool                     isSquareFree                    (const Polynom& f);
    static Factors                  SquareFreeFactorization         (const Polynom& p);
    static Factors                  DistinctDegreeFactorization     (const Polynom& p);
    static Polynom                  GenRandPolynom                  (const Polynom& p);
    static Factors                  EqualDegreeFactorization        (const Polynom& p, int d);
    static Factors                  EqualDegreeFactorization        (Factors factors);

private:
    Polynom*                        polynom;
    pthread_mutex_t*                mutex;
    pthread_cond_t*                 cond;

    bool                            busy;
    Method                          method;
};

#endif