#include <iostream>
#include <sstream>
#include "Game Data Handler.h"
#include "unitClass.h"
#include "mapSuperClass.h"
#include "tools.h"
#include "rules.h"
#include "graphicsloader.h"
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

void drawNodeGui()
{
}

void drawGui(map_node * node, armyClass * unionArmy, armyClass * confedArmy, unitClass *currentUnits[2], SDL_Surface * screen)
{
	ostringstream slottxt[3];
	for(int i = 0; i < 3; ++i)
		slottxt[i] << "";

	//clear away the guiframe
	SDL_FillRect(screen, &IH::Instance()->GUIFrameRect, 0x000000);
	//draw the art frame first
	//drawATile(IH::Instance()->GUIGameFrame, &IH::Instance()->GUIFrameRect, 0, screen, IH::Instance()->GUIFrameRect.x, IH::Instance()->GUIFrameRect.y);
	if(currentUnits[0])
	{
		slottxt[0] << currentUnits[0]->getName() << "\n";
		slottxt[0] << currentUnits[0]->getPower() << "\n";
	}
	if(currentUnits[1])
	{
		slottxt[1] << currentUnits[1]->getName() << "\n";
		slottxt[1] << currentUnits[1]->getPower() << "\n";
	}
	if(node)
	{
		switch(node->type)
		{
		case clear:
			slottxt[2] << "Clear\nMove cost is " << IH::Instance()->gameRules->clearMovePenalty << " point(s).\n";
			break;
		case forest:
			slottxt[2] << "Forest\nMove cost is " << IH::Instance()->gameRules->forestMovePenalty << " point(s).\n";
			break;
		case rough:
			slottxt[2] << "Rough\nMove cost is " << IH::Instance()->gameRules->roughMovePenalty << " point(s).\n";
			break;
		case roughForest:
			slottxt[2] << "Forested Rough\nMove cost is " << IH::Instance()->gameRules->forestroughMovePenalty << " point(s).\n";
			break;
		case river:
			slottxt[2] << "River\nNo movement.\n";
			break;
		}
	}
	if(IH::Instance()->unit1Selected)
		SDL_FillRect(screen, &IH::Instance()->UISlots[0], IH::Instance()->UIbkColor);
	if(IH::Instance()->unit2Selected)
		SDL_FillRect(screen, &IH::Instance()->UISlots[1], IH::Instance()->UIbkColor);
	for(int i = 0; i < 3; ++i)
	{
		printStrings(slottxt[i].str(), IH::Instance()->UISlots[i], screen, IH::Instance()->textColor, IH::Instance()->font1);
	}
}

