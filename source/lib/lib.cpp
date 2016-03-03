#include <iostream>

extern "C" void SomeFunction(void)
{
    std::cout << "Test function" << std::endl;
}