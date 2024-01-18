/**
* \file
* \brief Файл с реализацией методов класса Polynom и перегрузкой операторов
*/

#include "polynom.h"
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

extern "C" Polynom* create_polynom(std::string strPolynom)
{
    return new Polynom(strPolynom);
}

extern "C" void destroy_polynom(Polynom* object)
{
    delete object;
}

/**
 * \brief Конструктор класса, инициализирующий поля
 * \param[in] dim размерность поля
 * \param[in] coef вектор коеффициентов многочлена
 */ 
Polynom::Polynom(int dim, vector<int> coef)
{
    m_coef      = coef;
    init(dim);
}

Polynom::Polynom(int dim, uint64_t coefCount)
{
    m_pField    = Field::GetInstance(dim);
    this->irreducible   = NEED_CHECK;
    m_coef.resize(coefCount);
}

/**
 * \brief Конструктор класса, разбирающий строку сожержащую полином 
 * \param[in] strPolynom строка содержащая полином в формате "ПРИЗНАК_ПРИВОДИМОСТИ РАЗМЕРНОСТЬ_ПОЛЯ КОЭФФИЦИЕНТ КОЭФФИЦИЕНТ ... КОЭФФИЦИЕНТ"
 */
Polynom::Polynom(string strPolynom)
{
    istringstream iss(strPolynom);
    
    string sub;
    iss >> sub;
    int dimGF = stoi(sub);
    
    while(iss >> sub)
    {
        m_coef.push_back(stoi(sub));
    }

    init(dimGF);
}

void Polynom::init(int dim)
{
    m_pField    = Field::GetInstance(dim);

    this->irreducible   = NEED_CHECK;

//    for (int i = 2; i <= m_dimGF; i++)
//    {
//        int degree     = 1;
//        int primePower = i;
//
//        while (primePower < m_dimGF)
//        {
//            primePower *= i;
//            degree++;
//        }
//
//        if (primePower == m_dimGF)
//        {
//            prime       = i;
//            primeDeg    = degree;
//            break;
//        }
//    }

    RemoveLeadingZeros();
}

Polynom::~Polynom(){} // don't change

int Polynom::getDim() const
{
    return m_pField->GetDim();
}
/**
 * Возвращает коэффициенты многочлена
 * \return коэффициенты многочлена
 */ 
vector<int> Polynom::getCoef() const
{
    return m_coef;
}

int Polynom::GetLeadingCoef() const
{
    return !m_coef.empty() ? m_coef[m_coef.size() - 1] : 0;
}

/**
 * Возвращает указатель на массив коэффициентов многочлена
 * \return указатель на массив коэффициентов многочлена
 */ 
vector<int>& Polynom::getRefCoef()
{
    return m_coef;
}

/**
 * Возвращает статус приводимости многочлена
 * \return статус приводимости многочлена
 */ 
PolynomState Polynom::isIrreducible() const
{
    return irreducible;
}
/**
 * \Устанавливает статус приводимости полинома
 * \param[in] state новый статус приводимости
*/ 
void Polynom::setIrreducible(PolynomState state) const
{
   irreducible = state;
}

/**
 * Возвращает степень многочлена
 * \return степень многочлена
 */ 
unsigned Polynom::getDegree() const
{
    return m_coef.size() ? m_coef.size() - 1 : 0;
}

unsigned Polynom::getPrime() const
{
    return m_pField->GetPrime();
}

unsigned Polynom::getPrimeDeg() const
{
    return m_pField->GetPrimeDegree();
}

Polynom& Polynom::operator=(const Polynom& p)
{
    if(this == &p)
        return *this;

    irreducible = p.irreducible;
    m_coef      = p.m_coef;
    m_pField    = p.m_pField;
    
    return *this;
}

Polynom& Polynom::operator+=(const Polynom& p)
{
    assert(m_pField == p.m_pField);

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
    
    return *this;
}

Polynom operator+(Polynom lp, const Polynom& rp)
{
    lp += rp;
    return lp;
}

