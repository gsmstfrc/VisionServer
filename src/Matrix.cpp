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
 * PURSPOSE: Implement various matrix classes and functions.
 *
 * CREATED ON: 05/10/10
 *
 * COPYRIGHT: (C) Copyright 2010 Mythreal Studios. All rights reserved.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "Matrix.h"

CMMatrix4x4::CMMatrix4x4()
{
	setIdentity();
}

void CMMatrix4x4::setIdentity()
{
	memset(mData.array, 0, sizeof(float)*16);
	M11 = 1;
	M22 = 1;
	M33 = 1;
	M44 = 1;
}

void CMMatrix4x4::multiply(const CMMatrix4x4 &mat)
{
	CMMatrix4x4 copy = (*this);
	
	M11 = copy.M11*mat.M11 + copy.M12*mat.M21 + copy.M13*mat.M31 + copy.M14*mat.M41;
	M12 = copy.M11*mat.M12 + copy.M12*mat.M22 + copy.M13*mat.M32 + copy.M14*mat.M42;
	M13 = copy.M11*mat.M13 + copy.M12*mat.M23 + copy.M13*mat.M33 + copy.M14*mat.M43;
	M14 = copy.M11*mat.M14 + copy.M12*mat.M24 + copy.M13*mat.M34 + copy.M14*mat.M44;
	
	M21 = copy.M21*mat.M11 + copy.M22*mat.M21 + copy.M23*mat.M31 + copy.M24*mat.M41;
	M22 = copy.M21*mat.M12 + copy.M22*mat.M22 + copy.M23*mat.M32 + copy.M24*mat.M42;
	M23 = copy.M21*mat.M13 + copy.M22*mat.M23 + copy.M23*mat.M33 + copy.M24*mat.M43;
	M24 = copy.M21*mat.M14 + copy.M22*mat.M24 + copy.M23*mat.M34 + copy.M24*mat.M44;
	
	M31 = copy.M31*mat.M11 + copy.M32*mat.M21 + copy.M33*mat.M31 + copy.M34*mat.M41;
	M32 = copy.M31*mat.M12 + copy.M32*mat.M22 + copy.M33*mat.M32 + copy.M34*mat.M42;
	M33 = copy.M31*mat.M13 + copy.M32*mat.M23 + copy.M33*mat.M33 + copy.M34*mat.M43;
	M34 = copy.M31*mat.M14 + copy.M32*mat.M24 + copy.M33*mat.M34 + copy.M34*mat.M44;
	
	M41 = copy.M41*mat.M11 + copy.M42*mat.M21 + copy.M43*mat.M31 + copy.M44*mat.M41;
	M42 = copy.M41*mat.M12 + copy.M42*mat.M22 + copy.M43*mat.M32 + copy.M44*mat.M42;
	M43 = copy.M41*mat.M13 + copy.M42*mat.M23 + copy.M43*mat.M33 + copy.M44*mat.M43;
	M44 = copy.M41*mat.M14 + copy.M42*mat.M24 + copy.M43*mat.M34 + copy.M44*mat.M44;
}

CMVector4 CMMatrix4x4::multiplyVec(const CMVector4 &vec)
{
    
    F32 x = vec.getX();
    F32 y = vec.getY();
    F32 z = vec.getZ();
    F32 w = vec.getW();
    
    CMVector4 res;
    res.setX(M11*x + M12*y + M13*z + M14*w);
    res.setY(M21*x + M22*y + M23*z + M24*w);
    res.setZ(M31*x + M32*y + M33*z + M34*w);
    res.setW(M41*x + M42*y + M43*z + M44*w);
    
    return res;
}

void CMMatrix4x4::multiplyScalar(const F32 val)
{
    for (int i = 0; i < 16; i++)
        mData.array[i] *= val;
}

F32 CMMatrix4x4::determinant()
{
    return M11 * (M22 * M33 - M23 * M32) 
        + M21 * (M13 * M32 - M12 * M33)
        + M31 * (M12 * M23 - M13 * M22);
}

