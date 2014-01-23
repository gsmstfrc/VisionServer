/*
 *
 * FILE: Vector.cpp
 *
 * AUTHOR: Ian Ewell
 *
 * PROJECT: Mythreal Engine
 *
 * MODULE: Math
 *
 * PURSPOSE: Implement various vector classes and functions.
 *
 * CREATED ON: 05/10/10
 *
 * COPYRIGHT: (C) Copyright 2010 Mythreal Studios. All rights reserved.
 *
 */

#include <math.h>
#include "Vector.h"

CMVector2::CMVector2()
{
	mX = 0;
	mY = 0;
}

CMVector2::CMVector2(F32 x, F32 y)
{
	mX = x;
	mY = y;
}

void CMVector2::normalize()
{
    // Prevent division by zero
    if (mX == 0 && mY == 0)
        return;
    
	F32 f = 1.0f / (F32)sqrt((F32)mX*(F32)mX + (F32)mY*(F32)mY);
	mX *= f;
	mY *= f;
}

void CMVector2::add(CMVector2 &vec)
{
    mX += vec.mX;
    mY += vec.mY;
}

void CMVector2::subtract(CMVector2 &vec)
{
    mX -= vec.mX;
    mY -= vec.mY;
}

void CMVector2::multiplyScalar(F32 s)
{
    mX *= s;
    mY *= s;
}

void CMVector2::divideScalar(F32 s)
{
    mX /= s;
    mY /= s;
}

F32 CMVector2::length()
{
    return sqrtf(mX*mX + mY*mY);
}

//////////////////////////////////////////////////////

CMVector3::CMVector3()
{
	mX = 0;
	mY = 0;
	mZ = 0;
}

CMVector3::CMVector3(F32 x, F32 y, F32 z)
{
	mX = x;
	mY = y;
	mZ = z;
}

void CMVector3::normalize()
{
    if (mX == 0.0f && mY == 0.0f && mZ == 0.0f)
        return;
    
	F32 f = 1.0f / (F32)sqrt((F32)mX*(F32)mX + (F32)mY*(F32)mY + (F32)mZ*(F32)mZ);
	mX *= f;
	mY *= f;
	mZ *= f;
}

void CMVector3::add(const CMVector3 &vec)
{
    mX += vec.mX;
    mY += vec.mY;
    mZ += vec.mZ;
}

void CMVector3::subtract(const CMVector3 &vec)
{
    mX -= vec.mX;
    mY -= vec.mY;
    mZ -= vec.mZ;
}

void CMVector3::multiplyScalar(F32 s)
{
    mX *= s;
    mY *= s;
    mZ *= s;
}

void CMVector3::divideScalar(F32 s)
{
    mX /= s;
    mY /= s;
    mZ /= s;
}

F32 CMVector3::length() const
{
    return sqrtf(mX*mX + mY*mY + mZ*mZ);
}

F32 CMVector3::distanceTo(const CMVector3 &other) const
{
    return sqrtf((other.mX-mX)*(other.mX-mX) +
                 (other.mY-mY)*(other.mY-mY) +
                 (other.mZ-mZ)*(other.mZ-mZ));
}

F32 CMVector3::dot(const CMVector3 &vec) const
{
    return mX*vec.mX + mY*vec.mY + mZ*vec.mZ;
}

CMVector3 CMVector3::cross(const CMVector3 &vec) const
{
    return CMVector3(mY*vec.mZ - mZ*vec.mY,
                     mZ*vec.mX - mX*vec.mZ,
                     mX*vec.mY - mY*vec.mX);
}

const void CMVector3::translateGL()
{
}

//////////////////////////////////////////////////////

CMVector4::CMVector4()
{
	mX = 0;
	mY = 0;
	mZ = 0;
    mW = 1;
}

CMVector4::CMVector4(F32 x, F32 y, F32 z, F32 w)
{
	mX = x;
	mY = y;
	mZ = z;
    mW = w;
}

CMVector4::CMVector4(const CMVector3 &vec)
{
    mX = vec.getX();
    mY = vec.getY();
    mZ = vec.getZ();
    mW = 1;
}

void CMVector4::normalize()
{
	F32 f = 1.0f / (F32)sqrt((F32)mX*(F32)mX + (F32)mY*(F32)mY + (F32)mZ*(F32)mZ);
	mX *= f;
	mY *= f;
	mZ *= f;
}

void CMVector4::add(CMVector4 &vec)
{
    mX += vec.mX;
    mY += vec.mY;
    mZ += vec.mZ;
}

void CMVector4::subtract(CMVector4 &vec)
{
    mX -= vec.mX;
    mY -= vec.mY;
    mZ -= vec.mZ;
}

void CMVector4::multiplyScalar(F32 s)
{
    mX *= s;
    mY *= s;
    mZ *= s;
}

void CMVector4::divideScalar(F32 s)
{
    mX /= s;
    mY /= s;
    mZ /= s;
}
