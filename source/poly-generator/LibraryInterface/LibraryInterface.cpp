/**
\file
\brief Файл с реализацием методов класса LibraryInterface

*/


#include "LibraryInterface.h"

#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>

using std::string;

/**
\brief Конструктор класса.

Загружает динамическую библиотеку, и сопоставляет методам класса LibraryInterface
соответсвующие методы библиотеки libirredpoly.so.
\param[in] libraryName название библиотеки
*/
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
        return;
    }
    
    createPolynom = (Polynom* (*)(string))dlsym(handle, "create_polynom");
    destroyPolynom = (void (*)(Polynom*))dlsym(handle, "destroy_polynom");
    
    createScheduler = (Scheduler* (*)(list<Polynom*>, int, Method))dlsym(handle, "create_scheduler");
    destroyScheduler = (void (*)(Scheduler*))dlsym(handle, "destroy_scheduler");
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

/**
Возвращает строку с возникшими в процессе подключения ошибками
\return строка, содержащая сообщения об ошибках
*/
string LibraryInterface::GetErrorMessage()
{
    return errorMessage;    
}