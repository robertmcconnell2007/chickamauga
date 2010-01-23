#include "messageHandler.h"
#include "dataPacket.h"
#include "UDP.h"

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
bool MessageHandler::setupClient(const char* targetIP)
{
	return message.start(targetIP);
}
bool MessageHandler::sendMessage(string *outgoingMessage, int flag)
{
	if(outSize == PACKETBUFFER)
		return false;
	flagArrayOut[outSize] = flag;
	stringArrayOut[outSize] = *outgoingMessage;
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
