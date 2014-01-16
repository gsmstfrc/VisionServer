//
// FRC Vision Server
// FRC 2014
// FRC Team 3318
// Written by Ian Ewell
// (C) 2014 GSMST Robotics
//

#ifndef CORE_STREAM_H
#define CORE_STREAM_H

#include <string>

class CCStream
{
public:
	// Read funtions:
	virtual float readFloat() = 0;
    virtual double readDouble() = 0;
	virtual int   readInt() = 0;
	virtual short readShort() = 0;
	virtual char  readChar() = 0;
	virtual unsigned int   readUInt() = 0;
	virtual unsigned short readUShort() = 0;
	virtual unsigned char  readUChar() = 0;
    virtual std::string readString() = 0;
	virtual bool  read(int length, void *buffer) = 0;
	
	// Write functions:
	virtual void  writeFloat(float value) = 0;
    virtual void  writeDouble(double value) = 0;
	virtual void  writeInt(int value) = 0;
	virtual void  writeShort(short value) = 0;
	virtual void  writeChar(char value) = 0;
    virtual void  writeUInt(unsigned int value) = 0;
	virtual void  writeUShort(unsigned short value) = 0;
	virtual void  writeUChar(unsigned char value) = 0;
	virtual void  write(int length, void *buffer) = 0;
	
	//Stream funtions:
	virtual void  rewind() = 0;
	virtual void  seekTo(long pos) = 0;
	virtual long  getPos() = 0;
	virtual long  getSize() = 0;
};

#endif
