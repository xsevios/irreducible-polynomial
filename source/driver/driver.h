/**
\file
\brief Заголовочный файл класса Driver

Данный файл содержит в себе определение основного класса оболочки библиотеки
*/


#ifndef _DRIVER_H
#define _DRIVER_H

#include "LibraryInterface/LibraryInterface.h"
#include "Config/config.h"

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
 * Основной класс оболочки библиотеки
 */ 
class Driver
{
    LibraryInterface* lib;
    Config* conf;
    Scheduler* scheduler;
    list<Polynom*> polynoms;///<список обробатываемых полиномов
    string libPath;///<Путь к библиотеки, требующей подключения
    string confPath;///<Путь к конфигурационному файлу с основными настройками приложения
    
public:
    Driver(string libPath, string confPath);
    ~Driver();
    
    void loadLibrary();
    void readConfig();
    void initScheduler();
    void startScheduler();
    void readPolynoms();
    void writePolynoms();
};

#endif
