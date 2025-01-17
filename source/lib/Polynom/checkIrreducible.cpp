#include "checkIrreducible.h"

#include <cassert>
#include <random>
#include <algorithm>

//#define DEBUG

#ifdef DEBUG
#define LOG_TRACE std::cout << "Entering " << __FUNCTION__ << "() - (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;
#else
#define LOG_TRACE
#endif

void PolynomChecker::setPoly(PolynomExt* polynom, pthread_mutex_t* mutex, pthread_cond_t* cond)
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

/**
 Проверяет, является ли многочлен из класса PolynomExt неприводимым на нерасширенном поле Галуа. 
\return polynom->setIrreducible присваивается значение REDUCIBLE или IRREDUCIBLE, что соответствует приводимости или неприводимости многочлена.
 */
void PolynomChecker::matlab()
{
    polynom->setIrreducible(IRREDUCIBLE);
    PolynomExt* at = polynom;
    
    if (at->GetDegree() == 0)
    {
        polynom->setIrreducible(IRREDUCIBLE);
        return;
    }
    else if (at->GetCoef()[0] == 0)
    {
        if (at->GetDegree() == 1)
            polynom->setIrreducible(IRREDUCIBLE);
        else
            polynom->setIrreducible(REDUCIBLE);
            
        return;
    }
    int n = pow(polynom->GetPrime(), polynom->GetDegree() / 2 + 1) - 1;
    
    vector<int> test_dec;
    
    for (int i = polynom->GetPrime() + 1; i <= n; ++i)
    {
        if(i % polynom->GetPrime() != 0)
        {
            test_dec.push_back(i);
        }
    }
    
    PolynomExt* test_poly = new PolynomExt(polynom->GetField(), vector<int>(at->GetDegree()));
   
    unsigned idx = 0;
    
    while (idx < test_dec.size())
    {
        int tmp = test_dec[idx];
        
        test_poly->getRefCoef().resize(at->GetDegree());
        
        for (unsigned i = 0; i < polynom->GetDegree(); ++i)
        {
            assert(i < test_poly->getRefCoef().size());
            test_poly->getRefCoef()[i] = tmp % polynom->GetPrime();
            tmp = tmp / polynom->GetPrime();
        }
        
        while(!test_poly->getRefCoef().empty() && !test_poly->getRefCoef()[test_poly->getRefCoef().size()-1]) 
            test_poly->getRefCoef().pop_back();
        
        PolynomExt r = ((*at) % *test_poly);
        
        if(r.GetCoef().size() == 0)
        {
    	    polynom->setIrreducible(REDUCIBLE);
    	    break;
		}

		idx++;
    }
    
    delete test_poly;
}

/// Алгоритм Евклида для многочленов
PolynomExt gcd(const PolynomExt& a, const PolynomExt& b)
{
    if (b.IsZero())
    {
        if (a.GetField()->IsPrimeField())
            return a * a.GetField()->GetMultInverse(a.GetLeadingCoef());
        else
            return a * a.GetField()->GetMultInverse(a.GetLeadingExtCoef());
    }

    PolynomExt d = gcd(b, a % b);
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
    BerlekampTest(*polynom);
}

PolynomState PolynomChecker::BerlekampTest(const PolynomExt& f)
{
    vector<int> tmp = f.GetCoef();
    PolynomExt pTmp = f;
    pTmp = pTmp / pTmp.GetLeadingCoef();               ///< Нормировка многочлена
    PolynomExt polyDer = pTmp.Derivative();               ///< polyDer --  производная проверяемого многочлена
    if (!polyDer.IsZero())
    {
        PolynomExt firstCheck = gcd(pTmp, polyDer);       ///< firstCheck -- НОД(f(x), f'(x))
        if (!firstCheck.GetDegree())
        {
            PolynomState answer = checkMatrix(pTmp);   ///< checkMatrix составляет матрицу и проверяет её ранг для данного многочлена
            f.setIrreducible(answer);
        }
        else
            f.setIrreducible(REDUCIBLE);
    }
    else
        f.setIrreducible(REDUCIBLE);

    return f.isIrreducible();
}

