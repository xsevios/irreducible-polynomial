/**
\file
\brief Заголовочный файл класса LibraryInterface

Данный файл содержит в себе определение класса, обеспечивающего вызов функций 
динамической библиотеки
*/

#ifndef _LIBRARYINTERFACE_H
#define _LIBRARYINTERFACE_H

#include <iostream>
#include <string>
#include <stdio.h>
#include "../../lib/lib.h"

using namespace std;
///Класс, предназначенный для подключения динамической бибилиотеки к приложению
/**
 * вызов функций динамической библиотеки по срествам dlfcn.h
 */
class LibraryInterface
{
    void* handle;///<описатель подключения
    string errorMessage;///<строка аккумулирующая ошибки
    char cCurrentPath[FILENAME_MAX];///<путь к текущей, на момент запуска приложения, дериктории
public:
    /**
    Создает объект библиотечного класса Polynom, вызвая функцию библиотеки create_polynom
    \return Указатель на созданный объект
    \param[in] strPolynom строка, содержащая информацию о полиноме
    */ 
    Polynom* (*createPolynom)(string strPolynom);
    void (*destroyPolynom)(Polynom*);
    
    /**
    Создает объект библиотечного класса Scheduler, вызвая функцию библиотеки create_scheduler
    \return Указатель на созданный объект
    */ 
    Scheduler* (*createScheduler)(list<Polynom*>, int, Method);
    void (*destroyScheduler)(Scheduler*);
    
    LibraryInterface(string libraryName);
    
    operator bool() const;
    string GetErrorMessage();
};

#endif