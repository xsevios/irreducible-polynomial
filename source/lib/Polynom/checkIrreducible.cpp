#include "checkIrreducible.h"
#include <assert.h>

//#define DEBUG

#ifdef DEBUG
#define LOG_TRACE std::cout << "Entering " << __FUNCTION__ << "() - (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;
#else
#define LOG_TRACE
#endif

void PolynomChecker::init(Polynom* p)
{
    polynom = p;
    busy = true;
}

void PolynomChecker::checkPol(void)
{
    polynom->setIrreducible(IRREDUCIBLE);
    Polynom* at = polynom;
    
    if (at->getDegree() == 0)
    {
        polynom->setIrreducible(IRREDUCIBLE);
        return;
    }
    else if (at->getCoef()[0] == 0)
    {
        if (at->getDegree() == 1)
            polynom->setIrreducible(IRREDUCIBLE);
        else
            polynom->setIrreducible(REDUCIBLE);
            
        return;
    }
    int n = pow(polynom->getDim(), polynom->getDegree()/2 + 1) - 1;
    
    vector<int> test_dec;
    
    for (int i = polynom->getDim()+1; i <= n; ++i)
    {
        if(i % polynom->getDim() != 0)
        {
            test_dec.push_back(i);
        }
    }
    
    Polynom* test_poly = new Polynom(polynom->getDim(), vector<int>(at->getDegree()));
   
    unsigned idx = 0;
    
    while (idx < test_dec.size())
    {
        int tmp = test_dec[idx];
        
        LOG_TRACE
        
        test_poly->getRefCoef().resize(at->getDegree());
        
        for (int i = 0; i < polynom->getDegree(); ++i)
        {
            assert(i < test_poly->getRefCoef().size());
            test_poly->getRefCoef()[i] = tmp % polynom->getDim();
            tmp = tmp / polynom->getDim();
        }
        
        while(!test_poly->getRefCoef().empty() && !test_poly->getRefCoef()[test_poly->getRefCoef().size()-1]) 
            test_poly->getRefCoef().pop_back();
        
        LOG_TRACE
        at->print(cout);
        test_poly->print(cout);
        Polynom r = ((*at) % *test_poly);
        LOG_TRACE
        r.print(cout);
        
        if(r.getCoef().size() == 0)
        {
    	    polynom->setIrreducible(REDUCIBLE);
    	    break;
		}

		idx++;
    }
    
    delete test_poly;
}

void* PolynomChecker::check(void *arg)
{
    ((PolynomChecker *)arg)->checkPol();
    ((PolynomChecker *)arg)->free();
    pthread_exit(NULL);
}

bool PolynomChecker::isBusy()
{
    return busy;
}

void PolynomChecker::free()
{
    busy = false;
}

PolynomChecker::PolynomChecker()
{
    busy = false;
}

