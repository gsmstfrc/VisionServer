//
//  TextureManager.cpp
//  mythreal
//
//  Created by Ian Ewell on 5/2/13.
//
//

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "TextureManager.h"
#include "Logger.h"

static U32 nextPowof2(U32 input)
{
    U32 out = 1;
    while (out < input) out <<= 1;
    return out;
}

static U32 log2(U32 input)
{
    U32 i = 0;
    while (input <<= 1 >= 1) i++;
    return i;
}

void CGRBitmapAllocator::Initialize()
{
    mAllocatedMemory = 0;
    mUsedObjectList.clear();
    mFreedObjectList.clear();
}

void CGRBitmapAllocator::Shutdown()
{
    for (BitmapInfo info : mUsedObjectList)
    {
        if (info.storage)
            delete [] info.storage;
    }
    
    for (BitmapInfo info : mFreedObjectList)
    {
        if (info.storage)
            delete [] info.storage;
    }
    
    mUsedObjectList.clear();
    mFreedObjectList.clear();
    mAllocatedMemory = 0;
}

U8* CGRBitmapAllocator::Allocate(U32 width, U32 height, U32 compcount, EBitmapFormat format)
{
    for (auto it = mFreedObjectList.begin(); it != mFreedObjectList.end(); it++)
    {
        if ((*it).width == width &&
            (*it).height == height &&
            (*it).compcount == compcount &&
            (*it).format == format)
        {
            BitmapInfo info = (*it);
            mFreedObjectList.erase(it);
            mUsedObjectList.push_front(info);
            return (*it).storage;
        }
    }
    
    BitmapInfo info;
    info.width = width;
    info.height = height;
    info.compcount = compcount;
    info.format = format;
    info.mipLevel = log2(std::max(width, height))-1;
    info.storage = new U8[width*height*compcount];
    mUsedObjectList.push_front(info);
    
    mAllocatedMemory += width*height*compcount;
    
    return info.storage;
}

