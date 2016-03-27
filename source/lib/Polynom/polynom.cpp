#include "polynom.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <assert.h>  

extern "C" Polynom* create_polynom(std::string strPolynom)
{
    return new Polynom(strPolynom);
}

extern "C" void destroy_polynom(Polynom* object)
{
    delete object;
}

Polynom::Polynom(int dim, vector<int> coef)
{
    this->dimGF = dim;
    this->coef = coef;
    this->irreducible = NEED_CHECK;
}

Polynom::Polynom(string strPolynom)
{
    irreducible = NEED_CHECK;
    istringstream iss(strPolynom);
    
    string sub;
    iss >> sub;
    dimGF = stoi(sub);
    
    while(iss)
    {
        iss >> sub;
        coef.push_back(stoi(sub));
    }
}

Polynom::~Polynom(){} // don't change

int Polynom::getDim() const
{
    return dimGF;
}

vector<int> Polynom::getCoef() const
{
    return coef;
}

PolynomState Polynom::isIrreducible() const
{
    return irreducible;
}

void Polynom::setIrreducible(PolynomState state)
{
   irreducible = state;
}

int Polynom::getDegree()
{
    return coef.size() -1;
}

Polynom& Polynom::operator=(const Polynom& p)
{
    dimGF = p.dimGF;
    irreducible = p.irreducible;
    coef = p.coef;
    
    return *this;
}

Polynom& Polynom::operator+=(const Polynom& p)
{
    assert(dimGF == p.dimGF);
    
    if (coef.size() < p.coef.size())
        coef.resize(p.coef.size());
    
    for(unsigned i = 0; i < coef.size(); i++)
    {
        coef[i] += p.coef[i];
        
        if (coef[i] >= dimGF)
            coef[i] -= dimGF;
    }
    
    while(!coef.empty() && !coef[p.coef.size()-1]) 
        coef.pop_back();
    
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
    
    vector<int> resCoef(coef.size() + p.coef.size() - 2, 0);
    
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
        if (!coef[i])
            continue;
        
        for (unsigned j = 0; j < small->coef.size(); j++)
        {
            resCoef[i + j] = (resCoef[i + j] + (large->coef[i] * small->coef[j])) % dimGF;
        }
    }
    
    while(!coef.empty() && !coef[p.coef.size()-1]) 
        coef.pop_back();
    
    coef = resCoef;
    
    return *this;
}

Polynom operator*(Polynom lp, const Polynom& rp)
{
    lp *= rp;
    return lp;
}

Polynom operator*(const int number, const Polynom& p)
{
    assert(number >= p.dimGF);
    
    if(!number)
        return Polynom(p.dimGF, vector<int>());
    else if(number == 1)
        return p;
    
    Polynom res = p;
    
    for(vector<int>::iterator i = res.coef.begin(); i < res.coef.end(); i++)
        *i = (*i * number) % p.dimGF;
    
    return res;
}

Polynom operator*(const Polynom& p, const int number)
{
    return number*p;
}

Polynom operator-(const Polynom& p)
{
    return -1*p;
}

Polynom Polynom::operator-()
{
    return -1*(*this);
}


Polynom& Polynom::operator/=(const Polynom& p)
{
    assert(dimGF == p.dimGF);
    
    Polynom divider(dimGF, p.coef);
    Polynom source = p / (*(--(p.coef.end())));
    vector<int> resCoef;
    
    if(coef.size() - divider.coef.size() > 0)
    {
        divider.coef.insert(divider.coef.begin(), coef.size() - divider.coef.size(), 0);
    }
    else if(coef.size() - divider.coef.size() < 0)
    {
        this->coef = vector<int>();
        return *this;    
    }
    
    while(coef.size() >= divider.coef.size())
    {
        divider = (*(--coef.end())) * source;
        resCoef.insert(resCoef.begin(), *--(p.coef.end()));
        *this -= divider;
    }
    
    this->coef = resCoef;
    
    return *this;
}

Polynom operator/(Polynom lp, const Polynom& rp)
{
    lp /= rp;
    return lp;
}

Polynom& Polynom::operator%=(const Polynom& p)
{
    // TODO
    assert(dimGF == p.dimGF);
    
    Polynom divider(dimGF, p.coef);
    Polynom source = p / (*(--(p.coef.end())));
    
    if(coef.size() - divider.coef.size() > 0)
    {
        divider.coef.insert(divider.coef.begin(), coef.size() - divider.coef.size(), 0);
    }
    else if(coef.size() - divider.coef.size() < 0)
    {
        this->coef = vector<int>();
        return *this;    
    }
    
    while(coef.size() >= divider.coef.size())
    {
        divider = (*(--coef.end())) * source;
        *this -= divider;
    }
    
    return *this;
}

Polynom operator%(Polynom lp, const Polynom& rp)
{
    lp %= rp;
    return lp;
}

// Расширенный алгоритм Евклида
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

Polynom operator/(const Polynom& p, const int number)
{
    assert((number >= p.dimGF) || number == 0);
    
    if (number == 1)
        return p;
    
    int inverse = 0;
    int y = 0;
    gcdex(number, p.dimGF, inverse, y);
    inverse = (inverse % p.dimGF + p.dimGF) % p.dimGF;
    
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

int& Polynom::operator[](size_t id)
{
    return coef[id]; 
}

const int& Polynom::operator[](size_t id) const 
{
    return coef[id]; 
}
