//
//  ShaderGLES.cpp
//  mythreal
//
//  Created by Ian Ewell on 4/17/13.
//
//

#include <stdlib.h>
#include "ShaderGLES.h"
#include "VertexBuffer.h"
#include <GLES2/gl2.h>

bool CGRShaderGLES::_compile()
{
    GLint status = 0;
    mShaderID = glCreateProgram();
    
    for (U32 i = 0; i < SHADER_COUNT; i++)
    {
        if (!_comileShader(i))
        {
            glDeleteProgram(mShaderID);
            return false;
        }
    }
    
    // Bind the vertex attributes to our program
    glBindAttribLocation(mShaderID, ATTRIB_VERTEX, "position");
    glBindAttribLocation(mShaderID, ATTRIB_UV, "texCoord");
    glBindAttribLocation(mShaderID, ATTRIB_NORMAL, "normal");
    glBindAttribLocation(mShaderID, ATTRIB_WEIGHTS, "weights");
    glBindAttribLocation(mShaderID, ATTRIB_INFLUENCES, "influences");
    
    // Now link the shader
    glLinkProgram(mShaderID);
    glGetProgramiv(mShaderID, GL_LINK_STATUS, &status);
    
    return true;
}

void CGRShaderGLES::_initUniforms()
{
    GLint uniformCount;
    GLint maxLength;
    glGetProgramiv(mShaderID, GL_ACTIVE_UNIFORMS, &uniformCount);
    glGetProgramiv(mShaderID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);
    char uniformName[maxLength];
    for (U32 i = 0; i < uniformCount; i++)
    {
        GLint size;
        GLenum type;
        glGetActiveUniform(mShaderID, i, maxLength, NULL, &size, &type, &uniformName[0]);
        
        std::string name = uniformName;
        name = name.substr(0, name.find('['));
        
        Uniform uniform;
        uniform._name = name;
        uniform._bindloc = i;
        uniform._size = size;
        switch (type)
        {
            case GL_FLOAT:
                uniform._type = UNIFORM_FLOAT;
                break;
            case GL_FLOAT_VEC2:
                uniform._type = UNIFORM_VECTOR2;
                break;
            case GL_FLOAT_VEC3:
                uniform._type = UNIFORM_VECTOR3;
                break;
            case GL_FLOAT_VEC4:
                uniform._type = UNIFORM_VECTOR4;
                break;
            case GL_FLOAT_MAT4:
                uniform._type = UNIFORM_MATRIX4;
                break;
            case GL_SAMPLER_2D:
                uniform._type = UNIFORM_SAMPLER2D;
                break;
            case GL_SAMPLER_CUBE:
                uniform._type = UNIFORM_SAMPLER_CUBE;
                break;
            default:
                break;
        }
        mUniforms.push_back(uniform);
    }
}

bool CGRShaderGLES::_comileShader(U32 shader)
{
    GLuint sh;
    switch (shader)
    {
        case SHADER_VERTEX:
            sh = glCreateShader(GL_VERTEX_SHADER);
            break;
        case SHADER_PIXEL:
            sh = glCreateShader(GL_FRAGMENT_SHADER);
            break;
        default:
            CCLogger::LogError("CGRShaderGLES::_compileShader()", "Invalid shader type");
            break;
    }
    
    // Append some stuffs to the shader
    std::string shaderSource = "#define VERSION 100\n";
    shaderSource += "#define GLES_SHADER\n";
    shaderSource += mShaders[shader];
    
    const GLchar *src = shaderSource.c_str();
    glShaderSource(sh, 1, &src, NULL);
    glCompileShader(sh);
    
    GLint status, loglength;
    glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &loglength);
    if (loglength > 0)
    {
        GLchar *log = (GLchar *)malloc(loglength);
        glGetShaderInfoLog(sh, loglength, &loglength, log);
        CCLogger::Log("CGRShaderGLES::_compileShader()", log);
        free(log);
    }
    
    glGetShaderiv(sh, GL_COMPILE_STATUS, &status);
    if (status == 0)
    {
        glDeleteShader(sh);
        return false;
    }
    
    glAttachShader(mShaderID, sh);
    return true;
}

void CGRShaderGLES::_uniformValues(const CGRUniformBufferHandle &handle)
{
    const std::vector<CGRUniformHandle> *uniforms = handle->getUniforms();
    for (auto it = uniforms->begin(); it != uniforms->end(); it++)
    {
        if ((*it)->_dirty)
            continue;
        switch ((*it)->uniformType)
        {
            case UNIFORM_FLOAT:
                glUniform1fv((GLint)(*it)->_bindLoc, (GLsizei)(*it)->size, (GLfloat*)&(handle->mData[(*it)->_offset]));
                break;
            case UNIFORM_VECTOR2:
                glUniform2fv((GLint)(*it)->_bindLoc, (GLsizei)(*it)->size, (GLfloat*)&(handle->mData[(*it)->_offset]));
                break;
            case UNIFORM_VECTOR3:
                glUniform3fv((GLint)(*it)->_bindLoc, (GLsizei)(*it)->size, (GLfloat*)&(handle->mData[(*it)->_offset]));
                break;
            case UNIFORM_VECTOR4:
                glUniform4fv((GLint)(*it)->_bindLoc, (GLsizei)(*it)->size, (GLfloat*)&(handle->mData[(*it)->_offset]));
                break;
            case UNIFORM_MATRIX4:
                glUniformMatrix4fv((GLint)(*it)->_bindLoc, (GLsizei)(*it)->size, GL_FALSE, (GLfloat*)&(handle->mData[(*it)->_offset]));
                break;
        }
        (*it)->_dirty = false;
    }
}

void CGRShaderGLES::_bind(const CGRUniformBufferHandle &handle)
{
    glUseProgram(mShaderID);
    _uniformValues(handle);
}

void CGRShaderGLES::_unbind()
{
    glUseProgram(0);
}
