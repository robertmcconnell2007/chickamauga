#include "Game Data Handler.h"

static const int SCREEN_WIDTH = 860;
static const int SCREEN_HEIGHT = 640;
static const int SCREEN_BPP = 32;

IH::IH()
{
	playingMatch = false;
	runningGame = true;
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);
	map = NULL;
}

IH::~IH()
{
	SDL_FreeSurface(screen);
	if(map)
		map->deleteMap();
	if(gameRules)
		gameRules->deleteRules();
	players[0].deletePlayer();
	players[1].deletePlayer();
}

IH* IH::Instance()
{
	static IH instance;
	return &instance;
}

void IH::createMatch(string mapName, string rulesName, char player1IP[20], char player2IP[20])
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