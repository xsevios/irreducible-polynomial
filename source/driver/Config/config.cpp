#include "config.h"
#include <iostream>
#include <fstream>
#include <string>

Config::Config(string path)
{
    try // пробуем загрузить данные из файла
    {
        ifstream f_o(path.c_str()); // открываем поток 
        string str, substr; // str-считываемая строка из файла, substr-сочетание, которое ищется в строке str
	    int pr;             // позиция найденного вхождения
	    
	    while(getline(f_o,str)) // считываение строк и приравние каждой по очереди str
	    {
	        if (str[0]=='#') // проверяем на наличие комментария
	            continue;
	         
	        pr=str.find("="); // ищем начало вхождения "="
		    substr=str.substr(0,pr-1); // считываем слово, находящееся до "=" 
		    
		    if (substr=="numThread") // проверяем, что нашли 
			{
				substr=str.substr(pr+2); // выделяем строку, являющуюся значением 
				numThread = atoi(substr.c_str()); // преобразовываем в int
			}
			else if (substr=="pathToSourse") 
			{
			    substr=str.substr(pr+2); 
				fileIn = substr;
			}
			else if (substr=="pathToResult") 
			{
			    substr=str.substr(pr+2); 
				fileOut = substr;
		    }
		    else if (substr=="method")
		    {
		    	substr=str.substr(pr+2);
		    	method = (Method)atoi(substr.c_str());
		    }
		    else // считываемое значение не определено или не требуется в нашем случае
		    {
		    	continue;
		    }

	    }
    }
    catch(...) // в случае ошибок устанавливаем стандартные значения
    {
        numThread = 1;
        fileIn = "in";
        fileOut = "out";
    }
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
    return fileIn;
}

string Config::getOutFileName()
{
    return fileOut;
}

Method Config::getMethod()
{
    return method;
}