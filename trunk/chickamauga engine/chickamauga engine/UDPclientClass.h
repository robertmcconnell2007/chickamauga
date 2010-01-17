#pragma once

#include <stdio.h>      /* for printf(), fprintf() */
#include <winsock.h>    /* for socket(),... */
#include <stdlib.h>     /* for exit() */

#define MAXPENDING 5    /* Maximum outstanding connection requests */
#define SENDBUFSIZE 128   /* Size of receive buffer */

#define MYMESG			"hello world"
#define SRVR_ADDRESS	"5.217.162.130"
#define SRVR_PORT		7654

void DieWithError(char *errorMessage)
{
	fprintf(stderr,"%s\n", errorMessage);
	exit(1);
}
struct dataPacket;


class UDPclientClass
{
private:
    SOCKET srvrSock;                    /* Socket descriptor for server */
    struct sockaddr_in srvrAddr; /* Local address */
    struct sockaddr_in clntAddr; /* Client address */
    int clntLen;            /* Length of client address data structure */
    WSADATA wsaData;                 /* Structure for WinSock setup communication */
	char *sendbuf;        /* Buffer for echo string */
	int send_len;                    /* Size of received message */
	int msgsize;
	dataPacket outgoing;
	dataPacket *incomming;
	char ip[20];
public:
	UDPclientClass();
	UDPclientClass(const char* targetComp);
	bool sendMessage(const dataPacket* info);
	bool checkMessage(dataPacket* info);
};
UDPclientClass::UDPclientClass()
{
    if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) /* Load Winsock 2.0 DLL */
    {
        fprintf(stderr, "WSAStartup() failed");
        exit(1);
    }
    // Create socket for incoming connections
    if ((srvrSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
        DieWithError("socket() failed");

    // Construct local address structure
    memset(&clntAddr, 0, sizeof(clntAddr));   /* Zero out structure */
    clntAddr.sin_family = AF_INET;                /* Internet address family */
    clntAddr.sin_addr.s_addr = inet_addr(SRVR_ADDRESS); /* Any incoming interface */
    clntAddr.sin_port = htons(SRVR_PORT);      /* Local port */
	strcpy(ip,SRVR_ADDRESS);
	unsigned long cmdptr = 1;
	ioctlsocket(srvrSock, FIONBIO, &cmdptr);
}
UDPclientClass::UDPclientClass(const char* targetComp)
{
    if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) /* Load Winsock 2.0 DLL */
    {
        fprintf(stderr, "WSAStartup() failed");
        exit(1);
    }
    // Create socket for incoming connections
    if ((srvrSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
        DieWithError("socket() failed");

    // Construct local address structure
    memset(&clntAddr, 0, sizeof(clntAddr));   /* Zero out structure */
    clntAddr.sin_family = AF_INET;                /* Internet address family */
    clntAddr.sin_addr.s_addr = inet_addr(targetComp); /* Any incoming interface */
    clntAddr.sin_port = htons(SRVR_PORT);      /* Local port */
	strcpy(ip,targetComp);
	unsigned long cmdptr = 1;
	ioctlsocket(srvrSock, FIONBIO, &cmdptr);
}
bool UDPclientClass::sendMessage(const dataPacket* info)
{
    // Set the size of the in-out parameter
    clntLen = sizeof(clntAddr);
	// Receive message from client
	outgoing = *info;
	sendbuf = reinterpret_cast<char *>(&outgoing);
	if ((send_len = sendto(srvrSock, sendbuf, msgsize, 0,(struct sockaddr *) &clntAddr, clntLen)) == SOCKET_ERROR) {
		DieWithError("sendto() failed");
		return false;
	}
	printf("Sent %d of %d bytes to %s:%u\n", send_len,msgsize, SRVR_ADDRESS, SRVR_PORT);
	return true;

}
bool UDPclientClass::checkMessage(dataPacket* info)
{
    clntLen = sizeof(clntAddr);
	int err = 0;
	int recvMsgSize = recvfrom(srvrSock, sendbuf, msgsize, 0, (struct sockaddr *) &clntAddr, &clntLen);
	if(recvMsgSize == SOCKET_ERROR)
	{
		if((err = WSAGetLastError()) != WSAEMSGSIZE && err != WSAEWOULDBLOCK)
		{
			printf("failed to recieve: %d\n", err);
			return false;
		}
	}
	if(recvMsgSize > 0)
	{
		incomming = reinterpret_cast<dataPacket *>(sendbuf);
		*info = *incomming;
		printf("Received message: %s\n", incomming->message);
		return true;
	}
	return false;
}