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

int getMultInverse(int num, int dim);
int msb(int x);

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
    int                     dimGF;          ///< Размерность поля
    int                     prime;          ///< Характеристика поля
    int                     primeDeg;       ///<
    vector<int>             coef;           ///< Массив коэфициентов многочлена
    mutable PolynomState    irreducible;    ///< Статус приводимости многочлена
    
public:
                            Polynom             (int dimGF, vector<int> coef);
                            Polynom             (string strPolynom);
    virtual                ~Polynom             ();

    void                    setIrreducible      (PolynomState) const;

    virtual vector<int>     getCoef             () const;
    vector<int>&            getRefCoef          ();
    virtual int             getDim              () const;
    virtual int             GetLeadingCoef      () const;

    virtual PolynomState    isIrreducible       () const;
    unsigned                getDegree           () const;
    unsigned                getPrime            () const;
    unsigned                getPrimeDeg         () const;
    void                    print               (ostream& out = cout) const;

    Polynom                 Exp                 (int p) const;
    Polynom                 BinExp              (long long M, const Polynom &f) const;
    Polynom                 Derivative          () const;
    bool                    IsZero              () const;
    void                    RemoveLeadingZeros  ();

    int&                    operator[]          (size_t id);
    const int&              operator[]          (size_t id) const;

    Polynom&                operator=           (const Polynom& p);
    Polynom&                operator+=          (const Polynom& p);
    Polynom&                operator-=          (const Polynom& p);
    Polynom&                operator*=          (const Polynom& p);
    Polynom&                operator/=          (const Polynom& p);
    Polynom&                operator%=          (const Polynom& p);
    Polynom&                operator*=          (const int number);

    Polynom                 operator>>          (const int number) const;

    Polynom                 operator-           ();
    friend Polynom          operator-           (const Polynom& p);
    friend Polynom          operator+           (Polynom lp, const Polynom& rp);
    friend Polynom          operator-           (Polynom lp, const Polynom& rp);
    friend Polynom          operator*           (Polynom lp, const Polynom& rp);
    friend Polynom          operator/           (Polynom lp, const Polynom& rp);
    friend Polynom          operator%           (Polynom lp, const Polynom& rp);

    friend Polynom          operator*           (const int number, const Polynom& p);
    friend Polynom          operator*           (const Polynom& p, const int number);
    friend Polynom          operator/           (const Polynom& p, const int number);

    friend bool             operator==          (const Polynom& lp, const Polynom& rp);
    friend bool             operator!=          (const Polynom& lp, const Polynom& rp);
    friend bool             operator==          (const Polynom& lp, int i);
    friend bool             operator!=          (const Polynom& lp, int i);

private:
    void                    init                ();
};

#endif