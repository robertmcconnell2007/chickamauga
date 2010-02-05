#include "messageHandler.h"
#include "dataPacket.h"
#include "UDP.h"
#include <math.h>

struct dataPacket;

MessageHandler::MessageHandler()
{
	inSize = 0;
	outSize = 0;
}
MessageHandler::~MessageHandler()
{
    WSACleanup();
}
bool MessageHandler::setupHost()
{
	return message.start();
}
int stringToInt2(string str)
{
	int newInt = 0;
	int length = 0;

	length = str.length();
	for(int i = length - 1, j = 0; i >= 0; --i, ++j)
	{
		newInt += (int)((int)(str.c_str()[i] - '0') * pow(10.0, (double)j));
	}

	return newInt;
}
string validateIP(string targetIP)
{
	string tempstr = targetIP;
	int tempint = 0, n = 0;
	for(int i = 0; i < 4; i++)
	{
		if(i != 3)
		{
			n = tempstr.find(".",0);
			tempint = stringToInt2(tempstr.substr(0,n));
		}
		else
		{
			tempint = stringToInt2(tempstr);
			tempstr.clear();
		}
		if(tempint > 255)
		{
			return "";
		}
		if(i != 3)
			tempstr = tempstr.substr(0,n);
	}
	return targetIP;
}
bool MessageHandler::setupClient(string targetIP)
{
	targetIP = validateIP(targetIP);
	return message.start(targetIP.c_str());
}
bool MessageHandler::sendMessage(string outgoingMessage, int flag)
{
	if(outSize == PACKETBUFFER)
		return false;
	flagArrayOut[outSize] = flag;
	stringArrayOut[outSize] = outgoingMessage;
	outSize++;
	return true;
}
bool MessageHandler::sendNextMessage()
{
	if(outSize <= 0)
		return false;
	packet.flag = flagArrayOut[0];
	strcpy(packet.stuff,stringArrayOut[0].c_str());
	updateArrayOut();
	return message.sendMessage(&packet);
}
bool MessageHandler::getMessage(string *incommingMessage, int *flag)
{
	if(inSize <= 0)
		return false;
	*incommingMessage = stringArrayIn[0];
	*flag = flagArrayIn[0];
	updateArrayIn();
	return true;
}
bool MessageHandler::checkMessages()
{
	bool validMessage = false;
	if(message.checkMessage(&packet))
	{
		if(inSize == PACKETBUFFER)
			return false;
		for(int i = 0; i < RECVSUCCESSFUL; ++i)
		{
			if(packet.flag == i)
				validMessage = true;
		}
		if(!validMessage)
			return false;
		flagArrayIn[inSize] = packet.flag;
		stringArrayIn[inSize] = packet.stuff;
		inSize++;
		return true;
	}
	return false;
}
void MessageHandler::updateArrayIn()
{
	for(int i = 0; i < inSize-1; i++)
	{
		flagArrayIn[i] = flagArrayIn[i+1];
		stringArrayIn[i] = stringArrayIn[i+1];
	}
	inSize -= 1;
}
void MessageHandler::updateArrayOut()
{
	for(int i = 0; i < outSize-1; i++)
	{
		flagArrayOut[i] = flagArrayOut[i+1];
		stringArrayOut[i] = stringArrayOut[i+1];
	}
	outSize -= 1;
}

int MessageHandler::getLastUDPError() 
{ 
	return message.getLastError(); 
}
