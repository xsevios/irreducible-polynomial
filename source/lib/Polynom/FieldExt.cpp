//
// Created by nikit on 12.02.2023.
//

#include "FieldExt.h"
#include "polynom_ext.h"
#include "../Utils/utils.h"


const FieldExt * FieldExt::GetInstance(int p)
{
    return m_vPrimeFields.find(p) == m_vPrimeFields.end() ? m_vPrimeFields.emplace(p, new FieldExt(p)).first->second : m_vPrimeFields[p];
}

const FieldExt * FieldExt::GetInstance(const PolynomExt* f)
{
    return m_sExtFields.find(f->GetHash()) == m_sExtFields.end() ? m_sExtFields.emplace(f->GetHash(), new FieldExt(f)).first->second : m_sExtFields[f->GetHash()];
}

const PolynomExt* FieldExt::GetPolynom() const
{
    return m_pPolynom;
}

FieldExt::FieldExt(const int p) : m_p(p)
{
    m_inversions.push_back(1);

    for (int i = 2; i <= p; i++)
    {
        m_inversions.push_back(GetMultInverse(i));
    }
}

FieldExt::FieldExt(const PolynomExt* f) :
    m_pPolynom(new PolynomExt(*f)),
    m_p(f->GetField()->m_p),
    m_k(f->GetField()->GetDegree() * f->GetDegree())
{

}

/// Расширенный алгоритм Евклида
int gcdex(int a, int b, int& x, int& y);

/// Обратный по умножению элемент к elem над полем размерности p
int FieldExt::GetMultInverse(const int elem) const
{
    assert(this->GetSubfield() == nullptr);

    int inverse = 0;
    int y = 0;

    int modNum = elem % m_p;

    if (modNum < 0)
        modNum += m_p;

    gcdex(modNum, m_p, inverse, y);
    inverse = (inverse % m_p + m_p) % m_p;

    return inverse;
}

/// Обратный по умножению элемент к elem над подполем
PolynomExt FieldExt::GetMultInverse(const PolynomExt* elem) const
{
    assert(elem->GetField() == this->GetSubfield());
    return elem->BinExp(ipow(this->m_p, this->m_k) - 2, *this->GetPolynom());
}

const FieldExt * FieldExt::GetSubfield() const
{
    return m_pPolynom == nullptr ? nullptr : m_pPolynom->GetField();
}

bool FieldExt::IsPrimeField() const {
    return this->GetPolynom() == nullptr;
}
