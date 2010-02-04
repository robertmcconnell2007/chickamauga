#include "player.h"
#include "Game Data Handler.h"

void player::deletePlayer()
{
}

void player::startTurn()
{
	playerArmy.resetMoves();
	IH::Instance()->map->cleanMap();
}