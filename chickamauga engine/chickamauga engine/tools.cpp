#include <iostream>
#include <sstream>
#include <vector>
#include "Game Data Handler.h"
#include "unitClass.h"
#include "mapSuperClass.h"
#include "tools.h"
#include "rules.h"
#include "graphicsloader.h"
#include "battleCalc.h"

#include "messageHandler.h"
#include "UDP.h"
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

int stringToInt(string str)
{
	int newInt = 0;
	int length = 0;

	length = str.length();
	for(int i = length - 1; i >= 0; ++i)
		newInt += ((int)(str.c_str()[i] - '0') * (i * 10));

	return newInt;
}

bool getUnitsAroundNode(map_node * node, int path, armyClass * army, unitClass * unit1, unitClass * unit2)
{
	bool foundUnit1 = false;
	unit1 = NULL;
	unit2 = NULL;
	for(int i = 0; i < army->currentSize; ++i)
	{
		if(path < 3)
		{
			if(army->armyArray[i]->getX() == node->nodeEdges[path]->upperNode->col &&
				army->armyArray[i]->getY() == node->nodeEdges[path]->upperNode->row)
			{
				if(!foundUnit1)
				{
					unit1 = army->armyArray[i];
					foundUnit1 = true;
				}
				else
					unit2 = army->armyArray[i];
			}
		}
		else
		{
			if(army->armyArray[i]->getX() == node->nodeEdges[path]->lowerNode->col &&
				army->armyArray[i]->getY() == node->nodeEdges[path]->lowerNode->row)
			{
				if(!foundUnit1)
				{
					unit1 = army->armyArray[i];
					foundUnit1 = true;
				}
				else
					unit2 = army->armyArray[i];
			}
		}
	}
	if(foundUnit1)
		return true;
	return false;
}

