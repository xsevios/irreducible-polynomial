/**
* \file
* \brief Файл с реализацией методов класса PolynomExt и перегрузкой операторов
*/

#include "polynom_ext.h"
#include "../PolynomGenerator/polynom_generator.h"

#include <iostream>
#include <string>
#include <map>
#include <cmath>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <cassert>
#include <utility>

//#define DEBUG

#ifdef DEBUG
#define LOG_TRACE std::cout << "Entering " << __FUNCTION__ << "() - (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;
#else
#define LOG_TRACE
#endif

extern "C" PolynomExt* create_polynom_ext(std::string strPolynomExt)
{
    return new PolynomExt(strPolynomExt);
}

extern "C" void destroy_polynom_ext(PolynomExt* object)
{
    delete object;
}

ostream& operator<<(ostream& out, const PolynomExt& p)
{
    static const char* plus = " + ";
    static const char* empty = "";

    if (p.GetField()->IsPrimeField())
    {
        const char* sep = empty;

        vector<int> coeffs = p.GetCoef();
        auto i = coeffs.size();
        while (i--)
        {
            if (coeffs[i])
            {
                out << sep << "(" << coeffs[i] << ")" << "x_" << ipow(p.GetField()->GetPrime(), p.GetField()->GetDegree()) << "^" << i;
                sep = plus;
            }
        }
    }
    else
    {
        const char* sep = empty;

        for (auto coeff = p.GetRefExtCoef().rbegin(); coeff != p.GetRefExtCoef().rend(); coeff++)
        {
            out << sep << "(" << coeff->second << ")x_" << ipow(p.GetField()->GetPrime(), p.GetField()->GetDegree()) << "^" << coeff->first;
            sep = plus;
        }
    }

    return out;
}

/**
 * \brief Конструктор класса, инициализирующий поля
 * \param[in] dim размерность поля
 * \param[in] coef вектор коеффициентов многочлена
 */
PolynomExt::PolynomExt(const FieldExt* pField, const std::map<int, PolynomExt>& extCoef)
{
    m_pField    = pField;
    m_extCoef   = extCoef;
}

/**
 * \brief Конструктор класса, разбирающий строку сожержащую полином
 * \param[in] strPolynomExt строка содержащая полином в формате "ПРИЗНАК_ПРИВОДИМОСТИ РАЗМЕРНОСТЬ_ПОЛЯ КОЭФФИЦИЕНТ КОЭФФИЦИЕНТ ... КОЭФФИЦИЕНТ"
 */
PolynomExt::PolynomExt(const string& strPolynomExt)
{
    istringstream iss(strPolynomExt);

    string sub;
    iss >> sub;
    int p = stoi(sub);

    while(iss >> sub)
    {
        m_coef.push_back(stoi(sub));
    }

    init(p);
}

PolynomExt::PolynomExt(const FieldExt* pField, const vector<int>& coef)
{
    m_pField = pField;

    if (pField->IsPrimeField())
    {
        m_coef = coef;
    }
    else
    {
        for (int i = 0; i < coef.size(); i++)
        {
            if (coef[i] != 0)
            {
                m_extCoef.emplace(i, PolynomExt(pField->GetSubfield(), std::vector<int>(1, coef[i])));
            }
        }
    }
}

PolynomExt::PolynomExt(const FieldExt* pField, uint64_t coefCount)
{
    assert(pField->IsPrimeField());
    m_pField = pField;
    m_coef.resize(coefCount);
}

void PolynomExt::init(int dim)
{
    m_pField = FieldExt::GetInstance(dim);
    RemoveLeadingZeros();
}

PolynomExt::~PolynomExt(){} // don't change

int PolynomExt::GetDim() const
{
    return ipow(m_pField->GetPrime(), m_pField->GetDegree());
}

/**
 * Возвращает коэффициенты многочлена
 * \return коэффициенты многочлена
 */
vector<int> PolynomExt::GetCoef() const
{
    return m_coef;
}

int PolynomExt::GetLeadingCoef() const
{
    return !m_coef.empty() ? m_coef[m_coef.size() - 1] : 0;
}

/**
 * Возвращает указатель на массив коэффициентов многочлена
 * \return указатель на массив коэффициентов многочлена
 */
