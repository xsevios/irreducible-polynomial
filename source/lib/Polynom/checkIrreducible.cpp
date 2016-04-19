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

/// Алгоритм Евклида для многочленов
Polynom gcd(Polynom a, Polynom b)
{
	if (a.isZero())
		return b;
	Polynom d = gcd(b%a, a);
	return d;
}

/// Расширенный алгоритм Евклида для чисел
int gcdex(int a, int b, int& x, int& y);

/// Часть алгоритма Берлекэмпа. Проверяет многочлен на неприводимость
/**
Описание алгоритма:
Дан многочлен f(x) степени n над полем GF(p). Необходимо узнать является ли он неприводимым. 
1. Нормируем многочлен, то есть умножаем его на обратный элемент коэффициента при максимальной степени. 
2. Находим производную f'(x)полученного многочлена. Если производная оказалась равна 0, то многочлен является степенью какого-то другого многочлена, то есть он приводим. В противном случае продолжаем алгоритм. 
3. Вычисляем НОД(f(x), f'(x)). Если НОД оказался многочленом, то, очевидно, f(x) делится на него, то есть f(x) приводим. Если НОД - число, то продолжаем. 
4. Строим матрицу B размером n x n: 
Элементами матрицы B являются коэффициенты многочленов вида x^(ip) по модулю f(x), где i = 0, 1, ... , n-1. Таким образом, первый многочлен - x^0 = 1, то есть первая строка выглядит как [1, 0, ..., 0] (n-1 нулей). Второй многочлен имеет вид x^p mod(f(x)). Если n>p, то это будет строка [0, ..., 0, 1, 0, ..., 0], где 1 стоит на p-м месте. В противном случае находится остаток от деления x^p на f(x). Аналогично находятся все последующие строки B. 
5. Вычитаем из матрицы B единичную матрицу C=B-E. 
6. Находим ранг матрицы C. Если ранг этой матрицы r такой, что n - r = 1, то многочлен неприводим. В противном случае он приводим.
*/
void PolynomChecker::berlekamp()
{
    Polynom* test_poly = new Polynom(polynom->getDim(), polynom->getCoef());
    vector<int> tmp = test_poly->getCoef();
    *test_poly = *test_poly / tmp[test_poly->getDegree()]; ///< Нормировка многочлена
	Polynom polyDer = test_poly->derivative(); ///< polyDer --  производная проверяемого многочлена
	if (!polyDer.isZero())
	{
		Polynom firstCheck = gcd(*test_poly, polyDer); ///< firstCheck -- НОД(f(x), f'(x))
		if (!firstCheck.getDegree())
		{
			PolynomState answer = checkMatrix(*test_poly); ///< checkMatrix составляет матрицу и проверяет её ранг для данного многочлена
			polynom->setIrreducible(answer);
		}
		else
			polynom->setIrreducible(REDUCIBLE);
	}
	else
		polynom->setIrreducible(REDUCIBLE);
	delete test_poly;
}

/// Cоставляет матрицу для данного многочлена
/**
Для того, чтобы избежать большого (близкого к n) количества делений, используется следующий алгоритм:
Сначала в матрицу записываются коэффициенты всех многочленов вида x^(ip), для которых ip < n.
Далее находится x^n mod(f(x)). Пусть он имеет вид a_0 + a_1*x + ... + a_(n - 1)*x^(n - 1).
Тогда многочлен x^(n + 1) mod(f(x)) будет иметь вид a_0*x + a_1*x^2 + ... + a_(n - 1)*x^n.
Однако, x^n mod(f(x)) уже известен, поэтому x^(n + 1) mod(f(x)) имеет вид b_0 + b_1*x + ... + b_(n - 1)*x^(n - 1), где b_0 = a_0*a_(n - 1), b_1 = a_1*a_(n - 1) + a_0, и т.д.
В матрицу записываются коэффициенты многочленов, степень которых равна ip, i = 0, 1, ... , n-1.
*/
vector<vector<int>> PolynomChecker::generateMatrix(const Polynom& p)
{
    int matrixSize = p.getDegree();
	int tempDeg = 0;
	int step = p.getDim();
	vector<int> row;
	vector<vector<int>> resMatrix;
	
	/// Добавление в матрицу строк для многочленов, у которых ip < n.
	for (;tempDeg < matrixSize; tempDeg += step)
	{
		for(int i = 0; i < matrixSize; i++)
			row.push_back(i == tempDeg);
		resMatrix.push_back(row);
		row.clear();
	}
	
	/// Построение многочлена x^n
	for (int i = 0; i < matrixSize; i++)
		row.push_back(0);
	row.push_back(1);
	Polynom tempPoly(step, row);
	row.clear();
	
	tempPoly %= p;
	
	vector<int> referenceRow; ///< Коэффициенты многочлена x^n mod(f(x))
	for (int i = 0; i < matrixSize; i++)
		referenceRow.push_back(tempPoly[i]);
	
	row = referenceRow;
	
	/// Добавление в матрицу строки для многочлена, у которого ip = n
	if (tempDeg == matrixSize)
	{
	    resMatrix.push_back(row);
	    tempDeg += step;
	}
	
	/// Добавление в матрицу строк для многочленов, у которых ip > n.
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
	
	/// Вычитает из полученной матрицы единичную
	for (int i = 0; i < matrixSize; i++)
	{
		resMatrix[i][i]--;
		if (resMatrix[i][i] < 0)
		    resMatrix[i][i] += step;
	}
	return resMatrix;
}

/// Прямой ход метода Гаусса над полем GF(p)
/**
\param[in] matrix Матрица, к которой применяется метод Гаусса
\param[in] dimGF Характеристика поля GF(p)
*/
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


/// Узнаёт ранг матрицы над полем GF(p)
/**
Ранг равен количеству ненулевых строк в матрице после прямого хода метода Гаусса.
\param[in] m Матрица, ранг которой надо узнать
\param[in] dimGF Характеристика поля GF(p)
*/
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

/// Проверяет, является ли ранг матрицы r для данного полинома степени n таким, что n - r = 1.
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

