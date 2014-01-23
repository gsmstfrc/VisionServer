//
//  VertexBufferGLES.cpp
//  mythreal
//
//  Created by Ian Ewell on 3/25/13.
//
//

#include "VertexBufferGLES.h"

void CGRVertexBufferGLES::_allocate(U32 size)
{
    // We really only need to do something if there are VBOs
    if (mUseVBOs)
    {
        // Generate VBO and upload vertex data to GPU
        glGenBuffers(1, &mVBO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, size, mVertexArray, GL_STATIC_DRAW);
        
    }
}

void CGRVertexBufferGLES::_bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    for (std::vector<BindInfo>::iterator it = mBindInfos.begin(); it != mBindInfos.end(); it++)
    {
        glVertexAttribPointer((*it).bindLoc, (*it).compCount, GL_FLOAT, GL_FALSE, (*it).stride, (GLvoid*)(*it).offset);
        glEnableVertexAttribArray((*it).bindLoc);
    }
}

void CGRVertexBufferGLES::_unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/*void CGRVertexBufferGLES::_fill(const void* data)
{
    glBindBuffer(GL_ARRAY_BUFFER, mBuffer);
    if (!mAllocated)
    {
        GLenum usage;
        switch (mType)
        {
            case CGVBODynamic:
                usage = GL_DYNAMIC_DRAW;
                break;
            case CGVBOStatic:
                usage = GL_STATIC_DRAW;
                break;
            case CGVBOStream:
                usage = GL_STREAM_DRAW;
                break;
        }
        glBufferData(GL_ARRAY_BUFFER, mSize*mComponentCount*sizeof(F32), data, usage);
    }
    else
        glBufferSubData(GL_ARRAY_BUFFER, 0, mSize*mComponentCount*sizeof(F32), data);
}*/

CGRVertexBufferGLES::~CGRVertexBufferGLES()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
