// echo.h
#ifndef ECHO_H
#define ECHO_H
#include <iostream>

inline void echo_command(const char *variable)
{
    const char *value = std::getenv(variable);
    if (value == nullptr)
    {
        std::cout << "\n";
    }
    else
    {
        std::cout << value << "\n";
    }
}


#endif