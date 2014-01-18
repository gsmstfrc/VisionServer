//
// FRC Vision Server
// FRC 2014
// FRC Team 3318
// Written by Ian Ewell
// (C) 2014 GSMST Robotics
//

#include "FileStream.h"
#include "Types.h"

CCFileStream::CCFileStream()
{
	mPosition = 0;
	mFile = NULL;
}

CCFileStream::~CCFileStream()
{
	closeFile();
}

bool CCFileStream::openFile(const std::string &file, const char *options)
{
	mFileName = file;
	mFile = fopen(file.c_str(), options);
	if (mFile)
		return true;
	return false;
}

void CCFileStream::closeFile()
{
	if (mFile)
		fclose(mFile);
	mFile = NULL;
}

float CCFileStream::readFloat()
{
	if (mFile)
	{
		float buf;
		fread(&buf, (size_t)4, (size_t)1, mFile);
		return buf;
	}
	return 0;
}

double CCFileStream::readDouble()
{
	if (mFile)
	{
		double buf;
		fread(&buf, (size_t)8, (size_t)1, mFile);
		return buf;
	}
	return 0;
}

int CCFileStream::readInt()
{
	if (mFile)
	{
		int buf;
		fread(&buf, (size_t)4, (size_t)1, mFile);
		return buf;
	}
	return 0;
}

short CCFileStream::readShort()
{
	if (mFile)
	{
		short buf;
		fread(&buf, (size_t)2, (size_t)1, mFile);
		return buf;
	}
	return 0;
}

char CCFileStream::readChar()
{
	if (mFile)
	{
		short buf;
		fread(&buf, (size_t)1, (size_t)1, mFile);
		return buf;
	}
	return 0;
}

unsigned int CCFileStream::readUInt()
{
	if (mFile)
	{
		int buf;
		fread(&buf, (size_t)4, (size_t)1, mFile);
		return buf;
	}
	return 0;
}

unsigned short CCFileStream::readUShort()
{
	if (mFile)
	{
		short buf;
		fread(&buf, (size_t)2, (size_t)1, mFile);
		return buf;
	}
	return 0;
}

unsigned char CCFileStream::readUChar()
{
	if (mFile)
	{
		short buf;
		fread(&buf, (size_t)1, (size_t)1, mFile);
		return buf;
	}
	return 0;
}

std::string CCFileStream::readString()
{
    if (mFile)
    {
        U32 length = readUInt();
        char buffer[length];
        read(length, buffer);
        return std::string(buffer, length);
    }
    return "";
}

bool CCFileStream::read(int length, void *buffer)
{
	if (mFile)
	{
		fread(buffer, (size_t)length, (size_t)1, mFile);
		return true;
	}
	return false;
}

void CCFileStream::writeFloat(float value)
{
	if (mFile)
	{
		fwrite(&value, sizeof(float), (size_t)1, mFile);
	}
	return;
}

void CCFileStream::writeDouble(double value)
{
	if (mFile)
	{
		fwrite(&value, sizeof(double), (size_t)1, mFile);
	}
	return;
}

void CCFileStream::writeInt(int value)
{
	if (mFile)
	{
		fwrite(&value, sizeof(int), (size_t)1, mFile);
	}
	return;
}

void CCFileStream::writeShort(short value)
{
	if (mFile)
	{
		fwrite(&value, sizeof(short), (size_t)1, mFile);
	}
	return;
}

void CCFileStream::writeChar(char value)
{
	if (mFile)
	{
		fwrite(&value, sizeof(char), (size_t)1, mFile);
	}
	return;
}

void CCFileStream::writeUInt(unsigned int value)
{
	if (mFile)
	{
		fwrite(&value, sizeof(int), (size_t)1, mFile);
	}
	return;
}

void CCFileStream::writeUShort(unsigned short value)
{
	if (mFile)
	{
		fwrite(&value, sizeof(short), (size_t)1, mFile);
	}
	return;
}

void CCFileStream::writeUChar(unsigned char value)
{
	if (mFile)
	{
		fwrite(&value, sizeof(char), (size_t)1, mFile);
	}
	return;
}


void CCFileStream::write(int length, void *buffer)
{
	if (mFile)
	{
		if (buffer)
			fwrite(buffer, (size_t)length, (size_t)1, mFile);
		else 
		{
			// Writing a bunch of zeros is better than having mis-alignment in the file.
			void *foo = (char*)malloc(length);
			memset(foo, 0, (size_t)length);
			fwrite(foo, (size_t)length, (size_t)0, mFile);
			free(foo);
			
			//Give an error.
			printf("FileStream.cpp: Runtime ERROR: Write from a NULL pointer to file %s!", mFileName.c_str());
		}
	}
	return;

}

void CCFileStream::rewind()
{
	if (mFile)
	{
		fseek(mFile, 0, SEEK_SET);
	}
	return;
}

void CCFileStream::seekTo(long pos)
{
	if (mFile)
	{
		fseek(mFile, pos, SEEK_SET);
	}
	return;

}

long CCFileStream::getPos()
{
	if (mFile)
	{
		return ftell(mFile);
	}
	return 0;
}

long CCFileStream::getSize()
{
	if (mFile)
	{
		// Store our current location.
		long loc = ftell(mFile);
	
		// Get the file size.
		fseek(mFile, 0, SEEK_END);
		long size = ftell(mFile);
	
		//Restore current location.
		fseek(mFile, loc, SEEK_SET);
	
		return size;
	}
	return 0;

}
