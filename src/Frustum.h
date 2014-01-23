//
//  Frustum.h
//  mythreal
//
//  Created by Ian Ewell on 9/30/12.
//
//

#ifndef __mythreal__Frustum__
#define __mythreal__Frustum__

#include <iostream>

#include "Types.h"

class CMFrustum
{
protected:
    F32 mLeft;
    F32 mRight;
    F32 mBottom;
    F32 mTop;
    F32 mNear;
    F32 mFar;
    
public:
    CMFrustum();
    CMFrustum(F32 left, F32 right,
              F32 bottom, F32 top,
              F32 near, F32 far);
    
    inline F32 getLeft() { return mLeft; }
    inline F32 getRight() { return mRight; }
    inline F32 getBottom() { return mBottom; }
    inline F32 getTop() { return mTop; }
    inline F32 getNear() { return mNear; }
    inline F32 getFar() { return mFar; }
    
};

#endif /* defined(__mythreal__Frustum__) */
