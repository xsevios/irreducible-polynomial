#include "config.h"
#include <iostream>
#include <string>

Config::Config(string path)
{
    numThread = 1; /* Заглушка */
}

Config::~Config()
{
}


Config::operator bool() const
{
    return true;
}

string Config::GetErrorMessage()
{
    return errorMessage;
}

int Config::getNumThread()
{
    return numThread;
}

string Config::getInFileName()
{
    //return "in";
    return fileIn;
}

string Config::getOutFileName()
{
    //return "out";
    return fileOut;
}