/// Составляет матрицу для данного многочлена
/**
Для того, чтобы избежать большого (близкого к n) количества делений, используется следующий алгоритм:
Сначала в матрицу записываются коэффициенты всех многочленов вида x^(ip), для которых ip < n.
Далее находится x^n mod(f(x)). Пусть он имеет вид a_0 + a_1*x + ... + a_(n - 1)*x^(n - 1).
Тогда многочлен x^(n + 1) mod(f(x)) будет иметь вид a_0*x + a_1*x^2 + ... + a_(n - 1)*x^n.
Однако, x^n mod(f(x)) уже известен, поэтому x^(n + 1) mod(f(x)) имеет вид b_0 + b_1*x + ... + b_(n - 1)*x^(n - 1), где b_0 = a_0*a_(n - 1), b_1 = a_1*a_(n - 1) + a_0, и т.д.
В матрицу записываются коэффициенты многочленов, степень которых равна ip, i = 0, 1, ... , n-1.
*/
vector<vector<int>> PolynomChecker::generateMatrix(const PolynomExt& p)
{
    int matrixSize = p.GetDegree();
	int tempDeg = 0;
	int step = p.GetPrime();
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
	PolynomExt tempPoly(FieldExt::GetInstance(step), row);
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

    unsigned eliminated = 0;
	for (unsigned i = 0; i < matrix.size(); ++i)
	{
        unsigned eliminateFrom = i - eliminated;
        unsigned k = eliminateFrom;
        bool stop = false;

		while (matrix[k][i] == 0)
        {
            if (k + 1 == matrix.size())
            {
                eliminated++;
                stop = true;
                break;
            }

            k++;
        }

        if (stop)
            continue;

		if (k != eliminateFrom)
			matrix[k].swap(matrix[eliminateFrom]);

        int inverse = 0;
        int y = 0;
        int tmp = matrix[eliminateFrom][i];

        gcdex(tmp, dimGF, inverse, y);
        inverse = (inverse % dimGF + dimGF) % dimGF;
        for (unsigned k = i; k < matrix.size(); ++k)
            matrix[eliminateFrom][k] = matrix[eliminateFrom][k] * inverse % dimGF;

        for (unsigned j = eliminateFrom + 1; j < matrix.size(); ++j)
		{
			coef = matrix[j][i];
			for (unsigned k = i; k < matrix[j].size(); ++k)
			{
				matrix[j][k] = (matrix[j][k] - matrix[eliminateFrom][k] * coef) % dimGF;
				if (matrix[j][k] < 0)
					matrix[j][k] += dimGF;
			}
		}
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
    {
        for (unsigned j = 0; j < m.size(); j++)
        {
            if (m[i][j] != 0)
            {
                rank++;
                break;
            }
        }
    }

    return rank;
}

/// Проверяет, является ли ранг матрицы r для данного полинома степени n таким, что n - r = 1.
PolynomState PolynomChecker::checkMatrix(const PolynomExt& p)
{
    int rank = getRank(generateMatrix(p), p.GetPrime());
	return p.GetDegree() - rank == 1 ? IRREDUCIBLE : REDUCIBLE;
}

/// General Irreducibility Test Algorithm based in Cantor-Zassenhaus Factorization Method
void PolynomChecker::cantorzassenhaus()
{
    CantorZassenhausTest(*polynom);
}

void* PolynomChecker::check(void *arg)
{
    auto polyChecker = static_cast<PolynomChecker *>(arg);

    switch(polyChecker->method)
    {
        case Matlab:
            polyChecker->matlab();
            break;
        case Berlekamp:
            polyChecker->berlekamp();
            break;
        case CantorZassenhaus:
            polyChecker->cantorzassenhaus();
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

///  Проверяет, имеет ли полином кратные множители
bool PolynomChecker::isSquareFree(const PolynomExt &f)
{
    PolynomExt c = gcd(f, f.Derivative());
    return c.GetDegree() == 0;
}

PolynomState PolynomChecker::CantorZassenhausTest(const PolynomExt& f)
{
    if (!isSquareFree(f))
    {
        f.setIrreducible(REDUCIBLE);
        return REDUCIBLE;
    }

    PolynomExt x(f.GetField(), std::vector<int>{0, 1});
    int q = f.GetDim();
    int n = f.GetDegree();

    for (int r = (n / 4) + 1; r <= n / 2; r++)
    {
        PolynomExt sqr = x.BinExp(q, r, f);
        sqr -= x;
        sqr %= f;
        PolynomExt g = gcd(f, sqr);

        if (g != 1)
        {
            f.setIrreducible(REDUCIBLE);
            return REDUCIBLE;
        }
    }

    f.setIrreducible(IRREDUCIBLE);
    return IRREDUCIBLE;
}

Factors PolynomChecker::CantorZassenhausFactorization(const PolynomExt& f)
{
    Factors factors;

    // Factors and the number (g, s) in which this factors should be powered in order to get f(x)
    // i.e. f(x) = Product of g_i(x) ^ s_i
    Factors sffFactors = SquareFreeFactorization(f);
    if (sffFactors.size() > 1 || (!sffFactors.empty() && sffFactors.begin()->first != 1))
    {
        for (auto it = sffFactors.begin(); it != sffFactors.end(); it++)
        {
            auto completeFactors = CantorZassenhausFactorization(it->second);
            for (auto & factor : completeFactors)
                factors.insert({it->first, factor.second});
        }

        return factors;
    }

    // Each pair (g, r) represents a polynomial g(x) which is the product of deg(g)/r distinct irreducibles of degree r
    factors = DistinctDegreeFactorization(f);
    if (factors.empty())
    {
        factors.insert({f.GetDegree(), f});
    }

    factors = EqualDegreeFactorization(factors);
    return factors;
}

Factors PolynomChecker::KaltofenShoupFactorization(const PolynomExt& f)
{
    Factors factors;

    // Factors and the number (g, s) in which this factors should be powered in order to get f(x)
    // i.e. f(x) = Product of g_i(x) ^ s_i
    Factors sffFactors = SquareFreeFactorization(f);
    if (sffFactors.size() > 1 || (!sffFactors.empty() && sffFactors.begin()->first != 1))
    {
        for (auto it = sffFactors.begin(); it != sffFactors.end(); it++)
        {
            auto completeFactors = KaltofenShoupFactorization(it->second);
            for (auto & factor : completeFactors)
                factors.insert({it->first, factor.second});
        }

        return factors;
    }

    // Each pair (g, r) represents a polynomial g(x) which is the product of deg(g)/r distinct irreducibles of degree r
    factors = DistinctDegreeShoupFactorization(f);
    if (factors.empty())
    {
        factors.insert({f.GetDegree(), f});
    }

    factors = EqualDegreeFactorization(factors);
    return factors;
}

/**
 Performs the Kaltofen-Shoup irreducibility test on a given polynomial.

 This method verifies whether the input polynomial is irreducible over its associated field
 using the Kaltofen-Shoup algorithm, which involves checks for square-freeness and distinct-degree
 factorization.

 @param f The polynomial to be tested for irreducibility.
 @return The irreducibility state of the polynomial:
 - IRREDUCIBLE: If the polynomial is proven to be irreducible.
 - REDUCIBLE: If the polynomial is proven to be reducible.
 */
PolynomState PolynomChecker::KaltofenShoupTest(const PolynomExt& f)
{
    if (f.GetDegree() <= 1)
    {
        f.setIrreducible(IRREDUCIBLE);
        return IRREDUCIBLE;
    }

    Factors factors;

    if (!isSquareFree(f))
    {
        f.setIrreducible(REDUCIBLE);
        return REDUCIBLE;
    }

    // Each pair (g, r) represents a polynomial g(x) which is the product of deg(g)/r distinct irreducibles of degree r
    factors = DistinctDegreeShoupFactorization(f, true);
    if (factors.empty())
    {
        factors.insert({f.GetDegree(), f});
    }

    if (factors.size() == 1 && factors.begin()->first == f.GetDegree())
    {
        f.setIrreducible(IRREDUCIBLE);
        return IRREDUCIBLE;
    }

    f.setIrreducible(REDUCIBLE);
    return REDUCIBLE;
}

///  Square free factorization - разложение на кратные множители
/**
\param[in] p Полином
\return std::vector<PolynomExt> Кратные множители
*/
Factors PolynomChecker::SquareFreeFactorization(const PolynomExt &p)
{
    Factors R;

    PolynomExt c = gcd(p, p.Derivative());
    PolynomExt w = p / c;

    for (int degree = 1; w != 1; degree++)
    {
        PolynomExt y = gcd(w, c);
        PolynomExt fac = w / y;

        if (fac != 1)
            R.insert({degree, fac});

        w = y;
        c = c / y;
    }

    if (c != 1)
    {
        PolynomExt cTmp = c;
        int p = cTmp.GetDim();
        for (int i = cTmp.GetDegree(); i > 1; i--)
        {
            if (cTmp[i] != 0)
            {
                c[i] = 0;
                c[i / p] = cTmp[i];
            }
        }
        c.RemoveLeadingZeros();

        Factors RTmp = SquareFreeFactorization(c);

        for (auto it = RTmp.begin(); it != RTmp.end(); it++)
        {
            R.insert({it->first * p, it->second});
        }
    }

    return R;
}

/// Distinct degree factorization - факторизация различными степенями
/**
\param[in] f Исходный полином
\return std::vector<PolynomExt> Множители каждой степени от 1 до deg(f)
*/
Factors PolynomChecker::DistinctDegreeFactorization(const PolynomExt &p)
{
    Factors factors;
    PolynomExt f   = p;
    auto q      = f.GetDim();
    auto degree = f.GetDegree();

    for (int r = 1; r < degree; r++)
    {
        PolynomExt x(f.GetField(), std::vector<int>{0, 1});
        PolynomExt sqr = x.BinExp(q, r, f);
        PolynomExt h = (sqr - x) % f;
        PolynomExt g = gcd(h, f);

        while (g != 1)
        {
            factors.insert({r, g});
            f /= g;
            g = gcd(h, f);
        }
    }

    return factors;
}

/**
 * @brief Optimizes the parameter p for the Brent-Kung algorithm to minimize computation cost.
 *
 * This function calculates the optimal value of the parameter p, which is used in the Brent-Kung
 * parallel prefix algorithm to divide tasks into subproblems. The goal is to minimize the total
 * computational cost based on the provided parameters depth, reductionCost, and propagationCost.
 *
 * The computational cost function is defined as:
 *     cost = propagationCost * (p - 1) + reductionCost * floor((depth - 1) / p),
 * where:
 * - depth: The depth of computation or the total number of steps to perform.
 * - reductionCost: The cost of reduction operations (combining intermediate results).
 * - propagationCost: The cost of propagation operations (spreading results to dependent tasks).
 *
 * The function iterates over all possible values of p (from 2 to depth) and finds the value of p
 * that minimizes the cost function.
 *
 * @param depth The depth of the computation (total steps).
 * @param reductionCost The cost of reduction operations.
 * @param propagationCost The cost of propagation operations.
 * @return The optimal value of p that minimizes the computational cost.
 *
 * @example
 * // Example usage
 * long depth = 8, reductionCost = 10, propagationCost = 5;
 * long optimal_p = BrentKungOptPow(depth, reductionCost, propagationCost);
 * std::cout << "Optimal p: " << optimal_p << std::endl;
 */
long BrentKungOptPow(long depth, long reductionCost, long propagationCost) {
    long optimalP = 1;
    long minCost = reductionCost * (depth - 1); // Initial cost for p = 1

    for (long currentP = 2; currentP <= depth; ++currentP) {
        long currentCost = propagationCost * (currentP - 1) + reductionCost * ((depth - 1) / currentP);
        if (currentCost < minCost) {
            optimalP = currentP;
            minCost = currentCost;
        }
    }
    return optimalP;
}

/**
 * @brief Finds the position of the most significant set bit in a number.
 *
 * Returns the zero-based index of the most significant set bit (1) in the binary representation
 * of an unsigned long integer. If the input is 0, returns -1.
 *
 * @param number The input number to check.
 * @return Zero-based index of the most significant set bit, or -1 if no bits are set.
 */
int FindMostSignificantSetBit(unsigned long number) {
    if (number == 0) return -1; // No set bits
    return static_cast<int>(std::log2(number)); // Floor of log2(number)
}

/**
 * @brief Performs distinct-degree factorization of a polynomial.
 *
 * This method implements the Shoup distinct-degree factorization algorithm for decomposing
 * a given polynomial over a finite field into factors of distinct degrees.
 *
 * @param f The input polynomial to be factorized.
 * @param test A flag indicating whether to check for irreducibility or to make complete factorization.
 * @return A structure of type `Factors`, containing the distinct-degree factors of the polynomial.
 *
 * The algorithm works by finding and separating irreducible factors of the input polynomial, grouped by their degrees.
 * It is particularly efficient for finite field polynomials.
 *
 */
Factors PolynomChecker::DistinctDegreeShoupFactorization(const PolynomExt &f, bool test)
{
    auto p = f.GetPrime();
    auto n = f.GetDegree();
    Factors factors;

    if (n < 2)
    {
        factors.insert({n, f});
        return factors;
    }

    auto B = n / 2;
    auto l = static_cast<uint32_t>(sqrt(B));
    auto m = (B + l - 1) / l;

    std::vector<PolynomExt> h;
    std::vector<PolynomExt> H;
    std::vector<PolynomExt> I;

    h.emplace_back(f.GetField(), std::vector<int>{0, 1});

    // Step 1
    h.push_back(h[0].BinExp(p, f));

    // Step 2
    auto bo = BrentKungOptPow(n, l-1, 1);
    auto ro = l <= 1 ? 0 : (bo - 1) / (l - 1) + ((n - 1) / bo);
    if (FindMostSignificantSetBit(p) <= ro)
    {
        for (int i = 2; i <= l; i++)
            h.push_back(h[i - 1].BinExp(p, f));
    }
    else
    {
        auto xq = hiPowers(h[1], bo,  f, p);

        for (int i = 3; i <= l+1; i++)
            h.push_back(DDFShoupEval(h[i - 2], xq, f, p));
    }

    // Step 3, 4
    auto xq = hiPowers(h[l], BrentKungOptPow(n, m-1, 1),  f, p);
    for (int j = 1; j <= m; j++)
    {
        // // Step 3
        if (j == 1)
            H.push_back(xq[1]);
        else
            H.push_back(DDFShoupEval(H[j - 2], xq, f, p));
    
        // Step 4
        I.push_back((H[j - 1] - h[0]) % f);
        for (int i = 1; i <= l - 1; i++)
        {
            I[j - 1] *= (H[j - 1] - h[i]) % f;
        }
    }

    // Step 5
    auto one = PolynomExt(f.GetField(), std::vector<int>{1});
    auto f_ = f;
    for (int j = 1; j <= m; j++)
    {
        auto g = gcd(f_, I[j - 1]);

        if (g.GetDegree())
            f_ = f_ / g;

        for (int i = l - 1; i >= 0; i--)
        {
            auto u = gcd(g, H[j - 1] - h[i]);

            if (u.GetDegree())
            {
                factors.insert({l * j - i, u});

                if (test)
                {
                    return factors;
                }

                g = g / u;
            }

            if (!g.GetDegree())
                break;
        }
    }

    if (f_ != 1)
        factors.insert({f_.GetDegree(), f_});

    return factors;
}

std::vector<PolynomExt> PolynomChecker::hiPowers(const PolynomExt& XP, long bo, const PolynomExt& T, long p)
{
    int useSqr = 2 * XP.GetDegree() >= T.GetDegree();
    std::vector<PolynomExt> V;

    auto one = T.GetField()->GetOne();
    V.push_back(one);

    if (bo == 0)
        return V;

    V.push_back(XP);

    if (bo == 1)
        return V;

    V.push_back(XP.BinExp(2, T));

    if (useSqr)
    {
        for (int i = 4; i < bo + 2; i++)
        {
            if (i & 1)
                V.push_back((V[((i + 1) >> 1) - 1]).BinExp(2, T));
            else
                V.push_back(V[i - 2] * XP % T);
        }
    }
    else
    {
        for (int i = 4; i < bo + 2; i++)
        {
            V.push_back(V[i - 2] * XP % T);
        }
    }

    return V;
}

std::vector<std::vector<int>> multiplyMatrices(
    const std::vector<std::vector<int>>& A,
    const std::vector<std::vector<int>>& B,
    const int prime) {

    int n = A.size();             // Rows in A
    int m = A[0].size();          // Columns in A (or rows in B)
    int p = B[0].size();          // Columns in B

    std::vector<std::vector<int>> C(n, std::vector<int>(p, 0));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < p; ++j) {
            for (int k = 0; k < m; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
            C[i][j] %= prime;
        }
    }

    return C;
}

PolynomExt PolynomChecker::DDFShoupEval(const PolynomExt& Q, const std::vector<PolynomExt>& x, const PolynomExt& T, long p)
{
    long m = T.GetDegree();

    long l = x.size(), lQ = Q.GetDegree() + 1, n,  d;

    if (lQ == 0)
        return T.GetField()->GetZero();

    if (lQ <= l)
    {
        n = l;
        d = 1;
    }
    else
    {
        n = l - 1;
        d = (lQ + n - 1) / n;
    }

    std::vector A(m, std::vector<int>(n));
    std::vector B(n, std::vector<int>(d));

    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            A[i][j] = x[j].getRefCoef().size() > i ? x[j][i] : 0;

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < d; j++)
        {
            const size_t id = i + j * n;
            B[i][j] = Q.getRefCoef().size() > id ?  Q[id] : 0;
        }
    }

    auto C = multiplyMatrices(A, B, p);

    const PolynomExt& g = x[l - 1];
    auto Tn = T / T.GetLeadingCoef();

    std::vector<int> coeffS;
    for (int i = 0; i < m; i++)
        coeffS.push_back(C[i][d - 1]);

    PolynomExt S(Q.GetField(), coeffS);
    S.RemoveLeadingZeros();

    for (int i = d - 1; i > 0; i--)
    {
        std::vector<int> coeffS;
        for (int j = 0; j < m; j++)
            coeffS.push_back(C[j][i - 1]);

        PolynomExt Ci(Q.GetField(), coeffS);
        Ci.RemoveLeadingZeros();

        S = S * g % Tn + Ci;
    }

    return S;
}

