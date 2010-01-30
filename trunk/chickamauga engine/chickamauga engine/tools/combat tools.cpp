#include "combat tools.h"

bool alreadyInAttkDef(unitClass * unit)
{
	for(int i = 0; i < IH::Instance()->currentBattle.attackers.size(); ++i)
	{
		if(unit == IH::Instance()->currentBattle.attackers.at(i))
			return true;
	}
	for(int i = 0; i < IH::Instance()->currentBattle.defenders.size(); ++i)
	{
		if(unit == IH::Instance()->currentBattle.defenders.at(i))
			return true;
	}
	return false;
}

bool getUnitsAroundNode(map_node * node, int path, armyClass * army, unitClass * &unit1, unitClass * &unit2)
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

bool getUnitsOnNode(map_node * node, armyClass * army, unitClass * &unit1, unitClass * &unit2)
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

int battle::calcBattle()
{
	unitClass * unit = NULL;
	unitClass * otherUnit1 = NULL, *otherUnit2 = NULL;
	map_node * node = NULL;
	armyClass * attkr = &IH::Instance()->players[IH::Instance()->playersTurn].playerArmy;
	armyClass * dfndr = &IH::Instance()->players[!IH::Instance()->playersTurn].playerArmy;
	bool usableUnit = false;
	if(!(attackers.size()>0))
	{
		return -1;
	}
	for(int i = 0; i < attackers.size(); ++i)
	{
		node = &IH::Instance()->map->getMap()[attackers.at(i)->getY()-1][attackers.at(i)->getX()-1];
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
						if(!canFightOther(node->nodeEdges[i]->upperNode, attkr) &&
							!alreadyInAttkDef(otherUnit1))
							defenders.push_back(otherUnit1);
					}
					else
					{
						if(!canFightOther(node->nodeEdges[i]->lowerNode, attkr) &&
							!alreadyInAttkDef(otherUnit1))
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
						if(!canFightOther(node->nodeEdges[i]->upperNode, attkr) &&
							!alreadyInAttkDef(otherUnit2))
							defenders.push_back(otherUnit2);
					}
					else
					{
						if(!canFightOther(node->nodeEdges[i]->lowerNode, attkr) &&
							!alreadyInAttkDef(otherUnit2))
							defenders.push_back(otherUnit2);
					}
				}
			}
		}
	}
	for(int i = 0; i < defenders.size(); ++i)
	{
		node = &IH::Instance()->map->getMap()[defenders.at(i)->getY()-1][defenders.at(i)->getX()-1];
		for(int j = 0; j < 6; ++j)
		{
			usableUnit = true;
			getUnitsAroundNode(node, j, attkr, otherUnit1, otherUnit2);
			if(otherUnit1 && !otherUnit1->completedCombat())
			{
				for(int k = 0; k < attackers.size(); ++k)
				{
					if(otherUnit1 == attackers.at(k))
						usableUnit = false;
				}
				if(usableUnit)
				{
					if(i < 3)
					{
						if(!canFightOther(node->nodeEdges[i]->upperNode, dfndr) &&
							!alreadyInAttkDef(otherUnit1))
							attackers.push_back(otherUnit1);
					}
					else
					{
						if(!canFightOther(node->nodeEdges[i]->lowerNode, dfndr) &&
							!alreadyInAttkDef(otherUnit1))
							attackers.push_back(otherUnit1);
					}
				}
			}
			usableUnit = true;
			if(otherUnit2 && !otherUnit2->completedCombat())
			{
				for(int k = 0; k < attackers.size(); ++k)
					if(otherUnit2 == attackers.at(k))
						usableUnit = false;
				if(usableUnit)
				{
					if(i < 3)
					{
						if(!canFightOther(node->nodeEdges[i]->upperNode, dfndr) &&
							!alreadyInAttkDef(otherUnit2))
							attackers.push_back(otherUnit2);
					}						
					else
					{
						if(!canFightOther(node->nodeEdges[i]->lowerNode, dfndr) &&
							!alreadyInAttkDef(otherUnit2))
							attackers.push_back(otherUnit2);
					}						
				}
			}
		}
	}
	unitClass * bob;
	for(int i = 0; i < attackers.size(); ++i)
		bob = attackers.at(i);
	for(int i = 0; i < defenders.size(); ++i)
		bob = defenders.at(i);
	///////////////////acutal battle calc now////////////
	///well assume that everyone can attack everybody and creeks
	///have already been accounted for
	battleCalculator bCalc;
	int result;
	int attackerPower=0;
	int defenderPower=0;
	for(int i=0; i<attackers.size(); i++)
	{
		attackerPower+=attackers.at(i)->getPower();
	}
	for(int i=0; i<defenders.size(); i++)
	{
		node=&IH::Instance()->map->getMap()[defenders.at(i)->getY()-1][defenders.at(i)->getX()-1];
		if(node->type==rough||node->type==roughForest)
		{
			defenderPower+=(defenders.at(i)->getPower()*IH::Instance()->gameRules->roughDefBonus);
		}
		else
		{
			defenderPower+=defenders.at(i)->getPower();
		}
	}
	result=bCalc.doBattle(attackerPower,defenderPower);
	//in results, if attack results in unit losses, vector will be
	//cleared. if attack results in a retreat, then vector of victorious will be cleared, retreater will be
	//cleared after all retreats have gone through
	switch(result)
	{
	case attackRetreat:
		{
			IH::Instance()->retreatCalled=true;
			for(int i=0; i<attackers.size(); i++)
			{
				attackers.at(i)->retreat=true;
				attackers.at(i)->setCompleteCombat();				
			}
			//for now just delete the defenders
			//will put in advance later COM
			for(int i=0; i<defenders.size(); i++)
			{
				defenders.at(i)->setCompleteCombat();
			}
			defenders.clear();
			break;
		}
	case attackElim:
		{
			for(int i=0; i<attackers.size(); i++)
			{
				for(int k=0; k<attkr->currentSize; k++)
				{
					if(attkr->armyArray[k]==attackers.at(i))
					{
						attkr->moveUnit(attkr->armyArray[k],MUFField,MUTKilled);
					}
				}
			}
			attackers.clear();
			for(int i=0; i<defenders.size(); i++)
			{
				defenders.at(i)->setCompleteCombat();
			}
			defenders.clear();
			break;
		}
	case defendRetreat:
		{
			IH::Instance()->retreatCalled=true;
			for(int i=0; i<defenders.size(); i++)
			{
				defenders.at(i)->retreat=true;
				defenders.at(i)->setCompleteCombat();
			}
			for(int i=0; i<attackers.size(); i++)
			{
				attackers.at(i)->setCompleteCombat();
			}
			attackers.clear();
			break;
		}
	case defendElim:
		{
			for(int i=0; i<defenders.size(); i++)
			{
				for(int k=0; k<dfndr->currentSize; k++)
				{
					if(dfndr->armyArray[k]==defenders.at(i))
					{
						dfndr->moveUnit(dfndr->armyArray[k],MUFField,MUTKilled);
					}
				}
			}
			defenders.clear();
			for(int i=0; i<attackers.size(); i++)
			{
				attackers.at(i)->setCompleteCombat();
			}
			attackers.clear();
			break;
		}
	case exchange:
		{
			break;
		}
	}
	IH::Instance()->preppingCombat = false;
	return 0;
}