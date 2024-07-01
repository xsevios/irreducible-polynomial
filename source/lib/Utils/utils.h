#ifndef _UTILS_H
#define _UTILS_H

#include <cstdint>

#include "../BigInt/bigint.h"


/** Перечесление описывающее приводимость полинома
 * NEED_CHECK - требует проверки полинома(состояние по умолчанию)
 * REDUCIBLE - полином является приводимым
 * IRREDUCIBLE - полином не прифодим
*/
enum PolynomState
{
    NEED_CHECK  = -1,
    REDUCIBLE   = 0,
    IRREDUCIBLE = 1
};

template<class T>
inline bool checkBit(T num, uint64_t pos)
{
    return ((num) & ((uint64_t)1 << (pos)));
}

template<class T>
inline int msb(T x)
{
    int num = 0;

    while (x >>= 1)
    {
        num++;
    }

    return num;
}

template<>
inline bool checkBit<bigint>(bigint num, uint64_t pos)
{
    for (int i = 0; i < pos; i++)
    {
        num /= 2;
    }

    return num.z[0] % 2;
}

template<>
inline int msb<bigint>(bigint x)
{
    int num = 0;

    while (!(x /= 2).isZero())
    {
        num++;
    }

    return num;
}

template<class T>
bool isPrime(T n)
{
    T n_sqrt = sqrt(n);

    if (n_sqrt * n_sqrt == n)
        return false;

    if (n % 2 == 0 && n != 2)
        return false;

    for (T i = 3; i <= n_sqrt; i += 2)
    {
        if (n % i == 0)
        {
            return false;
        }
    }

    return true;
}

template<class T>
int ipow(T base, T exp)
{
    T result = 1;
    for (;;)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        if (!exp)
            break;
        base *= base;
    }

    return result;
}

#endif