Factors PolynomChecker::EqualDegreeFactorization(Factors factors)
{
    while (true)
    {
        for (auto it = factors.begin(); it != factors.end(); it++)
        {
            if (it->second.GetDegree() > it->first)
            {
                Factors edfFactors = EqualDegreeFactorization(it->second, it->first);

                if (!edfFactors.empty())
                {
                    factors.erase(it);
                    factors.merge(edfFactors);
                    it = factors.begin();
                }
            }
        }

        bool complete = true;
        for (auto & factor : factors)
        {
            if (factor.first != factor.second.GetDegree())
            {
                complete = false;
                break;
            }
        }

        if (complete)
            break;
    }

    return factors;
}


Factors PolynomChecker::EqualDegreeFactorization(const PolynomExt &p, int d)
{
    Factors factors;

    auto one = PolynomExt(p.GetField(), std::vector<int>{1});
    int c = (powl(p.GetDim(), d) - 1) / 2;
    int v = p.GetField()->GetDegree();

    PolynomExt h = GenRandPolynom(p);
    PolynomExt g(p.GetField(), std::vector<int>{});

    if (p.GetPrime() == 2)
    {
        for (int j = 0; j <= v * d - 1; j++)
        {
            g += h.BinExp(2, j, p);
        }
    }
    else
    {
        g = gcd(p, h);
    }

    if (p.GetPrime() != 2 && g == 1)
    {
        g = (h.Exp(c) - one) % p;
    }

    PolynomExt pgGcd = gcd(p, g);
    if (pgGcd != 1 && pgGcd != p)
    {
        factors.insert({d, pgGcd});
        factors.insert({d, p / pgGcd});
    }

    return factors;
}

