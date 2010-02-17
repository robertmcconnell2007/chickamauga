#include <sstream>
#include "interface tools.h"
#include "combat tools.h"
#include "draw tools.h"
#include "../messageHandler.h"



void reinforceDialog(SDL_Event event)
{
	if(IH::Instance()->canReinforce && IH::Instance()->selectedNode->reinforce <= IH::Instance()->gameRules->unitMovePoints)
	{
		if(clickedIn(event, IH::Instance()->okBox))
		{
			IH::Instance()->menuUp = false;
			if(IH::Instance()->currentUnits[0] == NULL)
				IH::Instance()->canReinforce = false;
			else
			{
				IH::Instance()->players[IH::Instance()->playersTurn]->playerArmy.moveUnit(IH::Instance()->currentUnits[0], MUFReinforce, MUTField);
				moveUnit(IH::Instance()->currentUnits[0],IH::Instance()->map, IH::Instance()->selectedNode->row-1, IH::Instance()->selectedNode->col-1);
				//firstClick(IH::Instance()->map, IH::Instance()->selectedNode, IH::Instance()->players[IH::Instance()->playersTurn].playerArmy, IH::Instance()->players[!IH::Instance()->playersTurn].playerArmy);
				IH::Instance()->currentUnits[0]->resetMove();
				IH::Instance()->selectedNode->reinforce += 1;
				MessageHandler::Instance()->sendMessage(IH::Instance()->currentUnits[0]->getName(),REINFORCEUNIT);
				ostringstream oss;
				MessageHandler::Instance()->sendMessage(oss.str(),MOVEUNIT);
				cancelClick(IH::Instance()->map);
				IH::Instance()->canReinforce = false;
			}
		}
		else if(event.motion.x > IH::Instance()->reinforceBox.x + 25 && event.motion.x < 275 + IH::Instance()->reinforceBox.x && event.motion.y > IH::Instance()->reinforceBox.y + 100 && event.motion.y < 175 + IH::Instance()->reinforceBox.y)
		{
			int x = event.motion.x - IH::Instance()->reinforceBox.x - 25;
			int y = event.motion.y - IH::Instance()->reinforceBox.y - 100;
			x = x/IH::Instance()->players[0]->playerArmy.armyArray[0]->getUnitRect()->w;
			y = y/IH::Instance()->players[0]->playerArmy.armyArray[0]->getUnitRect()->h;
			int j = 0;
			for(int i = 0; i < IH::Instance()->players[IH::Instance()->playerIam]->playerArmy.reinforcementSize;i++)
			{
				if(IH::Instance()->players[IH::Instance()->playerIam]->playerArmy.reinforcements[i]->getReinforceTurn() <= IH::Instance()->getCurrentTurn())
				{
					j++;
				}
			}
			if(x+(y*10) < j)
				IH::Instance()->currentUnits[0] = IH::Instance()->players[IH::Instance()->playersTurn]->playerArmy.reinforcements[x+(y*10)];
		}
	}
}

void exitDialog(SDL_Event event)
{
	if(IH::Instance()->canExit)
	{
		if(clickedIn(event, IH::Instance()->yesBox))
		{
			IH::Instance()->menuOption = 0;
			IH::Instance()->menuUp = false;
		}
		else if(clickedIn(event, IH::Instance()->noBox))
		{
			IH::Instance()->menuOption = 1;
			IH::Instance()->menuUp = false;
		}
	}
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
	if(IH::Instance()->currentUnits[0] != NULL && IH::Instance()->currentUnits[0]->hasMoved())
	{
		if(!IH::Instance()->currentUnits[1])
			return false;
		if(IH::Instance()->currentUnits[1]->hasMoved())
			return false;
		
	}
	if(IH::Instance()->currentUnits[0]->getType() == 0)
		moveTrain(node,(IH::Instance()->gameRules->unitMovePoints)-(node->reinforce));
	else
		moveTo(node,(IH::Instance()->gameRules->unitMovePoints)-(node->reinforce));
	//else
	//{
	//	if(IH::Instance()->currentUnits[0]->getType() == 0)
	//		moveTrain(node,IH::Instance()->gameRules->unitMovePoints);
	//	else
	//		moveTo(node,IH::Instance()->gameRules->unitMovePoints);
	//}
	if(node->exit && ((IH::Instance()->currentUnits[0] != NULL && !IH::Instance()->currentUnits[0]->hasMoved()) || (IH::Instance()->currentUnits[1] != NULL && !IH::Instance()->currentUnits[1]->hasMoved())))
	{
		IH::Instance()->canExit = true;
		IH::Instance()->menuUp = true;
	}
	if(!(IH::Instance()->currentUnits[0] || IH::Instance()->currentUnits[1]))
		cancelClick(map);
	return true;
}

void moveUnit(unitClass * unitToMove, mapSuperClass * map, int newX, int newY)
{
	setEnemyNodes(IH::Instance()->players[!IH::Instance()->playersTurn]->playerArmy, map);
	unitToMove->setPosition(newY+1,newX+1);
	if(map->getMap()[newX][newY].enemy)
		unitToMove->setNeedCombat();
	//uncomment below line to restrict units to
	//one move per turn
	if(map->getMap()[newX][newY].control)
		map->getMap()[newX][newY].controlBlue = !IH::Instance()->playersTurn;
	unitToMove->setMoved();
	map->clearEnemy();
}