Polynom& Polynom::operator-=(const Polynom& p)
{
    assert(m_pField == p.m_pField);

    int fieldPrime = m_pField->GetPrime();

    auto nSubCoefSize = p.m_coef.size();

    if (m_coef.size() < nSubCoefSize)
        m_coef.resize(nSubCoefSize);

    for(unsigned i = 0; i < m_coef.size(); i++)
    {
        if (i == nSubCoefSize)
            break;

        m_coef[i] -= p.m_coef[i];
        if (m_coef[i] < 0)
            m_coef[i] += fieldPrime;
    }

    RemoveLeadingZeros();

    return *this;
}

Polynom operator-(Polynom lp, const Polynom& rp)
{
    lp -= rp;
    return lp;
}

Polynom& Polynom::operator*=(const Polynom& p)
{
    assert(m_pField == p.m_pField);

    coefBuf.resize(getDegree() + p.getDegree() + 1);
    memset(coefBuf.data(), 0, sizeof(int) * coefBuf.size());
    
    const Polynom* large;
    const Polynom* small;
    
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
    
    return *this;
}

Polynom &Polynom::operator*=(const int number)
{
    auto pField = GetField();
    auto fieldPrime = pField->GetPrime();

    int modNum = number % pField->GetPrime();

    if (modNum < 0)
        modNum += fieldPrime;

    if(!modNum)
    {
        m_coef.resize(0);
        return *this;
    }
    else if(modNum == 1)
    {
        return *this;
    }

    for(auto i = m_coef.begin(); i < m_coef.end(); i++)
        *i = (*i * modNum) % fieldPrime;

    return *this;
}

Polynom operator*(Polynom lp, const Polynom& rp)
{
    lp *= rp;
    return lp;
}

Polynom operator*(const int number, const Polynom& p)
{
    auto pField = p.GetField();
    auto fieldPrime = pField->GetPrime();
    auto fieldDim = pField->GetDim();

    int modNum = number % pField->GetPrime();
    
    if (modNum < 0)
        modNum += fieldPrime;
    
    if(!modNum)
        return Polynom(fieldDim, vector<int>());
    else if(modNum == 1)
        return p;
    
    Polynom res = p;
    
    for(vector<int>::iterator i = res.m_coef.begin(); i < res.m_coef.end(); i++)
        *i = (*i * modNum) % fieldPrime;
        
    return res;
}

Polynom operator*(const Polynom& p, const int number)
{
    return number * p;
}

Polynom operator-(const Polynom& p)
{
    return -1 * p;
}

Polynom Polynom::operator>>(const int number) const
{
    if (number == 0)
        return Polynom(getDim(), getCoef());

    Polynom res(getDim(), m_coef.size() + number);
    memmove(res.m_coef.data() + number, m_coef.data(), (m_coef.size()) * sizeof(int));
    return res;
}

Polynom& Polynom::operator/=(const Polynom& p)
{
    assert(m_pField == p.m_pField);

    if (m_coef.size() < p.getCoef().size())
    {
        this->m_coef = vector<int>();
        return *this;
    }

    Polynom divider(m_pField->GetDim(), p.m_coef);
    int degreeDiff = 0;
    vector<int> resCoef(m_coef.size());

    while ((degreeDiff = m_coef.size() - p.m_coef.size()) >= 0)
    {
        int leadingCoef = (*this)[m_coef.size() - 1];
        int multNumber = (m_pField->GetMultInverse(p[p.m_coef.size() - 1]) * leadingCoef) % getDim();
        divider = p >> degreeDiff;
        Polynom product = divider * multNumber;
        resCoef[degreeDiff] = multNumber;
        *this -= product;
    }

    m_coef = resCoef;
    RemoveLeadingZeros();

    return *this;
}

Polynom operator/(Polynom lp, const Polynom& rp)
{
    lp /= rp;
    return lp;
}

