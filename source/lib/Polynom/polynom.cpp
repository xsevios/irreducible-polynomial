#include "polynom.h"
#include <iostream>
#include <string>
#include <sstream>

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

Polynom& Polynom::operator=(const Polynom& p)
{
    dimGF = p.dimGF;
    irreducible = p.irreducible;
    coef = p.coef;
    
    return *this;
}

Polynom Polynom::operator+(const Polynom& p)
{
    int new_dim = dimGF;
    
    int count = coef.size() > p.coef.size() ? coef.size() : p.coef.size();
    
    vector<int> new_coef;
    new_coef.resize(count);
    for(unsigned i = 0; i < new_coef.size(); i++)
    {
        new_coef[i] = 0;
        
        if (i < coef.size())
            new_coef[i] += coef[i];
            
        if (i < p.coef.size())
            new_coef[i] += p.coef[i];
        
        new_coef[i] %= new_dim;
    }
    
    Polynom result(new_dim, new_coef);
    return result;
}

Polynom Polynom::operator-(const Polynom& p)
{
    int new_dim = dimGF;
    unsigned count = coef.size() > p.coef.size() ? coef.size() : p.coef.size();
    
    vector<int> new_coef;
    new_coef.resize(count);
    
    for(unsigned i = 0; i < count; i++)
    {
        new_coef[i] = 0;
        
        if (i < coef.size())
            new_coef[i] += coef[i];
            
        if (i < p.coef.size())
            new_coef[i] -= p.coef[i];
            
        if(new_coef[i] < 0)
            new_coef[i] += new_dim;
    }
    
    Polynom result(new_dim, new_coef);
    return result;
}

Polynom Polynom::operator*(const Polynom& p)
{
    // TODO
}

Polynom Polynom::operator/(const Polynom& p)
{
    // TODO
}

bool Polynom::operator==(const Polynom& p)
{
    unsigned size = coef.size();

    if (size != p.coef.size())
        return false;
    else
        for(unsigned i = 0; i < size; i++)
            if (coef[i] != p.coef[i]) 
                return false;
    
    return true;
}

bool Polynom::operator!=(const Polynom& p)
{
    return !operator==(p);
}