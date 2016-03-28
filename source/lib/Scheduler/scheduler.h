#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <list>
#include <pthread.h>
#include "../Polynom/polynom.h"

class PolynomChecker;

using namespace std;

class Scheduler
{
    list<Polynom*> polynoms;
    unsigned numThreads;
public:
    Scheduler(list<Polynom*>, unsigned);
    unsigned countBusy(PolynomChecker* pCheck);
    virtual ~Scheduler();
    virtual void start();
};

#endif