vector<int>& PolynomExt::getRefCoef()
{
    return m_coef;
}

/**
 * Возвращает указатель на массив коэффициентов многочлена
 * \return указатель на массив коэффициентов многочлена
 */
const vector<int>& PolynomExt::getRefCoef() const
{
    return m_coef;
}

/**
 * Возвращает статус приводимости многочлена
 * \return статус приводимости многочлена
 */
PolynomState PolynomExt::isIrreducible() const
{
    return m_eIrreducible;
}
/**
 * \Устанавливает статус приводимости полинома
 * \param[in] state новый статус приводимости
*/
void PolynomExt::setIrreducible(PolynomState state) const
{
    m_eIrreducible = state;
}

/**
 * Возвращает степень многочлена
 * \return степень многочлена
 */
unsigned PolynomExt::GetDegree() const
{
    if (GetField()->IsPrimeField())
    {
        return m_coef.size() ? m_coef.size() - 1 : 0;
    }
    else
    {
        return m_extCoef.size() ? m_extCoef.rbegin()->first : 0;
    }
}

unsigned PolynomExt::GetPrime() const
{
    return m_pField->GetPrime();
}

PolynomExt& PolynomExt::operator=(const PolynomExt& p)
{
    if(this == &p)
        return *this;

    m_eIrreducible  = p.m_eIrreducible;
    m_coef          = p.m_coef;
    m_extCoef       = p.m_extCoef;
    m_pField        = p.m_pField;

    return *this;
}

PolynomExt& PolynomExt::operator+=(const PolynomExt& p)
{
    assert(m_pField == p.m_pField);

    if (GetField()->IsPrimeField())
    {
        int fieldPrime = m_pField->GetPrime();

        if (m_coef.size() < p.m_coef.size())
            m_coef.resize(p.m_coef.size());

        for(unsigned i = 0; i < m_coef.size(); i++)
        {
            if (i == p.m_coef.size())
                break;

            m_coef[i] += p.m_coef[i];
            if (m_coef[i] >= fieldPrime)
                m_coef[i] -= fieldPrime;
        }

        RemoveLeadingZeros();
    }
    else
    {
        for (const auto& coeff : p.GetRefExtCoef())
        {
            auto deg = coeff.first;
            auto it = this->m_extCoef.find(deg);
            if (it == m_extCoef.end())
            {
                this->m_extCoef.emplace(deg, coeff.second);
            }
            else
            {
                it->second += coeff.second;
                eraseEmptyExtCoef(it);
            }
        }
    }

    return *this;
}

PolynomExt operator+(PolynomExt lp, const PolynomExt& rp)
{
    lp += rp;
    return lp;
}

PolynomExt& PolynomExt::operator-=(const PolynomExt& p)
{
    assert(m_pField == p.m_pField);

    if (GetField()->IsPrimeField())
    {
        int fieldPrime = m_pField->GetPrime();

        auto nSubCoefSize = p.m_coef.size();

        if (m_coef.size() < nSubCoefSize)
            m_coef.resize(nSubCoefSize);

        for (unsigned i = 0; i < m_coef.size(); i++)
        {
            if (i == nSubCoefSize)
                break;

            m_coef[i] -= p.m_coef[i];
            if (m_coef[i] < 0)
                m_coef[i] += fieldPrime;
        }

        RemoveLeadingZeros();
    }
    else
    {
        for (const auto& coeff : p.GetRefExtCoef())
        {
            auto i = coeff.first;
            auto it = this->m_extCoef.find(i);
            if (it == m_extCoef.end())
            {
                this->m_extCoef.emplace(i, -coeff.second);
            }
            else
            {
                it->second -= coeff.second;
                eraseEmptyExtCoef(it);
            }
        }
    }

    return *this;
}

PolynomExt operator-(PolynomExt lp, const PolynomExt& rp)
{
    lp -= rp;
    return lp;
}

