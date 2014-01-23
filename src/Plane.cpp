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


#include "Plane.h"

CMPlane::CMPlane()
{
    d = 0;
}

CMPlane::CMPlane(const CMVector3  &normal, const CMVector3 &point)
{
    set(normal, point);
}

void CMPlane::set(const CMVector3  &normal, const CMVector3 &point)
{
    mX = normal.getX();
    mY = normal.getY();
    mZ = normal.getZ();
    d = -(mX*point.getX() + mY*point.getY() + mZ*point.getZ());
}

F32  CMPlane::distance(const CMVector3 &point) const
{
    return dot(point) + d;
}

bool CMPlane::intercectRay(const CMVector3 &pos, const CMVector3 &dir, CMVector3 *intercection) const
{
    F32 dotr = dot(dir);
    if (dotr == 0)
        return false;
    
    *intercection = pos;
    CMVector3 v = dir;
    v.multiplyScalar(distance(pos)/dotr);
    intercection->subtract(v);
    return true;
}