void CMMatrix4x4::inverse()
{
    CMMatrix4x4 copy = (*this);
    F32 det = determinant();
    if (det == 0)
        return;
    F32 invDet = 1/det;
    
    copy.M11 = (mData.array[5]*mData.array[10] - mData.array[6]*mData.array[9]) * invDet;
    copy.M12 = (mData.array[9]*mData.array[2] - mData.array[10]*mData.array[1]) * invDet;
    copy.M13 = (mData.array[1]*mData.array[6] - mData.array[2]*mData.array[5]) * invDet;
    
    copy.M21 = (mData.array[6]*mData.array[8] - mData.array[4]*mData.array[10]) * invDet;
    copy.M22 = (mData.array[10]*mData.array[0] - mData.array[8]*mData.array[2]) * invDet;
    copy.M23 = (mData.array[2]*mData.array[4] - mData.array[0]*mData.array[6]) * invDet;
    
    copy.M31 = (mData.array[4]*mData.array[9] - mData.array[5]*mData.array[8]) * invDet;
    copy.M32 = (mData.array[8]*mData.array[1] - mData.array[9]*mData.array[0]) * invDet;
    copy.M33 = (mData.array[0]*mData.array[5] - mData.array[1]*mData.array[4]) * invDet;
    
    M11 = copy.M11;
    M12 = copy.M12;
    M13 = copy.M13;
    
    M21 = copy.M21;
    M22 = copy.M22;
    M23 = copy.M23;
    
    M31 = copy.M31;
    M32 = copy.M32;
    M33 = copy.M33;
    
    copy.M11 = -M14;
    copy.M12 = -M24;
    copy.M13 = -M34;
    
    M14 = M11*copy.M11 + M12*copy.M12 + M13*copy.M13;
    M24 = M21*copy.M11 + M22*copy.M12 + M23*copy.M13;
    M34 = M31*copy.M11 + M32*copy.M12 + M33*copy.M13;
}

CMMatrix4x4 CMMatrix4x4::transpose() const
{
    CMMatrix4x4 result;
    
    result.M11 = M11;
    result.M12 = M21;
    result.M13 = M31;
    result.M14 = M41;
    result.M21 = M12;
    result.M22 = M22;
    result.M23 = M32;
    result.M24 = M42;
    result.M31 = M13;
    result.M32 = M23;
    result.M33 = M33;
    result.M34 = M43;
    result.M41 = M14;
    result.M42 = M24;
    result.M43 = M34;
    result.M44 = M44;
    
    return result;
}

void CMMatrix4x4::makeShadowMatrix(const CMPlane &plane, const CMVector3 &light)
{
    F32 a = plane.getX();
    F32 b = plane.getY();
    F32 c = plane.getZ();
    F32 d = plane.getD();
    
    float x = light.getX();
    float y = light.getY();
    float z = light.getZ();
    
    float dot = a*x+b*y+c*z+d;
    
    M11 = dot - x*a;
    M12 = -x*b;
    M13 = -x*c;
    M14 = -x*d;
    
    M21 = -y*a;
    M22 = dot - b*y;
    M23 = -y*c;
    M24 = -y*d;
    
    M31 = -z*a;
    M32 = -z*b;
    M33 = dot - c*y;
    M34 = -z*d;
    
    M41 = -a;
    M42 = -b;
    M43 = -c;
    M44 = dot - d;
}

void CMMatrix4x4::makePerspectiveMatrix(float FOV, float aspect, float near, float far)
{
    F32 radians = FOV*3.141592654/180;
    float dZ = far - near;
    float sine = sinf(radians);
    if ((dZ == 0) || (sine == 0) || (aspect == 0))
        return;
        
    float cotan = cosf(radians)/sine;
    
    setIdentity();
    M11 = cotan/aspect;
    M22 = cotan;
    M33 = -(far + near)/dZ;
    M43 = -1;
    M34 = -2*near*far/dZ;
    M44 = 0;
}

