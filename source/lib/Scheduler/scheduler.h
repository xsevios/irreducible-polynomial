#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <list>
#include <pthread.h>
#include "../Polynom/polynom.h"

using namespace std;

class Scheduler
{
    unsigned int numThreads;
    volatile static unsigned int numProc;
    list<Polynom*> polynoms;
    static pthread_mutex_t mutex;
public:
    Scheduler(list<Polynom*>, int numThread);
    virtual ~Scheduler();
    virtual void start();
    static void* main_func(void *arg);
};

#endif