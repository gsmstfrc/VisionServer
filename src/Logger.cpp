//
//  Logger.cpp
//  mythreal
//
//  Created by Ian Ewell on 10/1/12.
//
//

#include "Logger.h"

void CCLogger::Log(std::string caller, std::string message)
{
    // Should be improved
    std::cout << caller << ":" << message << "\n";
}

void CCLogger::LogError(std::string caller, std::string message)
{
    // Should be improved
    std::cout << caller << ": Error:" << message << "\n";
}