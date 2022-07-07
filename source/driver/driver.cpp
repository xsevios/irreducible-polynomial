/**
* \file
* \brief Файл с реализацией методов класса Driver
*/


#include <fstream>
#include <cstdlib>
#include <cassert>
#include "LibraryInterface/LibraryInterface.h"
#include "Config/config.h"
#include "driver.h"

using namespace std;
/**
 * Перегрузка оператора << для вывода полинома в формате
 * "ПРИЗНАК_ПРИВОДИМОСТИ РАЗМЕРНОСТЬ_ПОЛЯ КОЭФФИЦИЕНТ КОЭФФИЦИЕНТ ... КОЭФФИЦИЕНТ"
 */
ostream& operator<<(ostream& out, const Polynom& p)
{
    out << p.isIrreducible() << " " << p.getDim();
    
    vector<int> coef = p.getCoef();
    for(unsigned i = 0; i < coef.size(); i++)
        out << " " << coef[i];

    return out;
}
/**
 * \brief Конструктор класса
 * \param[in] libPath путь к библиотеки
 * \param[in] confPath путь к конфигурационному файлу
*/
Driver::Driver(string libPath, string confPath) : libPath(libPath), confPath(confPath) {}
/**
 * \brief Деструктор класса
 * Освобождает память контейнера list<Polinom>
 */ 
Driver::~Driver()
{
    if (scheduler != nullptr)
        lib->destroyScheduler(scheduler);

    if (conf != nullptr)
        delete conf;

    for (list<Polynom*>::iterator cur = polynoms.begin(); cur != polynoms.end(); cur = polynoms.begin())
    {
        lib->destroyPolynom(*cur);
        polynoms.erase(cur);
    }
}

/**
 * \brief Загружает библиотеку по средствам класса LibraryInterface
 */
void Driver::loadLibrary()
{
    lib = new LibraryInterface(libPath);
    
    if(!(*lib))
    {
        std::cerr << lib->GetErrorMessage();
        exit(ERROR_LIB);
    }
}

/**
 * \brief Считывает параметры конфигурационного файла
 */
void Driver::readConfig()
{
    conf = new Config(confPath);
    
    if(!(*conf))
    {
        std::cerr << conf->GetErrorMessage();
        exit(ERROR_READ_CONF);
    }
}

/**
 * \brief Считывает список полиномов из текстового файла для обработки
 */
void Driver::readPolynoms()
{
    ifstream file(conf->getInFileName().c_str());
    
    if(file.is_open())
    {
        string strPolynom;
        while (getline(file, strPolynom))
        {
            Polynom* polynom = lib->createPolynom(strPolynom);
            polynoms.push_back(polynom);
        }
        file.close();
    }
    else
    {
        std::cerr << "File with polynoms not found or not available.\n\t" << "Path:" << conf->getInFileName().c_str() << std::endl;
        exit(ERROR_READ_POLY);
    }
}

/**
 * \brief Создает объект библиотечного класса Scheduler
 * Вызывает конструктор класса Scheduler, передавая ему необходимые параметры,
 * (список полиномов для проверки; количесво доступных ядер; флаг, маскирующий 
 *  используемый для проверки метод) по средствам механизмам класса LibraryInterface
 */
void Driver::initScheduler()
{
    scheduler = lib->createScheduler(polynoms, conf->getNumThread(), conf->getMethod());
}

void Driver::startScheduler()
{
    scheduler->start();
}

/**
 * Пишет полиномы в текстовый файл(путь к которому задается в конфигурационном
 * файле)в формате
 * "ПРИЗНАК_ПРИВОДИМОСТИ РАЗМЕРНОСТЬ_ПОЛЯ КОЭФФИЦИЕНТ КОЭФФИЦИЕНТ ... КОЭФФИЦИЕНТ"
 */
void Driver::writePolynoms()
{
    ofstream fout(conf->getOutFileName().c_str());
    
    if(!fout.is_open())
    {
        std::cerr << "File isn't available.\n\t" << "Path:" << conf->getOutFileName().c_str() << std::endl;
        exit(ERROR_WRITE_POLY);
    }
    
    for(list<Polynom*>::iterator i = polynoms.begin(); i != polynoms.end(); i++)
            fout << *(*i) << endl;
        
    fout.close();
}
