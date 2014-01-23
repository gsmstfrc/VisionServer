/*
 *
 * FILE: Matrix.h
 *
 * AUTHOR: Ian Ewell
 *
 * PROJECT: Mythreal Engine
 *
 * MODULE: Math
 *
 * PURSPOSE: Declare various matrix classes and functions.
 *
 * CREATED ON: 05/10/10
 *
 * COPYRIGHT: (C) Copyright 2010 Mythreal Studios. All rights reserved.
 *
 */

#ifndef MATH_MATRIX_H
#define MATH_MATRIX_H

//Includes:
#include <string.h>
#include "Types.h"
#include "Vector.h"
#include "Plane.h"

class CMMatrix4x4
{
private:
	
	/* Reference diagram:
	 *
	 * 0  1  2  3
	 * 4  5  6  7
	 * 8  9  10 11
	 * 12 13 14 15
	 *
	 */
	union
	{
		struct
		{
			F32 _0; F32 _1; F32 _2; F32 _3; F32 _4; F32 _5; F32 _6; F32 _7; F32 _8;
			F32 _9; F32 _10; F32 _11; F32 _12; F32 _13; F32 _14; F32 _15;
		} val;
		
		F32 array[16];
		
	} mData;
    
	//Now for some defines for easier access.
	#define M11 mData.array[0]
	#define M12 mData.array[1]
	#define M13 mData.array[2]
	#define M14 mData.array[3]
	#define M21 mData.array[4]
	#define M22 mData.array[5]
	#define M23 mData.array[6]
	#define M24 mData.array[7]
	#define M31 mData.array[8]
	#define M32 mData.array[9]
	#define M33 mData.array[10]
	#define M34 mData.array[11]
	#define M41 mData.array[12]
	#define M42 mData.array[13]
	#define M43 mData.array[14]
	#define M44 mData.array[15]
	
public:
	
	CMMatrix4x4();
	
	void setIdentity();
	
	void multiply(const CMMatrix4x4 &mat);
    CMVector4 multiplyVec(const CMVector4 &vec);
    void multiplyScalar(const F32 val);
    
    F32 determinant();
    void inverse();
    
    CMMatrix4x4 transpose() const;
    
    void makeShadowMatrix(const CMPlane &plane, const CMVector3 &light);
    
    void makePerspectiveMatrix(float FOV, float aspect, float near, float far);
    
    void lookAtMatrix(const CMVector3 &position, const CMVector3 &target);
    void lookAtMatrix(const CMVector3 &position, const CMVector3 &target, const CMVector3 &up);
    
    void rotate(F32 angle, F32 x, F32 y, F32 z);
    
    F32* getArray();
    
    void loadMatrixGL();
    
    void multMatrixGL();
    
    void setColumn(int index, const CMVector4 &vec);
    CMVector4 getColumn(int index);
    
    inline bool operator==(const CMMatrix4x4 &comp)
    {
        return memcmp(&mData, &comp.mData, sizeof(mData)) == 0;
    }
    
    inline bool operator!=(const CMMatrix4x4 &comp)
    {
        return memcmp(&mData, &comp.mData, sizeof(mData)) != 0;
    }
};

#endif
