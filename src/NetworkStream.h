//
//  NetworkStream.h
//  SocialServer
//
//  Created by Ian Ewell on 10/18/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef SocialServer_NetworkStream_h
#define SocialServer_NetworkStream_h

#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Stream.h"
#include "Types.h"

class CCNetworkStream : public CCStream
{
private:
	int mListenSocket;
    int mConnectionSocket;
    struct sockaddr_in mHostAddress;
    struct sockaddr_in mClientAddress;
    socklen_t mSocketSize;
	
public:
	CCNetworkStream();
	~CCNetworkStream();
	
	// Network Stream Functions:
	int listenAndConnect();
    int setServer();
    int connectToServer(const std::string &ip, unsigned int port);
	
	// Read funtions:
	virtual float readFloat();
    virtual double readDouble();
	virtual int   readInt();
	virtual short readShort();
	virtual char  readChar();
    virtual unsigned int   readUInt();
	virtual unsigned short readUShort();
	virtual unsigned char  readUChar();
    virtual std::string readString();
	virtual bool  read(int length, void *buffer);
	
	// Write functions:
	virtual void  writeFloat(float value);
    virtual void  writeDouble(double value);
	virtual void  writeInt(int value);
	virtual void  writeShort(short value);
	virtual void  writeChar(char value);
    virtual void  writeUInt(unsigned int value);
	virtual void  writeUShort(unsigned short value);
	virtual void  writeUChar(unsigned char value);
    virtual void  writeString(const std::string &string);
	virtual void  write(int length, void *buffer);
	
	//Stream funtions:
	virtual void  rewind();
	virtual void  seekTo(long pos);
	virtual long  getPos();
	virtual long  getSize();
};


#endif
