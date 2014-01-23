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
    glGenRenderbuffers(1, &mRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, mRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, mFBWidth, mFBHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, mRenderbuffer);
    
    GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Error: could not create framebuffer\n";
        destroy();
        return false;
    }

    if (mRenderToTexture) {
        glGenTextures(1, &mTextureID);
        glBindTexture(GL_TEXTURE_2D, mTextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mFBWidth, mFBHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureID, 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void GPUFramebuffer::destroy()
{
}

////////////////////////////////////////////////////////////////

VisionGPUPipeline::VisionGPUPipeline()
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
                                        ILCLIENT_ENABLE_INPUT_BUFFERS));
    if (ret != 0) {
        std::cout << "Error: Could not create image renderer component\n";
        goto gt_error;
    }

    mJPGDecoder->glrenderer->handle =
        ILC_GET_HANDLE(mJPGDecoder->glrenderer->component);

    OMX_PORT_PARAM_TYPE port;
    port.nSize = sizeof(OMX_PORT_PARAM_TYPE);
    port.nVersion.nVersion = OMX_VERSION;

    OMX_GetParameter(mJPGDecoder->glrenderer->handle,
                     OMX_IndexParamImageInit, &port);
    if (port.nPorts != 2) {
        std::cout << "Error: Expected 2 ports for renderer\n";
        goto gt_error;
    }
    mJPGDecoder->glrenderer->inport = port.nStartPortNumber;
    mJPGDecoder->glrenderer->outport = port.nStartPortNumber+1;


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

bool VisionGPUPipeline::createContext()
{
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
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
    glFinish();
    eglSwapBuffers(mDisplay, mSurface);

    std::cout << "Created graphics context\n";

    return true;
}
