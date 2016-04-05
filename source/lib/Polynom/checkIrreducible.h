#ifndef _CHECK_IRREDUCIBLE_H
#define _CHECK_IRREDUCIBLE_H

#include "polynom.h"
#include "../Scheduler/scheduler.h"

class PolynomChecker
{
    Polynom* polynom;
    bool busy;
    Method method;
    
public:
    bool isBusy();
    void free();
    PolynomChecker();
    void init(Method method);
    void setPoly(Polynom* p);
    static void* check(void *arg);
    
    void matlab();
    void berlekamp();
    
    vector<vector<int>> generateMatrix(const Polynom& p);
    int getRank(vector<vector<int>> m, int dimGF);
    PolynomState checkMatrix(const Polynom& p);
};

#endif