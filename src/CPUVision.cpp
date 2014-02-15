//
// FRC Vision Server
// FRC 2014
// FRC Team 3318
// Written by Ian Ewell <ianewellgmsmt@gmail.com>
// (C) 2014 GSMST Robotics
//

#include "CPUVision.h"
#include "CountTable.h"

CPUVision::CPUVision() : mBitmap(nullptr)
{
}

CPUVision::~CPUVision()
{
}

U32 CPUVision::countPixels()
{
    if (mBitmap == nullptr)
        return 0;

    int count = 0;
    for (int i = 0; i < mWidth*mHeight/8; i++) {
        if ((*((U32*)(mBitmap+i))) == 0) {
            i += 3;
            continue;
        }
        count += countTable[mBitmap[i]];
    }
    return count;
}
