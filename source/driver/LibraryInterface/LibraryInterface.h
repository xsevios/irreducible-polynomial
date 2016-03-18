#ifndef _LIBRARYINTERFACE_H
#define _LIBRARYINTERFACE_H

#include <iostream>
#include <string>
#include <stdio.h>
#include "../../lib/lib.h"

using namespace std;

class LibraryInterface
{
    void* handle;
    string errorMessage;
    char cCurrentPath[FILENAME_MAX];
public:
    Polynom* (*createPolynom)(string strPolynom);
    void (*destroyPolynom)(Polynom*);
    
    Scheduler* (*createScheduler)(list<Polynom*>, int);
    void (*destroyScheduler)(Scheduler*);
    
    LibraryInterface(string libraryName);
    
    operator bool() const;
    string GetErrorMessage();
};

#endif