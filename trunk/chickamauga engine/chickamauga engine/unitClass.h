#pragma once
#include "SDL.h"
#include "GraphicsLoader.h"
#include <fstream>
#include <string>

struct vector2d
{
	int x;
	int y;
};
enum terrainTypes;
struct map_node;
struct node_edge;
class mapSuperClass;

class unitClass
{
private:
	std::string name;
	bool hasMovedThisTurn;
	int power;
	SDL_Rect unitRect;
	vector2d position;
	int reinforceTurn;
public:
	unitClass();
	int getX(){return position.x;}
	int getY(){return position.y;}
	int getPower(){return power;}
	void setPower(int p){power=p;}
	std::string getName() {return name;}
	void setName(string s){name=s;}
	SDL_Rect getSize();
	void setPosition(int,int);
	void setMoved() {hasMovedThisTurn = true;}
	bool hasMoved() {return hasMovedThisTurn;}
	void drawUnit(int xShift,int yShift,int mapWidth,int mapHeight, SDL_Surface* a_screen,SDL_Surface *armyColors);
	void resetMove() {hasMovedThisTurn = false;}
	void setReinforceTurn(int r){reinforceTurn=r;}
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
	void resetMoves();
	void loadArmy(char * unitFile,char * armyColorFile);
	void drawArmy(int xShift,int yShift,int mapWidth,int mapHeight, SDL_Surface* a_screen);
};


