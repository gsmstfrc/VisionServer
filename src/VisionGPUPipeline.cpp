//
// FRC Vision Server
// FRC 2014
// FRC Team 3318
// Written by Ian Ewell <ianewellgmsmt@gmail.com>
// (C) 2014 GSMST Robotics
//

#include <iostream>
#include <bcm_host.h>
#include "VisionGPUPipeline.h"

unsigned int _getMilliseconds();

GPUFramebuffer::GPUFramebuffer() :
                    mFBWidth(128),
                    mFBHeight(128),
                    mFramebuffer(0),
                    mRenderToTexture(true),
                    mTextureID(0)
{
}

GPUFramebuffer::GPUFramebuffer(U32 width, U32 height) :
                    mFBWidth(width),
                    mFBHeight(height),
                    mFramebuffer(0),
                    mRenderToTexture(true),
                    mTextureID(0)
{
}

GPUFramebuffer::~GPUFramebuffer()
{
    if (mFramebuffer)
        destroy();
}

bool GPUFramebuffer::create()
{
    glGenFramebuffers(1, &mFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);

    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mFBWidth, mFBHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureID, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Error: could not create framebuffer\n";
        destroy();
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void GPUFramebuffer::destroy()
{
}

void GPUFramebuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
}

void GPUFramebuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

////////////////////////////////////////////////////////////////

VisionGPUPass::VisionGPUPass() : mInputWidth(640),
                                 mInputHeight(480),
                                 mOutputWidth(640),
                                 mOutputHeight(480),
                                 mRenderToScreen(false),
                                 mVertexShaderFile("shaders/default.vs"),
                                 mFragmentShaderFile("shaders/default.fs")
{
}

VisionGPUPass::~VisionGPUPass()
{
    _destroyVertexBuffers();
    _destroyShaders();
    _destroyFramebuffer();
}

bool VisionGPUPass::_allocFrameBuffer()
{
    mFramebuffer.setWidth(mOutputWidth);
    mFramebuffer.setHeight(mOutputHeight);
    mFramebuffer.setRenderToTexture(true);
    std::cout << mOutputWidth << "x" << mOutputHeight << "FB created\n";
    return mFramebuffer.create();
}

void VisionGPUPass::_destroyFramebuffer()
{
    mFramebuffer.destroy();
}

bool VisionGPUPass::_loadShaders()
{
    mShader = new CGRShaderGLES;

    CCFileStream vertStream;
    if (!vertStream.openFile(mVertexShaderFile, "r")) {
        std::cout << "Error: Could not load vertex shader\n";
        return false;
    }
    U32 vertSize = vertStream.getSize();
    char *vertShaderSrc = new char[vertSize];
    vertStream.read(vertSize, vertShaderSrc);
    vertStream.closeFile();
    mShader->addShader(SHADER_VERTEX, vertShaderSrc);
    delete[] vertShaderSrc;

    CCFileStream fragStream;
    if (!fragStream.openFile(mFragmentShaderFile, "r")) {
        std::cout << "Error: Could not load fragment shader\n";
        return false;
    }
    U32 fragSize = fragStream.getSize();
    char *fragShaderSrc = new char[fragSize];
    fragStream.read(fragSize, fragShaderSrc);
    fragStream.closeFile();
    mShader->addShader(SHADER_PIXEL, fragShaderSrc);
    delete[] fragShaderSrc;

    mShader->activate();
    mUniformBuffer = mShader->createUniformBuffer();
    
    return true;
}

void VisionGPUPass::_destroyShaders()
{
}

bool VisionGPUPass::_setupVertexBuffers()
{
    mVertexBuffer = new CGRVertexBufferGLES;

    // Vertex array for rectangle
    CVertexArray vertices(6);
    vertices[0] = {-1.0f, -1.0f, 0.0f };
    vertices[1] = {1.0f, -1.0f, 0.0f };
    vertices[2] = {1.0f, 1.0f, 0.0f };
    vertices[3] = {1.0f, 1.0f, 0.0f };
    vertices[4] = {-1.0f, 1.0f, 0.0f };
    vertices[5] = {-1.0f, -1.0f, 0.0f };
    mVertexBuffer->addBuffer(vertices, ATTRIB_VERTEX);

    // UV array for rectangle
    CUVArray uvs(6);
    uvs[0] = {0.0f, 1.0f };
    uvs[1] = {1.0f, 1.0f };
    uvs[2] = {1.0f, 0.0f };
    uvs[3] = {1.0f, 0.0f };
    uvs[4] = {0.0f, 0.0f };
    uvs[5] = {0.0f, 1.0f };
    mVertexBuffer->addBuffer(uvs, ATTRIB_UV);

    mVertexBuffer->activate();

    return true;
}

