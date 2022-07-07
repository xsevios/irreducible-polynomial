/**
\file
\brief Заголовочный файл класса Tester

Данный файл содержит в себе определение основного класса оболочки библиотеки
*/


#ifndef _Tester_H
#define _Tester_H

#include "LibraryInterface/LibraryInterface.h"

using namespace std;
 
enum ErrorCode
{
    ERROR_LIB = 1,
    ERROR_READ_CONF,
    ERROR_READ_POLY,
    ERROR_WRITE_POLY
};

ostream& operator<<(ostream& out, const Polynom& p);
/**
 * \brief Основной класс оболочки библиотеки
 */ 
class Tester
{
    LibraryInterface  * lib         = nullptr;
    string              libPath;                ///<Путь к библиотеки, требующей подключения

public:
    Tester(string libPath);
    ~Tester();
    
    void loadLibrary();
    void runTest();
};

#endif
