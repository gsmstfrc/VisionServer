//
// FRC Vision Server
// FRC 2014
// FRC Team 3318
// Written by Ian Ewell
// (C) 2014 GSMST Robotics
//

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "AxisCamera.h"
#include "FileStream.h"

AxisCamera::AxisCamera() :
    mAxisConnection(),
    mImageWidth(640),
    mImageHeight(480),
    mImageCompression(10),
    mGreyscale(false),
    mFrameCount(0),
    mFPS(30),
    mRunning(false),
    mReceiver(nullptr) 
{
}

void AxisCamera::start()
{
    if (mAxisConnection.connectToServer(mIPAdr, 80) < 0) {
        std::cout << "Error: Could not connect to axis camera\n";
        return;
    }

    // Image buffer for max theoretical image size
    U8 *imgbuf = new U8[mImageWidth*mImageHeight*3];
    
    std::cout << "Requesting motion JPG from AXIS\n";
    std::ostringstream stream;
    stream << "GET /axis-cgi/mjpg/video.cgi?resolution=" << mImageWidth << "x" << mImageHeight << "&compression=" << mImageCompression << "&color=" << !mGreyscale << "&nbrofframes=" << mFrameCount << "&fps=" << mFPS << "\r\n";
    std::cout << "Request: " << stream.str();
    mAxisConnection.writeString(stream.str());

    std::string ok = mAxisConnection.readString();
    if (ok.compare("HTTP/1.0 200 OK") != 0) {
        std::cout << "Error: did not receive HTTP OK\n";
        return;
    } 
    
    // Consume nonimportant lines
    std::cout << "Waiting for boundary\n";
    while (mAxisConnection.readString().compare("--myboundary") != 0);
    mAxisConnection.readString(); // Consume content type
    std::string length = mAxisConnection.readString();
    size_t imageLength = atoi(length.substr(16).c_str());
    mAxisConnection.readString();
    mAxisConnection.read(imageLength, imgbuf);
    std::cout << "Recieved image of length " << imageLength << "\n";

    CCFileStream fstream;
    fstream.openFile("outjpg.jpg", "wb");
    fstream.write(imageLength, imgbuf);
    fstream.closeFile();
}
