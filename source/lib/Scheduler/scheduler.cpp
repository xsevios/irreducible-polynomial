#include "scheduler.h"
#include <pthread.h>
#include <iostream>
#include "../Polynom/checkIrreducible.h"

extern PolynomChecker polynomChecker;

extern "C" Scheduler* create_scheduler(list<Polynom*> polynoms, int numThread)
{
  return new Scheduler(polynoms, numThread);
}

extern "C" void destroy_scheduler(Scheduler* object)
{
    delete object;
}

Scheduler::Scheduler(list<Polynom*> p, int numThread)
{
    numThreads=numThread;
    //numProc = 0;
    polynoms = p;
    //pthread_mutex_init(&mutex, NULL);
    //mutex = PTHREAD_MUTEX_INITIALIZER; 
}

Scheduler::~Scheduler()
{
    
}

struct PolynomStruct
{
    Polynom* p;
    int flag;
};

void* Scheduler::main_func(void *arg)
{
    PolynomStruct* name = (PolynomStruct*) arg;
    polynomChecker(name->p);
    
    //pthread_mutex_lock(&mutex);
    //pthread_mutex_unlock(&mutex);
    
    name->flag = 0;
    pthread_exit(NULL);
}

void Scheduler::start()
{
    int id=0, numProc = 0;
    //pthread_mutex_init(&mutex, NULL);
    pthread_t Threads[numThreads];
    PolynomStruct freeThread[numThreads];
    
    for (list<Polynom*>::iterator i = polynoms.begin(), j = polynoms.end(); i != j; ++i)
    {
        while (numProc >= numThreads);
        
        //pthread_mutex_lock(&mutex);
        numProc++;
        //pthread_mutex_unlock(&mutex);
        
        while (!freeThread[id].flag)
            id++;
        
        freeThread[id].flag = 1;
        freeThread[id].p = *(i);
        pthread_create(&Threads[id], NULL, main_func, &freeThread[id]);
        id = 0;
    }
    
    //for (int i=numThreads-1; i>=0; --i)
    //    pthread_join(Threads[i], NULL);
}