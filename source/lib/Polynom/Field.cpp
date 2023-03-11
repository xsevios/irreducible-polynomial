//
// Created by nikit on 12.02.2023.
//

#include "Field.h"

const Field * Field::GetInstance(int dim)
{
    return m_fields.find(dim) == m_fields.end() ? m_fields.emplace(dim, new Field(dim)).first->second : m_fields[dim];
}

Field::Field(int dim)
{
    m_dimGF = dim;

    m_inversions.push_back(1);

    for (int i = 2; i <= m_dimGF; i++)
    {
        m_inversions.push_back(getMultInverse(i));

        if (m_prime == 0)
        {
            int degree     = 1;
            int primePower = i;

            while (primePower < m_dimGF)
            {
                primePower *= i;
                degree++;
            }

            if (primePower == m_dimGF)
            {
                m_prime    = i;
                m_primeDeg = degree;
            }
        }
    }
}

/// Расширенный алгоритм Евклида
int gcdex(int a, int b, int& x, int& y) {
    if (a == 0)
    {
        x = 0; y = 1;
        return b;
    }
    int x1, y1;
    int d = gcdex(b%a, a, x1, y1);
    x = y1 - (b / a) * x1;
    y = x1;
    return d;
}

/// Обратный по умножению элемент к num над полем размерности dim
int Field::getMultInverse(int num) const
{
    int inverse = 0;
    int y = 0;

    int modNum = num % m_dimGF;

    if (modNum < 0)
        modNum += m_dimGF;

    gcdex(modNum, m_dimGF, inverse, y);
    inverse = (inverse % m_dimGF + m_dimGF) % m_dimGF;

    return inverse;
}
