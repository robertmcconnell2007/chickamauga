#include <iostream>
#include "Game Data Handler.h"
#include "unitClass.h"
#include "tools.h"
#include "rules.h"
using namespace std;
enum terrainTypes;

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
///primary map and unit control functions
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////


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


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/// main loop functions
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////



void IH::handlePrimaryInput()
{

	switch(IH::Instance()->event.type)
	{
	case SDLK_ESCAPE:
	case SDL_QUIT:
		endGame();
		break;
	case SDL_MOUSEMOTION:
		actualX = event.motion.x - screenShiftX;
		actualY = event.motion.y - screenShiftY;
		actualX = (actualX-6)/38;
		if(((actualX+1)%2) == 1)
			actualY = (actualY)/44;
		else
			actualY = (actualY-22)/44;
		if(event.motion.x < 15)
			xMove = 1;
		else if(event.motion.x >= screenSize.x - 15)
			xMove = -1;
		else
			xMove = 0;
		if(event.motion.y < 15)
			yMove = 1;
		else if(event.motion.y >= screenSize.y - 15)
			yMove = -1;
		else
			yMove = 0;
		break;
	case SDL_MOUSEBUTTONDOWN:
		mouseDown = true;
		firstX = actualX;
		firstY = actualY;
		break;
	case SDL_MOUSEBUTTONUP:
		mouseDown = false;
		if(firstX == actualX && firstY == actualY)
		{
			if(actualX >= 0 && actualX < map->width && actualY >= 0 && actualY < map->height)
			{
				if(unitSelected)
				{
					if(secondClick(map, &map->getMap()[selectedX][selectedY],actualX,actualY, players[0].playerArmy, players[1].playerArmy))
					{
						unitSelected=false;
					}
					else
					{
						unitSelected=false;
					}
				}
				else if(firstClick(map, &map->getMap()[actualX][actualY], players[0].playerArmy, players[1].playerArmy))
				{
					unitSelected=true;
					selectedX=actualX;
					selectedY=actualY;

				}
			}
		}
		break;
	case SDL_KEYDOWN:
		switch(IH::Instance()->event.key.keysym.sym)
		{
		case SDLK_ESCAPE:
			IH::Instance()->endGame();
			break;
		//case SDLK_SPACE:
		//	cout << "OMG\n";
		//	break;
		case SDLK_SPACE:
			cout << "making map\n";
			createMatch("mapData/mapData/ChickamaugaMapData2.txt", 0,0);
		default:
			break;
		}
		break;
	}
	//show what hex the mouse is over
	if(map)
	{
		if(actualX >= 0 && actualX < map->width && actualY >= 0 && actualY < map->height)
		{
			map->hilightHex(actualX,actualY);
		}
	}
}

void IH::update(int mspassed)
{
	screenShiftX += xMove*5;
	screenShiftY += yMove*5;
	if(playingMatch)
	{
	}
	else
	{
	}
}

void IH::drawAll()
{
	SDL_FillRect(screen,NULL,0x000000);
	if(logo)
	{
		apply_surface(0,0,ourLogo,screen);
	}
	if(splashScreen)
	{
		apply_surface(0,0,titleScreen,screen);
	}
	if(playingMatch)
	{
		map->drawMap(screenShiftX, screenShiftY, screen);
		players[0].playerArmy.drawArmy(screenShiftX,screenShiftY,map->width,map->height,screen);
		players[1].playerArmy.drawArmy(screenShiftX,screenShiftY,map->width,map->height,screen);
	}
	else
	{
	}
	if(SDL_Flip(screen) == -1)
		return;
}