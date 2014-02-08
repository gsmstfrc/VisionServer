#include "VisionGPUPipeline.h"
#include "Lifecam.h"
#include "FileStream.h"
#include <iostream>
#include <time.h>

unsigned int _getMilliseconds()
{
	timeval time;
	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000) + (time.tv_usec / 1000);
}

int main(int argc, char *argv[])
{
    //AxisCamera camera;
    //camera.setIPAddress("10.33.18.3");
    //std::cout << "Attempting to connect to Axis camera\n";
    //camera.start();

    Lifecam lc;
    lc.openCamera(0);
    lc.start();

    VisionGPUPipeline pipeline;
    pipeline.setInputWidth(640);
    pipeline.setInputHeight(480); 
    pipeline.createContext();

    if (argc > 1) {
        CCFileStream fstream;
        fstream.openFile(argv[1], "rb");
        long imageSize = fstream.getSize();
        char* imageBuffer = new char[imageSize];
        fstream.read(imageSize, imageBuffer);
        fstream.closeFile();
        U8 *buff;
        int len = lc.captureFrame(&buff);
        pipeline._OMAXDecodeJPG((char*)buff, len);
        std::cout << "Decoded JPG\n";
        sleep(1);
        //delete[] imageBuffer;

        VisionGPUPass pass;
        if (pass.initialize()) {
            std::cout << "Render pass initialized\n";
            pass.setInputImageID(pipeline.getTexID());
            U32 time1 = _getMilliseconds();
            pass.execute();
            U32 time2 = _getMilliseconds();
            std::cout << "Frame time: " << time2-time1 << "\n";
            pass.saveImage("output.raw");
            U32 time4 = _getMilliseconds();
            std::cout << "Write to disk time: " << time4-time1 << "\n";
        } 

/*        char *output = new char[640*480*4];
        memset(output, 0, 640*480*4);
        //pipeline.TEST_getDecodedJPG(output);
        CCFileStream outStream;
        outStream.openFile("output.raw", "wb");
        outStream.write(640*480*4, output);
        outStream.closeFile();
        delete[] output;
    */}
    else {
        U8 *buff;
        int len = 0;

        VisionGPUPass pass;
        pass.setInputWidth(640);
        pass.setInputHeight(480);
        pass.setRenderToScreen(true);
        if (pass.initialize()) {
            std::cout << "Render pass initialized\n";
        } 
        else
            exit(-1);

        while ((len = lc.captureFrame(&buff)) <= 0);
        pipeline._OMAXDecodeJPG((char*)buff, len);
        pass.setInputImageID(pipeline.getTexID());
        sleep(4);

        while (1) {
            while ((len = lc.captureFrame(&buff)) <= 0);

            pipeline._OMAXDecodeJPG((char*)buff, len);
            pass.setInputImageID(pipeline.getTexID());
            pass.execute();
            pipeline.TEST_swapbuffer();
        } 
    }

    while (1);
}
