/**
* \file
* \brief Заголовочный файл класса Polynom
*
* Данный файл содержит в себе определение класса обеспечивающего хранение полиномов
* над конечным полем, а так же функционал по работе с ними
*/

#ifndef _POLYNOM_H
#define _POLYNOM_H

#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <math.h>
using namespace std;

/** Перечесление описывающее приводимость полинома
 * NEED_CHECK - требует проверки полинома(состояние по умолчанию)
 * REDUCIBLE - полином является приводимым
 * IRREDUCIBLE - полином не прифодим
*/
enum PolynomState
{
    NEED_CHECK  = -1,
    REDUCIBLE   = 0,
    IRREDUCIBLE = 1
};

/**
 * \brief Описывает многочлен над конечным полем и основные операции над ним
 */ 
class Polynom
{
    int dimGF;///< Размерность поля
    vector<int> coef;///< Массив коэфициентов многочлена
    PolynomState irreducible;///< Статус приводимости многочлена
    
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