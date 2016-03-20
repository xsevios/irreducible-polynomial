#include "checkIrreducible.h"

void PolynomChecker::init(Polynom* p)
{
    polynom = p;
    busy = true;
}

void* PolynomChecker::check(void *arg)
{
    //some work
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