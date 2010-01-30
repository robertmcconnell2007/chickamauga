#pragma once
#include "SDL.h"
#include "GraphicsLoader.h"
#include "rules.h"
#include <fstream>
#include <string>

struct vector2d
{
	int x;
	int y;
};

enum operation
{//MUT = move unit to, MUF = moveUnitFrom
	MUTKilled,
	MUTExited,
	MUTField,
	MUFField,
	MUFReinforce,
};

enum terrainTypes;
struct map_node;
struct node_edge;
class mapSuperClass;

class unitClass
{
private:
	std::string name;
	bool movedThisTurn;
	int power;
	bool inCombatPrep;
	bool alreadyCompletedCombat;
	bool needsCombat;
	SDL_Rect unitRect;
	vector2d position;
	int reinforceTurn;
public:
	unitClass();
	~unitClass();
	bool retreat;
	int getX(){return position.x;}
	int getY(){return position.y;}
	int getPower(){return power;}
	void setPower(int p){power=p;}
	std::string getName() {return name;}
	void setName(string s){name=s;}
	SDL_Rect getSize();
	void setComPrep(bool state) {inCombatPrep = state;}
	void setCompleteCombat() {alreadyCompletedCombat = true; needsCombat=false;}
	void setNeedCombat() {needsCombat = true;}
	bool needCombat() {return needsCombat;}
	bool comPrep() {return inCombatPrep;}
	bool completedCombat() {return alreadyCompletedCombat;}
	void setPosition(int,int);
	void setMoved();
	bool hasMoved(); 
	void drawUnit(int xShift,int yShift,int mapWidth,int mapHeight, SDL_Surface* a_screen,SDL_Surface *armyColors);
	void resetMove();
	void setReinforceTurn(int r){reinforceTurn=r;}
	void resetCombat();
};

class armyClass
{
private:
	SDL_Surface *armyColors;
public:
	//this is a doozy
	//this is for all units on map
	unitClass **  armyArray;
	//this is for all units that have fled
	unitClass **  exitedUnits;
	//this is for all units killed
	unitClass **  deadUnits;
	//this is for each set of reinforcements
	unitClass ** reinforcements;
	int fullSize;
	int currentSize;
	int reinforcementSize;
	int deadSize;
	int exitedSize;
	int currentStrength();
	int exitedStrength();
	int deadStrength();
	int reinforcingStrength();
	void resetMoves();
	void loadArmy(char * unitFile,char * armyColorFile);
	void drawArmy(int xShift,int yShift,int mapWidth,int maHeight, SDL_Surface* a_screen);
	unitClass * findUnit(string lookUpName);
	bool haveLiveUnitAt(int xcord, int ycord);
	int checkStatus(unitClass* unit);
	bool moveUnit(unitClass* unit, int moveFrom, int moveTo);
	void resetAllCombat();
};


