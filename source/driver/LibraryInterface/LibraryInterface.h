#ifndef _LIBRARYINTERFACE_H
#define _LIBRARYINTERFACE_H

#include <iostream>
#include <string>
#include <stdio.h>

using std::string;

class LibraryInterface
{
    void* handle;
    string errorMessage;
    char cCurrentPath[FILENAME_MAX];
public:
    void (*SomeFunction)();
    
    LibraryInterface(string libraryName);
    
    operator bool() const;
    string GetErrorMessage();
};

#endif