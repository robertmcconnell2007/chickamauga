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
	QUIT,
	GAMEBEGIN,
};

class MessageHandler
{
private:
	udpClass message;
	int flagArrayIn[PACKETBUFFER];
	string stringArrayIn[PACKETBUFFER];
	int flagArrayOut[PACKETBUFFER];
	string stringArrayOut[PACKETBUFFER];
	int inSize;
	int outSize;
	dataPacket packet;
	MessageHandler();
	~MessageHandler();
	void updateArrayIn();
	void updateArrayOut();
public:
	bool setupHost();
	bool setupClient(const char* targetIP);
	bool sendMessage(string outgoingMessage, int flag);
	bool sendNextMessage();
	bool checkMessages();
	bool getMessage(string* incommingMessage, int *flag);
	int getLastUDPError();
	static MessageHandler* Instance()
	{
		static MessageHandler instance;
		return &instance;
	}
};