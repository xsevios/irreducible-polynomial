/**
* \file
* \brief Файл с реализацией методов класса Polynom и перегрузкой операторов
*/

#include "polynom.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <cassert>

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
    this->dimGF         = dim;
    this->coef          = coef;

    init();
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
    dimGF = stoi(sub);
    
    while(iss >> sub)
    {
        coef.push_back(stoi(sub));
    }

    init();
}

void Polynom::init()
{
    this->irreducible   = NEED_CHECK;

    for (int i = 2; i <= dimGF; i++)
    {
        int degree     = 1;
        int primePower = i;

        while (primePower < dimGF)
        {
            primePower *= i;
            degree++;
        }

        if (primePower == dimGF)
        {
            prime       = i;
            primeDeg    = degree;
            break;
        }
    }
}

Polynom::~Polynom(){} // don't change

int Polynom::getDim() const
{
    return dimGF;
}
/**
 * Возвращает коэффициенты многочлена
 * \return коэффициенты многочлена
 */ 
vector<int> Polynom::getCoef() const
{
    return coef;
}

int Polynom::GetLeadingCoef() const
{
    return !coef.empty() ? coef[coef.size() - 1] : 0;
}

/**
 * Возвращает указатель на массив коэффициентов многочлена
 * \return указатель на массив коэффициентов многочлена
 */ 
vector<int>& Polynom::getRefCoef()
{
    return coef;
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
    return coef.size() ? coef.size() - 1 : 0;
}

unsigned Polynom::getPrime() const
{
    return prime;
}

unsigned Polynom::getPrimeDeg() const
{
    return primeDeg;
}

Polynom& Polynom::operator=(const Polynom& p)
{
    if(this == &p)
        return *this;
    
    dimGF       = p.dimGF;
    irreducible = p.irreducible;
    coef        = p.coef;
    prime       = p.prime;
    primeDeg    = p.primeDeg;
    
    return *this;
}