PolynomExt PolynomChecker::GenRandPolynom(const PolynomExt& p)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> distrib(0, p.GetPrime() - 1);

    PolynomExt pRes(p.GetField(), std::vector<int>{});

    while (pRes.GetDegree() == 0)
    {
        for (int i = 0; i < p.GetCoef().size() - 1; i++)
        {
            pRes.getRefCoef().push_back(distrib(gen) % p.GetPrime());
        }

        pRes.RemoveLeadingZeros();
    }

    return pRes;
}

PolynomState PolynomChecker::RabinsTest(const PolynomExt &f)
{
    if (!isSquareFree(f))
    {
        f.setIrreducible(REDUCIBLE);
        return REDUCIBLE;
    }

    int n = f.GetDegree();

    auto primeDivisors = GetPrimeDivisors(f.GetDegree(), true);
    std::reverse(primeDivisors.begin(), primeDivisors.end());

    std::vector<int> divisors;
    std::transform(primeDivisors.begin(), primeDivisors.end(),
                   std::back_inserter(divisors),
                   [n](const int& elem) { return n / elem; });

    PolynomExt x(f.GetField(), vector<int>{ 0, 1 });
    for (const auto& divisor : divisors)
    {
        PolynomExt sqr = x.BinExp(f.GetDim(), divisor, f);
        auto diff = sqr - x;
        PolynomExt h = diff % f;
        PolynomExt g = gcd(f, h);
        if (g != 1)
        {
            f.setIrreducible(REDUCIBLE);
            return REDUCIBLE;
        }
    }

    PolynomExt sqr = x.BinExp(f.GetDim(), f.GetDegree(), f);
    PolynomExt g = (sqr - x) % f;

    if (g != 0)
    {
        f.setIrreducible(REDUCIBLE);
        return REDUCIBLE;
    }

    f.setIrreducible(IRREDUCIBLE);
    return IRREDUCIBLE;
}

std::vector<int> PolynomChecker::GetPrimeDivisors(int n, bool distinct)
{
    std::vector<int> divisors;

    int k = 2;
    while (k <= n)
    {
        if (n % k != 0)
        {
            k++;
        }
        else
        {
            divisors.push_back(k);
            n /= k;
        }
    }

    if (distinct)
    {
        auto last = std::unique(divisors.begin(), divisors.end());
        divisors.erase(last, divisors.end());
    }

    return divisors;
}