PolynomExt& PolynomExt::operator*=(const PolynomExt& p)
{
    if (GetField()->IsPrimeField())
    {
        coefBuf.resize(GetDegree() + p.GetDegree() + 1);
        memset(coefBuf.data(), 0, sizeof(int) * coefBuf.size());

        const PolynomExt *large;
        const PolynomExt *small;

        if (m_coef.size() > p.m_coef.size())
        {
            large = this;
            small = &p;
        }
        else
        {
            large = &p;
            small = this;
        }

        for (unsigned i = 0; i < large->m_coef.size(); i++)
        {
            if (!large->m_coef[i])
                continue;

            for (unsigned j = 0; j < small->m_coef.size(); j++)
            {
                coefBuf[i + j] = (coefBuf[i + j] + (large->m_coef[i] * small->m_coef[j])) % m_pField->GetPrime();
            }
        }

        m_coef = coefBuf;
        RemoveLeadingZeros();
    }
    else
    {
        bool f_mul_elem = this->GetField()->GetSubfield() == p.GetField();
        bool elem_mul_f = this->GetField() == p.GetField()->GetSubfield();

        if (this->GetField() == p.GetField())
        {
            ExtCoeffs extCoeffs;

            for (const auto& first_coeff: this->GetRefExtCoef())
            {
                auto first_deg = first_coeff.first;
                for (const auto& second_coeff: p.GetRefExtCoef())
                {
                    auto second_deg = second_coeff.first;
                    auto mul = first_coeff.second * second_coeff.second;
                    int result_deg = 0;

                    if (first_deg > 0 && second_deg > 0)
                    {
                        result_deg = first_deg + second_deg;
                    }
                    else if (first_deg == 0)
                    {
                        result_deg = second_deg;
                    }
                    else if (second_deg == 0)
                    {
                        result_deg = first_deg;
                    }

                    auto it = extCoeffs.find(result_deg);

                    if (it == extCoeffs.end())
                    {
                        extCoeffs.emplace(result_deg, mul % *GetField()->GetPolynom());
                    }
                    else
                    {
                        it->second += mul;
                        it->second %= *GetField()->GetPolynom();
                    }
                }
            }

            for (auto it = extCoeffs.cbegin(), next_it = it; it != extCoeffs.cend(); it = next_it)
            {
                ++next_it;
                if (it->second.IsZero())
                {
                    extCoeffs.erase(it);
                }
            }

            m_extCoef = extCoeffs;
        }
        else if (f_mul_elem || elem_mul_f)
        {
            auto elem = f_mul_elem ? &p : this;

            bool bThisIsElem = this == elem;

            if (bThisIsElem)
            {
                elem = new PolynomExt(*elem);
                *this = p;
            }

            for (auto& coeff : m_extCoef)
            {
                coeff.second *= *elem;
                coeff.second %= *GetField()->GetPolynom();
            }

            if (bThisIsElem)
            {
                delete elem;
            }
        }
        else
            assert(0);
    }

    return *this;
}

PolynomExt &PolynomExt::operator*=(const int number)
{
    auto fieldPrime = GetField()->GetPrime();

    int modNum = number % fieldPrime;

    if (modNum < 0)
        modNum += fieldPrime;

    if (modNum == 1)
        return *this;

    if (modNum == 0)
    {
        m_coef.resize(0);
        m_extCoef.clear();
        return *this;
    }

    if (this->GetField()->IsPrimeField())
    {
        for (auto i = m_coef.begin(); i < m_coef.end(); i++)
            *i = (*i * modNum) % fieldPrime;
    }
    else
    {
        for (auto& coeff : m_extCoef)
            coeff.second *= modNum;
    }

    return *this;
}

PolynomExt operator*(PolynomExt lp, const PolynomExt& rp)
{
    lp *= rp;
    return lp;
}

PolynomExt operator*(const int number, const PolynomExt& p)
{
    int fieldPrime = p.GetField()->GetPrime();

    int modNum = number % fieldPrime;

    if (modNum < 0)
        modNum += fieldPrime;

    if(!modNum)
        return PolynomExt(p.GetField(), vector<int>());
    else if(modNum == 1)
        return p;

    PolynomExt res = p;

    if (res.GetField()->IsPrimeField())
    {
        for (auto i = res.m_coef.begin(); i < res.m_coef.end(); i++)
        {
            *i = (*i * modNum) % fieldPrime;
        }
    }
    else
    {
        for (auto& coeff : res.m_extCoef)
        {
            coeff.second *= number;
        }
    }

    return res;
}

PolynomExt operator*(const PolynomExt& p, const int number)
{
    return number * p;
}

