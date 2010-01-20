#pragma once
#include <iostream>
using namespace std;
#include "SDL.h"
#include "SDL_ttf.h"
#include "mapSuperClass.h"
#include "rules.h"
#include "player.h"
#include "fileLoader.h"

enum game_states
{
	atLogo,
	atTitleScreen,
	atMatchPrep,
	matchMainPhase,
	matchCombatPhase,
	reviewingMatch
};

class IH
{
private:
	IH();
	~IH();
	//map object
	mapSuperClass * map;
	
	//unit handler object
	SDL_Rect screenSize;
	int bitsperpixel;
	bool fullScreen;
	
	bool runningGame;
	int  gameState;
	int  screenShiftX, xMove;
	int  screenShiftY, yMove;
	//0 == player1, 1 == player2
	int  playersTurn;
	int  playerIam;
	//actual and first used to identify which hex the user clicks on
	int actualX, actualY;
	int firstX, firstY;
	bool mouseDown;
	//for unit movement
	bool nodeGui;
	int selectedX;
	int selectedY;
public:
	//2 player objects
	player players[2];
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
	
	//SDL_Rect GUIGameFrame;

	SDL_Rect GUIFrameRect;
	SDL_Rect UISlots[3];
	SDL_Rect GUIEndTurnBox;
	SDL_Rect u5050;

	//unit variables
	unitClass * currentUnits[2];
	map_node * selectedNode;
	bool enemyUnitsSelected;
	bool unit1Selected;
	bool unit2Selected;
	SDL_Event event;
	//public functions
	static IH * Instance();
	map_node ** returnMap() {return map->getMap();}
	void createMatch(string mapName, char player1IP[20], char player2IP[20]);
	int endMatch();//returns which player won

	bool isGameRunning()  {return runningGame;}
	void endGame();

	//these functions are defined in the tools.cpp file!
	void handlePrimaryInput();
	void update(int mspassed);
	void drawAll();
};