Polynom& Polynom::operator%=(const Polynom& p)
{
    assert(m_pField == p.m_pField);

    if (m_coef.size() < p.getCoef().size())
    {
        return *this;
    }

    Polynom divider(m_pField->GetDim(), p.m_coef);
    int degreeDiff = 0;

    while ((degreeDiff = m_coef.size() - p.m_coef.size()) >= 0)
    {
        int leadingCoef     = (*this)[m_coef.size() - 1];
        int multNumber      = (m_pField->GetMultInverse(p[p.m_coef.size() - 1]) * leadingCoef) % m_pField->GetPrime();
        divider             = p;
        divider             >>= degreeDiff;
        divider             *= multNumber;
        *this -= divider;
    }

    return *this;
}

Polynom operator%(Polynom lp, const Polynom& rp)
{
    lp %= rp;
    return lp;
}

Polynom operator/(const Polynom& p, const int number)
{
    assert(number != 0);
    int inverse = p.m_pField->GetMultInverse(number);
    return inverse * p;
}

bool operator==(const Polynom& lp, const Polynom& rp)
{
    assert(lp.m_pField == rp.m_pField);
    
    unsigned size = lp.m_coef.size();

    if (lp.m_coef.size() != rp.m_coef.size())
        return false;
    else
        for (unsigned i = 0; i < size; i++)
            if (lp[i] != rp[i]) 
                return false;
    
    return true;
}

bool operator!=(const Polynom& lp, const Polynom& rp)
{
    assert(lp.m_pField == rp.m_pField);
    
    return !(lp == rp);
}

bool operator==(const Polynom &lp, int i)
{
    Polynom p(lp.getDim(), std::vector<int>{i});
    return operator==(lp, p);
}

bool operator!=(const Polynom &lp, int i)
{
    Polynom p(lp.getDim(), std::vector<int>{i});
    return operator!=(lp, p);
}

int& Polynom::operator[](size_t id)
{
    return m_coef[id];
}

/**
 * \brief Перегрузка оператора[]
 * \return коэффициент при i-ой степени многочлена
 */ 
const int& Polynom::operator[](size_t id) const 
{
    return m_coef[id];
}

/// Находит производную многочлена
/**
    i-й коэффициент производной равен (i + 1)-му коэффициенту многочлена, умноженному на (i + 1); i = 0, 1, ..., (n-1), где n -- степень многочлена.
*/
Polynom Polynom::Derivative() const
{
    vector<int> derCoef;
    int derDim = getDim();
	
    for (size_t i = 1; i < m_coef.size(); i++)
        derCoef.push_back((i * m_coef[i]) % derDim);

    Polynom der(derDim, derCoef);
    der.RemoveLeadingZeros();

    return der;
}

/// Алгоритм воспроизведения многочлена в целую степень p
// TODO: https://math.stackexchange.com/questions/2652580/how-to-expand-nth-power-of-a-polynomial
Polynom Polynom::Exp(int p) const
{
    const Polynom& x = *this;

    Polynom res(this->getDim(), m_coef);
    while (p > 1)
    {
        res = res * x;
        p--;
    }

    return res;
}

/// Проверяет, является ли многочлен нулём
bool Polynom::IsZero() const
{
    return m_coef.empty();
}

/// Убирает нули при старших коэффициентах
void Polynom::RemoveLeadingZeros()
{
    while(!m_coef.empty() && !m_coef[m_coef.size() - 1])
        m_coef.pop_back();
}

void Polynom::print(ostream& out) const
{
#ifdef DEBUG
    out << this->m_dimGF << " ";
    for(unsigned i = 0; i < this->m_coef.size(); ++i)
    {
        out << this->m_coef[i] << " ";
    }
    out << endl;
#endif
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

Polynom Polynom::BinExp(int n, int exp, const Polynom &f) const
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

const Field *Polynom::GetField() const
{
    return m_pField;
}

Polynom &Polynom::operator>>=(const int number)
{
    if (number == 0)
        return *this;

    this->m_coef.resize(this->m_coef.size() + number);

    memmove(m_coef.data() + number, m_coef.data(), (m_coef.size() - number) * sizeof(int));

    for (int i = 0; i < number; i++)
        m_coef[i] = 0;

    return *this;
}