void CGRBitmapAllocator::Release(U8 *ptr)
{
    for (auto it = mUsedObjectList.begin(); it != mUsedObjectList.end(); it++)
    {
        if ((*it).storage == ptr)
        {
            BitmapInfo info = (*it);
            mUsedObjectList.erase(it);
            mFreedObjectList.push_front(info);
            break;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////

CGRBitmap::CGRBitmap(U32 width, U32 height)
{
    mWidth = nextPowof2(width);
    mHeight = nextPowof2(height);
    mType = BT_RGB;
    mFormat = BF_UBYTE;
    mComponentCount = 3;
    mBuffer = CGRBitmapAllocator::Allocate(width, height, 3, BF_UBYTE);
}

CGRBitmap::CGRBitmap(U32 width, U32 height, EBitmapType type, EBitmapFormat format)
{
    mWidth = nextPowof2(width);
    mHeight = nextPowof2(height);
    mType = type;
    mFormat = format;
    
    switch (mType)
    {
        case BT_R:
            mComponentCount = 1;
            break;
        case BT_RG:
            mComponentCount = 2;
            break;
        case BT_RGB:
            mComponentCount = 3;
            break;
        case BT_RGBA:
            mComponentCount = 4;
            break;
    }
    
    mBuffer = CGRBitmapAllocator::Allocate(mWidth, mHeight, mComponentCount, mFormat);
}

CGRBitmap::~CGRBitmap()
{
    if (mBuffer)
        CGRBitmapAllocator::Release(mBuffer);
}

CGRBitmap::Pixel& CGRBitmap::fetchPixel(S32 x, S32 y)
{
    if (x >= 0 && y >= 0)
        return (Pixel&)mBuffer[((y*mWidth)%mHeight+x%mWidth)*mComponentCount];
    else if (x >= 0 && y < 0)
        return (Pixel&)mBuffer[(mHeight-((y*mWidth)%mHeight)+x%mWidth)*mComponentCount];
    else if (x < 0 && y >= 0)
        return (Pixel&)mBuffer[((y*mWidth)%mHeight+(mWidth-x%mWidth))*mComponentCount];
    else
        return (Pixel&)mBuffer[(mHeight-((y*mWidth)%mHeight)+(mWidth-x%mWidth))*mComponentCount];
}

void CGRBitmap::fillBuffer(CCSafeArray<U8> buffer)
{
    if (buffer.getSize() != mWidth*mHeight*mComponentCount)
    {
        CCLogger::LogError("CGRBitmap::fillBuffer()", "Invalid buffer size");
        return;
    }
    memcpy(mBuffer, &buffer[0], mWidth*mHeight*mComponentCount);
}

void CGRBitmap::downSampleBilinear(CGRBitmap &src)
{
    if (mWidth != src.getWidth() >> 1 || mHeight != src.getHeight() >> 1)
    {
        CCLogger::LogError("CGRBitmap::downSampleBilinear()", "Source bitmap must be twice the size");
        return;
    }
    
    // Bilinear box filtering uses a 4x4 kernel with 0.25 weights
    for (U32 i = 0; i < mHeight; i++)
    {
        for (U32 j = 0; j < mWidth; j++)
        {
            for (U32 comp = 0; comp < mComponentCount; comp++)
            {
                operator()(i, j).array[comp] = (src(i*2,j*2).array[comp] +
                                                src(i*2,j*2+1).array[comp] +
                                                src(i*2+1,j*2).array[comp] +
                                                src(i*2+1,j*2+1).array[comp])/4;
            }
        }
    }
}

void CGRBitmap::upSampleBilinear(CGRBitmap &src)
{
    if (mWidth != src.getWidth() << 1 || mHeight != src.getHeight() << 1)
    {
        CCLogger::LogError("CGRBitmap::upSampleBilinear()", "Source bitmap must be half the size");
        return;
    }
    for (U32 i = 0; i < mHeight; i++)
    {
        for (U32 j = 0; j < mWidth; j++)
        {
            for (U32 comp = 0; comp < mComponentCount; comp++)
            {
                if (i%2 == 0 && j%2 == 0)
                    operator()(i, j).array[comp] = src(i/2,j/2).array[comp];
                else if (i%2 == 1 && j%2 == 0)
                    operator()(i, j).array[comp] = (src(i/2,j/2).array[comp] +
                                                    src(i/2+1,j/2).array[comp])/2;
                else if (i%2 == 0 && j%2 == 1)
                    operator()(i, j).array[comp] = (src(i/2,j/2).array[comp] +
                                                    src(i/2,j/2+1).array[comp])/2;
                else
                    operator()(i, j).array[comp] = (src(i/2,j/2).array[comp] +
                                                    src(i/2+1,j/2).array[comp] +
                                                    src(i/2,j/2+1).array[comp] +
                                                    src(i/2+1,j/2+1).array[comp])/4;
            }
        }
    }
}

static F32 Bessel0(F32 x)
{
    F32 d = 0;
    F32 ds = 1.0f;
    F32 s = 1.0f;
    while (ds > s*1e-6)
    {
        d += 2;
        ds *= x*x/(d*d);
        s += ds;
    }
    return s;
}

void CGRBitmap::downSampleKaiser(CGRBitmap &src)
{
    if (mWidth != src.getWidth() >> 1 || mHeight != src.getHeight() >> 1)
    {
        CCLogger::LogError("CGRBitmap::downSampleKaiser()", "Source bitmap must be twice the size");
        return;
    }
    
    U32 filterSize = 14;
    F32 coffTable[filterSize][filterSize];
    F32 InoAlpha = Bessel0(4.0f);
    F32 NP = filterSize/2;
    for (U32 i = 0; i < filterSize; i++)
    {
        for (U32 j = 0; j < filterSize; j++)
        {
            coffTable[i][j] = (Bessel0(4.0f*sqrtf(1.0f-(i*i/(NP*NP))))/InoAlpha) *
                              (Bessel0(4.0f*sqrtf(1.0f-(j*j/(NP*NP))))/InoAlpha);
        }
    }
}
