#include "Game Data Handler.h"
#include "mapSuperClass.h"

IH::IH()
{
	playersTurn = 2;
	gameState = 0;
	screenSize.x = 860;
	screenSize.y = 640;
	bitsperpixel = 32;
	screenShiftX = xMove = 0;
	screenShiftY = yMove = 0;
	fullScreen = false;
	mouseDown = false;
	runningGame = true;
	unitSelected=false;
	selectedX=selectedY=0;
	if(!fullScreen)
	{
		screen = SDL_SetVideoMode(screenSize.x, screenSize.y, bitsperpixel, SDL_SWSURFACE);
		titleScreen = load_my_image("TitlePage.bmp");
		ourLogo = load_my_image("DoubleTappLogo.bmp");
	}
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
	gameState = playingMatch;
	gameRules = new rules;
	//map object = new map(mapName)
	map = new mapSuperClass(mapName.c_str());
	gameRules->loadRules("chickamauga rule set.txt");
	gameRules->coutRules();
	//set up the players ip address, and any other neccessary data for them
	players[0].playerArmy.loadArmy("unionArmyUnits.txt","unionArmy.bmp");
	players[1].playerArmy.loadArmy("rebelArmyUnits.txt","rebelArmy.bmp");
}

int IH::endMatch()
{
	return 2;
}

void IH::endGame()
{
	if((gameState == playingMatch || 
		 gameState == atMatchPrep || 
		 gameState == reviewingMatch)) //&& there is a connection to the server
	{
		//send a message to the server saying that this player quit?
	}
	runningGame = false;
}