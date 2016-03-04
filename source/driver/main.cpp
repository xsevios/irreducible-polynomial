#include <iostream>
#include "LibraryInterface/LibraryInterface.h"

int main()
{
    LibraryInterface lib("libirredpoly.so");
    
    if(!lib)
    {
        std::cerr << lib.GetErrorMessage();
        return 1;
    }
    
    lib.SomeFunction();
    
    return 0;
}