PolynomExt operator-(const PolynomExt& p)
{
    return -1 * p;
}

PolynomExt PolynomExt::operator>>(const int number) const
{
    if (GetField()->IsPrimeField())
    {
        if (number == 0)
            return PolynomExt(GetField(), GetCoef());

        PolynomExt res(GetField(), m_coef.size() + number);
        memmove(res.m_coef.data() + number, m_coef.data(), (m_coef.size()) * sizeof(int));
        return res;
    }
    else
    {
        ExtCoeffs coeffs;

        for (const auto& coeff : m_extCoef)
            coeffs.emplace(coeff.first + number, coeff.second);

        return PolynomExt(GetField(), coeffs);
    }
}

PolynomExt& PolynomExt::operator/=(const PolynomExt& p)
{
    assert(m_pField == p.m_pField);

    if (p.GetField()->IsPrimeField())
    {
        if (m_coef.size() < p.GetCoef().size())
        {
            this->m_coef = vector<int>();
            return *this;
        }

        PolynomExt divider(GetField(), p.m_coef);
        int degreeDiff = 0;
        vector<int> resCoef(m_coef.size());

        while ((degreeDiff = m_coef.size() - p.m_coef.size()) >= 0)
        {
            int leadingCoef = (*this)[m_coef.size() - 1];
            int multNumber = (m_pField->GetMultInverse(p[p.m_coef.size() - 1]) * leadingCoef) % GetPrime();
            divider = p >> degreeDiff;
            divider *= multNumber;
            resCoef[degreeDiff] = multNumber;
            *this -= divider;
        }

        m_coef = resCoef;
        RemoveLeadingZeros();
    }
    else
    {
        if (this->GetDegree() < p.GetDegree())
        {
            this->m_extCoef.clear();
            return *this;
        }

        PolynomExt divider(p.GetField(), p.GetRefExtCoef());
        int degreeDiff = 0;
        ExtCoeffs resCoef;

        while ((degreeDiff = this->GetDegree() - p.GetDegree()) >= 0 && this->m_extCoef.size() > 0)
        {
            auto leadingCoef = this->GetLeadingExtCoef();
            auto multElem = (m_pField->GetMultInverse(&p.m_extCoef.rbegin()->second) * *leadingCoef) % *this->GetField()->GetPolynom();
            divider = p >> degreeDiff;
            divider *= multElem;

            resCoef.emplace(degreeDiff, multElem);
            *this -= divider;
        }

        m_extCoef = resCoef;
    }

    return *this;
}

PolynomExt operator/(PolynomExt lp, const PolynomExt& rp)
{
    lp /= rp;
    return lp;
}

PolynomExt& PolynomExt::operator%=(const PolynomExt& p)
{
    assert(m_pField == p.m_pField);

    if (GetField()->IsPrimeField())
    {
        if (m_coef.size() < p.GetCoef().size())
        {
            return *this;
        }

        auto degP = p.m_coef.size();

        int degreeDiff = 0;
        int nPrime = this->GetPrime();

        while ((degreeDiff = m_coef.size() - p.m_coef.size()) >= 0)
        {
            int leadingCoef = (*this)[m_coef.size() - 1];
            int multNumber = (m_pField->GetMultInverse(p[p.m_coef.size() - 1]) * leadingCoef) % m_pField->GetPrime();

            for (int i = 0; i < degP; i++)
            {
                this->m_coef[i + degreeDiff] -= (p.m_coef[i] * multNumber) % nPrime;
            }

            for (int i = 0; i < degP; i++)
            {
                if (this->m_coef[i + degreeDiff] < 0)
                    this->m_coef[i + degreeDiff] += nPrime;
            }

            this->RemoveLeadingZeros();
        }
    }
    else
    {
        if (this->GetDegree() < p.GetDegree())
        {
            return *this;
        }

        int degreeDiff = 0;

        while ((degreeDiff = this->GetDegree() - p.GetDegree()) >= 0 && this->m_extCoef.size() > 0)
        {
            auto leadingCoef = this->GetLeadingExtCoef();
            auto multElem = (m_pField->GetMultInverse(&p.m_extCoef.rbegin()->second) * *leadingCoef) % *this->GetField()->GetPolynom();

            for (const auto& coeff : p.m_extCoef)
            {
                auto curDeg = coeff.first + degreeDiff;
                auto it = this->m_extCoef.find(curDeg);
                if (it == this->m_extCoef.end())
                {
                    auto res = this->m_extCoef.emplace(curDeg, -(coeff.second * multElem) % *this->GetField()->GetPolynom());
                    it = res.first;
                }
                else
                {
                    it->second -= (coeff.second * multElem) % *this->GetField()->GetPolynom();
                }

                eraseEmptyExtCoef(it);
            }
        }
    }

    return *this;
}

