#ifndef _CHECK_IRREDUCIBLE_H
#define _CHECK_IRREDUCIBLE_H

#include "polynom.h"

class PolynomChecker
{
public:
    PolynomChecker();
    void operator()(Polynom* polynom);
};

#endif