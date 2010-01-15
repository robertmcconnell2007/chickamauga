#pragma once
#include "SDL.h"
#include "GraphicsLoader.h"
#include <fstream>

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
	int power;
	SDL_Rect unitRect;
	vector2d position;
public:
	unitClass();
	int getX(){return position.x;}
	int getY(){return position.y;}
	int getPower(){return power;}
	void setPower(int p){power=p;}
	SDL_Rect getSize();
	void setPosition(int,int);
	
	void drawUnit(int xShift,int yShift,int mapWidth,int mapHeight, SDL_Surface* a_screen,SDL_Surface *armyColors);

};

class armyClass
{
private:
	SDL_Surface *armyColors;
public:
	unitClass *armyArray;
	int size;
	armyClass(int);
	armyClass(char * unitFile,char * armyColorFile);
	void drawArmy(int xShift,int yShift,int mapWidth,int mapHeight, SDL_Surface* a_screen);
};


