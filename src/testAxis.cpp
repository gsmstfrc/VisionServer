#include "VisionGPUPipeline.h"
#include "CPUVision.h"
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

    U8 *imagebuff = new U8[640*480/8];
    CPUVision vision;
    vision.setBitmap(imagebuff, 640, 480);

        U8 *buff;
        int len = 0;

        VisionThresholdPass pass;
        pass.setInputWidth(640);
        pass.setInputHeight(480);
        pass.setRenderToScreen(false);
        if (pass.initialize()) {
            std::cout << "Render pass initialized\n";
        } 
        else
            exit(-1);

        VisionPackPass pack;
        pack.setInputWidth(640);
        pack.setInputHeight(480);
        pack.setRenderToScreen(false);
        if (!pack.initialize())
            exit(-1);
        pack.setInputImageID(pass.getTextureID());
        
        while ((len = lc.captureFrame(&buff)) <= 0);
        pipeline._OMAXDecodeJPG((char*)buff, len);
        sleep(4);
        int i = 0;
        while (1) {
            U32 time1 = _getMilliseconds();
            while ((len = lc.captureFrame(&buff)) <= 0);
            U32 time2 = _getMilliseconds();
            std::cout << "Frame capture time: " << time2 - time1 << "\n";
            U32 timeBegin = _getMilliseconds();
            time1 = _getMilliseconds();
            pipeline._OMAXDecodeJPG((char*)buff, len);
            time2 = _getMilliseconds();
            std::cout << "Decode time: " << time2 - time1 << "\n";
            pass.setInputImageID(pipeline.getTexID());
            time1 = _getMilliseconds();
            pass.execute();
            pack.setInputImageID(pass.getTextureID());
            pack.execute();
            //pipeline.TEST_swapbuffer();
            time2 = _getMilliseconds();
            std::cout << "Execution time: " << time2 - time1 << "\n";
            pack.readToBuffer(imagebuff);
            if (i == 100) {
                pass.saveImage("raw.raw");
                pack.saveImage("pack.raw");
            }
            i++;
            time1 = _getMilliseconds();
            U32 pixels = vision.countPixels();
            time2 = _getMilliseconds();
            std::cout << "Pixel count: " << pixels << "\n";
            std::cout << "Count time: " << time2-time1 << "\n";
            
            U32 timeEnd = _getMilliseconds();
            std::cout << "Frame time: " << timeEnd-timeBegin << "\n";
    }

    while (1);
}
