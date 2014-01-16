//
// FRC Vision Server
// FRC 2014
// FRC Team 3318
// Written by Ian Ewell
// (C) 2014 GSMST Robotics
//

#ifndef CORE_TYPES_H
#define CORE_TYPES_H

#include <pthread.h>

//Basic variable typedefs:

//8-bit
typedef unsigned char U8;
typedef signed char   S8;

//16-bit
typedef unsigned short U16;
typedef signed short   S16;

//32-bit
typedef unsigned int U32;
typedef signed int   S32;

//float
typedef float F32;

//double
typedef double F64;

//PI and conversions
#define MPI 3.1415926543
#define D_TO_R(degrees) (MPI*degrees)/180

#endif
