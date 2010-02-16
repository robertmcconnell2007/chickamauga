#include "UDP.h"
#include "dataPacket.h"
#define MSGDEBUG

#ifdef MSGDEBUG
#include "Game Data Handler.h"
#include <fstream>
#endif

#include <string>
using namespace std;

struct dataPacket;

bool udpClass::start()
{
	if(!ready)
	{
		if(WSAStartup(MAKEWORD(2,0),&wsaData) != 0)
		{
			errorCode = WSASTARTUPERROR;
			return false;
		}
		if((mySocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
		{
			errorCode = SOCKETINIERROR;
			return false;
		}
		char ac[80];
		if(gethostname(ac, sizeof(ac)) == SOCKET_ERROR)
		{
			errorCode = GETHOSTERROR;
			return false;
		}
		phe = gethostbyname(ac);
		if(phe == 0)
		{
			errorCode = GETHOSTIPERROR;
			return false;
		}
		memcpy(&compAddress, phe->h_addr_list[0], sizeof(struct in_addr));
		memset(&sockAddr,0,sizeof(sockAddr));
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(compAddress));
		sockAddr.sin_port = htons(SRVR_PORT);
		unsigned long cmdptr = 1;
		ioctlsocket(mySocket,FIONBIO,&cmdptr);
		if(bind(mySocket,(struct sockaddr *) &sockAddr, sizeof(sockAddr)) == SOCKET_ERROR)
		{
			errorCode = BINDERROR;
			return false;
		}
		isHost = true;
		ready = true;
		errorCode = READY;
		return true;
	}
	errorCode = ALREADYREADYERROR;
	return false;
}
bool udpClass::start(const char *targetIP)
{
	if(!ready)
	{
		if(WSAStartup(MAKEWORD(2,0),&wsaData) != 0)
		{
			errorCode = WSASTARTUPERROR;
			return false;
		}
		if((mySocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
		{
			errorCode = SOCKETINIERROR;
			return false;
		}
		if(targetIP == "")
		{
			errorCode = INVALIDIP;
			return false;
		}
		memset(&sockAddr,0,sizeof(sockAddr));
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_addr.s_addr = inet_addr(targetIP);
		sockAddr.sin_port = htons(SRVR_PORT);
		unsigned long cmdptr = 1;
		ioctlsocket(mySocket,FIONBIO,&cmdptr);
		ready = true;
		errorCode = READY;
		isHost = false;
		return true;
	}
	errorCode = ALREADYREADYERROR;
	return false;
}
bool udpClass::sendMessage(const dataPacket* info)
{
	if(ready)
	{
		dataBuffer = (char*)info;
#ifdef MSGDEBUG
		fstream outFile;
		outFile.open("sendFile.txt",fstream::out | fstream::app);
		outFile << SDL_GetTicks()-IH::Instance()->gameStart << " : Send message flagged:" << info->flag << ": Containing :" << info ->stuff << ":\n";
		outFile.close();
#endif
		if(isHost)
		{
			sockAddrLen = sizeof(client);
			dataLength = sendto(mySocket,dataBuffer,sizeof(dataPacket),0,(struct sockaddr *) &client, sockAddrLen);
		}
		else
		{
			sockAddrLen = sizeof(sockAddr);
			dataLength = sendto(mySocket,dataBuffer,sizeof(dataPacket),0,(struct sockaddr *) &sockAddr, sockAddrLen);
		}
		if(dataLength == SOCKET_ERROR)
		{
			if((err = WSAGetLastError()) != WSAEMSGSIZE && err != WSAEWOULDBLOCK)
			{
				errorCode = SENDBIGERROR;
				return false;
			}
			errorCode = SENDLITTLEERROR;
		}
		if(dataLength > 0)
		{
#ifdef MSGDEBUG
			fstream outFile2;
			outFile2.open("sendFile.txt",fstream::out | fstream::app);
			outFile2 << SDL_GetTicks()-IH::Instance()->gameStart << " : Send message successful : " << dataLength << "/" << sizeof(dataPacket) << " : sent\n";
			outFile2.close();
#endif
			errorCode = SENDSUCCESSFUL;
			return true;
		}
		return false;
	}
	errorCode = NOTREADYERROR;
	return false;
}
bool udpClass::checkMessage(dataPacket* info)
{
	
	if(ready)
	{
		if(isHost)
		{
			sockAddrLen = sizeof(client);
			dataLength = recvfrom(mySocket,dataBuffer,sizeof(dataPacket),0,(struct sockaddr *) &client, &sockAddrLen);
		}
		else
		{
			sockAddrLen = sizeof(sockAddr);
			dataLength = recvfrom(mySocket,dataBuffer,sizeof(dataPacket),0,(struct sockaddr *) &sockAddr, &sockAddrLen);
		}
		if(dataLength == SOCKET_ERROR)
		{
			if((err = WSAGetLastError()) != WSAEMSGSIZE && err != WSAEWOULDBLOCK)
			{
				errorCode = RECVBIGERROR;
				return false;
			}
			errorCode = RECVLITTLEERROR;
		}
		if(dataLength > 0)
		{
			*info = *(dataPacket*)dataBuffer;
			errorCode = RECVSUCCESSFUL;
#ifdef MSGDEBUG
			fstream outFile;
			outFile.open("recvFile.txt",fstream::out | fstream::app);
			outFile << SDL_GetTicks()-IH::Instance()->gameStart << " : Recv message successful : " << dataLength << "/" << sizeof(dataPacket) << " : flagged : " << info->flag << " : containing :" << info->stuff << ":\n";
			outFile.close();
#endif
			return true;
		}
		return false;
	}
	errorCode = NOTREADYERROR;
	return false;
}

int udpClass::getLastError()
{
	return errorCode;
}
void udpClass::shutDown()
{
	if(ready)
	{
		ready = false;
		WSACleanup();
	}
}