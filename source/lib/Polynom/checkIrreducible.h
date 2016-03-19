#ifndef _CHECK_IRREDUCIBLE_H
#define _CHECK_IRREDUCIBLE_H

#include "polynom.h"

class PolynomChecker
{
public:
    PolynomChecker();
    //void static check(Polynom* polynom);
    void operator()(Polynom* polynom);
};

#endif