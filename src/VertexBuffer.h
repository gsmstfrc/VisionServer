//
//  VertexBuffer.h
//  mythreal
//
//  Created by Ian Ewell on 3/23/13.
//
//

#ifndef __mythreal__VertexBuffer__
#define __mythreal__VertexBuffer__

#include <iostream>
#include <vector>
#include "Reference.h"
#include "SafeArray.h"

typedef enum
{
    CGVBOStatic,
    CGVBODynamic,
    CGVBOStream,
} EVBOType;

enum
{
    ATTRIB_VERTEX,
    ATTRIB_UV,
    ATTRIB_NORMAL,
    ATTRIB_WEIGHTS,
    ATTRIB_INFLUENCES,
    NUM_ATTRIBUTES
};

typedef struct
{
    F32 x;
    F32 y;
    F32 z;
    F32 w;
} SVertex4;

typedef struct
{
    F32 x;
    F32 y;
    F32 z;
} SVertex;

typedef struct
{
    F32 u;
    F32 v;
} SUV;

typedef struct
{
    F32 val;
} SScalar;

typedef CCSafeArray<SVertex4> CVertex4Array;
typedef CCSafeArray<SVertex> CVertexArray;
typedef CCSafeArray<SUV> CUVArray;
typedef CCSafeArray<SScalar> CScalarArray;

// Abstract class for accessing a vertex buffer object of a float array and up to four components per vertex
class CGRVertexBuffer : public CCReferenceBase
{
protected:
    friend class CCSafeArray<SVertex>;
    friend class CCSafeArray<SUV>;
    
    // Stores information about a vertex attribute buffer so that it can be processed and uploaded to the GPU
    class BufferInfoBase
    {
    public:
        U32 attribBind; // Shader attribute binding point
        U32 compCount; // Number of components for this vertex attribute
        U32 compSize; // Size of each component. Almost always a 4-byte float
        U32 elementCount; // Total numer of elements, or vertices, for this attribute
        void *data; // Pointer to the attribute data
    };
    
    // Class to ensure we have a reference to the vertex buffer so it isn't deallocated before the VBOs are actually created
    template <class T>
    class BufferInfo : public BufferInfoBase
    {
    public:
        CCSafeArray<T> reference;
    };
    
    // Used by the graphics implementation to setup and bind the buffers
    class BindInfo
    {
    public:
        U32 bindLoc; // Shader element bind location
        U32 offset; // Offset into buffer to reach the first element of this attribute
        U32 stride; // Number of bytes that separates each element from the next
        U32 compSize; // Size of each component (most likely 4-bytes)
        U32 compCount; // Number of components per element of this attribute
    };
    
    // Number of elements, or vertices, in the buffer
    U32 mElementCount;
    
    // Array of all the buffers for each of the vertex attributes. Array is cleared if using VBOs
    std::vector<BufferInfoBase*> mBuffers;
    
    // Array of bind infos used by the graphics implementation
    std::vector<BindInfo> mBindInfos;
    
    // Enabled by default. If enabled, the vertex data will be uploaded to the GPU via vertex buffer objects
    bool mUseVBOs;
    
    // Once activated, a buffer currently can't be modified, as data is uploaded to GPU and references to the data are deleted to free local memory
    bool mActivated;
    
    // Buffer to store combined and interleaved vertex data. Freed after being uploaded if using VBOs
    F32 *mVertexArray;
    
    // Internal buffer adding function
    bool _addBuffer(BufferInfoBase* base, U32 bindLoc, U32 compCount, U32 compSize, U32 elementCount, void* data);
    
    virtual void _allocate(U32 size)=0;
    virtual void _bind()=0;
    virtual void _unbind()=0;
    
public:
    CGRVertexBuffer();
    virtual ~CGRVertexBuffer();
    
    // Adds a buffer bound to a vertex attribute to the list. Vertex buffer must not be activated
    void addBuffer(CVertex4Array buffer, U32 bindLoc);
    void addBuffer(CVertexArray buffer, U32 bindLoc);
    void addBuffer(CUVArray buffer, U32 bindLoc);
    void addBuffer(CScalarArray buffer, U32 bindLoc);
    
    // Activates the vertex buffer so draw calls can be used on it
    void activate();
    
    // Binds the buffer so that an index buffer can render with this buffer
    void bind();
    void unbind();
};

typedef CCPointerStrong<CGRVertexBuffer> CGRVertexBufferRef;

#endif /* defined(__mythreal__VertexBuffer__) */
