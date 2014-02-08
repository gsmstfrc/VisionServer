//
// FRC Vision Server
// FRC 2014
// FRC Team 3318
// Written by Ian Ewell
// (C) 2014 GSMST Robotics
//

#include "Lifecam.h"
#include "huffman.h"

Lifecam::Lifecam() : mDeviceID(0),
                     mInputWidth(640),
                     mInputHeight(480),
                     mFrameRate(10),
                     mTempBuffer(nullptr),
                     mVideoDevice(nullptr),
                     mStatus(nullptr),
                     mPictureName(nullptr),
                     mIsRunning(false)
{
    for (int i = 0; i < 4; i++)
        mMemory[i] = nullptr;
}

bool Lifecam::openCamera(int deviceID)
{
    int ret = 0;

    mDeviceID = deviceID;
    std::stringstream stream;
    stream << "/dev/video" << deviceID;

    if ((mFileDecripter = open(stream.str().c_str(), O_RDWR))
            == -1) {
        std::cout << "Error: could not open camera device\n";
        return false;
    }

    ret = ioctl(mFileDecripter, VIDIOC_QUERYCAP, &mCapability);
    if (ret < 0) {
        std::cout << "Error: could not get camera capabilities\n";
        return false;
    }

    if ((mCapability.capabilities & V4L2_CAP_VIDEO_CAPTURE) == 0) {
        std::cout << "Error: video capture not supported\n";
        return false;
    }

    // Format
    mFormat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    mFormat.fmt.pix.width = mInputWidth;
    mFormat.fmt.pix.height = mInputHeight;
    mFormat.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    mFormat.fmt.pix.field = V4L2_FIELD_ANY;
    ret = ioctl(mFileDecripter, VIDIOC_S_FMT, &mFormat);
    if (ret < 0) {
        std::cout << "Error: could not set camera format\n";
        return false;
    }


    // Frame rate
    struct v4l2_streamparm *setfps;
    setfps = (struct v4l2_streamparm*)
        malloc(sizeof(struct v4l2_streamparm));
    memset(setfps, 0, sizeof(struct v4l2_streamparm));
    setfps->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    setfps->parm.capture.timeperframe.numerator = 1;
    setfps->parm.capture.timeperframe.denominator = mFrameRate;
    ret = ioctl(mFileDecripter, VIDIOC_S_PARM, setfps);

    // Buffers
    mRequestBuffers.count = 4;
    mRequestBuffers.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    mRequestBuffers.memory = V4L2_MEMORY_MMAP;
    ret = ioctl(mFileDecripter, VIDIOC_REQBUFS, &mRequestBuffers);
    if (ret < 0) {
        std::cout << "Error: Could not allocate buffers\n";
        return false;
    }

    // Memory mapping
    for (int i = 0; i < 4; i++) {
        memset(&mBuffer, 0, sizeof(struct v4l2_buffer));
        mBuffer.index = i;
        mBuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        mBuffer.memory = V4L2_MEMORY_MMAP;
        ret = ioctl(mFileDecripter, VIDIOC_QUERYBUF, &mBuffer);
        if (ret < 0) {
            std::cout << "Error: Could not query buffer\n";
            return false;
        }

        mMemory[i] = mmap(0, mBuffer.length, PROT_READ,
                            MAP_SHARED, mFileDecripter,
                            mBuffer.m.offset);
        if (mMemory[i] == MAP_FAILED) {
            std::cout << "Error: memory map failed\n";
            return false;
        }
    }

    // Buffer queueing
    for (int i = 0; i < 4; ++i) {
        memset(&mBuffer, 0, sizeof(struct v4l2_buffer));
        mBuffer.index = i;
        mBuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        mBuffer.memory = V4L2_MEMORY_MMAP;
        ret = ioctl(mFileDecripter, VIDIOC_QBUF, &mBuffer);
        if (ret < 0) {
            std::cout << "Error: Could not queue buffer\n";
            return false;
        }
    } 

    // Allocate temp buffer
    mTempBuffer = new U8[mInputWidth*mInputHeight << 1];
    return true;
}

bool Lifecam::start()
{
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    int ret;

    ret = ioctl(mFileDecripter, VIDIOC_STREAMON, &type);
    if (ret < 0) {
        std::cout << "Error: Could not start streaming\n";
        return false;
    }
    mIsRunning = true;
    return true;
}

int Lifecam::captureFrame(U8 **buffer)
{
    if (!mIsRunning)
        return 0;

    memset(&mBuffer, 0, sizeof(struct v4l2_buffer));
    mBuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    mBuffer.memory = V4L2_MEMORY_MMAP;

    int ret = ioctl(mFileDecripter, VIDIOC_DQBUF, &mBuffer);
    if (ret < 0) {
        std::cout << "Error: Could not dequeue buffer\n";
        return 0;
    }

    if (mBuffer.bytesused <= 0xaf)
        return 0;

    // Copy into buffer
    U8 *ptdeb, *ptlimit, *ptcur = (U8*)mMemory[mBuffer.index];
    ptdeb = ptlimit = ptcur;
    int pos = 0;
    ptlimit += mBuffer.bytesused;
    while ((((ptcur[0] << 8) | ptcur[1]) != 0xffc0) &&
                (ptcur < ptlimit))
        ptcur++;
    int sizein = ptcur - ptdeb;
    memcpy(mTempBuffer+pos, mMemory[mBuffer.index], sizein);
    pos += sizein;
    memcpy(mTempBuffer+pos, dht_data, sizeof(dht_data));
    pos += sizeof(dht_data);
    memcpy(mTempBuffer+pos, ptcur, mBuffer.bytesused-sizein);

    ret = ioctl(mFileDecripter, VIDIOC_QBUF, &mBuffer);
    if (ret < 0) {
        std::cout << "Error: Could not requeue buffer\n";
        return 0;
    }
    (*buffer) = mTempBuffer;
    return mBuffer.bytesused + sizeof(dht_data);
}
