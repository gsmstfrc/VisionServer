//
// FRC Vision Server
// FRC 2014
// FRC Team 3318
// Written by Ian Ewell
// (C) 2014 GSMST Robotics
//

#ifndef CORE_FILE_STREAM_H
#define CORE_FILE_STREAM_H

//Includes:
#include <iostream>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Stream.h"

class CCFileStream : public CCStream
{
private:
	std::string mFileName;
	int mPosition;
	FILE *mFile;
	
public:
	CCFileStream();
	~CCFileStream();
	
	// File Stream Functions:
	bool openFile(const std::string &file, const char *options);
	void closeFile();
	
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
	virtual void  write(int length, void *buffer);
	
	//Stream funtions:
	virtual void  rewind();
	virtual void  seekTo(long pos);
	virtual long  getPos();
	virtual long  getSize();
};

#endif
