#include "config.h"
#include <iostream>
#include <fstream>
#include <string>

/**
 Произдится считывание данных из файла, который лежит по пути path, и приравнивает их переменным
 \param[in] path Путь к файлу
 */
Config::Config(string path)
{
    try 
    {
        ifstream f_o(path.c_str()); 
        string str, substr; 
	    int pr;             
	    
	    while(getline(f_o,str))
	    {
	        if (str[0]=='#') 
	            continue;
	         
	        pr=str.find("="); 
		    substr=str.substr(0,pr-1); 
		    
		    if (substr=="numThread")
			{
				substr=str.substr(pr+2); 
				numThread = atoi(substr.c_str()); 
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
		    else
		    {
		    	continue;
		    }
	    }
    }
    catch(...) ///< в случае ошибок устанавливаем стандартные значения
    {
        numThread = 1;
        fileIn = "in";
        fileOut = "out";
        method = 2;
    }
}

/// Деструктор класса Config
Config::~Config()
{
}


Config::operator bool() const
{
    return true;
}

/**
Возвращает сообщение об ошибке
\return Сообщение об ошибке
*/
string Config::GetErrorMessage()
{
    return errorMessage;
}
/**
Возвращает значение количества ядер процессора
\return Количество ядер процессора
*/
int Config::getNumThread()
{
    return numThread;
}
/**
Возвращает значение пути входного файла
\return Входной файл
*/
string Config::getInFileName()
{
    return fileIn;
}
/**
Возвращает значение пути выходного файла
\return Выходной файл
*/
string Config::getOutFileName()
{
    return fileOut;
}
/**
Возвращает значение метода
\return Значение метода
*/
Method Config::getMethod()
{
    return method;
}