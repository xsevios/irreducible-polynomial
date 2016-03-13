#include "scheduler.h"
#include <iostream>

extern "C" Scheduler* create_scheduler(list<Polynom*>* polynoms, int numThread)
{
  return new Scheduler(polynoms, numThread);
}

extern "C" void destroy_scheduler(Scheduler* object)
{
    delete object;
}

Scheduler::Scheduler(list<Polynom*>* polynoms, int numThread)
{
    
}

Scheduler::~Scheduler()
{
}

void Scheduler::start()
{
    
}

