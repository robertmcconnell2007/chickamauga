#include <iostream>
#include "Game Data Handler.h"
#include "unitClass.h"
#include "tools.h"
#include "rules.h"
///<<<<<<< .mine
#include "SDL.h"		// SDL library
#include "SDL_ttf.h"	// true-type font library for SDL
#include<stdio.h>
using namespace std;
enum terrainTypes;

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
///primary map and unit control functions
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

void drawNodeGui(map_node * node, armyClass * unionArmy, armyClass * confedArmy, unitClass *currentUnits[2], SDL_Surface * screen)
{
	int k = 0;
	int defense = 0;
	int defCounter1 = 0;
	int defCounter2 = 0;
	int power = 0;
	int powCounter1 = 0;
	int powCounter2 = 0;
	int defenseMultiplier = 1;
	SDL_Color color1 = {0,0,0};
	SDL_Color color2 = {255,0,0};
	char * fontName = "C:\\windows\\fonts\\arial.ttf";
	TTF_Font *font = TTF_OpenFont (fontName, 14);
	SDL_Surface *maxDefenseImg = NULL;
	SDL_Surface *maxAttackImg = NULL;
	SDL_Rect firstRect;
	SDL_Rect secondRect;
	SDL_Rect defRect;
	SDL_Rect atkRect;
	//
	atkRect.x = 65;
	atkRect.y = 17;
	atkRect.h = 50;
	atkRect.w = 10;
	//
	defRect.x = 70;
	defRect.y = 0;
	defRect.h = 50;
	defRect.w = 10;
	//first unit on gui
	firstRect.x = 0;
	firstRect.y = 0;
	firstRect.h = 20;
	firstRect.w = 95;
	//second unit on gui
	secondRect.x = 0;
	secondRect.y = 18;
	secondRect.h = 20;
	secondRect.w = 95;
	if(node->type == rough || node->type == roughForest)
		defenseMultiplier = 2;
	for(int i = 0; i < unionArmy->size; ++i)
	{
		if(unionArmy->armyArray[i]->getX() == node->col && unionArmy->armyArray[i]->getY() == node->row)
		{
			//if there is a second unit on the node
			if(k == 1)
			{
				currentUnits[1] = unionArmy->armyArray[i];
				defCounter2 = unionArmy->armyArray[i]->getPower()*defenseMultiplier;
				powCounter2 = unionArmy->armyArray[i]->getPower();
			}
			//if there is 1 unit on the square
			if(k == 0)
			{
				currentUnits[0] = unionArmy->armyArray[i];
				k++;
				defCounter1 = (unionArmy->armyArray[i]->getPower())*defenseMultiplier;
				powCounter1 = (unionArmy->armyArray[i]->getPower());
			}
		}
	}
	for(int j = 0; j < confedArmy->size; ++j)
	{
		if(confedArmy->armyArray[j]->getX() == node->col && confedArmy->armyArray[j]->getY() == node->row)
		{
			if(k == 1)
			{
				currentUnits[1] = confedArmy->armyArray[j];
				defCounter2 = confedArmy->armyArray[j]->getPower() *defenseMultiplier;
				powCounter2 = confedArmy->armyArray[j]->getPower();
			}
			if(k == 0)
			{
				currentUnits[0] = confedArmy->armyArray[j];
				k++;
				defCounter1 = confedArmy->armyArray[j]->getPower() *defenseMultiplier;
				powCounter1 = confedArmy->armyArray[j]->getPower();
				//uses TTF to put the words "defense ="
			}
			
		}
	}
	defense = defCounter1 + defCounter2;
	power = powCounter1 + powCounter2;
	//uses TTF to put the words "defense = defense"
	SDL_FillRect(screen,&firstRect,0x0000ff);
	char * myInstructions = "defense = ";
	SDL_Surface *instructionImage = TTF_RenderText_Solid(font,myInstructions,color1);
	SDL_BlitSurface(instructionImage,NULL,screen,&firstRect);
	//puts the actual number of defense on the rect
	char maxDefense[256];
	_itoa(defense,maxDefense,10);
	if(maxDefenseImg)
		SDL_FreeSurface(maxDefenseImg);
	maxDefenseImg = TTF_RenderText_Solid(font,maxDefense,color1);
	SDL_BlitSurface(maxDefenseImg,NULL,screen,&defRect);

	//uses TTF to put the words "attack = attack"
	SDL_FillRect(screen,&secondRect,0xff0000);
	char * myInstructions2 = "attack = ";
	SDL_Surface *instructionImage2 = TTF_RenderText_Solid(font,myInstructions2,color1);
	SDL_BlitSurface(instructionImage2,NULL,screen,&secondRect);
	//puts the actual number of attack on the rect
	char maxAttack[256];
	itoa(power,maxAttack,10);
	if(maxAttackImg)
		SDL_FreeSurface(maxAttackImg);
	maxAttackImg = TTF_RenderText_Solid(font,maxAttack,color1);
	SDL_BlitSurface(maxAttackImg,NULL,screen,&atkRect);
	
}

void setSelectedUnits(map_node * node, armyClass * unionArmy, armyClass * confederateArmy)
{
}

