#ifndef _CONFIG_H
#define _CONFIG_H

#include <string>
#include "../../lib/lib.h"

using namespace std;

class Config
{
    int numThread;
    Method method;
    string fileIn;
    string fileOut;
    string errorMessage;
public:
    Config(string path);
    ~Config();
    
    operator bool() const;
    string GetErrorMessage();
    
    int getNumThread();
    Method getMethod();
    string getInFileName();
    string getOutFileName();
};

#endif