#pragma once
#include "dataPacket.h"
#include "UDP.h"
#include <vector>
#include <string>
using namespace std;

struct dataPacket;

#define PACKETBUFFER 20

enum messagerFlags
{
	GAMEFILENAME,
	GETIP,
	PICKFACTION,
	MOVEUNIT,
	KILLUNIT,
	REINFORCEUNIT,
	EXITUNIT,
	COMBATPHASE,
	STARTTURN,
	ENDTURN,
	DEFENDERRETREAT,
	CHATMESSAGE,
	QUIT
};

class MessageHandler
{
private:
	udpClass message;
	int flagArray[PACKETBUFFER];
	string stringArray[PACKETBUFFER];
	int size;
	dataPacket packet;
	MessageHandler();
	~MessageHandler();
	void updateArray();
public:
	bool setupHost();
	bool setupClient(const char* targetIP);
	bool sendMessage(string* outgoingMessage, int flag);
	bool checkMessages();
	bool getMessage(string* incommingMessage, int *flag);
	int getLastUDPError() { return message.getLastError(); }
	static MessageHandler& Instance()
	{
		static MessageHandler instance;
		return instance;
	}
};