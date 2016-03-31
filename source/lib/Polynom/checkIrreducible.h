#ifndef _CHECK_IRREDUCIBLE_H
#define _CHECK_IRREDUCIBLE_H

#include "polynom.h"

class PolynomChecker
{
    Polynom* polynom;
    pthread_mutex_t* mutex;
    pthread_cond_t* cond;
    
    bool busy;
    
public:
    PolynomChecker();
    void init(Polynom* polynom, pthread_mutex_t* mutex, pthread_cond_t* cond);
    
    bool isBusy();
    void free();
    
    static void* check(void *arg);
    void checkPol(void);
};

#endif