#pragma once
#include "unitClass.h"
struct player
{
	char IPAddress[20];
	int pointsEarned;
	int playerPosition;
	//unit array
	armyClass playerArmy;
	//something else?

	void startTurn();
	void deletePlayer();
};