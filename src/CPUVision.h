//
// FRC Vision Server
// FRC 2014
// FRC Team 3318
// Written by Ian Ewell <ianewellgmsmt@gmail.com>
// (C) 2014 GSMST Robotics
//

#ifndef CPU_VISION_H
#define CPU_VISION_H

#include "Types.h"

class CPUVision
{
    protected:
        U8 *mBitmap;
        U32 mWidth;
        U32 mHeight;

    public:
        CPUVision();
        virtual ~CPUVision();

        inline void setBitmap(U8 *bitmap, U32 width, U32 height)
        {
            mBitmap = bitmap;
            mWidth = width;
            mHeight = height;
        }

        // Processing functions
        U32 countPixels();
};

#endif
