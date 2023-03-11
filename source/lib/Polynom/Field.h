//
// Created by nikit on 12.02.2023.
//

#ifndef IRREDUCIBLE_POLYNOMIAL_FIELD_H
#define IRREDUCIBLE_POLYNOMIAL_FIELD_H

#include <vector>
#include <map>

int getMultInverse(int num, int dim);

class Field
{
public:
    static  const Field *               GetInstance         (int dim);
            int                         GetMultInverse      (int num)       const;
            int                         GetDim              ()              const;
            int                         GetPrime            ()              const;
            int                         GetPrimeDegree      ()              const;

private:
            explicit                    Field               (int dim);
            int                         getMultInverse      (int num)       const;

private:
   inline   static   std::map<int, Field*>
                                        m_fields;

            int                         m_dimGF     = 0;  ///< Размерность поля
            int                         m_prime     = 0;  ///< Характеристика поля
            int                         m_primeDeg  = 0;  ///< Степень поля над простым подполем
            std::vector<int>            m_inversions;     ///< Массив инверсий элементов поля
};


inline int Field::GetMultInverse(int num)   const { return m_inversions[num - 1];   }
inline int Field::GetDim        ()          const { return m_dimGF;                 }
inline int Field::GetPrime      ()          const { return m_prime;                 }
inline int Field::GetPrimeDegree()          const { return m_primeDeg;              }

#endif //IRREDUCIBLE_POLYNOMIAL_FIELD_H
