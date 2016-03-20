#ifndef _CHECK_IRREDUCIBLE_H
#define _CHECK_IRREDUCIBLE_H

#include "polynom.h"

class PolynomChecker
{
    Polynom* polynom;
    bool busy;
public:
    bool isBusy();
    void free();
    PolynomChecker();
    void init(Polynom* p);
    static void* check(void *arg);
};

#endif