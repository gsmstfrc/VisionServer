SDKROOT = /arm-uknown-linux-gnuabi/arm-unknown-linux-gnueabi/sysroot
CXX = /arm-uknown-linux-gnuabi/bin/arm-linux-gnueabihf-g++
CXXFLAGS = -DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -Wall -g -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize -pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM -Wno-psabi -std=c++11 
CC = /arm-uknown-linux-gnuabi/bin/arm-linux-gnueabihf-gcc
CFLAGS = -DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -Wall -g -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize -pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM -Wno-psabi  
LDFLAGS = -L$(SDKROOT)/opt/vc/lib -lGLESv2 -lEGL -lopenmaxil -lbcm_host -lvcos -lvchiq_arm -lpthread -lrt -L../libs/ilclient -L../libs/vgfont

CXXFLAGS += -I$(SDKROOT)/opt/vc/include/ -I$(SDKROOT)/opt/vc/include/interface/vcos/pthreads -I$(SDKROOT)/opt/vc/include/interface/vmcs_host/linux -I./ -I../libs/ilclient -I../libs/vgfont
CFLAGS += -I$(SDKROOT)/opt/vc/include/ -I$(SDKROOT)/opt/vc/include/interface/vcos/pthreads -I$(SDKROOT)/opt/vc/include/interface/vmcs_host/linux -I./ -I../libs/ilclient -I../libs/vgfont

SOURCES = testAxis.cpp NetworkStream.cpp FileStream.cpp AxisCamera.cpp VisionGPUPipeline.cpp Logger.cpp Matrix.cpp Plane.cpp RenderDevice.cpp SafeArray.cpp Shader.cpp ShaderGLES.cpp Vector.cpp VertexBuffer.cpp VertexBufferGLES.cpp Frustum.cpp Lifecam.cpp
SOURCES_C = ilclient.c ilcore.c 

OBJECTS = $(SOURCES:.cpp=.o)
OBJECTS_C = $(SOURCES_C:.c=.o)

all: $(SOURCES) $(SOURCES_C) testAxis

testAxis: $(OBJECTS) $(OBJECTS_C)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(OBJECTS_C) -o $@

.cpp.o: testAxis.cpp AxisCamera.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm *.o testAxis
