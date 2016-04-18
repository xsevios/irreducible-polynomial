/**
\file
\brief Заголовчный файл с описанием класса "планировщик"

Данный файл содержит в себе определение класса "планировщик", его полей и методов.
Класс планировщик отвечает за распараллеливание алгорнитма проверки многочлена на неприводимость
*/

#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <list>
#include <pthread.h>
#include "../Polynom/polynom.h"

class PolynomChecker;

using namespace std;

///Набор возможных методов проверки многочленов на неприводимость
enum Method {
    Matlab      = 1,///< Метод "Матлаб"
    Berlekamp   = 2///< Метода Берликэмпа
};

///Класс "планировщик" организует проверку многочленов на неприводимость в заданное число потоков
class Scheduler
{
    list<Polynom*> polynoms;///< Список многочленов
    unsigned numThreads;///< Количество потоков
    Method method;///< Метод проверки многочленов на неприводимость
    
public:
    Scheduler(list<Polynom*>, unsigned, Method);
    unsigned countBusy(PolynomChecker* pCheck);
    virtual ~Scheduler();
    virtual void start();
};

#endif