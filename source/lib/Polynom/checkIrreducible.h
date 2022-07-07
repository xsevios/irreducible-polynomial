#ifndef _CHECK_IRREDUCIBLE_H
#define _CHECK_IRREDUCIBLE_H

#include "polynom.h"
#include "../Scheduler/scheduler.h"

Polynom gcd(Polynom a, Polynom b);

class PolynomChecker
{
    Polynom* polynom;
    pthread_mutex_t* mutex;
    pthread_cond_t* cond;
    
    bool busy;
    Method method;
    
public:
    PolynomChecker();
    void init(Polynom* polynom, pthread_mutex_t* mutex, pthread_cond_t* cond);
    
    bool isBusy();
    void free();
    void init(Method method);
    void setPoly(Polynom* polynom, pthread_mutex_t* mutex, pthread_cond_t* cond);
    static void* check(void *arg);
    
    void matlab();
    void berlekamp();
    
    vector<vector<int>> generateMatrix(const Polynom& p);
    int getRank(vector<vector<int>> m, int dimGF);
    PolynomState checkMatrix(const Polynom& p);
};

#endif