/*
 *
 * FILE: Vector.h
 *
 * AUTHOR: Ian Ewell
 *
 * PROJECT: Mythreal Engine
 *
 * MODULE: Math
 *
 * PURSPOSE: Declare various vector classes and functions.
 *
 * CREATED ON: 05/10/10
 *
 * COPYRIGHT: (C) Copyright 2010 Mythreal Studios. All rights reserved.
 *
 */

#ifndef MATH_VECTOR_H
#define MATH_VECTOR_H

//Includes:
#include "Types.h"
#include <cmath>

#define FLOAT_EQUAL(x, y) (fabs(x-y) <= 0.00001)

class CMVector2
{
protected:
	F32 mX, mY;
	
public:
	CMVector2();
	CMVector2(F32 x, F32 y);
	
	void normalize();
    
    void add(CMVector2 &vec);
    void subtract(CMVector2 &vec);
    void multiplyScalar(F32 s);
    void divideScalar(F32 s);
    F32 length();
	
	inline F32 getX() const
	{
		return mX;
	}
	
	inline F32 getY() const
	{
		return mY;
	}
	
	inline void setX(F32 x)
	{
		mX = x;
	}
	
	inline void setY(F32 y)
	{
		mY = y;
	}
    
    inline CMVector2 operator+(F32 add)
    {
        return CMVector2(mX+add,mY);
    }
    
    inline void operator+=(F32 add)
    {
        mX += add;
    }
    
    inline CMVector2 operator-(F32 add)
    {
        return CMVector2(mX-add,mY);
    }
    
    inline void operator-=(F32 add)
    {
        mX -= add;
    }
    
    inline CMVector2 operator+(const CMVector2 &add)
    {
        return CMVector2(mX+add.mX,mY+add.mY);
    }
    
    inline void operator+=(const CMVector2 &add)
    {
        mX += add.mX;
        mY += add.mY;
    }
    
    inline CMVector2 operator-(const CMVector2 &add)
    {
        return CMVector2(mX-add.mX,mY-add.mY);
    }
    
    inline void operator-=(const CMVector2 &add)
    {
        mX -= add.mX;
        mY -= add.mY;
    }
    
    inline CMVector2 operator*(F32 scalar)
    {
        return CMVector2(mX*scalar, mY*scalar);
    }
    
    inline void operator*=(F32 scalar)
    {
        mX *= scalar;
        mY *= scalar;
    }
    
    inline bool operator==(const CMVector2& comp)
    {
        return FLOAT_EQUAL(mX, comp.mX) && FLOAT_EQUAL(mY, comp.mY);
    }
    
    inline bool operator!=(const CMVector2& comp)
    {
        return !FLOAT_EQUAL(mX, comp.mX) || !FLOAT_EQUAL(mY, comp.mY);
    }
};

class CMVector3
{
protected:
	F32 mX, mY, mZ;
	
public:
	CMVector3();
	CMVector3(F32 x, F32 y, F32 z);
	
	void normalize();
    
    void add(const CMVector3 &vec);
    void subtract(const CMVector3 &vec);
    void multiplyScalar(F32 s);
    void divideScalar(F32 s);
    
    F32 length() const;
    F32 distanceTo(const CMVector3 &other) const;
    
    F32 dot(const CMVector3 &vec) const;
    CMVector3 cross(const CMVector3 &vec) const;
    
    // Multiplies the current GL matrix by the vector.
    const void translateGL();
	
    inline F32 getX() const
	{
		return mX;
	}
	
	inline F32 getY() const
	{
		return mY;
	}
	
	inline F32 getZ() const
	{
		return mZ;
	}
	
	inline void setX(F32 x)
	{
		mX = x;
	}
	
	inline void setY(F32 y)
	{
		mY = y;
	}
	
	inline void setZ(F32 z)
	{
		mZ = z;
	}
    
    inline CMVector3 operator+(F32 add)
    {
        return CMVector3(mX+add,mY,mZ);
    }
    
    inline void operator+=(F32 add)
    {
        mX += add;
    }
    
    inline CMVector3 operator-(F32 add)
    {
        return CMVector3(mX-add,mY,mZ);
    }
    
