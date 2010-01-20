#include "Game Data Handler.h"
#include "mapSuperClass.h"
#include <fstream>

IH::IH()
{
	fileNames.loadFileInfo("Init Data/chickamaugaini.txt");
	screenSize.x = 860;
	screenSize.y = 640;
	bitsperpixel = 32;
	if(!fullScreen)
	{
		screen = SDL_SetVideoMode(screenSize.x, screenSize.y, bitsperpixel, SDL_SWSURFACE);
		
	}
	else
	{
	}
	playersTurn = 2;
	gameState = 0;
	
	screenShiftX = xMove = 0;
	screenShiftY = yMove = 0;
	fullScreen = false;
	nodeGui = false;
	mouseDown = false;
	runningGame = true;
	currentUnits[0] = NULL;
	currentUnits[1] = NULL;
	playersTurn = 0;
	playerIam = 0;
	selectedX = selectedY = 0;
	font1 = TTF_OpenFont(fileNames.ttf.c_str(), 14);
	font2 = TTF_OpenFont(fileNames.ttf.c_str(), 26);
	titleScreen = load_my_image(fileNames.title.c_str());
	ourLogo     = load_my_image(fileNames.logo.c_str());
	utilityTiles5050 = load_my_image(fileNames.u5050tiles.c_str());
	GUIGameFrame         = NULL;
	map = NULL;
	GUIFrameRect.h = 70;
	GUIFrameRect.w = screen->w;
	GUIFrameRect.y = screen->h - GUIFrameRect.h;
	GUIFrameRect.x = 0;
	for(int i = 0; i < 3; ++i)
	{
		UISlots[i].h = 60;
		UISlots[i].w = (screen->w - (screen->w * .5)) / 3;
		UISlots[i].x = (screen->w * .1) + (UISlots[i].w * i) + (10 * i);
		UISlots[i].y = GUIFrameRect.y + 10;
	}
	textColor.r = textColor.g = textColor.b = 0xFF;
	u5050.h = u5050.w = 50;
	GUIEndTurnBox.x = GUIFrameRect.x + GUIFrameRect.w - 60;
	GUIEndTurnBox.y = GUIFrameRect.y + 10;
	GUIEndTurnBox.h = GUIEndTurnBox.w = 50;
	UIbkColor = 0x05000;
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
	gameState = matchMainPhase;
	gameRules = new rules;
	//map object = new map(mapName)
	map = new mapSuperClass(mapName.c_str());
	gameRules->loadRules(fileNames.rules);
	gameRules->coutRules();
	//set up the players ip address, and any other neccessary data for them
	players[0].playerArmy.loadArmy((char*)fileNames.army1.c_str(),(char*)fileNames.army1colors.c_str());
	players[1].playerArmy.loadArmy((char*)fileNames.army2.c_str(),(char*)fileNames.army2colors.c_str());
	playerIam = 0;
}

int IH::endMatch()
{
	return 2;
}

void IH::endGame()
{
	if((gameState == matchMainPhase || 
		gameState == matchCombatPhase ||
		 gameState == atMatchPrep || 
		 gameState == reviewingMatch)) //&& there is a connection to the server
	{
		//send a message to the server saying that this player quit?
	}
	runningGame = false;
}