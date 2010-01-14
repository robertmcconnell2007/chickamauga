#pragma once
#include <iostream>
using namespace std;
#include "SDL.h"
#include "mapSuperClass.h"
#include "rules.h"
#include "player.h"

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
	bool playingMatch;
	bool runningGame;

	SDL_Surface * screen;
public:
	//public variables
	SDL_Event event;
	//public functions
	static IH * Instance();

	void createMatch(string mapName, string rulesName, char player1IP[20], char player2IP[20]);
	int endMatch();//returns which player won

	bool isPlayingMatch() {return playingMatch;}
	bool isGameRunning()  {return runningGame;}
	void endGame()        {runningGame = false;}

	//these functions are defined in the tools.cpp file!
	void handlePrimaryInput();
	void update(int mspassed);
	void drawAll();
};