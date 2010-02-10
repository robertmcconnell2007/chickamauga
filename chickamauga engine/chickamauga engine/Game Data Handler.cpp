#include "Game Data Handler.h"
#include "mapSuperClass.h"
#include "messageHandler.h"
#include <fstream>
#include <string>
#include "infoLog.h"
//#define DEBUG
//#define ENDGAMEDEBUG

gameFileHandler::gameFileHandler(string name)
{
	setGame(name);
}

void gameFileHandler::_clearAll()
{
	gameName = "";
	mapName = "";
	rulesName = "";
	blue_army = "";
	gray_army = "";
}

bool gameFileHandler::checkFileNames()
{
	ifstream gameFile, fileLoaders[4];
	gameFile.open(gameName.c_str());
	if(!gameFile.is_open())
		return false;
	getline(gameFile, mapName);
	getline(gameFile, rulesName);
	getline(gameFile, blue_army);
	getline(gameFile, gray_army);
	gameFile.close();
	fileLoaders[0].open(mapName.c_str());
	if(!fileLoaders[0].is_open())
		return false;
	fileLoaders[0].close();
	fileLoaders[1].open(rulesName.c_str());
	if(!fileLoaders[1].is_open())
		return false;
	fileLoaders[1].close();
	fileLoaders[2].open(blue_army.c_str());
	if(!fileLoaders[2].is_open())
		return false;
	fileLoaders[2].close();
	fileLoaders[3].open(gray_army.c_str());
	if(!fileLoaders[3].is_open())
		return false;
	fileLoaders[3].close();
	return true;
}

void gameFileHandler::setGame(string name)
{
	gameName = name;
}

void gameFileHandler::setFiles()
{
	IH::Instance()->fileNames.map = mapName;
	IH::Instance()->fileNames.rules = rulesName;
	IH::Instance()->fileNames.army1 = blue_army;
	IH::Instance()->fileNames.army2 = gray_army;
}