    inline void operator-=(F32 add)
    {
        mX -= add;
    }
    
    inline CMVector3 operator+(const CMVector2 &add)
    {
        return CMVector3(mX+add.getX(),mY+add.getY(), mZ);
    }
    
    inline void operator+=(const CMVector2 &add)
    {
        mX += add.getX();
        mY += add.getY();
    }
    
    inline CMVector3 operator-(const CMVector2 &add)
    {
        return CMVector3(mX-add.getX(),mY-add.getY(), mZ);
    }
    
    inline void operator-=(const CMVector2 &add)
    {
        mX -= add.getX();
        mY -= add.getY();
    }
    
    inline CMVector3 operator+(const CMVector3 &add)
    {
        return CMVector3(mX+add.getX(),mY+add.getY(), mZ+add.getZ());
    }
    
    inline void operator+=(const CMVector3 &add)
    {
        mX += add.getX();
        mY += add.getY();
        mZ += add.getZ();
    }
    
    inline CMVector3 operator-(const CMVector3 &add)
    {
        return CMVector3(mX-add.getX(),mY-add.getY(), mZ-add.getZ());
    }
    
    inline void operator-=(const CMVector3 &add)
    {
        mX -= add.getX();
        mY -= add.getY();
        mZ -= add.getZ();
    }
    
    inline CMVector3 operator*(F32 scalar)
    {
        return CMVector3(mX*scalar, mY*scalar, mZ*scalar);
    }
    
    inline void operator*=(F32 scalar)
    {
        mX *= scalar;
        mY *= scalar;
        mZ *= scalar;
    }
    
    inline bool operator==(const CMVector3& comp)
    {
        return FLOAT_EQUAL(mX, comp.mX) && FLOAT_EQUAL(mY, comp.mY) && FLOAT_EQUAL(mZ, comp.mZ);
    }
    
    inline bool operator!=(const CMVector3& comp)
    {
        return !FLOAT_EQUAL(mX, comp.mX) || !FLOAT_EQUAL(mY, comp.mY) || !FLOAT_EQUAL(mZ, comp.mZ);
    }
};


class CMVector4
{
protected:
	F32 mX, mY, mZ, mW;
	
public:
	CMVector4();
	CMVector4(F32 x, F32 y, F32 z, F32 w);
    CMVector4(const CMVector3 &vec);
	
	void normalize();
    
    void add(CMVector4 &vec);
    void subtract(CMVector4 &vec);
    void multiplyScalar(F32 s);
    void divideScalar(F32 s);
    
    // Multiplies the current GL matrix by the vector.
    const void translateGL();
	
    inline F32 getX() const
	{
		return mX;
	}
	
	inline F32 getY() const
	{
		return mY;
	}
	
	inline F32 getZ() const
	{
		return mZ;
	}
    
    inline F32 getW() const
    {
        return mW;
    }
	
	inline void setX(F32 x)
	{
		mX = x;
	}
	
	inline void setY(F32 y)
	{
		mY = y;
	}
	
	inline void setZ(F32 z)
	{
		mZ = z;
	}
    
    inline void setW(F32 w)
    {
        mW = w;
    }
    
    inline bool operator==(const CMVector4& comp)
    {
        return FLOAT_EQUAL(mX, comp.mX) && FLOAT_EQUAL(mY, comp.mY) && FLOAT_EQUAL(mZ, comp.mZ) && FLOAT_EQUAL(mW, comp.mW);
    }
    
    inline bool operator!=(const CMVector4& comp)
    {
        return !FLOAT_EQUAL(mX, comp.mX) || !FLOAT_EQUAL(mY, comp.mY) || !FLOAT_EQUAL(mZ, comp.mZ) || !FLOAT_EQUAL(mW, comp.mW);
    }
};

inline constexpr F32 operator "" _i(long double f)
{
    return f;
}

inline CMVector2 operator "" _j(long double f)
{
    return {0.0f, f};
}

inline CMVector3 operator "" _k(long double f)
{
    return {0.0f, 0.0f, f};
}

inline CMVector4 operator "" _w(long double f)
{
    return {0.0f, 0.0f, 0.0f, f};
}

#endif