Polynom& Polynom::operator+=(const Polynom& p)
{
    assert(dimGF == p.dimGF);
    
    if (coef.size() < p.coef.size())
        coef.resize(p.coef.size());
    
    for(unsigned i = 0; i < coef.size(); i++)
    {
        if (i == p.coef.size())
            break;
        
        coef[i] += p.coef[i];
        if (coef[i] >= prime)
            coef[i] -= prime;
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
    *this += -p;
    return *this;
}

Polynom operator-(Polynom lp, const Polynom& rp)
{
    lp -= rp;
    return lp;
}

Polynom& Polynom::operator*=(const Polynom& p)
{
    assert(dimGF == p.dimGF);
    
    vector<int> resCoef(getDegree() + p.getDegree() + 1, 0);
    
    const Polynom* large;
    const Polynom* small;
    
    if (coef.size() > p.coef.size())
    {
        large = this;
        small = &p;
    }
    else
    {
        large = &p;
        small = this;
    }
    
    for (unsigned i = 0; i < large->coef.size(); i++)
    {
        if (!large->coef[i])
            continue;
        
        for (unsigned j = 0; j < small->coef.size(); j++)
        {
            resCoef[i + j] = (resCoef[i + j] + (large->coef[i] * small->coef[j])) % prime;
        }
    }
    
    coef = resCoef;
    RemoveLeadingZeros();
    
    return *this;
}

Polynom &Polynom::operator*=(const int number)
{
    *this *= Polynom(this->getDim(), { number });
    return *this;
}

Polynom operator*(Polynom lp, const Polynom& rp)
{
    lp *= rp;
    return lp;
}

Polynom operator*(const int number, const Polynom& p)
{
    int modNum = number % p.prime;
    
    if (modNum < 0)
        modNum += p.prime;
    
    if(!modNum)
        return Polynom(p.dimGF, vector<int>());
    else if(modNum == 1)
        return p;
    
    Polynom res = p;
    
    for(vector<int>::iterator i = res.coef.begin(); i < res.coef.end(); i++)
        *i = (*i * modNum) % p.prime;
        
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

Polynom Polynom::operator-()
{
    return -1*(*this);
}

Polynom Polynom::operator>>(const int number) const
{
    Polynom res(getDim(), getCoef());
    res.coef.resize(coef.size() + number);

    if (number != 0)
    {
        for (int i = res.coef.size() - 1; i >= 0; i--)
        {
            res.coef[i] = i >= number ? res.coef[i - number] : 0;
        }
    }

    return res;
}

Polynom& Polynom::operator/=(const Polynom& p)
{
    assert(dimGF == p.dimGF);

    if (coef.size() < p.getCoef().size())
    {
        this->coef = vector<int>();
        return *this;
    }

    Polynom divider(dimGF, p.coef);
    int degreeDiff = 0;
    vector<int> resCoef(coef.size());

    while ((degreeDiff = coef.size() - p.coef.size()) >= 0)
    {
        int leadingCoef = (*this)[coef.size() - 1];
        int multNumber = (getMultInverse(p[p.coef.size() - 1], this->prime) * leadingCoef) % getDim();
        divider = p >> degreeDiff;
        Polynom product = divider * multNumber;
        resCoef[degreeDiff] = multNumber;
        *this -= product;
    }

    this->coef = resCoef;
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
    assert(dimGF == p.dimGF);

    if (coef.size() < p.getCoef().size())
    {
        return *this;
    }

    Polynom divider(dimGF, p.coef);
    int degreeDiff = 0;

    while ((degreeDiff = coef.size() - p.coef.size()) >= 0)
    {
        int leadingCoef     = (*this)[coef.size() - 1];
        int multNumber      = (getMultInverse(p[p.coef.size() - 1], this->prime) * leadingCoef) % this->prime;
        divider             = p >> degreeDiff;
        Polynom product     = divider * multNumber;
        *this -= product;
    }

    return *this;
}

Polynom operator%(Polynom lp, const Polynom& rp)
{
    lp %= rp;
    return lp;
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
int getMultInverse(int num, int dim)
{
    int inverse = 0;
    int y = 0;

    int modNum = num % dim;

    if (modNum < 0)
        modNum += dim;

    gcdex(modNum, dim, inverse, y);
    inverse = (inverse % dim + dim) % dim;

    return inverse;
}

Polynom operator/(const Polynom& p, const int number)
{
    assert(number != 0);
    int inverse = getMultInverse(number, p.prime);
    return inverse * p;
}

bool operator==(const Polynom& lp, const Polynom& rp)
{
    assert(lp.dimGF == rp.dimGF);
    
    unsigned size = lp.coef.size();

    if (size != rp.coef.size())
        return false;
    else
        for (unsigned i = 0; i < size; i++)
            if (lp[i] != rp[i]) 
                return false;
    
    return true;
}

bool operator!=(const Polynom& lp, const Polynom& rp)
{
    assert(lp.dimGF == rp.dimGF);
    
    return !(lp == rp);
}

bool operator==(const Polynom &lp, int i)
{
    Polynom p(lp.getDim(), { i });
    return operator==(lp, p);
}

bool operator!=(const Polynom &lp, int i)
{
    Polynom p(lp.getDim(), { i });
    return operator!=(lp, p);
}

int& Polynom::operator[](size_t id)
{
    return coef[id]; 
}

/**
 * \brief Перегрузка оператора[]
 * \return коэффициент при i-ой степени многочлена
 */ 
const int& Polynom::operator[](size_t id) const 
{
    return coef[id]; 
}

/// Находит производную многочлена
/**
    i-й коэффициент производной равен (i + 1)-му коэффициенту многочлена, умноженному на (i + 1); i = 0, 1, ..., (n-1), где n -- степень многочлена.
*/
Polynom Polynom::Derivative() const
{
    vector<int> derCoef;
    int derDim = getDim();
	
    for (size_t i = 1; i < coef.size(); i++)
        derCoef.push_back((i * coef[i]) % derDim);

    Polynom der(derDim, derCoef);
    der.RemoveLeadingZeros();

    return der;
}

/// Алгоритм воспроизведения многочлена в целую степень p
// TODO: https://math.stackexchange.com/questions/2652580/how-to-expand-nth-power-of-a-polynomial
Polynom Polynom::Exp(int p) const
{
    const Polynom& x = *this;

    Polynom res(dimGF, coef);
    while (p > 1)
    {
        res = res * x;
        p--;
    }

    return res;
}

#define CHECK_BIT(var, pos) ((var) & (1 << (pos)))

int msb(int x)
{
    int num = 0;

    while (x >>= 1)
    {
        num++;
    }

    return num;
}

/// Binary Exponentiation Algorithm for Computing x^M mod f (Reapeted squaring)
Polynom Polynom::BinExp(long long M, const Polynom &f) const
{
    const Polynom& x    = *this;
    Polynom g           = *this;

    for (int j = msb(M) - 1; j >= 0; j--)
    {
        g = (g * g) % f;

        if (CHECK_BIT(M, j))
        {
            g = (g * x) % f;
        }
    }

    return g;
}

/// Проверяет, является ли многочлен нулём
bool Polynom::IsZero() const
{
    return coef.empty();
}

/// Убирает нули при старших коэффициентах
void Polynom::RemoveLeadingZeros()
{
    while(!coef.empty() && !coef[coef.size() - 1])
        coef.pop_back();
}

void Polynom::print(ostream& out) const
{
#ifdef DEBUG
    out << this->dimGF << " ";
    for(unsigned i = 0; i < this->coef.size(); ++i)
    {
        out << this->coef[i] << " ";  
    }
    out << endl;
#endif
}