//checks the clicked node to see if there are any units on it
bool isUnits(map_node * node, armyClass * unionArmy, armyClass * confedArmy)
{
	for(int i = 0; i < unionArmy->currentSize; ++i)
	{
		if(unionArmy->armyArray[i]->getX() == node->col && unionArmy->armyArray[i]->getY() == node->row)
			return true;
		
	}
	for(int j = 0; j < confedArmy->currentSize; ++j)
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
		movementRequired = IH::Instance()->gameRules->unitMovePoints + 1;
	}
	else if(cleartype || roadtype)
	{
		movementRequired = IH::Instance()->gameRules->roadCost;
	}
	else if(trailtype)
	{
		movementRequired = IH::Instance()->gameRules->trailCost;
	}
	else if(foresttype)
	{
		movementRequired = IH::Instance()->gameRules->forestMovePenalty;
	}
	else if(roughtype)
	{
		movementRequired = IH::Instance()->gameRules->roughMovePenalty;
	}
	else if(roughforesttype)
	{
		movementRequired = IH::Instance()->gameRules->forestroughMovePenalty;
	}
	if(fordtype)
	{
		movementRequired += IH::Instance()->gameRules->fordMovePenalty;
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
	for(int k = 0; k < enemyArmy.currentSize; k++)
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
			for(int f = 0; f < first.currentSize; f++)
			{
				if(first.armyArray[f]->getX()-1 == j && first.armyArray[f]->getY()-1 == i)
				{
					mapPointer[i][j].numOfUnits += 1;
				}
			}
			for(int s = 0; s < second.currentSize; s++)
			{
				if(second.armyArray[s]->getX()-1 == j && second.armyArray[s]->getY()-1 == i)
				{
					mapPointer[i][j].numOfUnits += 1;
				}
			}
		}
	}
}
void cancelClick(mapSuperClass * map)
{
	map->clearEnemy();
	map->clearMovement();
	//need to clear the unit array.
	IH::Instance()->currentUnits[0] = NULL;
	IH::Instance()->currentUnits[1] = NULL;
	IH::Instance()->selectedNode    = NULL;
	IH::Instance()->unit1Selected   = false;
	IH::Instance()->unit2Selected   = false;
	//currentUnits[1] = NULL;
}
bool firstClick(mapSuperClass* map, map_node* node, armyClass currentArmy, armyClass enemyArmy)
{
	cancelClick(map);
	checkUnitStacks(map,currentArmy,enemyArmy);
	IH::Instance()->selectedNode = node;
	for(int i = 0; i < currentArmy.currentSize; ++i)
	{
		if( node->col == currentArmy.armyArray[i]->getX() &&
			node->row == currentArmy.armyArray[i]->getY())
		{
			IH::Instance()->unit1Selected = true;
			IH::Instance()->enemyUnitsSelected = false;
			if(!IH::Instance()->currentUnits[0])
				IH::Instance()->currentUnits[0] = currentArmy.armyArray[i];
			else if(!IH::Instance()->currentUnits[1])
			{
				IH::Instance()->unit2Selected = true;
				IH::Instance()->currentUnits[1] = currentArmy.armyArray[i];
			}
		}
	}
	for(int i = 0; i < enemyArmy.currentSize; ++i)
	{
		if( node->col == enemyArmy.armyArray[i]->getX() &&
			node->row == enemyArmy.armyArray[i]->getY())
		{
			IH::Instance()->unit1Selected = false;
			IH::Instance()->enemyUnitsSelected = true;
			if(!IH::Instance()->currentUnits[0])
				IH::Instance()->currentUnits[0] = enemyArmy.armyArray[i];
			else if(!IH::Instance()->currentUnits[1])
				IH::Instance()->currentUnits[1] = enemyArmy.armyArray[i];
		}
	}
	setEnemyNodes(enemyArmy, map);
	moveTo(node,IH::Instance()->gameRules->unitMovePoints);
	return true;
}
bool secondClick(mapSuperClass* map, map_node* node,int newX,int newY, armyClass currentArmy, armyClass enemyArmy, unitClass * unitMoving)
{
	if(IH::Instance()->enemyUnitsSelected)
		cancelClick(map);
	else if(map->getMap()[newX][newY].movement>=0)
	{
		if(!unitMoving->hasMoved() && 
			!(unitMoving->getX() == newY+1 && unitMoving->getY() == newX+1))//&&
			//IH::Instance()->playersTurn == IH::Instance()->playerIam)
		{
			unitMoving->setPosition(newY+1,newX+1);
			//uncomment below line to restrict units to
			//one move per turn
			unitMoving->setMoved();
		}
		return true;
	}
	return false;
}

