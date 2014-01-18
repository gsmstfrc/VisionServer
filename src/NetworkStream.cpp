//
// FRC Vision Server
// FRC 2014
// FRC Team 3318
// Written by Ian Ewell
// (C) 2014 GSMST Robotics
//

#include <iostream>

#include "NetworkStream.h"
#include <stdlib.h>
#include <string.h>

CCNetworkStream::CCNetworkStream()
{
    
    mConnectionSocket = -1;
}

CCNetworkStream::~CCNetworkStream()
{
    shutdown(mListenSocket, SHUT_RDWR);
    shutdown(mConnectionSocket, SHUT_RDWR);
}

int CCNetworkStream::setServer()
{
    mListenSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (mListenSocket == -1)
        return -1;
    int param = 1;
    setsockopt(mListenSocket, SOL_SOCKET, SO_REUSEADDR, &param, sizeof(int));
    
    mHostAddress.sin_family = AF_INET;
    mHostAddress.sin_port = htons(80);
    mHostAddress.sin_addr.s_addr = INADDR_ANY;
    memset(&(mHostAddress.sin_zero), '\0', 8);
    
    if (bind(mListenSocket, (struct sockaddr *)&mHostAddress, sizeof(struct sockaddr)) == -1)
    {
        return -1;
    }
    if (listen(mListenSocket, 20) == -1)
    {
        return -1; 
    }
    return 0;
}

int CCNetworkStream::listenAndConnect()
{
    if (mConnectionSocket != -1)
        shutdown(mConnectionSocket, SHUT_RDWR);
    
    mSocketSize = sizeof(struct sockaddr_in);
    mConnectionSocket = accept(mListenSocket, (struct sockaddr *)&mClientAddress, &mSocketSize);
    return 0;
}

int CCNetworkStream::connectToServer(const std::string &ip, U32 port)
{
    struct sockaddr_in address;
    
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(ip.c_str());

    mConnectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mConnectionSocket == -1) {
        return -1;
    }
    std::cout << "Connecting to " << ip << ":" << port << "\n";
    if (connect(mConnectionSocket, (struct sockaddr*)&address, sizeof(address)) < 0) {
        return -1;
    }
    return 0;
}

float CCNetworkStream::readFloat()
{
	if (mConnectionSocket != -1)
	{
		float buf;
        recv(mConnectionSocket, &buf, (size_t)4, 0);
		return buf;
	}
	return 0;
}

double CCNetworkStream::readDouble()
{
	if (mConnectionSocket != -1)
	{
		double buf;
		recv(mConnectionSocket, &buf, (size_t)8, 0);
		return buf;
	}
	return 0;
}

int CCNetworkStream::readInt()
{
	if (mConnectionSocket != -1)
	{
		int buf;
		recv(mConnectionSocket, &buf, (size_t)4, 0);
		return buf;
	}
	return 0;
}

short CCNetworkStream::readShort()
{
	if (mConnectionSocket != -1)
	{
		short buf;
		recv(mConnectionSocket, &buf, (size_t)2, 0);
		return buf;
	}
	return 0;
}

char CCNetworkStream::readChar()
{
	if (mConnectionSocket != -1)
	{
		short buf;
		recv(mConnectionSocket, &buf, (size_t)1, 0);
		return buf;
	}
	return 0;
}

unsigned int CCNetworkStream::readUInt()
{
	if (mConnectionSocket != -1)
	{
		int buf;
		recv(mConnectionSocket, &buf, (size_t)4, 0);
		return buf;
	}
	return 0;
}

unsigned short CCNetworkStream::readUShort()
{
	if (mConnectionSocket != -1)
	{
		short buf;
		recv(mConnectionSocket, &buf, (size_t)2, 0);
		return buf;
	}
	return 0;
}

unsigned char CCNetworkStream::readUChar()
{
	if (mConnectionSocket != -1)
	{
		short buf;
		recv(mConnectionSocket, &buf, (size_t)1, 0);
		return buf;
	}
	return 0;
}

std::string CCNetworkStream::readString()
{
    if (mConnectionSocket != -1)
    {
        std::string string;
#define EOL "\r\n"
#define EOL_SIZE 2
        char buffer;
        char buffer1;
        while (recv(mConnectionSocket, &buffer, (size_t)1, 0) == 1)
        {
            if (buffer == '\r')
            {
                recv(mConnectionSocket, &buffer1, (size_t)1, 0);
                if (buffer1 == '\n')
                {
                    std::cout << string << "\n";
                    return string;
                }
                else
                {
                    string.push_back(buffer);
                    string.push_back(buffer1);
                }
            }
            else
                string.push_back(buffer);
        }
        return string;
    }
    return "";
}

bool CCNetworkStream::read(int length, void *buffer)
{
	if (mConnectionSocket != -1)
	{
        recv(mConnectionSocket, buffer, (size_t)length, 0);
		return true;
	}
	return false;
}

void CCNetworkStream::writeFloat(float value)
{
	if (mConnectionSocket != -1)
	{
        send(mConnectionSocket, &value, sizeof(float), 0);
	}
	return;
}

void CCNetworkStream::writeDouble(double value)
{
	if (mConnectionSocket != -1)
	{
		send(mConnectionSocket, &value, sizeof(double), 0);
	}
	return;
}

void CCNetworkStream::writeInt(int value)
{
	if (mConnectionSocket != -1)
	{
		send(mConnectionSocket, &value, sizeof(int), 0);
	}
	return;
}

void CCNetworkStream::writeShort(short value)
{
	if (mConnectionSocket != -1)
	{
		send(mConnectionSocket, &value, sizeof(short), 0);
	}
	return;
}

void CCNetworkStream::writeChar(char value)
{
	if (mConnectionSocket != -1)
	{
		send(mConnectionSocket, &value, sizeof(char), 0);
	}
	return;
}

void CCNetworkStream::writeUInt(unsigned int value)
{
	if (mConnectionSocket != -1)
	{
		send(mConnectionSocket, &value, sizeof(int), 0);
	}
	return;
}

void CCNetworkStream::writeUShort(unsigned short value)
{
	if (mConnectionSocket != -1)
	{
		send(mConnectionSocket, &value, sizeof(short), 0);
	}
	return;
}

void CCNetworkStream::writeUChar(unsigned char value)
{
	if (mConnectionSocket != -1)
	{
		send(mConnectionSocket, &value, sizeof(char), 0);
	}
	return;
}

void CCNetworkStream::writeString(const std::string &string)
{
    if (mConnectionSocket != -1)
    {
        long length = string.length();
        int error = send(mConnectionSocket, string.c_str(), length, 0);
        if (error < 0)
            std::cout << "Send error\n";
    }
}

void CCNetworkStream::write(int length, void *buffer)
{
	if (mConnectionSocket != -1)
	{
		if (buffer)
			send(mConnectionSocket, buffer, (size_t)length, 0);
	}
	return;
    
}

void CCNetworkStream::rewind()
{
}

void CCNetworkStream::seekTo(long pos)
{
}

long CCNetworkStream::getPos()
{
	return 0;
}

long CCNetworkStream::getSize()
{
    return 0;
}
