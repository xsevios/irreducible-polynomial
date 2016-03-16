#ifndef _POLYNOM_H
#define _POLYNOM_H

#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
using namespace std;

class Polynom
{
    int dimGF;
    vector<double> coef;
    
    /*
     * Переменная содержит информацию о приводимости многочлена в следующем виде:
     * -1 - еще не проверен (не является опорной точкой для построения алгоритма проверки)
     *  0 - неприводимый
     *  1 - приводимый
     */
    int irreducible;
    
public:
    Polynom(int dimGF, vector<double> coef);
    Polynom(string strPolynom);
    virtual ~Polynom();
    
    void setIrreducible(bool);
    
    virtual vector<double> getCoef() const;
    virtual int getDim() const;
    virtual int isIrreducible() const;
    
    Polynom& operator=(const Polynom& p);

    Polynom operator+(const Polynom& p);
    Polynom operator-(const Polynom& p);
    Polynom operator*(const Polynom& p);
    Polynom operator/(const Polynom& p);
    
    friend ostream& operator<<(ostream& out, const Polynom& p);
    friend istream& operator>>(istream& in, Polynom& p);
    
    bool operator==(const Polynom& p);
    bool operator!=(const Polynom& p);
};

#endif