//checks the clicked node to see if there are any units on it
bool isUnits(map_node * node, armyClass * unionArmy, armyClass * confedArmy)
{
	for(int i = 0; i < unionArmy->size; ++i)
	{
		if(unionArmy->armyArray[i]->getX() == node->col && unionArmy->armyArray[i]->getY() == node->row)
			return true;
		
	}
	for(int j = 0; j < confedArmy->size; ++j)
	{
		if(confedArmy->armyArray[j]->getX() == node->col && confedArmy->armyArray[j]->getY() == node->row)
			return true;
	}
	return false;
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
	if(movement == 0 || node->enemy)
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
		map->setEnemy(enemyArmy.armyArray[k]->getX()-1,enemyArmy.armyArray[k]->getY()-1);
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
				if(first.armyArray[f]->getX()-1 == j && first.armyArray[f]->getY()-1 == i)
				{
					mapPointer[i][j].numOfUnits += 1;
				}
			}
			for(int s = 0; s < second.size; s++)
			{
				if(second.armyArray[s]->getX()-1 == j && second.armyArray[s]->getY()-1 == i)
				{
					mapPointer[i][j].numOfUnits += 1;
				}
			}
		}
	}
}
void cancelClick(mapSuperClass* map)
{
	map->clearEnemy();
	map->clearMovement();
}
bool firstClick(mapSuperClass* map, map_node* node, armyClass currentArmy, armyClass enemyArmy, unitClass * unitToMove)
{
	map->clearMovement();
	checkUnitStacks(map,currentArmy,enemyArmy);
	setEnemyNodes(enemyArmy, map);
	moveTo(node,6);
	return true;
}
bool secondClick(mapSuperClass* map, map_node* node,int newX,int newY, armyClass currentArmy, armyClass enemyArmy, unitClass * unitMoving)
{
	//map->clearEnemy();
	//map->clearMovement();
	if(map->getMap()[newX][newY].movement>=0)
	{
		if(!unitMoving->hasMoved())
		{
			unitMoving->setPosition(newY+1,newX+1);
			//uncomment below line to restrict units to
			//one move per turn
			//unitMoving->setMoved();
		}
		return true;
	}
	return false;
}
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/// main loop functions
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////



void IH::handlePrimaryInput()
{
	switch(gameState)
	{
	case atLogo:
		switch(event.type)
		{
		case SDL_KEYDOWN:
			switch(event.key.keysym.sym)
			{
			case SDLK_RETURN:
				cout << "making map\n";
				createMatch("mapData/mapData/ChickamaugaMapData.txt", 0,0);
				gameState = playingMatch;//gameState = atTitleScreen;
				break;
			}
			break;
		case SDL_QUIT:
			endGame();
			break;
		}
		break;
	case atTitleScreen:
		break;
	case atMatchPrep:
		break;
	case playingMatch:
		switch(event.type)
		{
		case SDL_KEYDOWN:
			switch(event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				break;
			case SDLK_1:
				if(nodeGui)
					selectedUnit = 0;
				break;
			case SDLK_2:
				if(nodeGui && currentUnits[1] != NULL)
					selectedUnit = 1;
				break;
			}
			break;
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
				// needs functionality
				// selected unit needs to be selected by the player from the UI
				if(unitSelected)
				{					
					secondClick(map, &map->getMap()[selectedX][selectedY],actualX,actualY, players[0].playerArmy, players[1].playerArmy, currentUnits[selectedUnit]);
					unitSelected=false;
					nodeGui = false;
					cancelClick(map);
				}
				else if(isUnits(&map->getMap()[firstX][firstY],&players[0].playerArmy,&players[1].playerArmy))
				{
					nodeGui = true;
					firstClick(map, &map->getMap()[actualX][actualY], players[0].playerArmy, players[1].playerArmy, currentUnits[selectedUnit]);
					unitSelected=true;
					selectedX=actualX;
					selectedY=actualY;
				}	
			}
		}
	case reviewingMatch:
		break;
	}
}

void IH::update(int mspassed)
{
	screenShiftX += xMove*5;
	screenShiftY += yMove*5;
	switch(gameState)
	{
	case atLogo:
		break;
	case atTitleScreen:
		break;
	case atMatchPrep:
		break;
	case playingMatch:
		break;
	case reviewingMatch:
		break;
	}
}

void IH::drawAll()
{
	SDL_FillRect(screen,NULL,0x000000);
	switch(gameState)
	{
	case atLogo:
		apply_surface(0,0,ourLogo,screen);
		break;
	case atTitleScreen:
		apply_surface(0,0,titleScreen,screen);
		break;
	case atMatchPrep:
		break;
	case playingMatch:
		map->drawMap(screenShiftX, screenShiftY, screen);
		players[0].playerArmy.drawArmy(screenShiftX,screenShiftY,map->width,map->height,screen);
		players[1].playerArmy.drawArmy(screenShiftX,screenShiftY,map->width,map->height,screen);
		if(nodeGui)
			drawNodeGui(&map->getMap()[firstX][firstY],&players[0].playerArmy,&players[1].playerArmy, currentUnits, screen);
		break;
	case reviewingMatch:
		break;
	}
	if(SDL_Flip(screen) == -1)
		return;
}