bool getUnitsOnNode(map_node * node, armyClass * army, unitClass * unit1, unitClass * unit2)
{
	bool foundUnit1 = false;
	unit1 = NULL;
	unit2 = NULL;
	for(int i = 0; i < army->currentSize; ++i)
	{
		if(army->armyArray[i]->getX() == node->col &&
			army->armyArray[i]->getY() == node->row)
		{
			if(!foundUnit1)
			{
				unit1 = army->armyArray[i];
				foundUnit1 = true;
			}
			else
				unit2 = army->armyArray[i];
		}
	}
	if(foundUnit1)
		return true;
	return false;
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
				if(node->nodeEdges[i]->lowerNode->movement >= 0 && tempMove != -1 && movement-tempMove > node->nodeEdges[i]->lowerNode->movement)
				{
					moveTo(node->nodeEdges[i]->lowerNode,movement-tempMove);
				}
				else if(tempMove != -1 && movement-tempMove >= 0)
				{
					moveTo(node->nodeEdges[i]->lowerNode,movement-tempMove);
				}
			}
			else
			{
				if(node->nodeEdges[i]->upperNode->movement >= 0 && tempMove != -1 && movement-tempMove > node->nodeEdges[i]->upperNode->movement)
				{
					moveTo(node->nodeEdges[i]->upperNode,movement-tempMove);
				}
				else if(tempMove != -1 && movement-tempMove >= 0)
				{
					moveTo(node->nodeEdges[i]->upperNode,movement-tempMove);
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
	if(!(IH::Instance()->currentUnits[0] || IH::Instance()->currentUnits[1]))
		cancelClick(map);
	return true;
}
bool secondClick(mapSuperClass* map, map_node* node,int newX,int newY, armyClass currentArmy, armyClass enemyArmy, unitClass * unitMoving)
{
	checkUnitStacks(map,currentArmy,enemyArmy);
	if(IH::Instance()->enemyUnitsSelected)
		cancelClick(map);
	else if(map->getMap()[newX][newY].movement>=0)
	{
		if(!unitMoving->hasMoved() && 
			!(unitMoving->getX() == newY+1 && unitMoving->getY() == newX+1))
		{
			unitMoving->setPosition(newY+1,newX+1);
			if(map->getMap()[newX][newY].enemy)
				unitMoving->setNeedCombat();
			//uncomment below line to restrict units to
			//one move per turn
			if(map->getMap()[newX][newY].control)
				map->getMap()[newX][newY].controlBlue = !IH::Instance()->playerIam;
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
bool setAttacker(mapSuperClass* map,map_node * node, armyClass * ourArmy,armyClass* theirArmy)
{
	setEnemyNodes(*theirArmy,map);
	for(int i=0; i<ourArmy->currentSize; i++)
	{
		if(ourArmy->armyArray[i]->getX() == node->col && ourArmy->armyArray[i]->getY() == node->row&&node->enemy)
		{
			ourArmy->armyArray[i]->attacking=!ourArmy->armyArray[i]->attacking;
			node->selected=!node->selected;
			map->clearEnemy();
			return true;
		}
	}
	return false;
}
//this will only occur if all attackers can attack. combined power is given from
//attackers range function. goes into battle calc, and then sets retreat variables
//in attacker or defender or exchange or death
void attackAndResults(mapSuperClass* map,map_node *tempNode,armyClass *ourArmy,armyClass* enemy,int target,int attackPower)
{
	//map_node tempNode;
	//tempNode=map->getMap()[enemy->armyArray[target]->getX()][enemy->armyArray[target]->getY()];
	battleCalculator bCalc;
	int result;
	int defenderPower=enemy->armyArray[target]->getPower();
	if(tempNode->type==rough||tempNode->type==roughForest)
	{
		defenderPower=defenderPower*2;
	}
	result=bCalc.doBattle(attackPower,defenderPower);
	

	///now for the fun part
	//retreats:set it here, check in input loop for spaces and whatnot
	//if its defeated do it here
	//will also do exchange damage here
	switch(result)
	{
	case 0:
		{
			for(int i=0; i<ourArmy->currentSize; i++)
			{
				if(ourArmy->armyArray[i]->attacking)
				{
					ourArmy->armyArray[i]->retreat=true;
					ourArmy->armyArray[i]->attacking=false;
					ourArmy->armyArray[i]->alreadyAttacked=true;
				}
			}
			IH::Instance()->retreatCalled=true;
			break;
		}
	case 1:
		{
			for(int i=0; i<ourArmy->currentSize; i++)
			{
				if(ourArmy->armyArray[i]->attacking)
				{
					ourArmy->moveUnit(ourArmy->armyArray[i],MUFField,MUTKilled);
				}
			}
			break;
		}
	case 2:
		{
			enemy->armyArray[target]->retreat=true;
			for(int i=0; i<ourArmy->currentSize; i++)
			{
				if(ourArmy->armyArray[i]->attacking)
				{
					ourArmy->armyArray[i]->attacking=false;
					ourArmy->armyArray[i]->alreadyAttacked=true;
				}
			}
			IH::Instance()->retreatCalled=true;
			break;
		}
	case 3:
		{
			//kill defender here
			for(int i=0; i<ourArmy->currentSize; i++)
			{
				if(ourArmy->armyArray[i]->attacking)
				{
					ourArmy->armyArray[i]->attacking=false;
					ourArmy->armyArray[i]->alreadyAttacked=true;
				}
			}
			break;
		}
	case 4:
		{
			//reduce attacker power by defender power...figure this out later
			break;
		}
	}
	

}


//will go through all attackers and check to see if they're 
//able to attack the enemy targeted
bool checkAttackersRange(mapSuperClass* map,map_node *tempNode,armyClass* ourArmy,armyClass* enemy,int target)
{	
	bool ok=false;
	int totalPower=0;
	//tempNode=map->getMap()[enemy->armyArray[target]->getX()-1][enemy->armyArray[target]->getY()-1];
	for(int i=0; i<ourArmy->currentSize; i++)
	{
		if(ourArmy->armyArray[i]->attacking)
		{
			for(int k=0; k<6; k++)
			{
				if(k<3)
				{
					if(tempNode->nodeEdges[k]->upperNode->col==ourArmy->armyArray[i]->getX()&&
						tempNode->nodeEdges[k]->upperNode->row==ourArmy->armyArray[i]->getY()&&!tempNode->nodeEdges[k]->creek_edge)
					{
						totalPower+=ourArmy->armyArray[i]->getPower();
						 ok=true;
					}
				}
				else
				{
					if(tempNode->nodeEdges[k]->lowerNode->col==ourArmy->armyArray[i]->getX()&&
						tempNode->nodeEdges[k]->lowerNode->row==ourArmy->armyArray[i]->getY()&&!tempNode->nodeEdges[k]->creek_edge)	
					{
						totalPower+=ourArmy->armyArray[i]->getPower();
						 ok=true;
					}
				}
			}
			if(!ok)
			{
				return false;
			}
			else
			{
				//ok=false;
			}
		}
	}
	attackAndResults(map,tempNode,ourArmy,enemy,target,totalPower);




	return true;
}
//takes the second click and finds the enemy and then goes to check the attackers
bool enemyTarget(mapSuperClass* map, map_node *node, armyClass* ourArmy, armyClass* targetedEnemy)
{
	int target=-1;
	for(int i=0; i<targetedEnemy->currentSize; i++)
	{
		if(targetedEnemy->armyArray[i]->getX() == node->col && targetedEnemy->armyArray[i]->getY() == node->row)
		{
			target=i;
		}
	}
	if(target==-1)
	{
		return false;
	}
	if(checkAttackersRange(map,node,ourArmy,targetedEnemy,target))
	{
		
	}
	else
	{
		return false;
	}
	return true;
}

void showRetreat(mapSuperClass* map,armyClass* ourArmy, armyClass* enemy)
{
	map_node *temp;
	map->clearEnemy();
	map->clearMovement();
	for(int i=0; i<ourArmy->currentSize; i++)
	{
		if(ourArmy->armyArray[i]->retreat)
		{
			setEnemyNodes(*enemy,map);
			temp=&map->getMap()[ourArmy->armyArray[i]->getY()-1][ourArmy->armyArray[i]->getX()-1];
			temp->selected=true;
			for(int k=0; k<6; k++)
			{
				if(k<3)
				{
					if(!temp->nodeEdges[k]->upperNode->enemy&&!temp->nodeEdges[k]->creek_edge)
					{
						temp->nodeEdges[k]->upperNode->selected=true;
					}
				}
				else
				{
					if(!temp->nodeEdges[k]->lowerNode->enemy&&!temp->nodeEdges[k]->creek_edge)
					{
						temp->nodeEdges[k]->lowerNode->selected=true;
					}

				}
			}
		}
	}
	for(int i=0; i<enemy->currentSize; i++)
	{
		if(enemy->armyArray[i]->retreat)
		{
			setEnemyNodes(*ourArmy,map);
			temp=&map->getMap()[enemy->armyArray[i]->getY()-1][enemy->armyArray[i]->getX()-1];
			temp->selected=true;
			for(int k=0; k<6; k++)
			{
				if(k<3)
				{
					if(!temp->nodeEdges[k]->upperNode->enemy&&!temp->nodeEdges[k]->creek_edge)
					{
						temp->nodeEdges[k]->upperNode->selected=true;
					}
				}
				else
				{
					if(!temp->nodeEdges[k]->lowerNode->enemy&&!temp->nodeEdges[k]->creek_edge)
					{
						temp->nodeEdges[k]->lowerNode->selected=true;
					}
				}
			}
		}
	}
}
void doRetreat(mapSuperClass* map,map_node *tempNode,armyClass* ourArmy,armyClass* enemy)
{
	bool retreatHappened=false;
	if(tempNode->selected)
	{
		for(int i=0; i<ourArmy->currentSize; i++)
		{
			if(ourArmy->armyArray[i]->retreat)
			{
				ourArmy->armyArray[i]->setPosition(tempNode->col,tempNode->row);
				ourArmy->armyArray[i]->retreat=false;
				retreatHappened=true;
			}
		}
		for(int i=0; i<enemy->currentSize; i++)
		{
			if(enemy->armyArray[i]->retreat)
			{
				enemy->armyArray[i]->setPosition(tempNode->col,tempNode->row);
				enemy->armyArray[i]->retreat=false;
				retreatHappened=true;
			}
		}
	}
	if(retreatHappened)
	{
		map->clearEnemy();
		map->clearMovement();
		IH::Instance()->retreatCalled=false;
	}
}

bool canFightOther(map_node * node, armyClass * army)
{
	unitClass * unit1 = NULL, * unit2 = NULL;
	for(int i = 0; i < 6; ++i)
	{
		getUnitsAroundNode(node, i, army, unit1, unit2);
		if(unit1 && !unit1->comPrep())
			return true;
		if(unit2 && !unit2->comPrep())
			return true;
	}
	return false;
}

bool clickAttacker(map_node * node, armyClass * attackerArmy, armyClass * defenderArmy)
{
	bool foundOtherCombatant = false;
	unitClass * unit1 = NULL, * unit2 = NULL;
	if(IH::Instance()->currentBattle.attackers.size() == 0)
	{
		IH::Instance()->preppingCombat = true;
	}
	else
	{
		for(int i = 0; i < 6; ++i)
		{
			getUnitsAroundNode(node, i, defenderArmy, unit1, unit2);
			if((unit1 != NULL && unit1->comPrep() && !unit1->completedCombat()) ||
				(unit2 != NULL && unit2->comPrep() && !unit2->completedCombat()))
				foundOtherCombatant = true;
		}
		if(!foundOtherCombatant)
			return false;
	}
	if(!getUnitsOnNode(node, attackerArmy, unit1, unit2))
	{
		return false;
	}
	if(unit1 != NULL)
	{
		unit1->setComPrep(true);
		IH::Instance()->currentBattle.attackers.push_back(unit1);
	}
	if(unit2 != NULL)
	{
		unit2->setComPrep(true);
		IH::Instance()->currentBattle.attackers.push_back(unit2);
	}
	
	for(int i = 0; i < 6; ++i)
	{
		getUnitsAroundNode(node, i, defenderArmy, unit1, unit2);
		if(unit1 || unit2)
		{
			if(unit1) unit1->setComPrep(true);
			if(unit2) unit2->setComPrep(true);
			if(i < 3)
			{
				if(!canFightOther(node->nodeEdges[i]->upperNode, attackerArmy))
				{
					if(unit1) IH::Instance()->currentBattle.defenders.push_back(unit1);
					if(unit2) IH::Instance()->currentBattle.defenders.push_back(unit2);
				}
			}
			else
			{
				if(!canFightOther(node->nodeEdges[i]->lowerNode, attackerArmy))
				{
					if(unit1) IH::Instance()->currentBattle.defenders.push_back(unit1);
					if(unit2) IH::Instance()->currentBattle.defenders.push_back(unit2);
				}
			}
		}
	}
	return true;
}

bool clickDefender(map_node * node, armyClass * attackerArmy, armyClass * defenderArmy)
{
	unitClass * unit1 = NULL, * unit2 = NULL;
	if(IH::Instance()->currentBattle.attackers.size() == 0)
		return false;
	bool foundOtherCombatant = false;
	for(int i = 0; i < 6; ++i)
	{
		getUnitsAroundNode(node, i, attackerArmy, unit1, unit2);
		if((unit1 != NULL && unit1->comPrep() && !unit1->completedCombat()) ||
			(unit2 != NULL && unit2->comPrep() && !unit2->completedCombat()))
			foundOtherCombatant = true;
	}
	if(!foundOtherCombatant)
		return false;
	if(unit1)
	{
		unit1->setComPrep(true);
		IH::Instance()->currentBattle.defenders.push_back(unit1);
	}
	if(unit2)
	{
		unit2->setComPrep(true);
		IH::Instance()->currentBattle.defenders.push_back(unit2);
	}
	for(int i = 0; i < 6; ++i)
	{
		getUnitsAroundNode(node, i, attackerArmy, unit1, unit2);
		if(unit1 || unit2)
		{
			if(unit1) unit1->setComPrep(true);
			if(unit2) unit2->setComPrep(true);
			if(i < 3)
			{
				if(!canFightOther(node->nodeEdges[i]->upperNode, defenderArmy))
				{
					if(unit1) IH::Instance()->currentBattle.attackers.push_back(unit1);
					if(unit2) IH::Instance()->currentBattle.attackers.push_back(unit2);
				}
			}
			else
			{
				if(!canFightOther(node->nodeEdges[i]->lowerNode, defenderArmy))
				{
					if(unit1) IH::Instance()->currentBattle.attackers.push_back(unit1);
					if(unit2) IH::Instance()->currentBattle.attackers.push_back(unit2);
				}
			}
		}
	}
	return true;
}

int battle::calcBattle()
{
	unitClass * unit = NULL;
	unitClass * otherUnit1 = NULL, *otherUnit2 = NULL;
	map_node * node = NULL;
	armyClass * attkr = &IH::Instance()->players[IH::Instance()->playersTurn].playerArmy;
	armyClass * dfndr = &IH::Instance()->players[!IH::Instance()->playersTurn].playerArmy;
	bool usableUnit = false;
	for(int i = 0; i < attackers.size(); ++i)
	{
		node = &IH::Instance()->map->getMap()[attackers.at(i)->getX()][attackers.at(i)->getY()];
		for(int j = 0; j < 6; ++j)
		{
			usableUnit = true;
			getUnitsAroundNode(node, i, dfndr, otherUnit1, otherUnit2);
			if(otherUnit1 && otherUnit1->completedCombat())
			{
				for(int k = 0; k < defenders.size(); ++k)
					if(otherUnit1 == defenders.at(i))
						usableUnit = false;
				if(usableUnit)
				{
					if(i < 3)
					{
						if(!canFightOther(node->nodeEdges[i]->upperNode, attkr))
							defenders.push_back(otherUnit1);
					}
					else
					{
						if(!canFightOther(node->nodeEdges[i]->lowerNode, attkr))
							defenders.push_back(otherUnit1);
					}
				}
			}
			usableUnit = true;
			if(otherUnit2 && otherUnit2->completedCombat())
			{
				for(int k = 0; k < defenders.size(); ++k)
					if(otherUnit2 == defenders.at(i))
						usableUnit = false;
				if(usableUnit)
				{
					if(i < 3)
					{
						if(!canFightOther(node->nodeEdges[i]->upperNode, attkr))
							defenders.push_back(otherUnit2);
					}
					else
					{
						if(!canFightOther(node->nodeEdges[i]->lowerNode, attkr))
							defenders.push_back(otherUnit2);
					}
				}
			}
		}
	}
	for(int i = 0; i < defenders.size(); ++i)
	{
		node = &IH::Instance()->map->getMap()[defenders.at(i)->getX()][defenders.at(i)->getY()];
		for(int j = 0; j < 6; ++j)
		{
			usableUnit = true;
			getUnitsAroundNode(node, i, attkr, otherUnit1, otherUnit2);
			if(otherUnit1 && otherUnit1->completedCombat())
			{
				for(int k = 0; k < attackers.size(); ++k)
					if(otherUnit1 == attackers.at(i))
						usableUnit = false;
				if(usableUnit)
				{
					if(i < 3)
					{
						if(!canFightOther(node->nodeEdges[i]->upperNode, dfndr))
							attackers.push_back(otherUnit1);
					}
					else
					{
						if(!canFightOther(node->nodeEdges[i]->lowerNode, dfndr))
							attackers.push_back(otherUnit1);
					}
				}
			}
			usableUnit = true;
			if(otherUnit2 && otherUnit2->completedCombat())
			{
				for(int k = 0; k < attackers.size(); ++k)
					if(otherUnit2 == attackers.at(i))
						usableUnit = false;
				if(usableUnit)
				{
					if(i < 3)
					{
						if(!canFightOther(node->nodeEdges[i]->upperNode, dfndr))
							attackers.push_back(otherUnit2);
					}						
					else
					{
						if(!canFightOther(node->nodeEdges[i]->lowerNode, dfndr))
							attackers.push_back(otherUnit2);
					}						
				}
			}
		}
	}
	return 0;
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/// main loop functions
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////



void IH::handlePrimaryInput()
{
	SDL_Rect tempRect;
	switch(gameState)
	{
	case atLogo:
		switch(event.type)
		{
		case SDL_KEYDOWN:
			switch(event.key.keysym.sym)
			{
			case SDLK_RETURN:				
				//gameState = atMatchPrep;
				gameState = atTitleScreen;
				break;
			}
			break;
		case SDL_QUIT:
			endGame();
			break;
		}
		break;
	case atTitleScreen:
		if(event.type == SDL_MOUSEBUTTONUP)
		{
			if(clickedIn(event, GameStartButton))
			{
				gameState = atMatchPrep;
			}
			if(clickedIn(event, GameQuitButton))
			{
				endGame();
			}
		}
		break;
	case atMatchPrep:
		if(event.type == SDL_MOUSEBUTTONUP)
		{
			if(!waiting && clickedIn(event, GameHotseatButton))
			{
				waiting = true;
				amHost = true;
				playingLAN = false;
				matchFileNames.setGame("chickamauga.txt");
				if(matchFileNames.checkFileNames())
				{
					matchFileNames.setFiles();
					createMatch();
					gameState = matchMainPhase;
				}
				else
				{
					gameState = atTitleScreen;
					cout << "FAILED TO LOAD FILES\n";
				}
			}
			if(!waiting && clickedIn(event, GameHostButton))
			{
				waiting = true;
				cout << "I'm prepping the match now\n";
				playingLAN = true;
				amHost = true;
				matchFileNames.setGame("chickamauga.txt");
				if(matchFileNames.checkFileNames())
				{
					matchFileNames.setFiles();
					MessageHandler::Instance()->setupHost();
					cout << MessageHandler::Instance()->getLastUDPError() << "\n";
				}
			}
			if(!waiting && clickedIn(event, GameJoinButton))
			{
				waiting = true;
				amHost = false;
				playingLAN = true;				
			}
		}
		else if(event.type == SDL_KEYDOWN && waiting && !amHost && playingLAN && !keysOff)
		{
			switch(event.key.keysym.sym)
			{
			case SDLK_RETURN:
				keysOff = true;
				MessageHandler::Instance()->setupClient((const char*)output.c_str());
				MessageHandler::Instance()->sendMessage(&output, GETIP);
				break;
			case SDLK_BACKSPACE:
				if(output.length() > 0)
					output = output.substr(0, output.length() - 1);
				break;
			default:
				if((event.key.keysym.sym >= 'a' && event.key.keysym.sym <= 'z') ||
					(event.key.keysym.sym >= 'A' && event.key.keysym.sym <= 'Z'))
				{
				}
				else if(((event.key.keysym.sym >= '0' && event.key.keysym.sym <= '9') || event.key.keysym.sym == '.') && output.length() < 15)
				{
					output += event.key.keysym.sym;
				}
				break;
			}
		}
		//matchFileNames.setGame("chickamauga.txt");
		//if(matchFileNames.checkFileNames())
		//{
		//	matchFileNames.setFiles();
		//	createMatch();
		//	gameState = matchMainPhase;
		//}
		//else
		//{
		//	cout << "FAILED    FAILED    FAILED TO LOAD FILES!!!\n";
		//}
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
				//playerIam = !playerIam;
				gameState = matchCombatPhase;
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
					cancelClick(map);
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
				if(currentUnits[0] || currentUnits[1])
				{	
					if(playerIam == 0 && playersTurn == 0)
					{
						if(unit1Selected)
							secondClick(map, &map->getMap()[selectedX][selectedY],actualX,actualY, players[0].playerArmy, players[1].playerArmy, currentUnits[0]);
						if(unit2Selected)
							secondClick(map, &map->getMap()[selectedX][selectedY],actualX,actualY, players[0].playerArmy, players[1].playerArmy, currentUnits[1]);
					}
					else if (playerIam == 1 && playersTurn == 1)
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
		{
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
					//playerIam = !playerIam;
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
						currentBattle.calcBattle();
					}
				}
				else if(firstX == actualX && firstY == actualY)
				{
					if(firstX < map->width && firstY < map->height)
					{
						selectedNode = &map->getMap()[firstX][firstY];
					}
					if(playersTurn == playerIam)
					{
						clickAttacker(selectedNode, &players[playersTurn].playerArmy, &players[!playersTurn].playerArmy);
						clickDefender(selectedNode, &players[playersTurn].playerArmy, &players[!playersTurn].playerArmy);
					}
					if(retreatCalled)
					{
						doRetreat(map,selectedNode,&players[0].playerArmy,&players[1].playerArmy);
					}
					//else if(playerIam)
					//{
					//if(setAttacker(map,&map->getMap()[firstX][firstY],&players[playerIam].playerArmy,&players[!playerIam].playerArmy))
					//{
					//	//why is this a seperate if statement? why not put it as a ! in place of the else
					//}
					//else
					//{
					//	if(enemyTarget(map,&map->getMap()[firstX][firstY],&players[playerIam].playerArmy,&players[!playerIam].playerArmy))
					//	{
					//		if(retreatCalled)
					//		{
					//			//map->clearMovement();
					//			showRetreat(map,&players[playerIam].playerArmy,&players[!playerIam].playerArmy);
					//		}
					//	}
					//	else
					//	{
					//		//spare else statment, need to use or get rid of
					//	}
					//}
					//}
					//else
					//{
					//	if(setAttacker(map,&map->getMap()[firstX][firstY],&players[0].playerArmy,&players[1].playerArmy))
					//	{

					//	}
					//	else
					//	{
					//		if(enemyTarget(map,&map->getMap()[firstX][firstY],&players[0].playerArmy,&players[1].playerArmy))
					//		{
					//			//same as above
					//			if(retreatCalled)
					//			{
					//				//map->clearMovement();
					//				showRetreat(map,&players[0].playerArmy,&players[1].playerArmy);
					//			}
					//		}
					//		else
					//		{

					//		}
					//	}
					//}
				}
			}
			break;
		}
	case reviewingMatch:
		break;		
	}	
}

void IH::update(int mspassed)
{
	//handle map borders
	bool switchState = false;
	switch(gameState)
	{
	case matchMainPhase:
	case matchCombatPhase:
	case reviewingMatch:
		screenShiftX += xMove*5;
		screenShiftY += yMove*5;
		if(screenShiftX > 50)
			screenShiftX = 50;
		if(screenShiftY > 44)
			screenShiftY = 44;
		if(screenShiftX < -(((map->width+1)*38)-screen->w+12))
			screenShiftX = -(((map->width+1)*38)-screen->w+12);
		if(screenShiftY < -(((map->height)*44)-screen->h+(GUIFrameRect.h*2)))
			screenShiftY = -(((map->height)*44)-screen->h+(GUIFrameRect.h*2));
		break;
	default:
		break;
	}
	switch(gameState)
	{
	case atLogo:
		break;
	case atTitleScreen:
		break;
	case atMatchPrep:
		break;
	case matchMainPhase:
		if(players[0].playerArmy.currentSize == 0 || players[1].playerArmy.currentSize == 0)
			gameState = reviewingMatch;
		break;
	case matchCombatPhase:
		if(playerIam == playersTurn)
		{
			switchState = true;
			for(int i = 0; i < players[playerIam].playerArmy.currentSize; ++i)
			{
				if(players[playerIam].playerArmy.armyArray[i]->needCombat() && !players[playerIam].playerArmy.armyArray[i]->completedCombat())
				{
					switchState = false;
					break;
				}
			}
		}
		if(switchState)
		{
			switchState = false;
			if(!playingLAN)
			{
				playerIam = !playerIam;
			}
			else
			{
				//SAMSAM send a message saying that the turn has ended, swap turns
			}
			playersTurn = !playersTurn;
			players[0].playerArmy.resetMoves();
			players[1].playerArmy.resetMoves();
		}
		else
		{
		}
		break;
	case reviewingMatch:
		//call something to calc the gameRules
		//gameRules->calcAllRules();
		break;
	}
	if(MessageHandler::Instance()->getMessage(&IH::Instance()->currentMessage, &IH::Instance()->currentMessageFlag))
		IH::Instance()->handleMessage();

	MessageHandler::Instance()->checkMessages();
	MessageHandler::Instance()->sendNextMessage();
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
		drawATile(utilityTiles5050, &u5050, 1, screen, GameStartButton.x, GameStartButton.y);
		drawATile(utilityTiles5050, &u5050, 2, screen, GameQuitButton.x, GameQuitButton.y);
		break;
	case atMatchPrep:
		drawATile(utilityTiles5050, &u5050, 3, screen, GameHostButton.x, GameHostButton.y);
		drawATile(utilityTiles5050, &u5050, 4, screen, GameJoinButton.x, GameJoinButton.y);
		drawATile(utilityTiles5050, &u5050, 5, screen, GameHotseatButton.x, GameHotseatButton.y);
		if(waiting)
			printStrings("Waiting for other player to join\n", GameMessageBox, screen, textColor, font1);
		if(output.length())
		{
			printStrings("\n" + output, GameMessageBox, screen, textColor, font1);
		}
		break;
	case matchMainPhase:
	case matchCombatPhase:
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

bool IH::handleMessage()
{
	string unitName, stringX, stringY;
	unitClass * unitToHandle;
	cout << currentMessage << "\n";
	int newX, newY, n = 0;
	switch(currentMessageFlag)
	{
	case GAMEFILENAME:
		matchFileNames.setGame(currentMessage);
		if(matchFileNames.checkFileNames())
			matchFileNames.setFiles();
		else
		{
			cout << "\n\nERROR, failure to load files\n\n";
			MessageHandler::Instance()->sendMessage((string*)"Failed to load files\n", QUIT);
		}
		return true;
		break;
	case GETIP:
		if(amHost)
		{
			cout << "OMG I GOT A PING\n";
			MessageHandler::Instance()->sendMessage(&matchFileNames.gameName, GAMEFILENAME);
		}
		else
		{
		}
		return true;
		break;
	case PICKFACTION:
		if(amHost)
		{
			int otherPrefferedFaction = (int)(currentMessage.c_str()[0] - '0');
			if(prefferedFaction == otherPrefferedFaction)
			{//choose randomly
				if(getRandomNum() % 2 == 0)
				{
					prefferedFaction = 0;
					otherPrefferedFaction = 1;
				}
				else
				{
					prefferedFaction = 1;
					otherPrefferedFaction = 0;
				}
			}
			ostringstream oss;
			oss << otherPrefferedFaction;
			playerIam = prefferedFaction;
			MessageHandler::Instance()->sendMessage(&oss.str(), PICKFACTION);

		}
		else
		{
			playerIam = (int)(currentMessage.c_str()[0] - '0');
		}
		return true;
		break;
	case MOVEUNIT:
		n = currentMessage.find('#', 0);
		unitName = currentMessage.substr(0, n);
		currentMessage = currentMessage.substr(n+1, -1);
		n = currentMessage.find('#', 0);
		stringX = currentMessage.substr(0, n);
		currentMessage = currentMessage.substr(n+1, -1);
		stringY = currentMessage;
		newX = stringToInt(stringX);
		newY = stringToInt(stringY);
		if(unitToHandle = players[!playerIam].playerArmy.findUnit(unitName))
			unitToHandle->setPosition(newX, newY);
		else
			return false;
		return true;
		break;
	case KILLUNIT:
		if(unitToHandle = players[!playerIam].playerArmy.findUnit(currentMessage))
			players[!playerIam].playerArmy.moveUnit(unitToHandle, MUFField, MUTKilled);
		else
			return false;
		return true;
		break;
	case REINFORCEUNIT:
		if(unitToHandle = players[!playerIam].playerArmy.findUnit(currentMessage))
			players[!playerIam].playerArmy.moveUnit(unitToHandle, MUFReinforce, MUTField);
		else
			return false;
		return true;
		break;
	case EXITUNIT:
		if(unitToHandle = players[!playerIam].playerArmy.findUnit(currentMessage))
			players[!playerIam].playerArmy.moveUnit(unitToHandle, MUFField, MUTExited);
		else
			return false;
		return true;
		break;
	case COMBATPHASE:
		return true;
		break;
	case STARTTURN:
		return true;
		break;
	case ENDTURN:
		return true;
		break;
	case DEFENDERRETREAT:
		return true;
		break;
	case CHATMESSAGE:
		return true;
		break;
	case QUIT:
		return true;
		break;
	}
	return false;
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