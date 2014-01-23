//
//  renderDevice.h
//  mythreal
//
//  Created by Ian Ewell on 9/30/12.
//
//

#ifndef __mythreal__renderDevice__
#define __mythreal__renderDevice__

#include <iostream>
#include <string>

#include "Matrix.h"
#include "Frustum.h"
#include "Types.h"
#include "VertexBuffer.h"

#define WORLD_MATRIX_STACK_SIZE 8

// Structure for viewport data
typedef struct
{
    U32 x;
    U32 y;
    U32 width;
    U32 height;
} SGRViewport;

// Matrix binding types
typedef enum
{
    GRWorldTransform,
    GRViewTransform,
    GRWorldViewTransform,
    GRProjectionTransform,
} EGRMatrixTransformationType;

// This defines a high level interface to the underlying graphics hardware, and is
// designed to primarily work with OpenGL or OpenGL ES 2.0
class CGRRenderDevice
{
public:
    CGRRenderDevice();
    virtual ~CGRRenderDevice();
    
    // Initialization/Deinitialization functions
    virtual void initialize();
    virtual void destroy();
    
    // Debugging functions
    virtual void pushDebugMarker(std::string markerName);
    virtual void popDebugMarker();

protected:
    //
    // Internal Variables
    //
    
    bool mInitialized;
    
protected:
    //
    // Render state variables
    //
    
    SGRViewport mViewport;
    bool mViewportDirty;
    
    // Matrices
    CMMatrix4x4 mWorldMatrix[WORLD_MATRIX_STACK_SIZE];
    U32 mWorldMatrixStackPos;
    bool mWorldMatrixDirty;
    
    CMMatrix4x4 mViewMatrix;
    bool mViewMatrixDirty;
    
    CMMatrix4x4 mProjectionMatrix;
    bool mProjectionMatrixDirty;
    
    CMFrustum mFrustum;
    bool mFrustumDirty;
    
protected:
    //
    // Internal functions that may be overridden by subclass renderer
    //
    
    // Internal functions for handling the beginning and end of a frame
    virtual void _beginFrame()=0;
    virtual void _endFrame()=0;
    
    // Loads a matrix into the renderer, which will eventually be bound to a shader for transformations
    virtual void bindMatrix(EGRMatrixTransformationType transformType, CMMatrix4x4 &bindMatrix)=0;
    
public:
    //
    // Public Graphics API
    //
    
    void beginFrame();
    void endFrame();
    
    virtual void clearRenderBuffer(U32 flags, CMVector3 clearColor, F32 clearDepth, U32 clearStencil)=0;
    
    void setFrustum(CMFrustum &frustum);
    CMFrustum getFrustum();
    
    void pushWorldMatrix();
    void popWorldMatrix();
    void setWorldMatrix(CMMatrix4x4 &mat);
    CMMatrix4x4 getWorldMatrix();
    
    void setViewMatrix(CMMatrix4x4 &mat);
    CMMatrix4x4 getViewMatrix();
    
    CMMatrix4x4 getPerspectiveMatrix();
    
    // Buffers
    virtual CGRVertexBufferRef createVertexBuffer(CVertexArray array, EVBOType type)=0;
    virtual CGRVertexBufferRef createVertexBuffer(CUVArray array, EVBOType type)=0;
};

#endif /* defined(__mythreal__renderDevice__) */
