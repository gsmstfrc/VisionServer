//
//  Logger.h
//  mythreal
//
//  Created by Ian Ewell on 10/1/12.
//
//

#ifndef __mythreal__Logger__
#define __mythreal__Logger__

#include <iostream>
#include <string>

class CCLogger
{
public:
    static void Log(std::string caller, std::string message);
    static void LogError(std::string caller, std::string message);
};

#endif /* defined(__mythreal__Logger__) */
