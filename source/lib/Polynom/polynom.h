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
    void setIrreducible(bool);
    
    vector<double> getCoef();
    int getDim();
    bool isIrreducible();
    
};

#endif