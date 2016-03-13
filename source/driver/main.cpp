#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include "LibraryInterface/LibraryInterface.h"
#include "../lib/Polynom/polynom.h"
#include "../lib/Scheduler/scheduler.h"
#include "Config/config.h"

using namespace std;

enum ErrorCode
{
    ERROR_LIB = 1,
    ERROR_CONF,
    ERROR_POLY
};

int main()
{
    // --- Подключение к библиотеке ---
    
    LibraryInterface lib("libirredpoly.so");
    if(!lib)
    {
        std::cerr << lib.GetErrorMessage();
        return ERROR_LIB;
    }
    
    //------------------------------------------
    
    // --- Чтение конфигурационного файла ---
    
    string pathToCfg = "";
    Config cfg(pathToCfg);
    if(!cfg)
    {
        std::cerr << cfg.GetErrorMessage();
        return ERROR_CONF;
    }
    // ---
    
    //--- Чтение файла с многочленами ---
    
    string pathToPol = "in";
    ifstream file(pathToPol.c_str());
    list<Polynom*> polynoms;
    string strPolynom;
    if(file.is_open())
    {
        while (getline(file, strPolynom))
        {
            Polynom* polynom = lib.createPolynom(strPolynom);
            polynoms.push_back(polynom);
        }
        file.close();
    }
    else
    {
        std::cerr << "File with polynoms not found or not available.\n\t" << "Path:" << pathToPol << std::endl;
        return ERROR_POLY;
    }
    
    //---
    
    
    //--- Планировщик ---
    
    Scheduler* schel = lib.createScheduler(&polynoms, cfg.getNumThread());
    schel->start();
    
    //---
    
    //--- WRITE RESULT IN FILE ---
    
    string pathToOut = "out";
    ofstream fout(pathToOut.c_str());
    if(fout.is_open())
    {
        for(list<Polynom*>::iterator i = polynoms.begin(); i != polynoms.end(); i++)
        {
            fout << (*i)->isIrreducible() << " " << (*i)->getDim();
            for(vector<double>::iterator j = (*i)->getCoef().begin(); j != (*i)->getCoef().end(); j++)
            {
                fout << " " << (*j);
            }
            fout << endl;
        }
        
        fout.close();
    }
    else
    {
        std::cerr << "File isn't available.\n\t" << "Path:" << pathToPol << std::endl;
        return ERROR_POLY;
    }
    
    //---
    
    
    //---FREE MEMORY---
    lib.destroyScheduler(schel);
    list<Polynom*>::iterator cur = polynoms.begin();
    while (cur != polynoms.end())
    {
        lib.destroyPolynom(*cur);
        polynoms.erase(cur);
        cur = polynoms.begin();
    }
    //---
    return 0;
}