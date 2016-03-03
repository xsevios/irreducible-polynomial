#include "LibraryInterface.h"

#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>

using std::string;

LibraryInterface::LibraryInterface(string libraryName)
{
    if (!getcwd(cCurrentPath, sizeof(cCurrentPath)))
    {
        errorMessage += (string)"Cannot get run path" + "\n";
    }
    
    string path = cCurrentPath;
    path += "/" + libraryName;
    
    handle = dlopen(path.c_str(), RTLD_LAZY);
    
    if(!handle)
    {
        errorMessage += (string)"Cannot open library: \n\t" + dlerror() + "\n";
    }
    
    SomeFunction = (void (*)())dlsym(handle, "SomeFunction");
}
    
LibraryInterface::operator bool() const
{
    if(handle == NULL)
    {
        return false;
    }
    else
    {
        return true;
    }
}

string LibraryInterface::GetErrorMessage()
{
    return errorMessage;    
}