PolynomExt operator%(PolynomExt lp, const PolynomExt& rp)
{
    lp %= rp;
    return lp;
}

PolynomExt operator/(const PolynomExt& p, const int number)
{
    assert(number != 0);
    int inverse = p.m_pField->GetMultInverse(number);
    return inverse * p;
}

bool operator==(const PolynomExt& lp, const PolynomExt& rp)
{
    assert(lp.m_pField == rp.m_pField);

    if (lp.GetField()->IsPrimeField())
    {
        unsigned size = lp.m_coef.size();

        if (lp.m_coef.size() != rp.m_coef.size())
            return false;
        else
            for (unsigned i = 0; i < size; i++)
                if (lp[i] != rp[i])
                    return false;

        return true;
    }
    else
    {
        auto& lp_coeffs = lp.GetRefExtCoef();
        auto& rp_coeffs = rp.GetRefExtCoef();

        for (const auto& lp_it : lp_coeffs)
        {
            const auto& rp_it = rp_coeffs.find(lp_it.first);
            if (rp_it == rp_coeffs.end())
                return false;

            if (rp_it->second != lp_it.second)
                return false;
        }

        for (const auto& rp_it : rp_coeffs)
        {
            const auto& lp_it = lp_coeffs.find(rp_it.first);
            if (lp_it == lp_coeffs.end())
                return false;
        }

        return true;
    }
}

bool operator!=(const PolynomExt& lp, const PolynomExt& rp)
{
    assert(lp.m_pField == rp.m_pField);

    return !(lp == rp);
}

bool operator==(const PolynomExt &lp, int number)
{
    auto fieldPrime = lp.GetField()->GetPrime();

    int modNum = number % fieldPrime;

    if (modNum < 0)
        modNum += fieldPrime;

    if (lp.GetField()->IsPrimeField())
    {
        if (modNum == 0)
        {
            return lp.m_coef.empty();
        }
        else
        {
            return lp.m_coef.size() == 1 && lp.m_coef[0] == modNum;
        }
    }
    else
    {
        if (modNum == 0)
        {
            return lp.m_extCoef.empty();
        }
        else
        {
            if (lp.m_extCoef.size() != 1)
                return false;

            auto it = lp.m_extCoef.find(0);
            if (it == lp.m_extCoef.end())
                return false;

            return it->second == modNum;
        }
    }
}

bool operator!=(const PolynomExt &lp, int number)
{
    return !operator==(lp, number);
}

int& PolynomExt::operator[](size_t id)
{
    return m_coef[id];
}

/**
 * \brief Перегрузка оператора[]
 * \return коэффициент при i-ой степени многочлена
 */
const int& PolynomExt::operator[](size_t id) const
{
    return m_coef[id];
}

/// Находит производную многочлена
/**
    i-й коэффициент производной равен (i + 1)-му коэффициенту многочлена, умноженному на (i + 1); i = 0, 1, ..., (n-1), где n -- степень многочлена.
*/
PolynomExt PolynomExt::Derivative() const
{
    if (GetField()->IsPrimeField())
    {
        vector<int> derCoef;
        int derDim = GetPrime();

        for (size_t i = 1; i < m_coef.size(); i++)
            derCoef.push_back((i * m_coef[i]) % derDim);

        PolynomExt der(GetField(), derCoef);
        der.RemoveLeadingZeros();

        return der;
    }
    else
    {
        ExtCoeffs coeffs;

        for (const auto& coeff : m_extCoef)
        {
            if (coeff.first > 0)
            {
                auto result = coeff.second * coeff.first;
                if (!result.IsZero())
                    coeffs.emplace(coeff.first - 1, result);
            }
        }

        return PolynomExt(GetField(), coeffs);
    }
}

