//
//  Shader.h
//  mythreal
//
//  Created by Ian Ewell on 4/13/13.
//
//

#ifndef __mythreal__Shader__
#define __mythreal__Shader__

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "SafeArray.h"
#include "Vector.h"
#include "Matrix.h"

enum
{
    SHADER_VERTEX,
    SHADER_PIXEL,
    SHADER_COUNT,
};

enum
{
    UNIFORM_FLOAT,
    UNIFORM_VECTOR2,
    UNIFORM_VECTOR3,
    UNIFORM_VECTOR4,
    UNIFORM_MATRIX4,
    UNIFORM_SAMPLER2D,
    UNIFORM_SAMPLER_CUBE,
    UNIFORM_COUNT,
};

// A handle that is used by a uniform buffer to set the value of a uniform
class CGRUniform : public CCReferenceBase
{
protected:
    friend class CGRShader;
    friend class CGRShaderGLES;
    friend class CGRUniformBuffer;
public:
    CGRUniform() {}
    ~CGRUniform() {};
    
    U32 uniformType;
    U32 size;
    std::string name;
protected:
    U32 _shaderID;
    U32 _offset;
    U32 _bindLoc;
    bool _dirty;
};

typedef CCPointerStrong<CGRUniform> CGRUniformHandle;

class CGRUniformBuffer : public CCReferenceBase
{
protected:
    friend class CGRShader;
    friend class CGRShaderGLES;
    
    // Disable direct creation of these
    CGRUniformBuffer(U32 shaderid);
    
    // List of all the uniforms in this buffer
    std::vector<CGRUniformHandle> mUniforms;
    
    // Map of the names to the actual uniforms
    std::map<std::string, CGRUniformHandle> mUniformMap;
    
    // Uniform buffers must be linked to a certain shader
    U32 mShaderID;
    
    // Keep track of our top offset as more values are added
    U32 mOffset;
    
    // Buffer of the data
    U8 *mData;
    
    void _addUniform(const std::string &name, U32 type, U32 size, U32 bindloc);
    void _allocateBuffer();
    
    // Internal template functions to copy data into the buffer. One is for single
    // values, and the other is for an array of values
    template <class T> void _uniformValue(const CGRUniformHandle &handle, const CCSafeArray<T> val)
    {
        if (!mData)
        {
            CCLogger::LogError("CGRUniformBuffer::_uniformValue()", "Uniform buffer not inited!");
            return;
        }
        
        if (handle->_shaderID != mShaderID)
        {
            CCLogger::LogError("CGRUniformBuffer::_uniformValue()", "Incorrect shader ID in uniform handle.");
            return;
        }
        
        // If the supplied array is smaller than requested, we will have to underfill the buffer
        U32 size = handle->size;
        if (handle->size > val.getSize())
        {
            size = val.getSize();
        }
        
        if (memcmp(&mData[handle->_offset], &val[0], size*sizeof(T)) == 0)
        {
            memcpy(&mData[handle->_offset], &val[0], size*sizeof(T));
            handle->_dirty = true;
        }
    }
    
    template <class T> void _uniformValue(CGRUniformHandle &handle, const T val)
    {
        if (!mData)
        {
            CCLogger::LogError("CGRUniformBuffer::_uniformValue()", "Uniform buffer not inited!");
            return;
        }
        
        if (handle->_shaderID != mShaderID)
        {
            CCLogger::LogError("CGRUniformBuffer::_uniformValue()", "Incorrect shader ID in uniform handle.");
            return;
        }
        
        if (((T*)mData)[handle->_offset/sizeof(T)] != val)
        {
            ((T*)mData)[handle->_offset/sizeof(T)] = val;
            handle->_dirty = true;
        }
    }
public:
    virtual ~CGRUniformBuffer();
    
    // Uniform accessing
    const std::vector<CGRUniformHandle>* getUniforms() const;
    CGRUniformHandle getUniformByName(const std::string &name) const;
    
    // These functions are used to supply uniform values and must supply an appropriate handle
    // These functions will accept a single value or an array for each uniform type
    void uniformValue(CGRUniformHandle &handle, F32 val);
    void uniformValue(CGRUniformHandle &handle, const CCSafeArray<F32> val);
    void uniformValue(CGRUniformHandle &handle, const CMVector2 val);
    void uniformValue(CGRUniformHandle &handle, const CCSafeArray<CMVector2> val);
    void uniformValue(CGRUniformHandle &handle, const CMVector3 val);
    void uniformValue(CGRUniformHandle &handle, const CCSafeArray<CMVector3> val);
    void uniformValue(CGRUniformHandle &handle, const CMVector4 val);
    void uniformValue(CGRUniformHandle &handle, const CCSafeArray<CMVector4> val);
    void uniformValue(CGRUniformHandle &handle, const CMMatrix4x4 val);
    void uniformValue(CGRUniformHandle &handle, const CCSafeArray<CMMatrix4x4> val);

};

typedef CCPointerStrong<CGRUniformBuffer> CGRUniformBufferHandle;

class CGRShader : public CCReferenceBase
{
protected:
    std::string mShaders[SHADER_COUNT];
    
    // Set to true once the shader is constructed and immutable
    bool mActivated;
    
    // The shader, or program, ID supplied by the graphics driver
    U32 mShaderID;
    
    // Internal storage of uniform infos
    struct Uniform
    {
    public:
        std::string _name;
        U32 _type;
        U32 _bindloc;
        U32 _size;
    };
    std::vector<Uniform> mUniforms;
    
    // Graphics API specific functions
    virtual bool _compile()=0;
    virtual void _initUniforms()=0;
    virtual void _bind(const CGRUniformBufferHandle &handle)=0;
    virtual void _unbind()=0;
    
public:
    CGRShader();
    virtual ~CGRShader(){};
    
    // Adds a shader source of a particular type to this shader construct
    void addShader(U32 shaderType, const std::string &shaderSource);
    
    // Activates (i.e. compiles and links) the shader and allows uniforms to be bound to it
    void activate();
    
    // Gets all the uniforms from the shader and allocates a handle buffer to store everything
    CGRUniformBufferHandle createUniformBuffer();
    
    // Binds and unbinds the shader
    void bind(const CGRUniformBufferHandle &handle);
    void unbind();
    
};

typedef CCPointerStrong<CGRShader> CGRShaderRef;

#endif /* defined(__mythreal__Shader__) */
