#include "messageHandler.h"
#include "dataPacket.h"

struct dataPacket;

MessageHandler::MessageHandler()
{
	size = 0;
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
	packet.flag = flag;
	strcpy(packet.stuff,outgoingMessage->c_str());
	return message.sendMessage(&packet);
}
bool MessageHandler::getMessage(string *incommingMessage, int *flag)
{
	if(size <= 0)
		return false;
	*incommingMessage = stringArray[0];
	*flag = flagArray[0];
	updateArray();
	return true;
}
bool MessageHandler::checkMessages()
{
	if(message.checkMessage(&packet))
	{
		if(size == PACKETBUFFER)
			return false;
		flagArray[size] = packet.flag;
		stringArray[size] = packet.stuff;
		size++;
		return true;
	}
	return false;
}
void MessageHandler::updateArray()
{
	for(int i = 0; i < size-1; i++)
	{
		flagArray[i] = flagArray[i+1];
		stringArray[i] = stringArray[i+1];
	}
	size -= 1;
}