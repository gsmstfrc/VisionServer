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
#include "types.h"

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
};

#endif
