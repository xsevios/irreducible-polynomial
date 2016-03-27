#include "checkIrreducible.h"


void PolynomChecker::init(Polynom* p)
{
    polynom = p;
    busy = true;
}



void PolynomChecker::checkPol(void)
{
    polynom->setIrreducible(IRREDUCIBLE);
    Polynom* at = polynom;
    if (at->getCoef()[0] == 0)
    {
        if (at->getDegree() == 1)
            polynom->setIrreducible(REDUCIBLE);
        else
            polynom->setIrreducible(IRREDUCIBLE);
    }
    else if (at->getDegree() == 0)
        polynom->setIrreducible(IRREDUCIBLE);
    
    else
    {
        int n = pow(polynom->getDim(), (polynom->getDegree()/2)+1)-1;
        Polynom* test_dec = new Polynom(polynom->getDim(), vector<int>());
        for (int i = polynom->getDim()+1; i <= n; ++i)
        {
            if(i % polynom->getDim() != 0)
            {
                test_dec->getCoef().push_back(i);
            }
        }
        Polynom* test_poly = new Polynom(polynom->getDim(), vector<int>(test_dec->getCoef()));

        unsigned idx = 0;
        
        while (idx < test_dec->getCoef().size())
        {
            int tmp = test_dec->getCoef()[idx];
            for (int i = 0; i < polynom->getDegree(); ++i)
            {
                test_poly->getCoef()[i] = fmod(tmp, polynom->getDim());
                tmp = tmp / polynom->getDim();
            }
            Polynom r = *at % *test_poly;
            
            bool id = 0;
			for (unsigned i = 0; i < r.getCoef().size(); ++i)
			{
				if (r.getCoef()[i] != 0)
					id = 1;
			}		
			if (id == 0)
			{
			    polynom->setIrreducible(REDUCIBLE);
				break;
			}
			idx++;
        }
    }
}




void* PolynomChecker::check(void *arg)
{
    ((PolynomChecker *)arg)->checkPol();
    cout << ((PolynomChecker*)arg)->polynom->isIrreducible() << endl;
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

