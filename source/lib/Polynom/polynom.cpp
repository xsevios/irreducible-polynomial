#include "polynom.h"
#include <iostream>
#include <string>

extern "C" Polynom* create_polynom(std::string strPolynom)
{
    return new Polynom(strPolynom);
}

extern "C" void destroy_polynom(Polynom* object)
{
    delete object;
}


Polynom::Polynom(int dim, vector<double> coef)
{
    this->dimGF = dim;
    this->coef = coef;
}

Polynom::~Polynom()
{
}


int Polynom::getDim()
{
    return dimGF;
}

vector<double> Polynom::getCoef()
{
    return coef;
}

int Polynom::isIrreducible()
{
    return irreducible;
}