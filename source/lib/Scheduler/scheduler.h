#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <list>
#include <pthread.h>
#include "../Polynom/polynom.h"

class PolynomChecker;

using namespace std;

enum Method {
    Matlab      = 1,
    Berlekamp   = 2
};

class Scheduler
{
    list<Polynom*> polynoms;
    unsigned numThreads;
    Method method;
public:
    Scheduler(list<Polynom*>, unsigned, Method);
    unsigned countBusy(PolynomChecker* pCheck);
    virtual ~Scheduler();
    virtual void start();
};

#endif