//
// Created by nikit on 12.02.2023.
//

#ifndef IRREDUCIBLE_POLYNOMIAL_FIELD_EXT_H
#define IRREDUCIBLE_POLYNOMIAL_FIELD_EXT_H

#include <vector>
#include <map>
#include <unordered_map>

class FieldExt;
class PolynomExt;

#include "polynom_ext.h"

class FieldExt
{
public:
    static  const FieldExt *            GetInstance         (int                p);
    static  const FieldExt *            GetInstance         (const PolynomExt*  f);
            PolynomExt                  GetMultInverse      (const PolynomExt*  f)  const;
            int                         GetMultInverse      (const int          n)  const;
   unsigned int                         GetPrime            ()                      const;
   unsigned int                         GetDegree           ()                      const;
   unsigned int                         GetDim              ()                      const;
            bool                        IsPrimeField        ()                      const;
      const FieldExt *                  GetSubfield         ()                      const;
      const PolynomExt *                GetPolynom          ()                      const;


private:
            explicit                    FieldExt            (const int          p);
            explicit                    FieldExt            (const PolynomExt*  f);

private:
    inline static std::map<int, FieldExt*>
                                        m_vPrimeFields;
    inline static std::unordered_map<std::size_t, FieldExt*>
                                        m_sExtFields;

      const int                         m_p         = 2;        ///< Характеристика поля
      const int                         m_k         = 1;        ///< Степень поля над простым подполем
      const PolynomExt *                m_pPolynom  = nullptr;  ///< Неприводимый многочлен степени n над текущим полем

            std::vector<int>            m_inversions;
};

inline unsigned int FieldExt::GetPrime      ()          const { return m_p; }
inline unsigned int FieldExt::GetDegree     ()          const { return m_k; }

#endif //IRREDUCIBLE_POLYNOMIAL_FIELD_H
