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

class Driver
{
    LibraryInterface* lib;
    Config* conf;
    Scheduler* scheduler;
    list<Polynom*> polynoms;
    string libPath;
    string confPath;
    
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
