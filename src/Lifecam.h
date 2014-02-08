//
// FRC Vision Server
// FRC 2014
// FRC Team 3318
// Written by Ian Ewell
// (C) 2014 GSMST Robotics
//

#ifndef LIFECAM_H 
#define LIFECAM_H

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include "NetworkStream.h"
#include "Types.h"

//
// @class Lifecam
//
// This class acts as an interface to the Microsoft Lifecam
// (but will probably work with other webcams as well).
// Basically we get the MJPEG to send off and configure some
// camera parameters such as color balance, exposure, focus,
// resolution, etc. Nothing too bad other that the ioctl stuff.
class Lifecam
{
    protected:
        int mFileDecripter;
        int mDeviceID;

        // Image parameters
        U32 mInputWidth;
        U32 mInputHeight;
        U32 mFrameRate;

        // V4l2 structs
        struct v4l2_capability mCapability;
        struct v4l2_format mFormat;
        struct v4l2_buffer mBuffer;
        struct v4l2_requestbuffers mRequestBuffers;

        // Buffers
        void *mMemory[4];
        U8 *mTempBuffer;
        U8 *mFrameBuffer;

        // Other stuff
        char *mVideoDevice;
        char *mStatus;
        char *mPictureName;

        bool mIsRunning;

    public:
        Lifecam();
        bool openCamera(int deviceID);

        bool start();
        int captureFrame(U8 **buffer);
};

#endif
