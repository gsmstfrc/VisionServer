//
//  VertexBuffer.cpp
//  mythreal
//
//  Created by Ian Ewell on 3/23/13.
//
//

#include <stdlib.h>
#include <string.h>
#include "VertexBuffer.h"
#include "Logger.h"

CGRVertexBuffer::CGRVertexBuffer()
{
    mUseVBOs = true;
    mActivated = false;
    
    mElementCount = 0;
    mVertexArray = NULL;
}

CGRVertexBuffer::~CGRVertexBuffer()
{
    for (std::vector<BufferInfoBase*>::iterator it = mBuffers.begin(); it != mBuffers.end(); it++)
        delete (*it);
    if (mVertexArray != NULL)
        delete [] mVertexArray;
}

bool CGRVertexBuffer::_addBuffer(BufferInfoBase* base, U32 bindLoc, U32 compCount, U32 compSize, U32 elementCount, void* data)
{
    if (mActivated)
    {
        CCLogger::LogError("CGRVertexBuffer::_addBuffer()", "Attempted to add buffer when vertex buffer is currently activated");
        return false;
    }
    
    if (elementCount == 0)
    {
        CCLogger::LogError("CGRVertexBuffer::_addBuffer()", "Attempted to add buffer with size of 0");
        return false;
    }
    
    if (mElementCount == 0)
        mElementCount = elementCount;
    
    if (mElementCount != elementCount)
    {
        CCLogger::LogError("CGRVertexBuffer::_addBuffer()", "Attempted to add buffer of incompatible size");
        return false;
    }
    
    if (bindLoc >= NUM_ATTRIBUTES)
    {
        CCLogger::LogError("CGRVertexBuffer::_addBuffer()", "Attempted to add buffer with invalid bind location");
        return false;
    }
    
    base->attribBind = bindLoc;
    base->compCount = compCount;
    base->compSize = compSize;
    base->elementCount = elementCount;
    base->data = data;
    
    mBuffers.push_back(base);
    
    return true;
}

void CGRVertexBuffer::addBuffer(CVertex4Array buffer, U32 bindLoc)
{
    BufferInfo<SVertex4>* bufferInfo = new BufferInfo<SVertex4>;
    bufferInfo->reference = (CCSafeArray<SVertex4>)buffer;
    if (!_addBuffer(bufferInfo, bindLoc, 4, 4, buffer.getSize(), &buffer[0]))
        delete bufferInfo;
}

void CGRVertexBuffer::addBuffer(CVertexArray buffer, U32 bindLoc)
{
    BufferInfo<SVertex>* bufferInfo = new BufferInfo<SVertex>;
    bufferInfo->reference = (CCSafeArray<SVertex>)buffer;
    if (!_addBuffer(bufferInfo, bindLoc, 3, 4, buffer.getSize(), &buffer[0]))
        delete bufferInfo;
}

void CGRVertexBuffer::addBuffer(CUVArray buffer, U32 bindLoc)
{
    BufferInfo<SUV>* bufferInfo = new BufferInfo<SUV>;
    bufferInfo->reference = (CCSafeArray<SUV>)buffer;
    if (!_addBuffer(bufferInfo, bindLoc, 2, 4, buffer.getSize(), &buffer[0]))
        delete bufferInfo;
}

void CGRVertexBuffer::addBuffer(CScalarArray buffer, U32 bindLoc)
{
    BufferInfo<SScalar>* bufferInfo = new BufferInfo<SScalar>;
    bufferInfo->reference = (CCSafeArray<SScalar>)buffer;
    if (!_addBuffer(bufferInfo, bindLoc, 1, 4, buffer.getSize(), &buffer[0]))
        delete bufferInfo;
}

void CGRVertexBuffer::activate()
{
    if (mActivated)
    {
        CCLogger::LogError("CGRVertexBuffer::activate()", "Vertex buffer already activated");
        return;
    }
    
    // First, calculate the size of the buffer. Currently assume everything is a float array
    U32 totalSize = 0;
    U32 elementSize = 0;
    for (std::vector<BufferInfoBase*>::iterator it = mBuffers.begin(); it != mBuffers.end();  it++)
    {
        totalSize += (*it)->compCount*(*it)->elementCount;
        elementSize += (*it)->compCount;
    }
    
    // Allocate our storage buffer
    mVertexArray = new F32[totalSize];
    
    // Loop through all the elements and interleave all the attributes, as in, they are stored in an array of elements, with each element having a single element of each attribute.
    // Example: Position0, UV0, Normal0, Position1, UV1, Normal1, etc.
    for (U32 element = 0; element < mElementCount; element++)
    {
        U32 offset = 0;
        for (std::vector<BufferInfoBase*>::iterator it = mBuffers.begin(); it != mBuffers.end();  it++)
        {
            // Copy in the data
            memcpy(&mVertexArray[element*elementSize+offset], &((F32*)(*it)->data)[element*(*it)->compCount], sizeof(F32)*(*it)->compCount);
            offset += (*it)->compCount;
        }
    }
    
    // Set up the bind infos for each vertex attribute so the GPU knows how to access the data
    U32 offset = 0;
    for (std::vector<BufferInfoBase*>::iterator it = mBuffers.begin(); it != mBuffers.end();  it++)
    {
        BindInfo bind;
        bind.bindLoc = (*it)->attribBind;
        bind.offset = offset*sizeof(F32);
        bind.stride = (elementSize)*sizeof(F32);
        bind.compSize = (*it)->compSize;
        bind.compCount = (*it)->compCount;
        mBindInfos.push_back(bind);
        offset += (*it)->compCount;
    }
    
    // Tell our GPU overlords to submit this buffer or whatever
    _allocate(totalSize*sizeof(F32));
    
    // We are finished with our buffer infos. Delete them to release the references
    for (std::vector<BufferInfoBase*>::iterator it = mBuffers.begin(); it != mBuffers.end(); it++)
        delete (*it);
    mBuffers.clear();
    
    // If we are using VBOs, the vertex data is on the GPU, so we don't need our local copy
    if (mUseVBOs)
    {
        delete [] mVertexArray;
        mVertexArray = NULL;
    }
    
    // We are activated
    mActivated = true;
}

void CGRVertexBuffer::bind()
{
    if (!mActivated)
    {
        CCLogger::LogError("CGRVertexBuffer::bind()", "Cannot bind non-activated vertex buffer");
        return;
    }
    _bind();
}

void CGRVertexBuffer::unbind()
{
    if (!mActivated)
    {
        CCLogger::LogError("CGRVertexBuffer::unbind()", "Cannot unbind non-activated vertex buffer");
        return;
    }
    _unbind();
}
