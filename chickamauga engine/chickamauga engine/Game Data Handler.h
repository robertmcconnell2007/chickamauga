#pragma once
#include <iostream>
using namespace std;
#include "SDL.h"
#include "SDL_ttf.h"
#include "mapSuperClass.h"
#include "rules.h"
#include "player.h"

enum game_states
{
	atLogo,
	atTitleScreen,
	atMatchPrep,
	playingMatch,
	reviewingMatch
};

class IH
{
private:
	IH();
	~IH();
	//map object
	mapSuperClass * map;
	//rules object
	rules * gameRules;
	//2 player objects
	player players[2];
	//unit handler object
	SDL_Rect screenSize;
	int bitsperpixel;
	bool fullScreen;
	
	bool runningGame;
	int  gameState;
	int  screenShiftX, xMove;
	int  screenShiftY, yMove;
	int  playersTurn;
	//actual and first used to identify which hex the user clicks on
	int actualX, actualY;
	int firstX, firstY;
	bool mouseDown;
	//for unit movement
	bool unitSelected;
	bool nodeGui;
	int selectedX;
	int selectedY;
public:
	//true type font stuff
	TTF_Font *font1;
	TTF_Font *font2;
	int fontSize1;
	int fontSize2;
	//surfaces and rects
	SDL_Surface * screen;
	SDL_Surface * ourLogo;
	SDL_Surface * titleScreen;
	SDL_Surface * GUIGameFrame;
	
	//SDL_Rect GUIGameFrame;


	//unit variables
	unitClass * currentUnits[2];
	int selectedUnit;
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