bool secondClick(mapSuperClass* map, map_node* node,int newX,int newY, armyClass currentArmy, armyClass enemyArmy, unitClass * unitMoving)
{
	checkUnitStacks(map,currentArmy,enemyArmy);
	map->clearEnemy();
	if(map->getMap()[newX][newY].numOfUnits > 1)
		cancelClick(map);
	else if(IH::Instance()->enemyUnitsSelected)
		cancelClick(map);
	else if(map->getMap()[newX][newY].movement>=0)
	{
		if(!unitMoving->hasMoved() && !(unitMoving->getX() == newY+1 && unitMoving->getY() == newX+1))
		{
			if(map->getMap()[newX][newY].exit && map->getMap()[newX][newY].movement > 0)
			{
				IH::Instance()->canExit = true;
				IH::Instance()->menuUp = true;
			}
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
	bool foundOtherCombatant = false, clickedArtillary = false;
	unitClass * unit1 = NULL, * unit2 = NULL;
	if(IH::Instance()->currentBattle.attackers.size() == 0)
	{
		IH::Instance()->preppingCombat = true;
		for(int i = 0; i < 6; ++i)
		{
			getUnitsAroundNode(node, i, defenderArmy, unit1, unit2);
			if((unit1 != NULL && !unit1->completedCombat()) ||
				(unit2 != NULL && !unit2->completedCombat()))
				foundOtherCombatant = true;
		}
		if(!foundOtherCombatant)
			return false;
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
		{
			getUnitsOnNode(node, attackerArmy, unit1, unit2);
			if((unit1 && unit1->getType() == 3) || (unit2 && unit2->getType() == 3))
			{
				//check for artillary combat
				if(foundDistantCombat(node, 3, defenderArmy))
					clickedArtillary = true;
				else
					return false;
			}
			else
				return false;
		}
	}
	if(!getUnitsOnNode(node, attackerArmy, unit1, unit2))
	{
		return false;
	}
	for(int i = 0; i < IH::Instance()->currentBattle.attackers.size(); ++i)
	{
		if(unit1 == IH::Instance()->currentBattle.attackers.at(i))
			return false;
	}
	if(unit1 != NULL && !(clickedArtillary && unit1->getType() != 3))
	{
		unit1->setComPrep(true);
		IH::Instance()->currentBattle.attackers.push_back(unit1);
	}
	if(unit2 != NULL && !(clickedArtillary && unit2->getType() != 3))
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
				if(!canFightOther(node->nodeEdges[i]->upperNode, attackerArmy) && !unit1->completedCombat())
				{
					if(unit1 && !alreadyInAttkDef(unit1)) IH::Instance()->currentBattle.defenders.push_back(unit1);
					if(unit2 && !alreadyInAttkDef(unit2)) IH::Instance()->currentBattle.defenders.push_back(unit2);
				}
			}
			else
			{
				if(!canFightOther(node->nodeEdges[i]->lowerNode, attackerArmy) && !unit1->completedCombat())
				{
					if(unit1 && !alreadyInAttkDef(unit1)) IH::Instance()->currentBattle.defenders.push_back(unit1);
					if(unit2 && !alreadyInAttkDef(unit2)) IH::Instance()->currentBattle.defenders.push_back(unit2);
				}
			}
		}
	}
	if(numDef == 1)
	{
		for(int i = 0; i < 6; ++i)
		{
			getUnitsAroundNode(node, i, defenderArmy, unit1, unit2);
			if(unit1 || unit2)
			{
				if(unit1) unit1->setComPrep(true);
				if(unit2) unit2->setComPrep(true);
				if(i < 3)
				{
					if(!unit1->completedCombat())
					{
						if(unit1 && !alreadyInAttkDef(unit1)) IH::Instance()->currentBattle.defenders.push_back(unit1);
						if(unit2 && !alreadyInAttkDef(unit2)) IH::Instance()->currentBattle.defenders.push_back(unit2);
					}
				}
				else
				{
					if(!unit1->completedCombat())
					{
						if(unit1 && !alreadyInAttkDef(unit1)) IH::Instance()->currentBattle.defenders.push_back(unit1);
						if(unit2 && !alreadyInAttkDef(unit2)) IH::Instance()->currentBattle.defenders.push_back(unit2);
					}
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
	if(!getUnitsOnNode(node, defenderArmy, unit1, unit2))
		return false;
	for(int i = 0; i < IH::Instance()->currentBattle.defenders.size(); ++i)
	{
		if(unit1 == IH::Instance()->currentBattle.defenders.at(i))
			return false;
	}
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
				if(!canFightOther(node->nodeEdges[i]->upperNode, defenderArmy) && !unit1->completedCombat())
				{
					if(unit1 && !alreadyInAttkDef(unit1)) IH::Instance()->currentBattle.attackers.push_back(unit1);
					if(unit2 && !alreadyInAttkDef(unit2)) IH::Instance()->currentBattle.attackers.push_back(unit2);
				}
			}
			else
			{
				if(!canFightOther(node->nodeEdges[i]->lowerNode, defenderArmy) && !unit1->completedCombat())
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
					if(!unit1->completedCombat())
					{
						if(unit1 && !alreadyInAttkDef(unit1)) IH::Instance()->currentBattle.attackers.push_back(unit1);
						if(unit2 && !alreadyInAttkDef(unit2)) IH::Instance()->currentBattle.attackers.push_back(unit2);
					}
				}
				else
				{
					if(!unit1->completedCombat())
					{
						if(unit1 && !alreadyInAttkDef(unit1)) IH::Instance()->currentBattle.attackers.push_back(unit1);
						if(unit2 && !alreadyInAttkDef(unit2)) IH::Instance()->currentBattle.attackers.push_back(unit2);
					}
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
	if(!IH::Instance()->canReinforce)
	{
		IH::Instance()->selectedNode    = NULL;
	}
	IH::Instance()->unit1Selected   = false;
	IH::Instance()->unit2Selected   = false;
	//currentUnits[1] = NULL;
}

void doRetreat(mapSuperClass *map , map_node *node, armyClass *attkrs,armyClass * dfndrs)
{
	battle * tempBattle=&IH::Instance()->currentBattle;
	ostringstream oss;
	if(tempBattle->attackers.size()>0)
	{
		setEnemyNodes(*dfndrs,map);
		if(node->selected && !node->enemy)
		{
			if(IH::Instance()->playingLAN)
			{
				oss << tempBattle->attackers.back()->getName() << "#" << node->row-1 << "#" << node->col-1;
				MessageHandler::Instance()->sendMessage(oss.str(), MOVEUNIT);
			}
			tempBattle->attackers.back()->setPosition(node->col,node->row);
			tempBattle->attackers.back()->setCompleteCombat();
			tempBattle->attackers.back()->retreat = false;
			tempBattle->attackers.pop_back();
		}
	}
	else if(tempBattle->defenders.size()>0)
	{
		setEnemyNodes(*attkrs,map);
		if(node->selected && !node->enemy)
		{
			if(IH::Instance()->playingLAN)
			{
				oss << tempBattle->defenders.back()->getName() << "#" << node->row-1 << "#" << node->col-1;
				MessageHandler::Instance()->sendMessage(oss.str(), MOVEUNIT);
			}
			tempBattle->defenders.back()->setPosition(node->col,node->row);
			tempBattle->defenders.back()->setCompleteCombat();
			tempBattle->defenders.back()->retreat = false;
			tempBattle->defenders.pop_back();
		}
	}
	if(tempBattle->attackers.empty() && tempBattle->defenders.empty())
	{
		if(IH::Instance()->playingLAN)
			MessageHandler::Instance()->sendMessage("done", DEFENDERRETREAT);
		IH::Instance()->retreatCalled = false;
	}
	map->clearMovement();
	showRetreater(map,&IH::Instance()->players[IH::Instance()->playersTurn]->playerArmy,&IH::Instance()->players[!IH::Instance()->playersTurn]->playerArmy);
	map->clearEnemy();
}

int checkEdgeTrain(node_edge* edge, int pos)
{
	if(edge == NULL)
		return -1;
	map_node* nodePointer;
	if(pos > 2)
		nodePointer = edge->lowerNode;
	else
		nodePointer = edge->upperNode;
	int numOccupy = nodePointer->numOfUnits;
	if(numOccupy > 0)
		return -1;
	if(edge->road_edge || (nodePointer->type == clear && edge->trail_edge))
		return 1;
	if(edge->trail_edge)
		return 2;
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
	if(node->movement < movement)
		node->movement = movement;
	else
		return;
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
					moveTo(node->nodeEdges[i]->lowerNode,movement-tempMove);
				else if(tempMove != -1 && movement-tempMove >= 0)
					moveTo(node->nodeEdges[i]->lowerNode,movement-tempMove);
			}
			else
			{
				if(node->nodeEdges[i]->upperNode->movement >= 0 && tempMove != -1 && movement-tempMove > node->nodeEdges[i]->upperNode->movement)
					moveTo(node->nodeEdges[i]->upperNode,movement-tempMove);
				else if(tempMove != -1 && movement-tempMove >= 0)
					moveTo(node->nodeEdges[i]->upperNode,movement-tempMove);
			}
		}
	}
}

void moveTrain(map_node* node,int movement)
{
	if(node->movement < movement)
		node->movement = movement;
	else
		return;
	if(movement == 0 || node->enemy)
		return;
	int tempMove;
	for(int i = 0; i < 6; i++)
	{
		tempMove = checkEdgeTrain(node->nodeEdges[i],i);
		if(tempMove != -1 || tempMove > movement)
		{
			if(i > 2)
			{
				moveTrain(node->nodeEdges[i]->lowerNode, movement-tempMove);
			}
			else
			{
				moveTrain(node->nodeEdges[i]->upperNode, movement-tempMove);
			}
		}
	}
}
