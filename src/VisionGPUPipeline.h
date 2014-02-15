//
// FRC Vision Server
// FRC 2014
// FRC Team 3318
// Written by Ian Ewell <ianewellgmsmt@gmail.com>
// (C) 2014 GSMST Robotics
//

#ifndef VISION_GPU_PIPELINE_H
#define VISION_GPU_PIPELINE_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <string>
#include "ilclient.h"
#include "types.h"
#include "ShaderGLES.h"
#include "VertexBufferGLES.h"
#include "FileStream.h"

typedef enum
{
    INPUT_FORMAT_RAW,
    INPUT_FORMAT_JPG,
} VisionInputFormat;

typedef enum
{
    OUTPUT_FORMAT_RGB,
    OUTPUT_FORMAT_LUMINANCE,
    OUTPUT_FORMAT_BITMASK,
} VisionOutputFormat;

//
// @class GPUFramebuffer
//
// This class abstracts a framebuffer object on the GPU. These FBOs are
// render targets that serve as temporary buffers to render image processing
// results into. These FBOs can be attached to texture objects (and in most
// cases will) which allow render to texture. These textures can be used as
// inputs to other passes, and are an efficient method of intermediate storage
class GPUFramebuffer
{
    protected:
        U32 mFBWidth;
        U32 mFBHeight;
        GLuint mFramebuffer;   
        GLuint mRenderbuffer;
        bool mRenderToTexture;
        GLuint mTextureID;

    public:
        GPUFramebuffer();
        GPUFramebuffer(U32 width, U32 height);
        virtual ~GPUFramebuffer();

        bool create();
        void destroy();

        void bind();
        void unbind();

        void setWidth(U32 width)
        {
            if (!mFramebuffer)
                mFBWidth = width;
        }

        U32 getWidth()
        {
            return mFBWidth;
        }

        void setHeight(U32 height)
        {
            if (!mFramebuffer)
                mFBHeight = height;
        }

        U32 getHeight()
        {
            return mFBHeight;
        }

        GLuint getTextureID()
        {
            return mTextureID;
        }

        void setRenderToTexture(bool RTT)
        {
            mRenderToTexture = RTT;
        }
};

// 
// @class VisionGPUPass
//
// This abstract class provides the basic infrastructure to do a vision pass.
// A pass simply takes an input texture of a certain size, and renders to an
// internal framebuffer (which may have an attached texture that may be fed
// into another pass). Most passes will take a set of parameters that depend
// on the particular effect, and draw a fullscreen quad that uses a shader
// that does the real math in the pass's calculation, though some passes may
// draw different shapes to perform distortion effects. A pass may also store
// references to other passes as outputs that depend on the results of this pass.
class VisionGPUPass
{
    protected:
        // Image parameters
        U32 mInputWidth;
        U32 mInputHeight;
        U32 mOutputWidth;
        U32 mOutputHeight;

        GLuint mInputImageID;

        // Internal resources
        GPUFramebuffer mFramebuffer;
        CGRShaderRef mShader;
        CGRUniformBufferHandle mUniformBuffer;
        std::string mVertexShaderFile;
        std::string mFragmentShaderFile;
        CGRVertexBufferRef mVertexBuffer;

        // Outputs
        std::vector<VisionGPUPass*> mOutputs;

        // Render to screen
        bool mRenderToScreen;

    public:
        VisionGPUPass();
        virtual ~VisionGPUPass();

        // Boring accessor methods
        virtual void setInputWidth(U32 width)
        {
            mInputWidth = width;
            mOutputWidth = width;
        }

        inline U32 getInputWidth()
        {
            return mInputWidth;
        }

        virtual void setInputHeight(U32 height)
        {
            mInputHeight = height;
            mOutputHeight = height;
        }

        inline U32 getInputHeight()
        {
            return mInputHeight;
        }

        inline U32 getOutputWidth()
        {
            return mOutputWidth;
        }

        inline U32 getOutputHeight()
        {
            return mOutputHeight;
        }

        inline void setInputImageID(GLuint id)
        {
            mInputImageID = id;
        }

        inline GLuint getTextureID()
        {
            return mFramebuffer.getTextureID();
        }

        inline void setRenderToScreen(bool val)
        {
            mRenderToScreen = val;
        }

    protected:
        // Some internal resource management stuff
        virtual bool _allocFrameBuffer();
        virtual void _destroyFramebuffer();
        virtual bool _loadShaders();
        virtual void _destroyShaders();
        virtual bool _setupVertexBuffers();
        virtual void _destroyVertexBuffers();

        // Rendering stuff
        virtual void _bindResources();
        virtual void _unbindResources();
        virtual void _setShaderParams(); 
        virtual void _draw();

    public:
        // Public API
        bool initialize();
        void execute();

        void saveImage(const std::string &location);
        void readToBuffer(U8 *buffer);

        void addOutput(VisionGPUPass* output);
};

