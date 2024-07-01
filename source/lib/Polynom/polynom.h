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

#include "Field.h"
#include "../Utils/utils.h"
#include "../BigInt/bigint.h"

using namespace std;

int getMultInverse(int num, int dim);

/**
 * \brief Описывает многочлен над конечным полем и основные операции над ним
 */ 
class Polynom
{
    vector<int>          m_coef;        ///< Массив коэфициентов многочлена
    const Field *        m_pField;      ///< Поле коэфициентов многочлена
    mutable PolynomState irreducible;   ///< Статус приводимости многочлена
    
public:
                            Polynom             (int dimGF, vector<int> coef);
                            Polynom             (int dimGF, uint64_t coefCount);
                            Polynom             (string strPolynom);
    virtual                ~Polynom             ();

    void                    setIrreducible      (PolynomState) const;

    virtual vector<int>     getCoef             () const;
    vector<int>&            getRefCoef          ();
    const vector<int>&      getRefCoef          () const;
    virtual int             getDim              () const;
    virtual int             GetLeadingCoef      () const;

    virtual PolynomState    isIrreducible       () const;
    const Field *           GetField            () const;
    unsigned                getDegree           () const;
    unsigned                getPrime            () const;
    unsigned                getPrimeDeg         () const;
    void                    print               (ostream& out = cout) const;

    Polynom                 Exp                 (int p) const;

    /// Binary Exponentiation Algorithm for Computing x^M mod f (Reapeted squaring)
    template<class T>
    Polynom BinExp(T M, const Polynom &f) const
    {
        const Polynom& x    = *this;
        Polynom g           = *this;

        for (int j = msb(M) - 1; j >= 0; j--)
        {
            g *= g;
            g %= f;

            if (checkBit(M, j))
            {
                g *= x;
                g %= f;
            }
        }

        return g;
    }

    Polynom BinExp(int n, int exp, const Polynom &f) const;

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
    Polynom&                operator>>=         (const int number);

    Polynom                 operator>>          (const int number) const;

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
    void                    init                (int dim);
    std::vector<int>        coefBuf;
};

#endif