IH::IH()
{
	attackerTotalPower=0;
	defenderTotalPower=0;
	prevSong = 0;
	currentTurn = 1;
	preppingCombat = false;
	beginWait = 0;
	timeOut = 10000;
	pingTime = 4000;
	connection = true;
	clickCancel = false;
	keysOff = false;
	fileNames.loadFileInfo("Init Data/chickamaugaini.txt");
	screenSize.x = 860;
	screenSize.y = 640;
	bitsperpixel = 32;
	amHost = false;
	canExit = false;
	canReinforce = false;
	menuUp = false;
	connected = false;
	menuOption = -1;
	escapeMenu = false;
	initialRulesDisplay = false;
	specificRulesDisplay = false;
	if(!fullScreen)
	{
		screen = SDL_SetVideoMode(screenSize.x, screenSize.y, bitsperpixel, SDL_SWSURFACE);		
	}
	else
	{
	}
	playersTurn = 0;
	gameState = 0;

	prefferedFaction = 10;
	otherPrefferedFaction = 10;
	
	screenShiftX = xMove = 0;
	screenShiftY = yMove = 0;
	fullScreen = false;
	nodeGui = false;
	mouseDown = false;
	runningGame = true;
	waiting = false;
	retreatCalled=false;
	canPickFaction = false;
	currentUnits[0] = NULL;
	currentUnits[1] = NULL;
	playersTurn = 0;
	playerIam = 0;

	//chat box info setup
	chatBox = new infoLog(128);
	chatString = "";
	startTyping = false;
	chatRect.w = 300;
	chatRect.h = 200;
	chatRect.x = 0;
	chatRect.y = 0;

	selectedX = selectedY = 0;
	font1 = TTF_OpenFont(fileNames.ttf.c_str(), 14);
	font2 = TTF_OpenFont(fileNames.ttf.c_str(), 26);
	titleScreen = load_my_image(fileNames.title.c_str());
	ourLogo     = load_my_image(fileNames.logo.c_str());
	utilityTiles5050 = load_my_image(fileNames.u5050tiles.c_str());
	menu = load_my_image(fileNames.menu.c_str());
	menuTiles = load_my_image(fileNames.menuOptions.c_str());
	turnTile = load_my_image(fileNames.turnCounter.c_str());
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
	u20060.h = 60;
	u20060.w = 200;
	GUIEndTurnBox.x = GUIFrameRect.x + GUIFrameRect.w - 60;
	GUIEndTurnBox.y = GUIFrameRect.y + 10;
	GUIEndTurnBox.h = GUIEndTurnBox.w = 50;
	UIbkColor = 0x05000;

	GameHotseatButton.h = GameJoinButton.h = GameHostButton.h = GameStartButton.h = basicRulesButton.h = GameQuitButton.h = u5050.h;
	GameHotseatButton.w = GameJoinButton.w = GameHostButton.w = GameStartButton.w = basicRulesButton.w = GameQuitButton.w = u5050.w;
	GameStartButton.x = 30;
	GameStartButton.y = screen->h - 10 - GameStartButton.h;
	GameQuitButton.x = screen->w - 30 - GameQuitButton.w;
	GameQuitButton.y = screen->h - 10 - GameQuitButton.h;
	GameHostButton.x = 30;
	GameJoinButton.x = 30;
	GameHotseatButton.x = 30;
	GameHostButton.y = 30;
	GameJoinButton.y = 80;
	GameHotseatButton.y = 130;
	GameMessageBox.w = 400;
	GameMessageBox.h = 50;
	GameMessageBox.x = screen->w - GameMessageBox.w;
	GameMessageBox.y = 0;
	matchEndOutputBox.x = matchEndOutputBox.y = 0;
	matchEndOutputBox.w = 300;
	matchEndOutputBox.h = 500;
	basicRulesButton.x = (screen->w / 2) - (basicRulesButton.w / 2);
	basicRulesButton.y = screen->h - 10 - basicRulesButton.h;

	basicRulesBox.h = 500;
	basicRulesBox.w = 600;
	basicRulesBox.x = (screen->w / 2) - (basicRulesBox.w / 2);
	basicRulesBox.y = 25;

	initialRulesDisplay = false;

	//menu button dimensions
	//275+55 = middle of the screen roughly, plus the offset for center of the scroll
	menuVolume.x = menuOptions.x = menuMain.x = menuClose.x = (275+55);
	menuOptions.y = 140;
	menuVolume.y = 140+90;
	menuMain.y = 140+180;
	menuClose.y = 140+270;
	menuVolume.h = menuOptions.h = menuMain.h = menuClose.h = 60;
	menuVolume.w = menuOptions.w = menuMain.w = menuClose.w = 200;


	//
	GUICalcCombatBox.h=GUICalcCombatBox.w=50;
	GUICalcCombatBox.x=GUIEndTurnBox.x-110;
	GUICalcCombatBox.y=GUIEndTurnBox.y;

	GUIResetCombatBox.h=GUIResetCombatBox.w=50;
	GUIResetCombatBox.x=GUICalcCombatBox.x+60;
	GUIResetCombatBox.y=GUICalcCombatBox.y;

	BlueOptionBox.h = GrayOptionBox.h = RandomOptionBox.h = u5050.h;
	BlueOptionBox.w = GrayOptionBox.w = RandomOptionBox.w = u5050.w;
	BlueOptionBox.x = GrayOptionBox.x = RandomOptionBox.x = 30;
	BlueOptionBox.y = 30;
	GrayOptionBox.y = 80;
	RandomOptionBox.y = 130;


	//initializing gameSound
	gameSound = new sound();

	yesNoBox.h = 200;
	yesNoBox.w = 300;
	yesNoBox.x = (screenSize.x/2)-(yesNoBox.w/2);
	yesNoBox.y = (screenSize.y/2)-(yesNoBox.h/2);
	noBox = yesBox = u5050;
	yesBox.x = yesNoBox.x + (yesNoBox.w/3)-(yesBox.w/2);
	yesBox.y = yesNoBox.y + (3*(yesNoBox.h/4))-(yesBox.h/2);
	noBox.x = yesNoBox.x + (2*(yesNoBox.w/3))-(noBox.w/2);
	noBox.y = yesNoBox.y + (3*(yesNoBox.h/4))-(noBox.h/2);

	reinforceBox.h = 200;
	reinforceBox.w = 300;
	reinforceBox.x = (screenSize.x/2)-(reinforceBox.w/2);
	reinforceBox.y = 50 + (screenSize.y/2)-(reinforceBox.h/2);
	okBox = u5050;
	okBox.x = reinforceBox.x + 25;
	okBox.y = reinforceBox.y + 25;

	ReturnToMenuBox.w = u20060.w;
	ReturnToMenuBox.h = u20060.h;
	ReturnToMenuBox.x = 0;//screen->w - ReturnToMenuBox.w;
	ReturnToMenuBox.y = screen->h - ReturnToMenuBox.h;

//box for current turn
	//turnRect.x = 0;
	//turnRect.y = 550;
	turnRect.w = 75;
	turnRect.h = 75;

	currentPage = 1;
	maxPages = -1;

#ifdef DEBUG
	output = "5.243.77.115";
#endif

#ifdef ENDGAMEDEBUG
	output = "chickamauga.txt";
	currentTurn = 14;
#endif
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
	MessageHandler::Instance()->sendMessage("OMG OMG OMG I CRASHEDED!",QUIT);
	MessageHandler::Instance()->sendNextMessage();
}

