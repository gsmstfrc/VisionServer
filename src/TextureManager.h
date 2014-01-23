//
//  TextureManager.h
//  mythreal
//
//  Created by Ian Ewell on 5/2/13.
//
//

#ifndef __mythreal__TextureManager__
#define __mythreal__TextureManager__

#include <iostream>
#include <list>
#include "SafeArray.h"

typedef enum
{
    BT_R,
    BT_RG,
    BT_RGB,
    BT_RGBA,
} EBitmapType;

typedef enum
{
    BF_UBYTE,
} EBitmapFormat;

typedef enum
{
    FT_NEAREST,
    FT_LINEAR,
    FT_LINEAR_MIPMAP,
} EFilterType;

// Because textures will often be loaded and unloaded on the fly as higher
// res textures are streamed and uploaded to the GPU, this allocator will
// save allocated memory for textures to quickly be loaded into and will
// keep track of the memory used
class CGRBitmapAllocator
{
    // Not the best implementation I have, but something for the intermediate
    // until I decide to work on optimizing this thing
    
protected:
    struct BitmapInfo
    {
        U32 width;
        U32 height;
        U32 mipLevel;
        U32 compcount;
        EBitmapFormat format;
        U8 *storage;
    };
    
protected:
    static U32 mAllocatedMemory;
    
    static std::list<BitmapInfo> mUsedObjectList;
    static std::list<BitmapInfo> mFreedObjectList;
    
public:
    static void Initialize();
    static void Shutdown();
    static U8* Allocate(U32 width, U32 height, U32 compcount, EBitmapFormat format);
    static void Release(U8* ptr);
};

// Stores a bitmap and has various functions for upscaling and downscaling.
// Currently enforces that width and height are of powers of 2
class CGRBitmap
{
protected:
    U32 mWidth;
    U32 mHeight;
    
    EBitmapType mType;
    EBitmapFormat mFormat;
    U32 mComponentCount;
    
    U8 *mBuffer;
    
public:
    union Pixel
    {
        struct comp
        {
            U8 r;
            U8 g;
            U8 b;
            U8 a;
        };
        U8 array[4];
    };
    
public:
    CGRBitmap(U32 width, U32 height);
    CGRBitmap(U32 width, U32 height, EBitmapType type, EBitmapFormat format);
    virtual ~CGRBitmap();
    
    inline U32 getWidth()
    {
        return mWidth;
    }
    
    inline U32 getHeight()
    {
        return mHeight;
    }
    
    inline EBitmapType getType()
    {
        return mType;
    }
    
    inline EBitmapFormat getFormat()
    {
        return mFormat;
    }
    
    inline Pixel& operator()(S32 x, S32 y)
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
    
    Pixel& fetchPixel(S32 x, S32 y);
    
    void fillBuffer(CCSafeArray<U8> buffer);
    
    void downSampleBilinear(CGRBitmap &src);
    void upSampleBilinear(CGRBitmap &src);
    
    void downSampleKaiser(CGRBitmap &src);
};

class CGRTextureStorage : public CCReferenceBase
{
protected:
    CGRTextureStorage(U32 width, U32 height, EBitmapFormat format = BF_UBYTE, EBitmapType type = BT_RGB, bool mips=true);
    virtual ~CGRTextureStorage();

protected:
    U32 mBaseWidth;
    U32 mBaseHeight;
    bool mUseMipmaps;
    EBitmapFormat mFormat;
    EBitmapType mType;
    
public:
    void uploadBitmap(const CGRBitmap &bitmap, U32 mipLevel);
    void copyTo(CCPointerStrong<CGRTextureStorage> dest);
};

struct STextureDesc
{
    U32 width;
    U32 height;
    EBitmapFormat format;
    EBitmapType type;
    EFilterType filterMin;
    EFilterType filterMag;
    bool mipmap;
    U32 anisotropicLevel;
};

class CGRTexture : public CCReferenceBase
{
protected:
    STextureDesc mTexDesc;
    U32 mTextureID;
    
public:
    CGRTexture(STextureDesc &desc);
    virtual ~CGRTexture();
    
    void loadBitmap(CGRBitmap &bitmap);
    virtual void bind()=0;
};

typedef CCPointerStrong<CGRTexture> CGRTextureHandle;

class CGRTextureManager
{
protected:
public:
    CGRTextureManager();
    virtual ~CGRTextureManager();
};

#endif /* defined(__mythreal__TextureManager__) */