/// Алгоритм воспроизведения многочлена в целую степень p
// TODO: https://math.stackexchange.com/questions/2652580/how-to-expand-nth-power-of-a-polynomial
PolynomExt PolynomExt::Exp(int p) const
{
    const PolynomExt& x = *this;

    PolynomExt res(this->GetField(), m_coef);
    while (p > 1)
    {
        res = res * x;
        p--;
    }

    return res;
}

/// Проверяет, является ли многочлен нулём
bool PolynomExt::IsZero() const
{
    if (GetField()->IsPrimeField())
        return m_coef.empty();
    else
        return m_extCoef.empty();
}

/// Убирает нули при старших коэффициентах
void PolynomExt::RemoveLeadingZeros()
{
    if (GetField()->IsPrimeField())
    {
        while (!m_coef.empty() && !m_coef[m_coef.size() - 1])
            m_coef.pop_back();
    }
}

void PolynomExt::print(ostream& out) const
{
    static const char* plus = " + ";
    static const char* empty = "";

    if (this->GetField()->IsPrimeField())
    {
        const char* sep = empty;

        vector<int> coeffs = this->GetCoef();
        auto i = coeffs.size();
        while (i--)
        {
            if (coeffs[i])
            {
                out << sep << "(" << coeffs[i] << ")" << "x_" << ipow(this->GetField()->GetPrime(), this->GetField()->GetDegree()) << "^" << i;
                sep = plus;
            }
        }
    }
    else
    {
        const char* sep = empty;

        for (auto coeff = this->GetRefExtCoef().rbegin(); coeff != this->GetRefExtCoef().rend(); coeff++)
        {
            out << sep << "(" << coeff->second << ")x_" << ipow(this->GetField()->GetPrime(), this->GetField()->GetDegree()) << "^" << coeff->first;
            sep = plus;
        }
    }
}

constexpr long long int MAX_SAFE_DOUBLE = 9007199254740992; // 2^53

constexpr int maxExp(int n)
{
    int exp = 1;
    while (std::pow((long double)n, (long double)exp) < MAX_SAFE_DOUBLE)
    {
        exp++;
    }

    return exp - 1;
}

PolynomExt PolynomExt::BinExp(int n, int exp, const PolynomExt &f) const
{
    static std::map<int, int> limits;

    if (limits[n] == 0)
    {
        limits[n] = maxExp(n);
    }

    if (exp < limits[n])
        return this->BinExp((long long) std::pow((long double)n, (long double)exp), f);

    bigint bigq = n;
    bigint bigr = exp;
    return this->BinExp(bigq ^ bigr, f);
}

const FieldExt *PolynomExt::GetField() const
{
    return m_pField;
}

const ExtCoeffs& PolynomExt::GetRefExtCoef() const {
    return this->m_extCoef;
}

std::size_t PolynomExt::GetHash() const
{
    const auto* pField = GetField();
    std::size_t hashValue = 0;
    std::hash<int> hasher;

    if (pField->IsPrimeField())
    {
        for (auto coef : this->getRefCoef())
        {
            hashValue ^= hasher(coef) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
        }

        hashValue ^= hasher(this->GetField()->GetPrime()) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
    }
    else
    {
        for (const auto& coef : this->GetRefExtCoef())
        {
            hashValue ^= hasher(coef.first) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
            hashValue ^= hasher(coef.second.GetHash()) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
        }
    }

    return hashValue;
}

void PolynomExt::eraseEmptyExtCoef(ExtCoeffs::iterator it)
{
    if (it->second.GetField()->IsPrimeField())
    {
        if (it->second.getRefCoef().empty())
        {
            this->m_extCoef.erase(it);
        }
    }
    else if (it->second.GetRefExtCoef().empty())
    {
        this->m_extCoef.erase(it);
    }
}

const PolynomExt* PolynomExt::GetLeadingExtCoef() const
{
    return !m_extCoef.empty() ? &m_extCoef.rbegin()->second : nullptr;
}

bool PolynomExt::CheckConsistensy() const
{
    if (this->GetField()->IsPrimeField())
        return true;

    for (const auto& coef : this->GetRefExtCoef())
    {
        if (coef.second.IsZero())
            return false;
    }

    return true;
}
