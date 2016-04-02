#ifndef _POLYNOM_H
#define _POLYNOM_H

#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <math.h>
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
    vector<int>& getRefCoef();
    virtual int getDim() const;
    
    virtual PolynomState isIrreducible() const;
    unsigned getDegree() const;
    Polynom& operator=(const Polynom& p);
    int& operator[](size_t id);
    const int& operator[](size_t id) const;
    void print(ostream& out = cout) const;
    
    Polynom derivative();
    bool isZero();
    
    Polynom operator-();
    friend Polynom operator-(const Polynom& p);
    
    Polynom& operator+=(const Polynom& p);
    Polynom& operator-=(const Polynom& p);
    Polynom& operator*=(const Polynom& p);
    Polynom& operator/=(const Polynom& p);
    Polynom& operator%=(const Polynom& p);
    Polynom& operator*=(const int number);
    
    friend Polynom operator+(Polynom lp, const Polynom& rp);
    friend Polynom operator-(Polynom lp, const Polynom& rp);
    friend Polynom operator*(Polynom lp, const Polynom& rp);
    friend Polynom operator/(Polynom lp, const Polynom& rp);
    friend Polynom operator%(Polynom lp, const Polynom& rp);
    
    friend Polynom operator*(const int number, const Polynom& p);
    friend Polynom operator*(const Polynom& p, const int number);
    friend Polynom operator/(const Polynom& p, const int number);
    
    friend bool operator==(const Polynom& lp, const Polynom& rp);
    friend bool operator!=(const Polynom& lp, const Polynom& rp);
};

#endif