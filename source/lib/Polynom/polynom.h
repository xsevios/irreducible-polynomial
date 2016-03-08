#ifndef _POLYNOM_H
#define _POLYNOM_H

#include <iostream>
#include <stdio.h>
#include <vector>
using namespace std;

class Polynom
{
    int dimGF;
    vector<double> coef;
    int irreducible;
    
public:
    Polynom(int dimGF, vector<double> coef);
    ~Polynom();
    
    void setDim(int dim);
    void setCoef(vector<double> coef);
    void initPolynom(int dimGF, vector<double> coef, int irreducible);
    void setIrreducible(bool);
    
    vector<double> getCoef();
    int getDim();
    bool isIrreducible();
    
    Polynom& operator=(Polynom p);

    Polynom operator+(const Polynom& p);
    Polynom operator-(const Polynom& p);
    Polynom operator*(const Polynom& p);
    Polynom operator/(const Polynom& p);
    
    friend ostream& operator<<(ostream& os, const Polynom& p);
    friend istream& operator>>(istream& is, Polynom& p);
    
    bool operator==(const Polynom& p);
    bool operator!=(const Polynom& p);
};

#endif