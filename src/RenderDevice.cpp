//
//  renderDevice.cpp
//  mythreal
//
//  Created by Ian Ewell on 9/30/12.
//
//

#include "RenderDevice.h"
#include "Logger.h"

CGRRenderDevice::CGRRenderDevice()
{
    // Setup default viewport
    mViewport.x = 0;
    mViewport.y = 0;
    mViewport.width = 768;
    mViewport.height = 1024;
    
    // Dirty variables
    mViewportDirty = false;
    mWorldMatrixDirty = false;
    mViewMatrixDirty = false;
    mProjectionMatrixDirty = false;
    mFrustumDirty = false;
    
    // Matrices
    mWorldMatrixStackPos = 0;
    mWorldMatrix[mWorldMatrixStackPos].setIdentity();
    mViewMatrix.setIdentity();
    mProjectionMatrix.setIdentity();
}

CGRRenderDevice::~CGRRenderDevice()
{
    
}

void CGRRenderDevice::initialize()
{
    
}

void CGRRenderDevice::destroy()
{
    
}

void CGRRenderDevice::pushDebugMarker(std::string markerName)
{
    
}

void CGRRenderDevice::popDebugMarker()
{
    
}

void CGRRenderDevice::beginFrame()
{
    
}

void CGRRenderDevice::endFrame()
{
    
}

void CGRRenderDevice::setFrustum(CMFrustum &frustum)
{
    mFrustum = frustum;
    mFrustumDirty = true;
}

CMFrustum CGRRenderDevice::getFrustum()
{
    return mFrustum;
}

void CGRRenderDevice::pushWorldMatrix()
{
    if (mWorldMatrixStackPos < WORLD_MATRIX_STACK_SIZE-1)
    {
        mWorldMatrixStackPos++;
        mWorldMatrix[mWorldMatrixStackPos] = mWorldMatrix[mWorldMatrixStackPos-1];
        mWorldMatrixDirty = true;
    }
    else
    {
        CCLogger::LogError("CGRRenderDevice::pushWorldMatrix()", "Attempted world matrix stack overflow");
    }
}

void CGRRenderDevice::popWorldMatrix()
{
    if (mWorldMatrixStackPos > 0)
    {
        mWorldMatrixStackPos--;
        mWorldMatrixDirty = true;
    }
    else
    {
        CCLogger::LogError("CGRRenderDevice::popWorldMatrix()", "Attempted world matrix stack underflow");
    }
}

void CGRRenderDevice::setWorldMatrix(CMMatrix4x4 &mat)
{
    mWorldMatrix[mWorldMatrixStackPos] = mat;
    mWorldMatrixDirty = true;
}

CMMatrix4x4 CGRRenderDevice::getWorldMatrix()
{
    return mWorldMatrix[mWorldMatrixStackPos];
}

void CGRRenderDevice::setViewMatrix(CMMatrix4x4 &mat)
{
    mViewMatrix = mat;
    mViewMatrixDirty = true;
}

CMMatrix4x4 CGRRenderDevice::getViewMatrix()
{
    return mViewMatrix;
}

CMMatrix4x4 CGRRenderDevice::getPerspectiveMatrix()
{
    return mProjectionMatrix;
}