void CMMatrix4x4::lookAtMatrix(const CMVector3 &position, const CMVector3 &target)
{
    CMVector3 forward = target;
    forward.subtract(position);
    forward.normalize();
    
    CMVector3 up;
    if (fabsf(forward.getX()) < 0.000001f && fabsf(forward.getZ()) < 0.000001f)
    {
        if (forward.getY() > 0.0f)
            up = CMVector3(0.0, 0.0f, -1.0f);
        else
            up = CMVector3(0.0f, 0.0f, 1.0f);
    }
    else
    {
        up = CMVector3(0.0f, 1.0f, 0.0f);
    }
    
    CMVector3 left = forward.cross(up);
    left.normalize();
    
    up = left.cross(forward);
    up.normalize();
    
    setIdentity();
    M11 = left.getX();
    M12 = left.getY();
    M13 = left.getZ();
    
    M21 = up.getX();
    M22 = up.getY();
    M23 = up.getZ();
    
    M31 = -forward.getX();
    M32 = -forward.getY();
    M33 = -forward.getZ();
    
    //M14 = -position.getX();
    //M24 = -position.getY();
    //M34 = -position.getZ();
    CMMatrix4x4 trans;
    trans.setColumn(3, CMVector4(-position.getX(), -position.getY(), -position.getZ(), 1.0f));
    multiply(trans);
    
}

void CMMatrix4x4::lookAtMatrix(const CMVector3 &position, const CMVector3 &target, const CMVector3 &up)
{
    CMVector3 forward = target;
    forward.subtract(position);
    forward.normalize();
    
    CMVector3 up2 = up;
        
    CMVector3 left = up.cross(forward);
    left.normalize();
    
    //up2 = forward.cross(left);
    //up2.normalize();
    
    setIdentity();
    M11 = left.getX();
    M21 = left.getY();
    M31 = left.getZ();
    
    M12 = forward.getX();
    M22 = forward.getY();
    M32 = forward.getZ();
    
    M13 = up2.getX();
    M23 = up2.getY();
    M33 = up2.getZ();
    
    M14 = position.getX();
    M24 = position.getY();
    M34 = position.getZ();
    //CMMatrix4x4 trans;
    //trans.setColumn(3, CMVector4(-position.getX(), -position.getY(), -position.getZ(), 1.0f));
    //multiply(trans);
    inverse();
}

void CMMatrix4x4::rotate(F32 angle, F32 x, F32 y, F32 z)
{
    F32 m, s, c;
    F32 xx, yy, zz, xy, yz, zx, xs, ys, zs, oneC;
    
    s = sinf(angle);
    c = cosf(angle);
    
    m = sqrtf(x*x + y*y + z*z);
    
    if (m == 0.0f)
    {
        setIdentity();
        return;
    }
    
    x /= m;
    y /= m;
    z /= m;
    
    xx = x * x;
	yy = y * y;
	zz = z * z;
	xy = x * y;
	yz = y * z;
	zx = z * x;
	xs = x * s;
	ys = y * s;
	zs = z * s;
	oneC = 1.0f - c;
    
    CMMatrix4x4 res;
    
    res.M11 = (oneC * xx) + c;
	res.M12 = (oneC * xy) - zs;
	res.M13 = (oneC * zx) + ys;
	res.M14 = 0.0;
    
	res.M21 = (oneC * xy) + zs;
	res.M22 = (oneC * yy) + c;
	res.M23 = (oneC * yz) - xs;
	res.M24 = 0.0;
    
	res.M31 = (oneC * zx) - ys;
	res.M32 = (oneC * yz) + xs;
	res.M33 = (oneC * zz) + c;
	res.M34 = 0.0;
    
	M41 = 0.0;
	M42 = 0.0;
	M43 = 0.0;
	M44 = 1.0;
    
    multiply(res);
}

F32* CMMatrix4x4::getArray()
{
    *this = transpose();
    return &mData.array[0];
    *this = transpose();
}

void CMMatrix4x4::loadMatrixGL()
{
    //glLoadMatrixf(transpose().mData.array);
}

void CMMatrix4x4::multMatrixGL()
{
    //glMultMatrixf(transpose().mData.array);
}

void CMMatrix4x4::setColumn(int index, const CMVector4 &vec)
{
    mData.array[index*4] = vec.getX();
    mData.array[index*4+1] = vec.getY();
    mData.array[index*4+2] = vec.getZ();
    mData.array[index*4+3] = vec.getW();
}

CMVector4 CMMatrix4x4::getColumn(int index)
{
    return CMVector4(mData.array[index*4],
                     mData.array[index*4+1],
                     mData.array[index*4+2],
                     mData.array[index*4+3]);
}