//
// @class VisionThresholdPass
//
// This class performs a threshold filter on the input image. A threshold
// simply filters colors that exist in a certain range. The pass has two
// parameters: the min color and the max color. For a pixel to pass through
// this filter, ALL the color components of the pixel must be in the range
// of all the channels in the color filter. The filter will filter the RGBA
// channels.
class VisionThresholdPass : public VisionGPUPass
{
    protected:
        // Parameters
        CMVector4 mMinColors;
        CMVector4 mMaxColors;

        virtual void _setShaderParams();

    public:
        VisionThresholdPass();

        inline void setMinColors(CMVector4 &vec)
        {
            mMinColors = vec;
        }

        inline CMVector4 getMinColors()
        {
            return mMinColors;
        }
        
        inline void setMaxColors(CMVector4 &vec)
        {
            mMaxColors = vec;
        }

        inline CMVector4 getMaxColors(CMVector4 &vec)
        {
            return mMaxColors;
        }
};

//
// @class VisionPackPass
//
//  This class performs a packing shader to an input threshold image. It
//  puts an image in a format so that each bit represents white or black
//  in an image and greatly reduces the size of the threshold masks
class VisionPackPass : public VisionGPUPass
{
    protected:
        virtual bool _setupVertexBuffers();
        virtual void _setShaderParams();
        virtual void _draw();
    public:
        VisionPackPass();
        
        virtual void setInputWidth(U32 width)
        {
            mInputWidth = width;
            mOutputWidth = width/32;
        }

        virtual void setInputHeight(U32 height)
        {
            mInputHeight = height;
            mOutputHeight = height;
        }
};

//
// @class VisionGPUPipeline
//
// This class is designed to provide all the GPU infrastructure to run the
// initial image processing routines on the GPU. In addition to maintaining
// the OpenGL ES 2.0 rendering context and the render buffers, this class
// controls the execution pipeline that different passes run. The input to
// this image pipeline may be an uncompressed bitmap or a compressed JPEG
// (which is then decompressed on the GPU using OpenMAX) and the output is
// a threshold bitmap where each bit indicates an accepted pixel or a reject.
// This bitmap must undergo further processing on the CPU level to get the
// required information out of it.
//
// The pipeling itself is designed to be flexible, and runs with multiple passes
// each backed by a GLSL shader to perform image processing routines on the image.
// The most common passes that may run in a typical pipeline may be distortion
// removal, thresholding, edge detection, color transformation, or blurring.
// All passes are subclasses of the VisionGPUPass class, and are registered into
// the pipeline to all execute serially.
class VisionGPUPipeline
{
    protected:
        // Input image parameters
        U32 mInputWidth;
        U32 mInputHeight;
        VisionInputFormat mInputFormat;

        // Ouput image parameters (should be same as input unless downsampling is involved)
        U32 mOutputWidth;
        U32 mOutputHeight;

        // Graphics context
        EGLDisplay mDisplay;
        EGLSurface mSurface;
        EGLContext mContext;

        // Camera input texture
        void *mCamTexData;
        GLuint mCamTexID;

        // OpenMAX JPG Decoder stuffs
        struct OMXComponent
        {
            COMPONENT_T *component;
            OMX_HANDLETYPE handle;
            int inport;
            int outport;
        };

        struct JPGDecoderInfo
        {
            ILCLIENT_T *client;
            OMXComponent *decoder;
            OMXComponent *glrenderer;
            OMX_BUFFERHEADERTYPE **inBuffHeader;
            int buffHeaderCount;
            OMX_BUFFERHEADERTYPE *outBuffHeader;

            JPGDecoderInfo() :
                client(nullptr),
                decoder(nullptr),
                glrenderer(nullptr),
                inBuffHeader(nullptr),
                buffHeaderCount(0),
                outBuffHeader(nullptr)
            {}
        };

        JPGDecoderInfo *mJPGDecoder;

        bool _OMAXInit();
    public:
        void _OMAXDecodeJPG(char *jpg, size_t length);
        void TEST_getDecodedJPG(char *decodeBuffer);
        void TEST_swapbuffer();
    protected:
        void _OMAXPortSettingsChanged();
        void _OMAXPortSettingsChangedAgain();

    public:
        VisionGPUPipeline();
        virtual ~VisionGPUPipeline();

        bool createContext();

    // Stupid hacky test stuff
        inline GLuint getTexID()
        {
            return mCamTexID;
        }

        inline void setInputWidth(U32 width)
        {
            mInputWidth = width;
            mOutputWidth = width;
        }

        inline U32 getInputWidth()
        {
            return mInputWidth;
        }

        inline void setInputHeight(U32 height)
        {
            mInputHeight = height;
            mOutputHeight = height;
        }

        inline U32 getInputHeight()
        {
            return mInputHeight;
        }

        inline U32 getOutputWidth()
        {
            return mOutputWidth;
        }

        inline U32 getOutputHeight()
        {
            return mOutputHeight;
        }

};

#endif
