#pragma once
#include <iostream>
using namespace std;
#include "SDL.h"
#include "SDL_ttf.h"
#include "mapSuperClass.h"
#include "rules.h"
#include "player.h"
#include "fileLoader.h"
#include "infoLog.h"
#include "sound.h"
#include <vector>

enum game_states
{
	atLogo,
	atTitleScreen,
	atMatchPrep,
	atMatchPrepSecond,
	matchMainPhase,
	matchCombatPhase,
	reviewingMatch
};

struct gameFileHandler
{
	string gameName;
	string mapName;
	string rulesName;
	string blue_army;
	string gray_army;
	gameFileHandler() {}
	gameFileHandler(string name);
	bool checkFileNames();
	void setGame(string name);
	void setFiles();
	void _clearAll();
};

struct battle
{
	vector<unitClass*> attackers;
	vector<unitClass*> defenders;
	int calcBattle();
};

class IH
{
private:
	IH();
	~IH();
	//map object
	
	//unit handler object
	SDL_Rect screenSize;
	int bitsperpixel;
	bool fullScreen;
	bool amHost;
	bool runningGame;
	bool waiting;
	bool canPickFaction;
	bool connected;
	int  gameState;
	int  screenShiftX, xMove;
	int  screenShiftY, yMove;
	int  prefferedFaction;
	int  otherPrefferedFaction;
	int  currentTurn;
	//0 == player1, 1 == player2
	
	void randomizefactions();
	//actual and first used to identify which hex the user clicks on
	int actualX, actualY;
	int firstX, firstY;
	bool mouseDown;
	//for unit movement
	bool nodeGui;
	int selectedX;
	int selectedY;
	void drawMenu();
public:
	int gameStart;
	mapSuperClass * map;
	sound * gameSound;
	battle currentBattle;
	int  playersTurn;
	int  playerIam;
	Uint32 beginWait;
	int timeOut;
	bool clickCancel;
	int pingTime;
	bool connection;

	int currentPage;
	int maxPages;
	
	//previous song
	int prevSong;
	//exit bool
	bool canExit;
	//entry bool
	bool canReinforce;

	SDL_Rect yesNoBox;
	SDL_Rect yesBox;
	SDL_Rect noBox;
	SDL_Rect reinforceBox;
	SDL_Rect okBox;

	//menu bool
	bool menuUp;
	bool escapeMenu;
	bool initialRulesDisplay;
	bool specificRulesDisplay;
	int menuOption;

	//chat box info
	infoLog *chatBox;
	string chatString;
	bool startTyping;
	SDL_Rect chatRect;

	gameFileHandler matchFileNames;
	string currentMessage;
	int currentMessageFlag;
	string output;
	//2 player objects
	player * players[2];
	//rules object
	rules * gameRules;
	//file names
	FileInfo fileNames;
	//true type font stuff
	TTF_Font *font1;
	TTF_Font *font2;
	int fontSize1;
	int fontSize2;
	SDL_Color textColor;
	Uint32 UIbkColor;
	//surfaces and rects
	SDL_Surface * screen;
	SDL_Surface * ourLogo;
	SDL_Surface * titleScreen;
	SDL_Surface * GUIGameFrame;
	SDL_Surface * utilityTiles5050;
	SDL_Surface * menu;
	SDL_Surface * menuTiles;
	SDL_Surface * turnTile;
	
	//SDL_Rect GUIGameFrame;

	SDL_Rect GUIFrameRect;
	SDL_Rect UISlots[3];
	SDL_Rect GUIEndTurnBox;
	SDL_Rect GUICalcCombatBox;
	SDL_Rect GUIResetCombatBox;
	SDL_Rect u5050;
	SDL_Rect u20060;
	SDL_Rect menuVolume;
	SDL_Rect menuOptions;
	SDL_Rect menuMain;
	SDL_Rect menuClose;
	SDL_Rect GameStartButton;
	SDL_Rect GameQuitButton;
	SDL_Rect GameHostButton;
	SDL_Rect GameJoinButton;
	SDL_Rect GameHotseatButton;
	SDL_Rect GameMessageBox;
	SDL_Rect BlueOptionBox;
	SDL_Rect GrayOptionBox;
	SDL_Rect RandomOptionBox;
	SDL_Rect ReturnToMenuBox;
	SDL_Rect matchEndOutputBox;
	SDL_Rect basicRulesButton;
	SDL_Rect basicRulesBox;
	SDL_Rect turnRect;

	//unit variables
	unitClass * currentUnits[2];
	map_node * selectedNode;
	bool retreatCalled;
	bool enemyUnitsSelected;
	bool unit1Selected;
	bool unit2Selected;
	bool playingLAN;
	bool preppingCombat;
	bool keysOff;
	int attackerTotalPower;
	int defenderTotalPower;
	SDL_Event event;
	//public functions
	static IH * Instance();
	map_node ** returnMap() {return map->getMap();}
	void createMatch();
	int endMatch();//returns which player won
	int getCurrentTurn(){return currentTurn;}

	bool isGameRunning()  {return runningGame;}
	void endGame();
	void _resetAll();

	void networkRetreat(string unitName);

	//these functions are defined in the tools.cpp file!
	void handlePrimaryInput();
	void update(int mspassed);
	void drawAll();
	bool handleMessage();	
};