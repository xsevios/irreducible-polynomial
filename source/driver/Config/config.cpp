#include "config.h"
#include <iostream>

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