//
//  Shader.cpp
//  mythreal
//
//  Created by Ian Ewell on 4/13/13.
//
//

#include "Shader.h"
#include "Logger.h"

CGRUniformBuffer::CGRUniformBuffer(U32 shaderid)
{
    mShaderID = shaderid;
    mOffset = 0;
    mData = NULL;
}

CGRUniformBuffer::~CGRUniformBuffer()
{
    if (mData)
        delete [] mData;
}

void CGRUniformBuffer::_addUniform(const std::string &name, U32 type, U32 size, U32 bindloc)
{
    CGRUniformHandle uniform;
    uniform = new CGRUniform();
    uniform->name = name;
    uniform->uniformType = type;
    uniform->size = size;
    uniform->_bindLoc = bindloc;
    uniform->_shaderID = mShaderID;
    uniform->_dirty = true;
    
    // Find out what we are dealing with and calculate the total size
    U32 totalSize = 0;
    switch (type)
    {
        case UNIFORM_FLOAT:
            totalSize = size*sizeof(F32);
            break;
        case UNIFORM_VECTOR2:
            totalSize = size*sizeof(CMVector2);
            break;
        case UNIFORM_VECTOR3:
            totalSize = size*sizeof(CMVector3);
            break;
        case UNIFORM_VECTOR4:
            totalSize = size*sizeof(CMVector4);
            break;
        case UNIFORM_MATRIX4:
            totalSize = size*sizeof(CMMatrix4x4);
            break;
        default:
            // Nothing we can use. Return!
            return;
    }
    
    // Set and increment the offset
    uniform->_offset = mOffset;
    mOffset += totalSize;
    
    // Insert this into our vector and map
    mUniforms.push_back(uniform);
    mUniformMap.insert(std::pair<std::string, CGRUniformHandle>(name, uniform));
}

void CGRUniformBuffer::_allocateBuffer()
{
    // Only allocate once
    if (mData)
        return;
    U32 size = 0;
    for (std::vector<CGRUniformHandle>::iterator it = mUniforms.begin(); it != mUniforms.end(); it++)
    {
        switch ((*it)->uniformType)
        {
            case UNIFORM_FLOAT:
                size += (*it)->size*sizeof(F32);
                break;
            case UNIFORM_VECTOR2:
                size += (*it)->size*sizeof(CMVector2);
                break;
            case UNIFORM_VECTOR3:
                size += (*it)->size*sizeof(CMVector3);
                break;
            case UNIFORM_VECTOR4:
                size += (*it)->size*sizeof(CMVector4);
                break;
            case UNIFORM_MATRIX4:
                size += (*it)->size*sizeof(CMMatrix4x4);
                break;
            default:
                // Should not be here
                return;
        }
    }
    mData = new U8[size];
}

const std::vector<CGRUniformHandle>* CGRUniformBuffer::getUniforms() const
{
    return &mUniforms;
}

CGRUniformHandle CGRUniformBuffer::getUniformByName(const std::string &name) const
{
    return mUniformMap.find(name)->second;
}

void CGRUniformBuffer::uniformValue(CGRUniformHandle &handle, F32 val)
{
    if (handle->uniformType != UNIFORM_FLOAT)
    {
        return;
    }
    _uniformValue(handle, val);
}

void CGRUniformBuffer::uniformValue(CGRUniformHandle &handle, CCSafeArray<F32> val)
{
    if (handle->uniformType != UNIFORM_FLOAT)
    {
        return;
    }
    _uniformValue(handle, val);
}

void CGRUniformBuffer::uniformValue(CGRUniformHandle &handle, CMVector2 val)
{
    if (handle->uniformType != UNIFORM_VECTOR2)
    {
        return;
    }
    _uniformValue(handle, val);
}

void CGRUniformBuffer::uniformValue(CGRUniformHandle &handle, CCSafeArray<CMVector2> val)
{
    if (handle->uniformType != UNIFORM_VECTOR2)
    {
        return;
    }
    _uniformValue(handle, val);
}

void CGRUniformBuffer::uniformValue(CGRUniformHandle &handle, CMVector3 val)
{
    if (handle->uniformType != UNIFORM_VECTOR3)
    {
        return;
    }
    _uniformValue(handle, val);
}

void CGRUniformBuffer::uniformValue(CGRUniformHandle &handle, CCSafeArray<CMVector3> val)
{
    if (handle->uniformType != UNIFORM_VECTOR3)
    {
        return;
    }
    _uniformValue(handle, val);
}

void CGRUniformBuffer::uniformValue(CGRUniformHandle &handle, CMVector4 val)
{
    if (handle->uniformType != UNIFORM_VECTOR4)
    {
        return;
    }
    _uniformValue(handle, val);
}

void CGRUniformBuffer::uniformValue(CGRUniformHandle &handle, CCSafeArray<CMVector4> val)
{
    if (handle->uniformType != UNIFORM_VECTOR4)
    {
        return;
    }
    _uniformValue(handle, val);
}

void CGRUniformBuffer::uniformValue(CGRUniformHandle &handle, CMMatrix4x4 val)
{
    if (handle->uniformType != UNIFORM_MATRIX4)
    {
        return;
    }
    _uniformValue(handle, val);
}

void CGRUniformBuffer::uniformValue(CGRUniformHandle &handle, CCSafeArray<CMMatrix4x4> val)
{
    if (handle->uniformType != UNIFORM_MATRIX4)
    {
        return;
    }
    _uniformValue(handle, val);
}

/////////////////////////////////////////////////////////////////////////////////////////////

CGRShader::CGRShader()
{
    mActivated = false;
}

void CGRShader::addShader(U32 shaderType, const std::string &shaderSource)
{
    if (shaderType >= SHADER_COUNT)
    {
        CCLogger::LogError("CGRShader::addShader()", "Invalid shader type");
        return;
    }
    mShaders[shaderType] = shaderSource;
}

void CGRShader::activate()
{
    if (_compile())
    {
        _initUniforms();
        mActivated = true;
    }
}

CGRUniformBufferHandle CGRShader::createUniformBuffer()
{
    if (!mActivated)
    {
        CCLogger::LogError("CGRShader::createUniformBuffer()", "Can't create buffer from non-active shader");
        return CGRUniformBufferHandle();
    }
    CGRUniformBufferHandle handle;
    handle = new CGRUniformBuffer(mShaderID);
    for (auto it = mUniforms.begin(); it != mUniforms.end(); it++)
    {
        handle->_addUniform((*it)._name, (*it)._type, (*it)._size, (*it)._bindloc);
    }
    return handle;
}

void CGRShader::bind(const CGRUniformBufferHandle &handle)
{
    if (!mActivated)
    {
        CCLogger::LogError("CGRShader::bind()", "Cannot bind non-active shader");
        return;
    }
    _bind(handle);
}

void CGRShader::unbind()
{
    if (!mActivated)
    {
        CCLogger::LogError("CGRShader::unbind()", "Cannot unbind non-active shader");
        return;
    }
    _unbind();
}