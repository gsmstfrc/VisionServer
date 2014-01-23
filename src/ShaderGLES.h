//
//  ShaderGLES.h
//  mythreal
//
//  Created by Ian Ewell on 4/17/13.
//
//

#ifndef __mythreal__ShaderGLES__
#define __mythreal__ShaderGLES__

#include <iostream>
#include "Shader.h"

class CGRShaderGLES : public CGRShader
{
protected:
    virtual bool _compile();
    virtual void _initUniforms();
    virtual void _bind(const CGRUniformBufferHandle &handle);
    virtual void _unbind();
    
    bool _comileShader(U32 shader);
    void _uniformValues(const CGRUniformBufferHandle &handle);
};

#endif /* defined(__mythreal__ShaderGLES__) */
