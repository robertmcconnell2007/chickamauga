#pragma once

#define DATA_PACKET_SIZE      256

struct dataPacket
{
	short flag;
	char stuff[DATA_PACKET_SIZE];
};
