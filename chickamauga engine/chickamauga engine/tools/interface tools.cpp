#include <sstream>
#include "interface tools.h"
#include "combat tools.h"
#include "../messageHandler.h"

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

void moveUnit(unitClass * unitToMove, mapSuperClass * map, int newX, int newY)
{
	unitToMove->setPosition(newY+1,newX+1);
	if(map->getMap()[newX][newY].enemy)
		unitToMove->setNeedCombat();
	//uncomment below line to restrict units to
	//one move per turn
	if(map->getMap()[newX][newY].control)
		map->getMap()[newX][newY].controlBlue = !IH::Instance()->playerIam;
	unitToMove->setMoved();
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
			moveUnit(unitMoving, map, newX, newY);
			if(IH::Instance()->playingLAN)
			{
				ostringstream bob;
				bob << unitMoving->getName() << "#" << newX << "#" << newY;
				MessageHandler::Instance()->sendMessage(bob.str(), MOVEUNIT); 
			}
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
	int numDef = 0;	
	for(int i = 0; i < 6; ++i)
	{
		getUnitsAroundNode(node, i, defenderArmy, unit1, unit2);
		if(unit1 || unit2)
		{
			numDef++;
			if(unit1) unit1->setComPrep(true);
			if(unit2) unit2->setComPrep(true);
			if(i < 3)
			{
				if(!canFightOther(node->nodeEdges[i]->upperNode, attackerArmy))
				{
					if(unit1 && !alreadyInAttkDef(unit1)) IH::Instance()->currentBattle.defenders.push_back(unit1);
					if(unit2 && !alreadyInAttkDef(unit2)) IH::Instance()->currentBattle.defenders.push_back(unit2);
				}
			}
			else
			{
				if(!canFightOther(node->nodeEdges[i]->lowerNode, attackerArmy))
				{
					if(unit1 && !alreadyInAttkDef(unit1)) IH::Instance()->currentBattle.defenders.push_back(unit1);
					if(unit2 && !alreadyInAttkDef(unit2)) IH::Instance()->currentBattle.defenders.push_back(unit2);
				}
			}
		}
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
				if(unit1 && !alreadyInAttkDef(unit1)) IH::Instance()->currentBattle.defenders.push_back(unit1);
				if(unit2 && !alreadyInAttkDef(unit2)) IH::Instance()->currentBattle.defenders.push_back(unit2);
			}
			else
			{
				if(unit1 && !alreadyInAttkDef(unit1)) IH::Instance()->currentBattle.defenders.push_back(unit1);
				if(unit2 && !alreadyInAttkDef(unit2)) IH::Instance()->currentBattle.defenders.push_back(unit2);
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
	if(!getUnitsOnNode(node, defenderArmy, unit1, unit2))
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
	int numAtk = 0;
	for(int i = 0; i < 6; ++i)
	{
		getUnitsAroundNode(node, i, attackerArmy, unit1, unit2);
		if(unit1 || unit2)
		{
			numAtk++;
			if(unit1) unit1->setComPrep(true);
			if(unit2) unit2->setComPrep(true);
			if(i < 3)
			{
				if(!canFightOther(node->nodeEdges[i]->upperNode, defenderArmy))
				{
					if(unit1 && !alreadyInAttkDef(unit1)) IH::Instance()->currentBattle.attackers.push_back(unit1);
					if(unit2 && !alreadyInAttkDef(unit2)) IH::Instance()->currentBattle.attackers.push_back(unit2);
				}
			}
			else
			{
				if(!canFightOther(node->nodeEdges[i]->lowerNode, defenderArmy))
				{
					if(unit1 && !alreadyInAttkDef(unit1)) IH::Instance()->currentBattle.attackers.push_back(unit1);
					if(unit2 && !alreadyInAttkDef(unit2)) IH::Instance()->currentBattle.attackers.push_back(unit2);
				}
			}
		}
	}
	if(numAtk == 1)
	{
		for(int i = 0; i < 6; ++i)
		{
			getUnitsAroundNode(node, i, attackerArmy, unit1, unit2);
			if(unit1 || unit2)
			{
				if(unit1) unit1->setComPrep(true);
				if(unit2) unit2->setComPrep(true);
				if(i < 3)
				{
					if(unit1 && !alreadyInAttkDef(unit1)) IH::Instance()->currentBattle.attackers.push_back(unit1);
					if(unit2 && !alreadyInAttkDef(unit2)) IH::Instance()->currentBattle.attackers.push_back(unit2);
				}
				else
				{
					if(unit1 && !alreadyInAttkDef(unit1)) IH::Instance()->currentBattle.attackers.push_back(unit1);
					if(unit2 && !alreadyInAttkDef(unit2)) IH::Instance()->currentBattle.attackers.push_back(unit2);
				}
			}
		}
	}
	return true;
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

void doRetreat(mapSuperClass *map , map_node *node, armyClass *attkrs,armyClass * dfndrs)
{
	battle * tempBattle=&IH::Instance()->currentBattle;
	if(tempBattle->attackers.size()>0)
	{
		setEnemyNodes(*dfndrs,map);
		if(node->selected&&!node->enemy)
		{
			tempBattle->attackers.back()->setPosition(node->col,node->row);
			tempBattle->attackers.back()->setCompleteCombat();
			tempBattle->attackers.pop_back();
		}
	}
	else if(tempBattle->defenders.size()>0)
	{
		setEnemyNodes(*attkrs,map);
		if(node->selected&&!node->enemy)
		{
			tempBattle->defenders.back()->setPosition(node->col,node->row);
			tempBattle->defenders.back()->setCompleteCombat();
			tempBattle->defenders.pop_back();
		}
	}
	if(tempBattle->attackers.empty()&&tempBattle->defenders.empty())
	{
		IH::Instance()->retreatCalled=false;
	}
	map->clearMovement();
	map->clearEnemy();	
}