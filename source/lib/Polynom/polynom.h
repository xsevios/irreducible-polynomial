#ifndef _POLYNOM_H
#define _POLYNOM_H

#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>

using namespace std;

enum PolynomState
{
    NEED_CHECK  = -1,
    REDUCIBLE   = 0,
    IRREDUCIBLE = 1
};

class Polynom
{
    int dimGF;
    vector<int> coef;
    PolynomState irreducible;
    
public:
    Polynom(int dimGF, vector<int> coef);
    Polynom(string strPolynom);
    virtual ~Polynom();
    
    void setIrreducible(PolynomState);
    
    virtual vector<int> getCoef() const;
    virtual int getDim() const;
    virtual PolynomState isIrreducible() const;
    
    Polynom& operator=(const Polynom& p);
    
    Polynom operator+(const Polynom& p);
    Polynom operator-(const Polynom& p);
    Polynom operator*(const Polynom& p);
    friend Polynom operator*(const int number, const Polynom& p);
    Polynom operator/(const Polynom& p);
    Polynom operator/(const int number);
    
    bool operator==(const Polynom& p);
    bool operator!=(const Polynom& p);
};

#endif