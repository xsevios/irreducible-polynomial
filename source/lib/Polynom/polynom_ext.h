/**
* \file
* \brief Заголовочный файл класса Polynom
*
* Данный файл содержит в себе определение класса обеспечивающего хранение полиномов
* над конечным полем, а так же функционал по работе с ними
*/

#ifndef _POLYNOM_EXT_H
#define _POLYNOM_EXT_H

#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <math.h>

#include "FieldExt.h"
#include "../Utils/utils.h"
#include "../BigInt/bigint.h"

using namespace std;

typedef std::map<int, PolynomExt> ExtCoeffs;

int getMultInverse(int num, int dim);
ostream& operator<<(ostream& out, const PolynomExt& p);

/**
 * \brief Описывает многочлен над конечным полем и основные операции над ним
 */
class PolynomExt
{
    vector<int>                 m_coef;                         ///< Массив коэфициентов многочлена
    ExtCoeffs                   m_extCoef;                      ///< Массив коэфициентов многочлена расширения поля
    const FieldExt *            m_pField        = nullptr;      ///< Поле коэфициентов многочлена
    mutable PolynomState        m_eIrreducible  = NEED_CHECK;   ///< Статус приводимости многочлена

public:
                            PolynomExt          (const FieldExt* pField, const vector<int>& coef);
                            PolynomExt          (const FieldExt* pField, uint64_t coefCount);
                            PolynomExt          (const FieldExt* pField, const ExtCoeffs& coef);
                            PolynomExt          (const string& strPolynom);
    virtual                ~PolynomExt          ();

    void                    setIrreducible      (PolynomState) const;

    virtual vector<int>     GetCoef             () const;
    const ExtCoeffs&        GetRefExtCoef       () const;
    vector<int>&            getRefCoef          ();
    const vector<int>&      getRefCoef          () const;
    virtual int             GetDim              () const;
    virtual int             GetLeadingCoef      () const;
    const PolynomExt*       GetLeadingExtCoef   () const;

    virtual PolynomState    isIrreducible       () const;
    const FieldExt *        GetField            () const;
    std::size_t             GetHash             () const;
    unsigned                GetDegree           () const;
    unsigned                GetPrime            () const;
    void                    print               (ostream& out = cout) const;

    PolynomExt              Exp                 (int p) const;

    /// Binary Exponentiation Algorithm for Computing x^M mod f (Reapeted squaring)
    template<class T>
    PolynomExt BinExp(T M, const PolynomExt &f) const
    {
        const PolynomExt& x    = *this;
        PolynomExt g           = *this;

//        std::cout << "BinExp start: " << g << std::endl<char, std::char_traits<char>>;

        for (int j = msb(M) - 1; j >= 0; j--)
        {
//            std::cout << "BinExp g: " << g << std::endl<char, std::char_traits<char>>;
            g *= g;

//            std::cout << "BinExp 11: " << g << std::endl<char, std::char_traits<char>>;
//            if (!g.CheckConsistensy())
//                exit(0);

            g %= f;

//            std::cout << "BinExp 12: " << g << std::endl<char, std::char_traits<char>>;
//            if (!g.CheckConsistensy())
//                exit(0);

            if (checkBit(M, j))
            {
                g *= x;

//                std::cout << "BinExp 21: " << g << std::endl<char, std::char_traits<char>>;
//                if (!g.CheckConsistensy())
//                    exit(0);
                g %= f;

//                std::cout << "BinExp 22: " << g << std::endl<char, std::char_traits<char>>;
//                if (!g.CheckConsistensy())
//                    exit(0);
            }

//            if (!g.CheckConsistensy())
//                exit(0);
        }

        return g;
    }

    PolynomExt BinExp(int n, int exp, const PolynomExt &f) const;

    PolynomExt              Derivative          () const;
    bool                    IsZero              () const;
    bool                    CheckConsistensy    () const;
    void                    RemoveLeadingZeros  ();

    int&                    operator[]          (size_t id);
    const int&              operator[]          (size_t id) const;

    PolynomExt&             operator=           (const PolynomExt& p);
    PolynomExt&             operator+=          (const PolynomExt& p);
    PolynomExt&             operator-=          (const PolynomExt& p);
    PolynomExt&             operator*=          (const PolynomExt& p);
    PolynomExt&             operator/=          (const PolynomExt& p);
    PolynomExt&             operator%=          (const PolynomExt& p);
    PolynomExt&             operator*=          (const int number);

    PolynomExt              operator>>          (const int number) const;

    friend PolynomExt       operator-           (const PolynomExt& p);
    friend PolynomExt       operator+           (PolynomExt lp, const PolynomExt& rp);
    friend PolynomExt       operator-           (PolynomExt lp, const PolynomExt& rp);
    friend PolynomExt       operator*           (PolynomExt lp, const PolynomExt& rp);
    friend PolynomExt       operator/           (PolynomExt lp, const PolynomExt& rp);
    friend PolynomExt       operator%           (PolynomExt lp, const PolynomExt& rp);

    friend PolynomExt       operator*           (const int number, const PolynomExt& p);
    friend PolynomExt       operator*           (const PolynomExt& p, const int number);
    friend PolynomExt       operator/           (const PolynomExt& p, const int number);

    friend bool             operator==          (const PolynomExt& lp, const PolynomExt& rp);
    friend bool             operator!=          (const PolynomExt& lp, const PolynomExt& rp);
    friend bool             operator==          (const PolynomExt& lp, int i);
    friend bool             operator!=          (const PolynomExt& lp, int i);

private:
    void                    init                (int dim);
    void                    eraseEmptyExtCoef(ExtCoeffs::iterator it);
    std::vector<int>        coefBuf;
};

#endif