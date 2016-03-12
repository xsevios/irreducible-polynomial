#ifndef _CONFIG_H
#define _CONFIG_H

#include <string>

using namespace std;

class Config
{
    int numThread;
    string errorMessage;
public:
    Config(string path);
    ~Config();
    
    operator bool() const;
    string GetErrorMessage();
    
    int getNumThread();
};

#endif