#ifndef _POLYNOM_H
#define _POLYNOM_H

#include <iostream>
#include <stdio.h>
#include <vector>
using std::string;

class Polynom
{
    int dimGF;
    vector<double> coef;
    bool Irreducible;
    
public:
    Polynom();
    Polynom(int dimGF, vector<double> coef);
    Polynom(int dimGF);
    ~Polynom()
    
    void setDim(int dim);
    void setCoef(vector<double> coef);
    void setIrreducible(bool);
    
    vector<double> getCoef();
    int getDim();
    bool isIrreducible();
    
};

#endif