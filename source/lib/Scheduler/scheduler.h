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
#include "../Polynom/polynom_ext.h"

class PolynomChecker;

using namespace std;

///Набор возможных методов проверки многочленов на неприводимость
enum Method {
    Matlab              = 1,    ///< Метод "Матлаб"
    Berlekamp           = 2,    ///< Метод Берлекэмпа
    CantorZassenhaus    = 3,    ///< Cantor–Zassenhaus algorithm
};

///Класс "планировщик" организует проверку многочленов на неприводимость в заданное число потоков
class Scheduler
{
    list<PolynomExt*> polynoms;///< Список многочленов
    unsigned numThreads;///< Количество потоков
    Method method;///< Метод проверки многочленов на неприводимость
    
public:
    Scheduler(list<PolynomExt*>, unsigned, Method);
    unsigned countBusy(PolynomChecker* pCheck);
    virtual ~Scheduler();
    virtual void start();
};

#endif