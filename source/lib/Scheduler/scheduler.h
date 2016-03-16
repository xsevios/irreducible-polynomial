#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <stdio.h>
#include <iostream>
#include <list>
#include "../Polynom/polynom.h"
#include "../Polynom/checkIrreducible.h"

using namespace std;

class Scheduler
{
    int numThread;
public:
    Scheduler(list<Polynom*>, int numThread);
    virtual ~Scheduler();
    virtual void start();
};

#endif