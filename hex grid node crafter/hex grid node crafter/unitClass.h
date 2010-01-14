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
unitClass::unitClass()
{
	//power=3;
	unitRect.x=unitRect.y=0;
	unitRect.h=unitRect.w=25;
	//position.x=position.y=2;
	
}
void unitClass::setPosition(int nX,int nY)
{
	position.x=nX;
	position.y=nY;
}
SDL_Rect unitClass::getSize()
{
	return unitRect;
}
void unitClass::drawUnit(int screenShiftx,int screenShifty,int mapWidth,int mapHeight,SDL_Surface *a_screen,SDL_Surface *armyColors)
{
	for(int i = 0; i < mapWidth; ++i)
	{
		for(int k = 0; k < mapHeight; ++k)
		{
			if(position.x-1==k&&position.y-1==i)
			{
				if(i%2==0)
				{
					drawATile(armyColors,&unitRect,power-1,a_screen, (i * 50) - (i*12) + screenShiftx+12, k * 44 + screenShifty+10);
				}
				else
				{
					drawATile(armyColors,&unitRect,power-1,a_screen, (i * 50) - (i*12) + screenShiftx+12, k * 44 + screenShifty+30);
				}
			}
		}
	}
}



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

armyClass::armyClass(int s)
{
	size=s;
	armyArray=new unitClass[size];
	for(int i=0; i<size; i++)
	{
		armyArray[i].setPosition(rand()%24+1,rand()%27+1);
		armyArray[i].setPower(rand()%6+1);
	}
	armyArray[0].setPosition(0,0);
	armyArray[0].setPower(7);
}
armyClass::armyClass(char * fileName, char * armyColorFile)
{
	int px,py,pow;
	ifstream infile;
	infile.open(fileName);
	infile>>size;
	armyArray=new unitClass[size];
	for(int i=0; i<size; i++)
	{
		infile>>py;
		infile>>px;
		infile>>pow;
		armyArray[i].setPosition(px,py);
		armyArray[i].setPower(pow);
	}
	infile.close();
	armyColors=load_my_image(armyColorFile);

}
void armyClass::drawArmy(int xShift,int yShift,int mapWidth,int mapHeight, SDL_Surface* a_screen)
{
	for(int i=0; i<size; i++)
	{
		armyArray[i].drawUnit(xShift,yShift,mapWidth,mapHeight,a_screen,armyColors);
	}
}
int checkEdge(node_edge* edge, int pos)
{
	if(edge == NULL)
		return -1;
	map_node* nodePointer;
	if(pos > 2)
		nodePointer = edge->lowerNode;
	else
		nodePointer = edge->upperNode;
	if(nodePointer->enemy)
	{
		return -2;
	}
	int numOccupy = nodePointer->numOfUnits;
	if(numOccupy >= 2)
		return -1;
	int movementRequired = 0;
	bool cleartype = false;
	bool foresttype = false;
	bool roughtype = false;
	bool roughforesttype = false;
	bool rivertype = false;
	bool roadtype = edge->road_edge;
	bool trailtype = edge->trail_edge;
	bool fordtype = edge->ford_edge;
	bool creektype = edge->creek_edge;
	bool bridgetype = edge->bridge_edge;
	if(nodePointer->type == clear)
		cleartype = true;
	if(nodePointer->type == forest)
		foresttype = true;
	if(nodePointer->type == rough)
		roughtype = true;
	if(nodePointer->type == roughForest)
		roughforesttype = true;
	if(nodePointer->type == river)
		rivertype = true;
	if(creektype || rivertype)
	{
		movementRequired = 7;
	}
	else if(cleartype || roadtype)
	{
		movementRequired = 1;
	}
	else if(trailtype)
	{
		movementRequired = 2;
	}
	else if(foresttype || roughtype)
	{
		movementRequired = 3;
	}
	else if(roughforesttype)
	{
		movementRequired = 6;
	}
	if(fordtype)
	{
		movementRequired += 1;
	}
	return movementRequired;
}
void moveTo(map_node* node,int movement)
{
	node->movement = movement;
	if(movement == 0)
		return;
	int tempMove;
	for(int i = 0; i < 6; i++)
	{
		tempMove = checkEdge(node->nodeEdges[i],i);
		if(tempMove != -1)
		{
			if(i > 2)
			{
				if(node->nodeEdges[i]->lowerNode->movement >= 0 && tempMove != -2 && tempMove != -1 && movement-tempMove > node->nodeEdges[i]->lowerNode->movement)
				{
					moveTo(node->nodeEdges[i]->lowerNode,movement-tempMove);
				}
				else if(tempMove != -2 && tempMove != -1 && movement-tempMove >= 0)
				{
					moveTo(node->nodeEdges[i]->lowerNode,movement-tempMove);
				}
				else if(tempMove != -1 && movement-tempMove >= 0)
				{
					moveTo(node->nodeEdges[i]->lowerNode,0);
				}
			}
			else
			{
				if(node->nodeEdges[i]->upperNode->movement >= 0 && tempMove != -2 && tempMove != -1 && movement-tempMove > node->nodeEdges[i]->upperNode->movement)
				{
					moveTo(node->nodeEdges[i]->upperNode,movement-tempMove);
				}
				else if(tempMove != -2 && tempMove != -1 && movement-tempMove >= 0)
				{
					moveTo(node->nodeEdges[i]->upperNode,movement-tempMove);
				}
				else if(tempMove != -1 && movement-tempMove >= 0)
				{
					moveTo(node->nodeEdges[i]->upperNode,0);
				}
			}
		}
	}
}
void setEnemyNodes(armyClass enemyArmy, mapSuperClass* map)
{
	for(int k = 0; k < enemyArmy.size; k++)
	{
		map->setEnemy(enemyArmy.armyArray[k].getX()-1,enemyArmy.armyArray[k].getY()-1);
	}
}
void checkUnitStacks(mapSuperClass* map, armyClass first, armyClass second)
{
	map_node** mapPointer = map->getMap();
	for(int i = 0; i < map->height; i++)
	{
		for(int j = 0; j < map->width; j++)
		{
			for(int f = 0; f < first.size; f++)
			{
				if(first.armyArray[f].getX()-1 == j && first.armyArray[f].getY()-1 == i)
				{
					mapPointer[i][j].numOfUnits += 1;
				}
			}
			for(int s = 0; s < second.size; s++)
			{
				if(second.armyArray[s].getX()-1 == j && second.armyArray[s].getY()-1 == i)
				{
					mapPointer[i][j].numOfUnits += 1;
				}
			}
		}
	}
}
bool firstClick(mapSuperClass* map, map_node* node, armyClass currentArmy, armyClass enemyArmy)
{
	map->clearMovement();
	checkUnitStacks(map,currentArmy,enemyArmy);
	for(int k = 0; k < currentArmy.size; k++)
	{
		if(currentArmy.armyArray[k].getY()-1 == node->col && currentArmy.armyArray[k].getX()-1 == node->row)
		{
			setEnemyNodes(enemyArmy, map);
			moveTo(node,6);
			return true;
		}
	}
	map->clearEnemy();
	map->clearMovement();
	return false;
}
bool secondClick(mapSuperClass* map, map_node* node,int newX,int newY, armyClass currentArmy, armyClass enemyArmy)
{
	for(int k = 0; k < currentArmy.size; k++)
	{
		if(currentArmy.armyArray[k].getY()-1 == node->col && currentArmy.armyArray[k].getX()-1 == node->row)
		{
			if(map->getMap()[newX][newY].movement>=0)
			{
				currentArmy.armyArray[k].setPosition(newY+1,newX+1);
				map->clearEnemy();
				map->clearMovement();
				return true;
			}
		}
	}
	map->clearEnemy();
	map->clearMovement();
	return false;
}
void cancelClick(mapSuperClass* map, map_node* node, armyClass currentArmy, armyClass enemyArmy)
{
	map->clearEnemy();
	map->clearMovement();
}
