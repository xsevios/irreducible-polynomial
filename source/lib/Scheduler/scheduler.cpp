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
    
    pthread_mutex_t mutex;
    pthread_mutexattr_t attr;
    pthread_cond_t cond;
    
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutexattr_init(&attr);
    pthread_mutex_init(&mutex, &attr);
    pthread_cond_init(&cond, NULL);
    
    pthread_mutex_lock(&mutex);
    for (list<Polynom*>::iterator i = polynoms.begin(), j = polynoms.end(); i != j; ++i)
    {
        while (countBusy(pCheck) >= numThreads)
            pthread_cond_wait(&cond, &mutex);
        
        for(unsigned j = 0; j < numThreads; j++)
        {
            if(!pCheck[j].isBusy())
            {
                pCheck[j].setPoly(*i, &mutex, &cond);
                pthread_create(&Threads[j], NULL, &PolynomChecker::check, &pCheck[j]);
                pthread_detach(Threads[j]);
                break;
            }
        }
        
    }
    
    while (countBusy(pCheck))
        pthread_cond_wait(&cond, &mutex);
    
    pthread_mutex_unlock(&mutex);
}
