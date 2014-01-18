SDKROOT = /arm-uknown-linux-gnuabi/arm-uknown-linux-gnuabi/sysroot
CXX = /arm-uknown-linux-gnuabi/bin/arm-linux-gnueabihf-g++
CXXFLAGS = -DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -Wall -g -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize -pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM -Wno-psabi -std=c++11 
LDFLAGS = -L$(SDKROOT)/opt/vc/lib -lGLESv2 -lEGL -lopenmaxil -lbcm_host -lvcos -lvchiq_arm -lpthread -lrt -L../libs/ilclient -L../libs/vgfont
INCLUDES = -I$(SDKROOT)/opt/vc/include/ -I$(SDKROOT)/opt/vc/include/interface/vcos/pthreads -I$(SDKROOT)/opt/vc/include/interface/vmcs_host/linux -I./ -I../libs/ilclient -I../libs/vgfont

testAxis: testAxis.o NetworkStream.o FileStream.o AxisCamera.o
	$(CXX) -o testAxis testAxis.o NetworkStream.o FileStream.o AxisCamera.o $(LDFLAGS)

testAxis.o: testAxis.cpp AxisCamera.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c testAxis.cpp

NetworkStream.o: NetworkStream.cpp NetworkStream.h Stream.h

FileStream.o: FileStream.cpp FileStream.h Stream.h

AxisCamera.o: AxisCamera.h NetworkStream.h Stream.h

clean:
	rm *.o testAxis
