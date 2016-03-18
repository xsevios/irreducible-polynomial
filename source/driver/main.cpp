#include "driver.h"

using namespace std;

int main()
{
    Driver driver("libirredpoly.so", "workSpace.cfg");
    
    driver.loadLibrary();
    driver.readConfig();
    driver.readPolynoms();
    driver.initScheduler();
    driver.startScheduler();
    driver.writePolynoms();
    
    return 0;
}