void VisionGPUPass::_destroyVertexBuffers()
{
}

void VisionGPUPass::_bindResources()
{
    if (mRenderToScreen)
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    else
        mFramebuffer.bind();
    mShader->bind(mUniformBuffer);
    mVertexBuffer->bind();
}

void VisionGPUPass::_unbindResources()
{
    mVertexBuffer->unbind();
    mShader->unbind();
    glBindTexture(GL_TEXTURE_2D, 0);
    mFramebuffer.unbind();
}

void VisionGPUPass::_setShaderParams()
{
    CGRUniformHandle texHandle = mUniformBuffer->getUniformByName("inTex");
    TextureData tex = {mInputImageID};
    mUniformBuffer->uniformValue(texHandle, tex);
}

void VisionGPUPass::_draw()
{
    glClear(GL_COLOR_BUFFER_BIT);
    if (mRenderToScreen)
        glViewport(100, 100, mOutputWidth, mOutputHeight);
    else
        glViewport(0, 0, mOutputWidth, mOutputHeight);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

bool VisionGPUPass::initialize()
{
    if (!_allocFrameBuffer())
       return false;
    if (!_loadShaders())
        return false;
    if (!_setupVertexBuffers())
        return false;
    return true;
}

void VisionGPUPass::execute()
{
    _setShaderParams();
    _bindResources();
    _draw();
    _unbindResources();
}

void VisionGPUPass::saveImage(const std::string &location)
{
    char *imagebuff = new char[mOutputWidth*mOutputHeight*4];
    mFramebuffer.bind();
    glReadPixels(0, 0, mOutputWidth, mOutputHeight, GL_RGBA,
                 GL_UNSIGNED_BYTE, imagebuff);
    mFramebuffer.unbind();
    CCFileStream outstream;
    outstream.openFile(location, "w");
    outstream.write(mOutputWidth*mOutputHeight*4, imagebuff);
    outstream.closeFile();
    delete[] imagebuff;
}

void VisionGPUPass::readToBuffer(U8 *buffer)
{
    mFramebuffer.bind();
    glReadPixels(0, 0, mOutputWidth, mOutputHeight,
                 GL_RGBA, GL_UNSIGNED_BYTE, buffer);
    mFramebuffer.unbind();
}

////////////////////////////////////////////////////////////////

VisionThresholdPass::VisionThresholdPass() : mMinColors(0.0, 0.0, 0.0, 0.0),
                                             mMaxColors(1.0, 1.0, 1.0, 1.0)
{
    mVertexShaderFile = "shaders/threshold.vs";
    mFragmentShaderFile = "shaders/threshold.fs";
}

void VisionThresholdPass::_setShaderParams()
{
    CGRUniformHandle texHandle = mUniformBuffer->getUniformByName("inTex");
    TextureData tex = {mInputImageID};
    mUniformBuffer->uniformValue(texHandle, tex);

    //CGRUniformHandle minHandle = mUniformBuffer->getUniformByName("mincolor");
    //mUniformBuffer->uniformValue(minHandle, mMinColors);
    //CGRUniformHandle maxHandle = mUniformBuffer->getUniformByName("maxcolor");
    //mUniformBuffer->uniformValue(maxHandle, mMaxColors);
}

////////////////////////////////////////////////////////////////

VisionPackPass::VisionPackPass()
{
    mInputWidth = 640;
    mInputHeight = 480;
    mOutputWidth = mInputWidth/32;
    mOutputHeight = mInputHeight;
    mVertexShaderFile = "shaders/pack.vs";
    mFragmentShaderFile = "shaders/pack.fs";
}

bool VisionPackPass::_setupVertexBuffers()
{
    mVertexBuffer = new CGRVertexBufferGLES;

    // Custom geometry setup to ensure we lookup with the right UVs
    U32 triCount = mOutputWidth;
    CVertexArray vertices(triCount*2);
    CUVArray uvs(triCount*2);
    F32 increment = 32.0f/(F32)mInputWidth;
    F32 uv = 0.0f;
    F32 minvt = -1.0f;
    F32 maxvt = -1.0f + 2*increment;
    int vt = 0;
    for (int i = 0; i < mOutputWidth; i++) {
        vertices[vt] = {minvt, -1.0f, 0.0f };
        vertices[vt+1] = {minvt, 1.0f, 0.0f };
        //vertices[vt+2] = {maxvt, 1.0f, 0.0f };
        //vertices[vt+3] = {maxvt, 1.0f, 0.0f };
        //vertices[vt+4] = {minvt, 1.0f, 0.0f };
        //vertices[vt+5] = {minvt, -1.0f, 0.0f };

        uvs[vt] = {uv, 0.0f };
        uvs[vt+1] = {uv, 1.0f };
        //uvs[vt+2] = {uv, 0.0f };
        //uvs[vt+3] = {uv, 0.0f };
        //uvs[vt+4] = {uv, 0.0f };
        //uvs[vt+5] = {uv, 1.0f };

        uv += increment;
        minvt += 2*increment;
        maxvt += 2*increment;
        vt += 2;
    }
    mVertexBuffer->addBuffer(vertices, ATTRIB_VERTEX);
    mVertexBuffer->addBuffer(uvs, ATTRIB_UV);
    mVertexBuffer->activate();
    return true;
}

void VisionPackPass::_draw()
{
    glClear(GL_COLOR_BUFFER_BIT);
    if (mRenderToScreen)
        glViewport(100, 100, mOutputWidth, mOutputHeight);
    else
        glViewport(0, 0, mOutputWidth, mOutputHeight);
    glDrawArrays(GL_LINES, 0, mOutputWidth*2);
}

void VisionPackPass::_setShaderParams()
{
    CGRUniformHandle texHandle = mUniformBuffer->getUniformByName("inTex");
    TextureData tex = {mInputImageID};
    mUniformBuffer->uniformValue(texHandle, tex);

    CGRUniformHandle increment = mUniformBuffer->getUniformByName("increment");
    mUniformBuffer->uniformValue(increment, 1/(F32)mInputWidth);
}
////////////////////////////////////////////////////////////////

VisionGPUPipeline::VisionGPUPipeline() : mInputWidth(640),
                                         mInputHeight(480)
{
}

VisionGPUPipeline::~VisionGPUPipeline()
{
}

bool VisionGPUPipeline::_OMAXInit()
{
    int ret;

    mJPGDecoder = new JPGDecoderInfo;
    if (mJPGDecoder == nullptr) {
        std::cout << "Error: Could not create JPG decoder\n";
        return false;
    }

    mJPGDecoder->client = ilclient_init(); 
    if (mJPGDecoder->client == nullptr) {
        std::cout << "Error: Could not create ilclient\n";
        goto gt_error;
    }

    if (OMX_Init() != OMX_ErrorNone) {
        std::cout << "Error: Could not initialize OpenMAX\n";
        goto gt_error;
    }

    // Create the actual decoder component
    mJPGDecoder->decoder = new OMXComponent;
    if (mJPGDecoder->decoder == nullptr) {
        std::cout << "Error: Could not create image decoder component\n";
        goto gt_error;
    }

    ret = ilclient_create_component(mJPGDecoder->client,
                                        &mJPGDecoder->decoder->component,
                                        "image_decode",
                                        (ILCLIENT_CREATE_FLAGS_T)(
                                        ILCLIENT_DISABLE_ALL_PORTS |
                                        ILCLIENT_ENABLE_INPUT_BUFFERS));
    if (ret != 0) {
        std::cout << "Error: Could not create image decoder component\n";
        goto gt_error;
    }

    mJPGDecoder->decoder->handle =
        ILC_GET_HANDLE(mJPGDecoder->decoder->component);

    OMX_PORT_PARAM_TYPE port;
    port.nSize = sizeof(OMX_PORT_PARAM_TYPE);
    port.nVersion.nVersion = OMX_VERSION;

    OMX_GetParameter(mJPGDecoder->decoder->handle,
                     OMX_IndexParamImageInit, &port);
    if (port.nPorts != 2) {
        std::cout << "Error: Expected 2 ports for decoder\n";
        goto gt_error;
    }
    mJPGDecoder->decoder->inport = port.nStartPortNumber;
    mJPGDecoder->decoder->outport = port.nStartPortNumber+1;

    // Create the render component
    mJPGDecoder->glrenderer = new OMXComponent;
    if (mJPGDecoder->glrenderer == nullptr) {
        std::cout << "Error: Could not create image renderer component\n";
        goto gt_error;
    }

    ret = ilclient_create_component(mJPGDecoder->client,
                                        &mJPGDecoder->glrenderer->component,
                                        "egl_render",
                                        (ILCLIENT_CREATE_FLAGS_T)(
                                        ILCLIENT_DISABLE_ALL_PORTS |
                                        ILCLIENT_ENABLE_INPUT_BUFFERS |
                                        ILCLIENT_ENABLE_OUTPUT_BUFFERS));
    if (ret != 0) {
        std::cout << "Error: Could not create image renderer component\n";
        goto gt_error;
    }

    mJPGDecoder->glrenderer->handle =
        ILC_GET_HANDLE(mJPGDecoder->glrenderer->component);

    port.nSize = sizeof(OMX_PORT_PARAM_TYPE);
    port.nVersion.nVersion = OMX_VERSION;

    OMX_GetParameter(mJPGDecoder->glrenderer->handle,
                     OMX_IndexParamVideoInit, &port);
    if (port.nPorts != 2) {
        std::cout << "Error: Expected 2 ports for renderer\n";
        goto gt_error;
    }
    mJPGDecoder->glrenderer->inport = port.nStartPortNumber;
    mJPGDecoder->glrenderer->outport = port.nStartPortNumber+1;

    // Initialize decoder
    ilclient_change_component_state(mJPGDecoder->decoder->component,
                                    OMX_StateIdle);

    // Image parameters
    OMX_IMAGE_PARAM_PORTFORMATTYPE imagePortFormat;
    memset(&imagePortFormat, 0, sizeof(OMX_IMAGE_PARAM_PORTFORMATTYPE));
    imagePortFormat.nSize = sizeof(OMX_IMAGE_PARAM_PORTFORMATTYPE);
    imagePortFormat.nVersion.nVersion = OMX_VERSION;
    imagePortFormat.nPortIndex = mJPGDecoder->decoder->inport;
    imagePortFormat.eCompressionFormat = OMX_IMAGE_CodingJPEG;
    OMX_SetParameter(mJPGDecoder->decoder->handle,
                     OMX_IndexParamImagePortFormat, &imagePortFormat);

    // Buffer requirements
    OMX_PARAM_PORTDEFINITIONTYPE portdef;
    portdef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    portdef.nVersion.nVersion = OMX_VERSION;
    portdef.nPortIndex = mJPGDecoder->decoder->inport;
    portdef.nBufferSize = 1048576; // 1 MB for JPGS
    OMX_GetParameter(mJPGDecoder->decoder->handle,
                     OMX_IndexParamPortDefinition, &portdef);

    // Enable port
    OMX_SendCommand(mJPGDecoder->decoder->handle,
                    OMX_CommandPortEnable,
                    mJPGDecoder->decoder->inport, NULL);
    mJPGDecoder->buffHeaderCount = portdef.nBufferCountActual;
    mJPGDecoder->inBuffHeader =
        (OMX_BUFFERHEADERTYPE**)malloc(sizeof(void*)*
                                       mJPGDecoder->buffHeaderCount);

    for (int i = 0; i < mJPGDecoder->buffHeaderCount; i++) {
        if (OMX_AllocateBuffer(mJPGDecoder->decoder->handle,
                               &mJPGDecoder->inBuffHeader[i],
                               mJPGDecoder->decoder->inport,
                               (void*)i,
                               portdef.nBufferSize) != OMX_ErrorNone) {
            std::cout << "Error: Could not allocate decode buffers\n";
            goto gt_error;
        }
    }

    ret = ilclient_wait_for_event(mJPGDecoder->decoder->component,
                                      OMX_EventCmdComplete,
                                      OMX_CommandPortEnable, 0,
                                      mJPGDecoder->decoder->inport, 0,
                                      0, 2000);
    if (ret != 0) {
        std::cout << "Error: Could not enable ports\n";
        goto gt_error;
    }

    ret = OMX_SendCommand(mJPGDecoder->decoder->handle,
                          OMX_CommandStateSet, OMX_StateExecuting,
                          NULL);
    /*if (ret != 0) {
        std::cout << "Error: Could not start decoder\n";
        goto gt_error;
    }
    ret = ilclient_wait_for_event(mJPGDecoder->decoder->component,
                                  OMX_EventCmdComplete,
                                  OMX_StateExecuting, 0, 0, 1, 0,
                                  2000);
    if (ret != 0) {
        std::cout << "Error: Execution did not start\n";
        goto gt_error;
    }*/

    if (ilclient_change_component_state(mJPGDecoder->decoder->component,
                                        OMX_StateExecuting) == -1) {
        std::cout << "Error: Could not start decoder execution\n";
        goto gt_error;
    }

    std::cout << "Initialized OpenMAX JPG decoder\n";

    return true;

gt_error:
    if (mJPGDecoder != nullptr) {
        if (mJPGDecoder->client != nullptr) {
            ilclient_destroy(mJPGDecoder->client);
        }
        delete mJPGDecoder;
    }
    return false;
}

void VisionGPUPipeline::_OMAXDecodeJPG(char *jpg, size_t length)
{
    char* offset = jpg;
    size_t toRead = length;
    bool bufferFilled = false;
    int bufferIndex = 0;

    while (toRead > 0) {
        OMX_BUFFERHEADERTYPE *header = mJPGDecoder->inBuffHeader[bufferIndex];

        bufferIndex++;
        if (bufferIndex >= mJPGDecoder->buffHeaderCount)
            bufferIndex = 0;

        if (toRead > header->nAllocLen)
            header->nFilledLen = header->nAllocLen;
        else
            header->nFilledLen = toRead;

        toRead = toRead - header->nFilledLen;

        U32 time1 = _getMilliseconds();

        memcpy(header->pBuffer, offset, header->nFilledLen);
        offset += header->nFilledLen;
        header->nOffset = 0;
        header->nFlags = 0;
        if (toRead <= 0)
            header->nFlags = OMX_BUFFERFLAG_EOS;

        int ret = OMX_EmptyThisBuffer(mJPGDecoder->decoder->handle,
                                      header);
        if (ret != OMX_ErrorNone) {
            std::cout << "Error: Could not empty JPG buffer\n";
            return;
        }

        int done = 0;
        while (done == 0) {
            if (mJPGDecoder->outBuffHeader == nullptr) {
                ret = ilclient_wait_for_event(mJPGDecoder->decoder->component,
                                              OMX_EventPortSettingsChanged,
                                              mJPGDecoder->decoder->outport,
                                              0, 0, 1, 0, 5);
                if (ret == 0) {
                   _OMAXPortSettingsChanged();
                }
            } else {
                ret = ilclient_remove_event(mJPGDecoder->decoder->component,
                                            OMX_EventPortSettingsChanged,
                                            mJPGDecoder->decoder->outport,
                                            0, 0, 1);
                if (ret == 0) {
                    _OMAXPortSettingsChangedAgain();
                }
            }

            if (header->nFilledLen == 0)
                done = 1;
        }

        U32 time2 = _getMilliseconds();
        std::cout << "Copy time: " << time2 - time1 << "\n";
        
        OMX_SendCommand(mJPGDecoder->glrenderer->handle,
                        OMX_CommandPortEnable,
                        mJPGDecoder->glrenderer->outport, NULL);
        U32 error = OMX_UseEGLImage(mJPGDecoder->glrenderer->handle,
                        &mJPGDecoder->outBuffHeader,
                        mJPGDecoder->glrenderer->outport,
                        NULL,
                        mCamTexData);
        if (mJPGDecoder->outBuffHeader == NULL) {
            std::cout << "Could not create output EGL image buffer\n";
            std::cout << "Code " << std::hex << error;
            return;
        }
        /*ilclient_wait_for_event(mJPGDecoder->glrenderer->component,
                                OMX_EventCmdComplete,
                                OMX_CommandPortEnable, 1, mJPGDecoder->glrenderer->outport, 1, 0, 2000);*/
        OMX_FillThisBuffer(mJPGDecoder->glrenderer->handle,
                           mJPGDecoder->outBuffHeader);

        /*ilclient_wait_for_event(mJPGDecoder->decoder->component,
                                OMX_EventBufferFlag, OMX_BUFFERFLAG_EOS,
                                1, mJPGDecoder->decoder->outport,
                                1, 0, 2000);
        ilclient_wait_for_event(mJPGDecoder->glrenderer->component,
                                OMX_EventBufferFlag,
                                OMX_BUFFERFLAG_EOS | OMX_BUFFERFLAG_ENDOFFRAME,
                                1, mJPGDecoder->glrenderer->outport,
                                1, 0, 2000);*/
        
    }
}

void VisionGPUPipeline::_OMAXPortSettingsChanged()
{
    std::cout << "Changing OMAX port settings\n";

    OMX_PARAM_PORTDEFINITIONTYPE port;
    port.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    port.nVersion.nVersion = OMX_VERSION;
    port.nPortIndex = mJPGDecoder->decoder->outport;
    OMX_GetParameter(mJPGDecoder->decoder->handle,
                     OMX_IndexParamPortDefinition, &port);


    port.nPortIndex = mJPGDecoder->glrenderer->inport;
    OMX_SetParameter(mJPGDecoder->glrenderer->handle,
                     OMX_IndexParamPortDefinition, &port);

    OMX_SetupTunnel(mJPGDecoder->decoder->handle,
                    mJPGDecoder->decoder->outport,
                    mJPGDecoder->glrenderer->handle,
                    mJPGDecoder->glrenderer->inport);
    OMX_SendCommand(mJPGDecoder->decoder->handle,
                    OMX_CommandPortEnable,
                    mJPGDecoder->decoder->outport, NULL);
    OMX_SendCommand(mJPGDecoder->glrenderer->handle,
                    OMX_CommandPortEnable,
                    mJPGDecoder->glrenderer->inport, NULL);
    if (ilclient_change_component_state(mJPGDecoder->glrenderer->component,
                                        OMX_StateIdle) == -1) {
        std::cout << "Error: Could not set OMX renderer to idle state\n";
        return;
    }

    ilclient_wait_for_event(mJPGDecoder->decoder->component,
                            OMX_EventCmdComplete, OMX_CommandPortEnable,
                            1, mJPGDecoder->decoder->outport, 1, 0, 2000);

    ilclient_wait_for_event(mJPGDecoder->glrenderer->component,
                            OMX_EventCmdComplete, OMX_CommandPortEnable,
                            1, mJPGDecoder->glrenderer->inport, 1, 0, 2000);

    ilclient_wait_for_event(mJPGDecoder->glrenderer->component,
                            OMX_EventPortSettingsChanged,
                            OMX_CommandPortEnable,
                            1, mJPGDecoder->decoder->outport, 1, 0, 2000);

    ilclient_change_component_state(mJPGDecoder->glrenderer->component,
                                    OMX_StateExecuting);

    port.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    port.nVersion.nVersion = OMX_VERSION;
    port.nPortIndex = mJPGDecoder->glrenderer->outport;
    OMX_GetParameter(mJPGDecoder->glrenderer->handle,
                     OMX_IndexParamPortDefinition, &port);

    unsigned int imageWidth =
             (unsigned int)port.format.image.nFrameWidth;
    unsigned int imageHeight =
             (unsigned int)port.format.image.nFrameHeight;

    if (imageWidth != mInputWidth || imageHeight != mInputHeight) {
        std::cout << "Error: Input JPG not equal to set input size\n";
        std::cout << "image width: " << imageWidth << " input width" << mInputWidth;
        std::cout << "image height: " << imageHeight << " input height" << mInputHeight;
        return;
    }
    
    // Allocate buffer
    ilclient_disable_port(mJPGDecoder->glrenderer->component,
                          mJPGDecoder->glrenderer->outport);


    // Create EGL image to render to
    glGenTextures(1, &mCamTexID);
    glBindTexture(GL_TEXTURE_2D, mCamTexID);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    
    mCamTexData = EGL_NO_IMAGE_KHR;
    while(mCamTexData == EGL_NO_IMAGE_KHR) {
        mCamTexData = eglCreateImageKHR(
                   mDisplay,
                   mContext,
                   EGL_GL_TEXTURE_2D_KHR,
                   (EGLClientBuffer)mCamTexID, 0);
    }
    if (mCamTexData == EGL_NO_IMAGE_KHR) {
        std::cout << "Error: Could not create EGL Texture\n";
        return;
    }

    std::cout << "Created render texture for JPG decoder\n";
}

void VisionGPUPipeline::_OMAXPortSettingsChangedAgain()
{
}

bool VisionGPUPipeline::createContext()
{
    std::cout << "Creating context\n";
    bcm_host_init();
    int32_t success = 0;
    EGLBoolean result;
    EGLint num_config;

    static EGL_DISPMANX_WINDOW_T nativeWindow;

    DISPMANX_ELEMENT_HANDLE_T dispmanElement;
    DISPMANX_DISPLAY_HANDLE_T dispmanDisplay;
    DISPMANX_UPDATE_HANDLE_T dispmanUpdate;
    VC_RECT_T dstRect;
    VC_RECT_T srcRect;

    static const EGLint eglAttribs[] = {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_NONE
    };

    static const EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    EGLConfig config;

    mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (mDisplay == EGL_NO_DISPLAY) {
        std::cout << "Error: could not open display\n";
        return false;
    }

    result = eglInitialize(mDisplay, NULL, NULL);
    if (result == EGL_FALSE) {
        std::cout << "Error: could not initialize display\n";
        return false;
    }

    result = eglChooseConfig(mDisplay, eglAttribs, &config, 1, &num_config);
    if (result == EGL_FALSE) {
        std::cout << "Error: could not get display configuration\n";
        return false;
    }

    result = eglBindAPI(EGL_OPENGL_ES_API);
    if (result == EGL_FALSE) {
        std::cout << "Error: could not bind OpenGL ES to EGL\n";
        return false;
    }

    mContext = eglCreateContext(mDisplay, config, EGL_NO_CONTEXT, contextAttribs);
    if (mContext == EGL_NO_CONTEXT) {
        std::cout << "Error: could not create EGL context\n";
        return false;
    }

    //success = graphics_get_display_size(0, 1920, 1280);
    //if (!success) {
    //    std::cout << "Error: Could not set display size\n";
    //    return false;
    //}

    dstRect.x = 0;
    dstRect.y = 0;
    dstRect.width = 1920;
    dstRect.height = 1080;

    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.width = 1920 << 16;
    srcRect.height = 1080 << 16;

    dispmanDisplay = vc_dispmanx_display_open(0);
    dispmanUpdate = vc_dispmanx_update_start(0);

    dispmanElement = vc_dispmanx_element_add(dispmanUpdate, dispmanDisplay,
            0, &dstRect, 0, &srcRect, DISPMANX_PROTECTION_NONE, 0, 0, (DISPMANX_TRANSFORM_T)0);

    nativeWindow.element = dispmanElement;
    nativeWindow.width = 1920;
    nativeWindow.height = 1080;
    vc_dispmanx_update_submit_sync(dispmanUpdate);

    mSurface = eglCreateWindowSurface(mDisplay, config, &nativeWindow, NULL);
    if (mSurface == EGL_NO_SURFACE) {
        std::cout << "Error: could not create surface\n";
        return false;
    }

    result = eglMakeCurrent(mDisplay, mSurface, mSurface, mContext);
    if (result == EGL_FALSE) {
        std::cout << "Error: could not make egl context current\n";
        return false;
    }

    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glDisable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
    glFinish();
    eglSwapBuffers(mDisplay, mSurface);

    std::cout << "Created graphics context\n";

    // Init OpenMAX
    return _OMAXInit();
}

void VisionGPUPipeline::TEST_getDecodedJPG(char *decodeBuffer) {
    if (decodeBuffer == nullptr)
        return;
    //glBindTexture(GL_TEXTURE_2D, mTextureID);
    //glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, decodeBuffer);
    //glBindTexture(GL_TEXTURE_2D, 0);
}

void VisionGPUPipeline::TEST_swapbuffer()
{
    eglSwapBuffers(mDisplay, mSurface);
}
