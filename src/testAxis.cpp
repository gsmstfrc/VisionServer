#include "AxisCamera.h"
#include <iostream>

int main(int argc, char *argv[])
{
    AxisCamera camera;
    camera.setIPAddress("10.33.18.3");
    std::cout << "Attempting to connect to Axis camera\n";
    camera.start();
}