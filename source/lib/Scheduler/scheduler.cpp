/**
\file
\brief Файл содержащий описание методов класса "планировщик"

Данный файл содержит определние конструктора класса "планировщик", 
метода подсчёта занятых потоков 
и метода распараллеливания алгоритма проверки многочлена на неприводимость
*/

#include "scheduler.h"
#include <pthread.h>
#include <iostream>
#include <assert.h>
#include "../Polynom/checkIrreducible.h"

extern PolynomChecker polynomChecker;

extern "C" Scheduler* create_scheduler(list<Polynom*> polynoms, int numThread, Method method)
{
    return new Scheduler(polynoms, numThread, method);
}

extern "C" void destroy_scheduler(Scheduler* object)
{
    delete object;
}

///Конструктор класса "планировщик"
Scheduler::Scheduler(list<Polynom*> p, unsigned numThread, Method method) : polynoms(p), numThreads(numThread), method(method) { }

///Деструктор класса "планировщик"
Scheduler::~Scheduler() { }

/**
Возвращает количество обрабатываемых в данный момент многочленов
\param[out] pCheck многочлены подлежащие проверке
\return количество проверяемых в данный момент многочленов
*/
unsigned Scheduler::countBusy(PolynomChecker* pCheck)
{
    int count = 0;
    
    for(unsigned i = 0; i < numThreads; i++)
        if(pCheck[i].isBusy())
            count++;
    
    return count;
}

/**
Распараллеливает проверку многочленов на неприводимость на указанное число потоков
*/
void Scheduler::start()
{
    pthread_t Threads[numThreads];///< Потоки
    PolynomChecker* pCheck = new PolynomChecker[numThreads];///< Многочлены на проверку
    
    for(unsigned j = 0; j < numThreads; j++)
        pCheck[j].init(method);
    
    pthread_mutex_t mutex;
    pthread_mutexattr_t attr;
    pthread_cond_t cond;
    
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutexattr_init(&attr);
    pthread_mutex_init(&mutex, &attr);
    pthread_cond_init(&cond, NULL);
    
    pthread_mutex_lock(&mutex);
    for (list<Polynom*>::iterator i = polynoms.begin(), j = polynoms.end(); i != j; ++i)
    {
        while (countBusy(pCheck) >= numThreads)///Проверяем, есть ли свободные потоки
            pthread_cond_wait(&cond, &mutex);///ждём изменения числа занятых потоков
        
        for(unsigned j = 0; j < numThreads; j++)///Проходим по всем потокам
        {
            if(!pCheck[j].isBusy())///находим первый не занятый поток
            {
                pCheck[j].setPoly(*i, &mutex, &cond);///Определяем многочлен на проверку
                pthread_create(&Threads[j], NULL, &PolynomChecker::check, &pCheck[j]);///Создаём поток для проверки многочлена
                pthread_detach(Threads[j]);///Отсоединеняем поток
                break;
            }
        }
        
    }
    
    while (countBusy(pCheck))///Ждём завершения всех созданных потоков
        pthread_cond_wait(&cond, &mutex);
    
    pthread_mutex_unlock(&mutex);
}
