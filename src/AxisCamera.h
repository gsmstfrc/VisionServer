//
// FRC Vision Server
// FRC 2014
// FRC Team 3318
// Written by Ian Ewell
// (C) 2014 GSMST Robotics
//

#ifndef AXIS_CAMERA_H
#define AXIS_CAMERA_H

#include <sstream>
#include "NetworkStream.h"
#include "Types.h"

class IFAxisImageReceiver
{
    public:
        virtual void receiveJPG(size_t size, char *data);
};

class AxisCamera
{
    protected:
      // Image parameters
      U32 mImageWidth;
      U32 mImageHeight;
      U32 mImageCompression;
      bool mGreyscale;
      U32 mFrameCount;
      U32 mFPS;

      // Network
      CCNetworkStream mAxisConnection;
      std::string mIPAdr;

      // Control
      bool mRunning;

      // Receiver callback class
      IFAxisImageReceiver *mReceiver;

    public:
      AxisCamera();

      void setImageWidth(U32 width) {
          mImageWidth = width;
      }

      U32 getImageWidth() {
          return mImageWidth;
      }

      void setImageHeight(U32 height) {
          mImageHeight = height;
      }

      U32 getImageHeight() {
          return mImageHeight;
      }

      void setImageCompression(U32 compression) {
          mImageCompression = compression;
      }

      U32 getImageCompression() {
          return mImageCompression;
      }
      
      void setGreyscale(bool grey) {
          mGreyscale = grey;
      }

      bool getGreyscale() {
          return mGreyscale;
      }

      void setFramecount(U32 frames) {
          mFrameCount = frames;
      }

      U32 getFramecount() {
          return mFrameCount;
      }

      void setFPS(U32 fps) {
          mFPS = fps;
      }

      U32 getFPS() {
          return mFPS;
      }

      void setIPAddress(const std::string &ip) {
          mIPAdr = ip;
      }

      std::string getIPAddress() {
          return mIPAdr;
      }

      void setReceiver(IFAxisImageReceiver *receiver) {
          mReceiver = receiver;
      }

      void start();
      void stop();
};

#endif