IH* IH::Instance()
{
	static IH instance;
	return &instance;
}

void IH::createMatch()
{
	gameRules = new rules;
	//map object = new map(mapName)
	map = new mapSuperClass(fileNames.map.c_str());
	//set up the players ip address, and any other neccessary data for them
	players[0].playerArmy.loadArmy((char*)fileNames.army1.c_str(),(char*)fileNames.army1colors.c_str());
	players[1].playerArmy.loadArmy((char*)fileNames.army2.c_str(),(char*)fileNames.army2colors.c_str());
	gameRules->loadRules(fileNames.rules);
	gameRules->coutRules();
	currentTurn = 1;
	// Start the sound when the game begins 
	//- - -- - - - - -   -- -- - -- -- -- -- --  - - -- - - -
	//- - -- - - - - -   -- -- - -- -- -- -- --  - - -- - - -
	if(playerIam == 0)
	{
		IH::Instance()->gameSound->playWAV(unionMusic);
		IH::Instance()->prevSong = unionMusic;
	}
	else
	{
		IH::Instance()->gameSound->playWAV(confederateMusic);
		IH::Instance()->prevSong = unionMusic;
	}
	//- - -- - - - - -   -- -- - -- -- -- -- --  - - -- - - -
	//- - -- - - - - -   -- -- - -- -- -- -- --  - - -- - - -
	//- - -- - - - - -   -- -- - -- -- -- -- --  - - -- - - -		
}

int IH::endMatch()
{
	return 2;
}

void IH::endGame()
{
	if(playingLAN)
	{
		MessageHandler::Instance()->sendMessage("Your opponent has left the game.", QUIT);
		MessageHandler::Instance()->sendNextMessage();
	}

	//if((gameState == matchMainPhase || 
	//	gameState == matchCombatPhase ||
	//	 gameState == atMatchPrep || 
	//	 gameState == reviewingMatch)) //&& there is a connection to the server
	//{
	//	//send a message to the server saying that this player quit?
	//}
	runningGame = false;
}


void IH::randomizefactions()
{
	if(getRandomNum() % 2 == 0)
	{
		prefferedFaction = 0;
		otherPrefferedFaction = 1;
	}
	else
	{
		prefferedFaction = 1;
		otherPrefferedFaction = 0;
	}
}

void IH::drawMenu()
{
	SDL_SetColorKey(menu,SDL_SRCCOLORKEY,0xffffff);
	SDL_Rect tempRect;
	tempRect.x = 275;
	tempRect.y = 50;
	SDL_BlitSurface(menu, NULL, screen , &tempRect);
	drawATile(menuTiles, &u20060, 0, screen, menuOptions.x,menuOptions.y);
	drawATile(menuTiles, &u20060, 1, screen, menuVolume.x, menuVolume.y);
	drawATile(menuTiles, &u20060, 2, screen, menuMain.x, menuMain.y);
	drawATile(menuTiles, &u20060, 3, screen, menuClose.x, menuClose.y);
}

void IH::_resetAll()
{
	gameSound->stopMusic(prevSong);
	amHost = false;
	waiting = false;
	canPickFaction = false;
	connected = false;
	gameState = atTitleScreen;
	prefferedFaction = 10;
	otherPrefferedFaction = 10;
	currentTurn = 0;
	map->deleteMap();
	map = NULL;
	if(currentBattle.attackers.size() > 0)
		currentBattle.attackers.clear();
	if(currentBattle.defenders.size() > 0)
		currentBattle.defenders.clear();
	canReinforce = false;
	canExit = false;
	menuUp = false;
	escapeMenu = false;
	menuOption = 0;
	matchFileNames._clearAll();
	gameRules->deleteRules();
	gameRules = NULL;
	retreatCalled = false;
	enemyUnitsSelected = false;
	unit1Selected = false;
	unit2Selected = false;
	currentUnits[0] = NULL;
	currentUnits[1] = NULL;
	selectedNode = NULL;
	playingLAN = false;
	preppingCombat = false;
	keysOff = false;
}