bool clickedIn(SDL_Event event, SDL_Rect rect)
{
	if(event.motion.x > rect.x &&
		event.motion.x < rect.x + rect.w &&
		event.motion.y > rect.y &&
		event.motion.y < rect.y + rect.h)
		return true;
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
				gameState = atMatchPrep;//gameState = atTitleScreen;
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
		cout << "I'm prepping the match now\n";
		matchFileNames.setGame("chickamauga.txt");
		if(matchFileNames.checkFileNames())
		{
			matchFileNames.setFiles();
			createMatch();
			gameState = matchMainPhase;
		}
		else
		{
			cout << "FAILED    FAILED    FAILED TO LOAD FILES!!!\n";
		}
		break;
	case matchMainPhase:
		switch(event.type)
		{
		case SDL_KEYDOWN:
			switch(event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				cancelClick(map);
				break;
			case SDLK_1:
				if(currentUnits[0] && !enemyUnitsSelected)
					unit1Selected = !unit1Selected;
				break;
			case SDLK_2:
				if(currentUnits[1] && !enemyUnitsSelected)
					unit2Selected = !unit2Selected;
				break;
			case SDLK_RETURN:
				playerIam = !playerIam;
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
			if(clickedIn(event, GUIFrameRect))
			{
				if(clickedIn(event, GUIEndTurnBox))
				{
					gameState = matchCombatPhase;
				}
				if(currentUnits[0] && clickedIn(event, UISlots[0]))
				{
					unit1Selected = !unit1Selected;
				}
				if(currentUnits[1] && clickedIn(event, UISlots[1]))
				{
					unit2Selected = !unit2Selected;
				}
			}
			else if(firstX == actualX && firstY == actualY)
			{
				if(firstX < map->width && firstY < map->height)
					selectedNode = &map->getMap()[firstX][firstY];
				if(unit1Selected || unit2Selected)
				{	
					if(playerIam == 0)
					{
						if(unit1Selected)
							secondClick(map, &map->getMap()[selectedX][selectedY],actualX,actualY, players[0].playerArmy, players[1].playerArmy, currentUnits[0]);
						if(unit2Selected)
							secondClick(map, &map->getMap()[selectedX][selectedY],actualX,actualY, players[0].playerArmy, players[1].playerArmy, currentUnits[1]);
					}
					else
					{
						if(unit1Selected)
							secondClick(map, &map->getMap()[selectedX][selectedY],actualX,actualY, players[1].playerArmy, players[0].playerArmy, currentUnits[0]);
						if(unit2Selected)
							secondClick(map, &map->getMap()[selectedX][selectedY],actualX,actualY, players[1].playerArmy, players[0].playerArmy, currentUnits[1]);
					}
					unit1Selected = unit2Selected = false;
					nodeGui = false;
					cancelClick(map);
				}
				else if(isUnits(&map->getMap()[firstX][firstY],&players[0].playerArmy,&players[1].playerArmy))
				{
					nodeGui = true;
					if(playerIam == 0)
						firstClick(map, &map->getMap()[actualX][actualY], players[0].playerArmy, players[1].playerArmy);
					else
						firstClick(map, &map->getMap()[actualX][actualY], players[1].playerArmy, players[0].playerArmy);
					selectedX=actualX;
					selectedY=actualY;
				}	
			}
		}
		break;
	case matchCombatPhase:
		//send a message to the server telling it to swapplayers
		//this next bit is testing
		players[0].startTurn();
		players[1].startTurn();
		gameState = matchMainPhase;
		break;
	case reviewingMatch:
		gameRules->calcAllRules();
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
	case matchMainPhase:
		break;
	case matchCombatPhase:
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
	case matchMainPhase:
		map->drawMap(screenShiftX, screenShiftY, screen);
		players[0].playerArmy.drawArmy(screenShiftX,screenShiftY,map->width,map->height,screen);
		players[1].playerArmy.drawArmy(screenShiftX,screenShiftY,map->width,map->height,screen);
		drawGui(selectedNode,&players[0].playerArmy,&players[1].playerArmy, currentUnits, screen);
		drawATile(utilityTiles5050, &u5050, 0, screen, GUIEndTurnBox.x, GUIEndTurnBox.y);
		break;
	case reviewingMatch:
		break;
	}
	if(SDL_Flip(screen) == -1)
		return;
}

