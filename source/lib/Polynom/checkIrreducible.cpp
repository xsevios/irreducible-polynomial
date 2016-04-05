#include "checkIrreducible.h"
#include <assert.h>

//#define DEBUG

#ifdef DEBUG
#define LOG_TRACE std::cout << "Entering " << __FUNCTION__ << "() - (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;
#else
#define LOG_TRACE
#endif

void PolynomChecker::setPoly(Polynom* polynom, pthread_mutex_t* mutex, pthread_cond_t* cond)
{
    this->polynom = polynom;
    this->mutex = mutex;
    this->cond = cond;
    busy = true;
}

void PolynomChecker::init(Method method)
{
    this->method = method;
}

void PolynomChecker::matlab()
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
        
        test_poly->getRefCoef().resize(at->getDegree());
        
        for (unsigned i = 0; i < polynom->getDegree(); ++i)
        {
            assert(i < test_poly->getRefCoef().size());
            test_poly->getRefCoef()[i] = tmp % polynom->getDim();
            tmp = tmp / polynom->getDim();
        }
        
        while(!test_poly->getRefCoef().empty() && !test_poly->getRefCoef()[test_poly->getRefCoef().size()-1]) 
            test_poly->getRefCoef().pop_back();
        
        Polynom r = ((*at) % *test_poly);
        
        if(r.getCoef().size() == 0)
        {
    	    polynom->setIrreducible(REDUCIBLE);
    	    break;
		}

		idx++;
    }
    
    delete test_poly;
}

// Алгоритм Евклида для многочленов
Polynom gcd(Polynom a, Polynom b)
{
	if (a.isZero())
		return b;
	Polynom d = gcd(b%a, a);
	return d;
}

// Расширенный алгоритм Евклида для чисел
int gcdex(int a, int b, int& x, int& y);

void PolynomChecker::berlekamp()
{
    Polynom* test_poly = new Polynom(polynom->getDim(), polynom->getCoef());
    vector<int> tmp = test_poly->getCoef();
    *test_poly = *test_poly / tmp[test_poly->getDegree()];
	Polynom polyDer = test_poly->derivative();
	if (!polyDer.isZero())
	{
		Polynom firstCheck = gcd(*test_poly, polyDer);
		if (!firstCheck.getDegree())
		{
			PolynomState answer = checkMatrix(*test_poly);
			polynom->setIrreducible(answer);
		}
		else
			polynom->setIrreducible(REDUCIBLE);
	}
	else
		polynom->setIrreducible(REDUCIBLE);
	delete test_poly;
}

vector<vector<int>> PolynomChecker::generateMatrix(const Polynom& p)
{
    int matrixSize = p.getDegree();
	int tempDeg = 0;
	int step = p.getDim();
	vector<int> row;
	vector<vector<int>> resMatrix;
	
	for (;tempDeg < matrixSize; tempDeg += step)
	{
		for(int i = 0; i < matrixSize; i++)
			row.push_back(i == tempDeg);
		resMatrix.push_back(row);
		for(int i = 0; i < matrixSize; i++)
		row.clear();
	}
	
	for (int i = 0; i < matrixSize; i++)
		row.push_back(0);
	row.push_back(1);
	Polynom tempPoly(step, row);
	row.clear();
	
	tempPoly %= p;
	
	vector<int> referenceRow;
	for (int i = 0; i < matrixSize; i++)
		referenceRow.push_back(tempPoly[i]);
	
	row = referenceRow;
	
	if (tempDeg == matrixSize)
	{
	    resMatrix.push_back(row);
	    tempDeg += step;
	}
	
	int maxDeg = (matrixSize-1)*step;
	for (int i = matrixSize; i < maxDeg; i++)
	{
	    int tmp = row[matrixSize-1];
		for (int j = matrixSize; j > 0; j--)
			row[j] = row[j-1];
		row[0] = 0;

		for (int j = 0; j < matrixSize; j++)
			row[j] = (row[j] + tmp * referenceRow[j]) % step;
		if (i == tempDeg-1)
		{
			resMatrix.push_back(row);
			
			tempDeg += step;
		}
	}
	
	
	for (int i = 0; i < matrixSize; i++)
	{
		resMatrix[i][i]--;
		if (resMatrix[i][i] < 0)
		    resMatrix[i][i] += step;
	}
	return resMatrix;
}

vector<vector<int>> gauss(vector<vector<int>> matrix, int dimGF)
{
	int coef;

	for (unsigned i = 0; i < matrix.size(); ++i)
	{
		unsigned k = i;
		while (matrix[k++][i] == 0)
			if (k == matrix.size())
				goto end_loop;

		if (--k != i)
			matrix[k].swap(matrix[i]);
		
		for (unsigned j = i + 1; j < matrix.size(); ++j)
		{
		    int inverse = 0;
			int y = 0;
			int tmp = matrix[i][i];
			gcdex(tmp, dimGF, inverse, y);
			inverse = (inverse % dimGF + dimGF) % dimGF;
			for (unsigned k = i; k < matrix.size(); ++k)
			    matrix[i][k] = matrix[i][k] * inverse % dimGF;
			coef = matrix[j][i] * inverse % dimGF;
			for (unsigned k = i; k < matrix.size(); ++k)
			{
				matrix[j][k] = (matrix[j][k] - matrix[i][k] * coef) % dimGF;
				if (matrix[j][k] < 0)
					matrix[j][k] += dimGF;
			}
		}
	end_loop:;
	}

	return matrix;
}

int PolynomChecker::getRank(vector<vector<int>> m, int dimGF)
{
    int rank = 0;
    m = gauss(m, dimGF);
    for (unsigned i = 0; i < m.size(); i++)
        for (unsigned j = 0; j < m.size(); j++)
            if (m[i][j] != 0)
            {
                rank++;
                break;
            }
    
    return rank;
}

PolynomState PolynomChecker::checkMatrix(const Polynom& p)
{
    int rank = getRank(generateMatrix(p), p.getDim());
	return p.getDegree() - rank == 1 ? IRREDUCIBLE : REDUCIBLE;
}

void* PolynomChecker::check(void *arg)
{
    switch(((PolynomChecker *)arg)->method)
    {
        case Matlab:
            ((PolynomChecker *)arg)->matlab();
            break;
        case Berlekamp:
            ((PolynomChecker *)arg)->berlekamp();
            break;
    }
    
    ((PolynomChecker *)arg)->free();
    
    pthread_mutex_lock(((PolynomChecker *)arg)->mutex);
    pthread_cond_signal(((PolynomChecker *)arg)->cond);
    pthread_mutex_unlock(((PolynomChecker *)arg)->mutex);
    
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

