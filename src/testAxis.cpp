#include "AxisCamera.h"
#include "VisionGPUPipeline.h"
#include "FileStream.h"
#include <iostream>

int main(int argc, char *argv[])
{
    AxisCamera camera;
    camera.setIPAddress("10.33.18.3");
    std::cout << "Attempting to connect to Axis camera\n";
    //camera.start();

    VisionGPUPipeline pipeline;
    pipeline.createContext();

    if (argc > 1) {
        CCFileStream fstream;
        fstream.openFile(argv[1], "rb");
        long imageSize = fstream.getSize();
        char* imageBuffer = new char[imageSize];
        fstream.read(imageSize, imageBuffer);
        fstream.closeFile();
        pipeline._OMAXDecodeJPG(imageBuffer, imageSize);
        std::cout << "Decoded JPG\n";
        sleep(1);
        //delete[] imageBuffer;

        VisionGPUPass pass;
        if (pass.initialize()) {
            std::cout << "Render pass initialized\n";
            pass.setInputImageID(pipeline.getTexID());
            pass.execute();
            pass.saveImage("output.raw");
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

    while (1);
}
