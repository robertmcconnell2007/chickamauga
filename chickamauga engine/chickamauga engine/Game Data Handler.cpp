#include "Game Data Handler.h"
#include "mapSuperClass.h"

IH::IH()
{
	screenSize.x = 860;
	screenSize.y = 640;
	bitsperpixel = 32;
	screenShiftX = xMove = 0;
	screenShiftY = yMove = 0;
	fullScreen = false;
	mouseDown = false;
	playingMatch = false;
	runningGame = true;
	if(!fullScreen)
		screen = SDL_SetVideoMode(screenSize.x, screenSize.y, bitsperpixel, SDL_SWSURFACE);
	else
	{
	}
	map = NULL;
}

IH::~IH()
{
//	SDL_FreeSurface(screen);
	if(map)
	{
		map->deleteMap();
		//delete map;
	}
	if(gameRules)
	{
		gameRules->deleteRules();
		//delete gameRules;
	}
	players[0].deletePlayer();
	players[1].deletePlayer();
}

IH* IH::Instance()
{
	static IH instance;
	return &instance;
}

void IH::createMatch(string mapName, char player1IP[20], char player2IP[20])
{
	playingMatch = true;
	gameRules = new rules;
	//map object = new map(mapName)
	map = new mapSuperClass(mapName.c_str());
	//set up the players ip address, and any other neccessary data for them
}

int IH::endMatch()
{
	return 2;
}