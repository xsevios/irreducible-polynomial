#include <fstream>
#include <cstdlib>
#include "LibraryInterface/LibraryInterface.h"
#include "Config/config.h"
#include "driver.h"

using namespace std;

ostream& operator<<(ostream& out, const Polynom& p)
{
    out << p.isIrreducible() << " " << p.getDim();
    
    vector<int> coef = p.getCoef();
    for(unsigned i = 0; i < coef.size(); i++)
        out << " " << coef[i];

    return out;
}

Driver::Driver(string libPath, string confPath) : libPath(libPath), confPath(confPath) {}

Driver::~Driver()
{
        lib->destroyScheduler(scheduler);
        delete conf;
        for (list<Polynom*>::iterator cur = polynoms.begin(); cur != polynoms.end(); cur = polynoms.begin())
        {
            lib->destroyPolynom(*cur);
            polynoms.erase(cur);
        }
}

void Driver::loadLibrary()
{
    lib = new LibraryInterface(libPath);
    
    if(!(*lib))
    {
        std::cerr << lib->GetErrorMessage();
        exit(ERROR_LIB);
    }
}

void Driver::readConfig()
{
    conf = new Config(confPath);
    
    if(!(*conf))
    {
        std::cerr << conf->GetErrorMessage();
        exit(ERROR_READ_CONF);
    }
}

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

void Driver::initScheduler()
{
    scheduler = lib->createScheduler(polynoms, conf->getNumThread());
}

void Driver::startScheduler()
{
    scheduler->start();
}

void Driver::writePolynoms()
{
    ofstream fout(conf->getOutFileName().c_str());
    
    if(fout.is_open())
    {
        for(list<Polynom*>::iterator i = polynoms.begin(); i != polynoms.end(); i++)
        {
            //fout << (*i)->isIrreducible() << " " << (*i)->getDim();
            //for(vector<double>::iterator j = (*i)->getCoef().begin(); j != (*i)->getCoef().end(); j++)
            //{
            //    fout << " " << (*j);
            //}
            //fout << endl;
            fout << (*i) << endl;
        }
        
        fout.close();
    }
    else
    {
        std::cerr << "File isn't available.\n\t" << "Path:" << conf->getOutFileName().c_str() << std::endl;
        exit(ERROR_WRITE_POLY);
    }
}