/*
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//////////Obsoleted Code////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

	//blitSurface = TTF_RenderText_Solid(IH::Instance()->font1, slottxt1.str().c_str(), IH::Instance()->textColor);
	//SDL_BlitSurface(blitSurface, NULL, screen, &tempRect);
	//SDL_Surface *instructionImage2 = TTF_RenderText_Solid(font,myInstructions2,color1);
	//SDL_BlitSurface(instructionImage2,NULL,screen,&secondRect);
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
	TTF_Font *font2 = TTF_OpenFont (fontName,26);
	SDL_Surface *maxDefenseImg = NULL;
	SDL_Surface *maxAttackImg = NULL;
	SDL_Surface *unit1StrImg = NULL;
	SDL_Surface *unit2StrImg = NULL;
	SDL_Rect firstRect;
	SDL_Rect secondRect;
	SDL_Rect unit1UnderliningRect;
	SDL_Rect unit2UnderliningRect;
	SDL_Rect unit1InfoRect;
	SDL_Rect unit2InfoRect;
	SDL_Rect defRect;
	SDL_Rect atkRect;
	//
	unit2InfoRect.x = 161;
	unit2InfoRect.y = 15;
	unit2InfoRect.h = 35;
	unit2InfoRect.w = 50;
	//
	unit1InfoRect.x = 110;
	unit1InfoRect.y = 15;
	unit1InfoRect.h = 35;
	unit1InfoRect.w = 50;
	//

	unit2UnderliningRect.x = 146;
	unit2UnderliningRect.y = 0;
	unit2UnderliningRect.h = 50;
	unit2UnderliningRect.w = 50;
	//
	unit1UnderliningRect.x = 95;
	unit1UnderliningRect.y = 0;
	unit1UnderliningRect.h = 50;
	unit1UnderliningRect.w = 50;
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
	for(int i = 0; i < unionArmy->currentSize; ++i)
	{
		if(unionArmy->armyArray[i]->getX() == node->col && unionArmy->armyArray[i]->getY() == node->row)
		{
			//if there is a second unit on the node
			if(k == 1)
			{
				currentUnits[1] = unionArmy->armyArray[i];
					SDL_FillRect(screen,&unit2UnderliningRect,0xffffff);
				defCounter2 = unionArmy->armyArray[i]->getPower()*defenseMultiplier;
				powCounter2 = unionArmy->armyArray[i]->getPower();
			}
			//if there is 1 unit on the square
			if(k == 0)
			{
				currentUnits[0] = unionArmy->armyArray[i];
				SDL_FillRect(screen,&unit1UnderliningRect,0xffffff);
				k++;
				defCounter1 = (unionArmy->armyArray[i]->getPower())*defenseMultiplier;
				powCounter1 = (unionArmy->armyArray[i]->getPower());
			}
		}
	}
	for(int j = 0; j < confedArmy->currentSize; ++j)
	{
		if(confedArmy->armyArray[j]->getX() == node->col && confedArmy->armyArray[j]->getY() == node->row)
		{
			if(k == 1)
			{
				currentUnits[1] = confedArmy->armyArray[j];
				SDL_FillRect(screen,&unit2UnderliningRect,0xffffff);
				defCounter2 = confedArmy->armyArray[j]->getPower() *defenseMultiplier;
				powCounter2 = confedArmy->armyArray[j]->getPower();
			}
			if(k == 0)
			{
				currentUnits[0] = confedArmy->armyArray[j];
					SDL_FillRect(screen,&unit1UnderliningRect,0xffffff);
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
	// first Units box
	char * myInstructions3 = "unit 1";
	SDL_Surface * instructionImage3 = TTF_RenderText_Solid(font,myInstructions3,color1);
	SDL_BlitSurface(instructionImage3,NULL,screen,&unit1UnderliningRect);
	char unit1Strength[256];
	itoa(powCounter1,unit1Strength,10);
	if(unit1StrImg)
		SDL_FreeSurface(maxAttackImg);
	unit1StrImg = TTF_RenderText_Solid(font2,unit1Strength,color2);
	SDL_BlitSurface(unit1StrImg,NULL,screen,&unit1InfoRect);
	//
	if(powCounter2 > 0)
	{
	char * myInstructions4 = "unit 2";
	SDL_Surface * instructionImage4 = TTF_RenderText_Solid(font,myInstructions4,color1);
	SDL_BlitSurface(instructionImage4,NULL,screen,&unit2UnderliningRect);
	char unit2Strength[256];
	itoa(powCounter2,unit2Strength,10);
	if(unit2StrImg)
		SDL_FreeSurface(maxAttackImg);
	unit2StrImg = TTF_RenderText_Solid(font2,unit2Strength,color2);
	SDL_BlitSurface(unit2StrImg,NULL,screen,&unit2InfoRect);
	}

*/