/*
 *
 * FILE: Matrix.cpp
 *
 * AUTHOR: Ian Ewell
 *
 * PROJECT: Mythreal Engine
 *
 * MODULE: Math
 *
 * PURSPOSE: Implement various plane classes and functions.
 *
 * CREATED ON: 04/30/11
 *
 * COPYRIGHT: (C) Copyright 2011 Mythreal Studios. All rights reserved.
 *
 */

#ifndef MATH_PLANE_H
#define MATH_PLANE_H

#include "Vector.h"

class CMPlane : public CMVector3
{
private:
    F32 d;
    
public:
    CMPlane();
    CMPlane(const CMVector3  &normal, const CMVector3 &point);
    
    void set(const CMVector3 &normal, const CMVector3 &point);
    F32  distance(const CMVector3 &point) const;
    bool intercectRay(const CMVector3 &pos, const CMVector3 &dir, CMVector3 *intercection) const;
    
    F32 getD() const
    {
        return d;
    }
};

#endif