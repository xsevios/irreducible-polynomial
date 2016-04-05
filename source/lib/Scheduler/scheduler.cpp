#include "scheduler.h"
#include <pthread.h>
#include <iostream>
#include <assert.h>
#include "../Polynom/checkIrreducible.h"

extern PolynomChecker polynomChecker;

extern "C" Scheduler* create_scheduler(list<Polynom*> polynoms, int numThread, Method method)
{
  return new Scheduler(polynoms, numThread, method);
}

extern "C" void destroy_scheduler(Scheduler* object)
{
    delete object;
}

Scheduler::Scheduler(list<Polynom*> p, unsigned numThread, Method method) : polynoms(p), numThreads(numThread), method(method) { }

Scheduler::~Scheduler() { }

unsigned Scheduler::countBusy(PolynomChecker* pCheck)
{
    int count = 0;
    
    for(unsigned i = 0; i < numThreads; i++)
        if(pCheck[i].isBusy())
            count++;
    
    return count;
}

void Scheduler::start()
{
    pthread_t Threads[numThreads];
    PolynomChecker* pCheck = new PolynomChecker[numThreads];
    
    for(unsigned j = 0; j < numThreads; j++)
        pCheck[j].init(method);
    
    for (list<Polynom*>::iterator i = polynoms.begin(), j = polynoms.end(); i != j; ++i)
    { 
        while (countBusy(pCheck) >= numThreads);

        for(unsigned j = 0; j < numThreads; j++)
            if(!pCheck[j].isBusy())
            {
                pCheck[j].setPoly(*i);
                pthread_create(&Threads[j], NULL, &PolynomChecker::check, &pCheck[j]);
                break;
            }
    }
    
    for(unsigned j = 0; j < numThreads && j < polynoms.size() ; j++)
        pthread_join(Threads[j], NULL);
}
