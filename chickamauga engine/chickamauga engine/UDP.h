#pragma once

#include <stdio.h>      /* for printf(), fprintf() */
#include <winsock.h>    /* for socket(),... */
#include <stdlib.h>     /* for exit() */
#include "dataPacket.h"

#define MAXPENDING 5    /* Maximum outstanding connection requests */
#define SRVR_PORT		7654


struct dataPacket;

enum errorCodes
{
	NONERROR,
	WSASTARTUPERROR,
	SOCKETINIERROR,
	GETHOSTERROR,
	GETHOSTIPERROR,
	BINDERROR,
	NOTREADYERROR,
	ALREADYREADYERROR,
	READY,
	SENDLITTLEERROR,
	SENDBIGERROR,
	SENDSUCCESSFUL,
	RECVLITTLEERROR,
	RECVBIGERROR,
	RECVSUCCESSFUL
};

class udpClass
{
private:
    SOCKET mySocket;	/* Socket descriptor for server */
	struct sockaddr_in sockAddr;
	struct in_addr compAddress;
	int sockAddrLen;
	int err;
	WSADATA wsaData;
	char *dataBuffer;
	int dataLength;
	struct hostent *phe;
	int errorCode;
	bool ready;
public:
	udpClass() 
	{ 
		ready = false; 
		dataBuffer = new char[sizeof(dataPacket)];
	}
	bool start();
	bool start(const char* targetIP);
	bool sendMessage(const dataPacket* info);
	bool checkMessage(dataPacket* info);
	int getLastError();
};