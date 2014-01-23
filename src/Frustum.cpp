//
//  Frustum.cpp
//  mythreal
//
//  Created by Ian Ewell on 9/30/12.
//
//

#include "Frustum.h"

CMFrustum::CMFrustum()
{
    mLeft = 0.0f;
    mRight = 0.0f;
    mBottom = 0.0f;
    mTop = 0.0f;
    mNear = 0.0f;
    mFar = 0.0f;
}

CMFrustum::CMFrustum(F32 left, F32 right,
                     F32 bottom, F32 top,
                     F32 near, F32 far)
{
    mLeft = left;
    mRight = right;
    mBottom = bottom;
    mTop = top;
    mNear = near;
    mFar = far;
}