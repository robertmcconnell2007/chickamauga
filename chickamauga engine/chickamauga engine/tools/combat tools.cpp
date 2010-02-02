#include "combat tools.h"
#include "draw tools.h"
#include "../messageHandler.h"




void resetCombat()
{
	battle *tempBattle;
	tempBattle= &IH::Instance()->currentBattle;
	if(!tempBattle->attackers.empty())
	{
		for(int i=0; i<tempBattle->attackers.size(); i++)
		{
			tempBattle->attackers.at(i)->setComPrep(false);
		}
	}
	if(!tempBattle->defenders.empty())
	{
		for(int i=0; i<tempBattle->defenders.size(); i++)
		{
			tempBattle->defenders.at(i)->setComPrep(false);
		}
	}
	IH::Instance()->preppingCombat=false;
	tempBattle->attackers.clear();
	tempBattle->defenders.clear();
}


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
			getUnitsAroundNode(node, j, dfndr, otherUnit1, otherUnit2);
			if(otherUnit1 && otherUnit1->completedCombat())
			{
				for(int k = 0; k < defenders.size(); ++k)
					if(otherUnit1 == defenders.at(i))
						usableUnit = false;
				if(usableUnit)
				{
					if(j < 3)
					{
						if(!canFightOther(node->nodeEdges[j]->upperNode, attkr) &&
							!alreadyInAttkDef(otherUnit1))
							defenders.push_back(otherUnit1);
					}
					else
					{
						if(!canFightOther(node->nodeEdges[j]->lowerNode, attkr) &&
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
					if(j < 3)
					{
						if(!canFightOther(node->nodeEdges[j]->upperNode, attkr) &&
							!alreadyInAttkDef(otherUnit2))
							defenders.push_back(otherUnit2);
					}
					else
					{
						if(!canFightOther(node->nodeEdges[j]->lowerNode, attkr) &&
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
					if(j < 3)
					{
						if(!canFightOther(node->nodeEdges[j]->upperNode, dfndr) &&
							!alreadyInAttkDef(otherUnit1))
							attackers.push_back(otherUnit1);
					}
					else
					{
						if(!canFightOther(node->nodeEdges[j]->lowerNode, dfndr) &&
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
					if(j < 3)
					{
						if(!canFightOther(node->nodeEdges[j]->upperNode, dfndr) &&
							!alreadyInAttkDef(otherUnit2))
							attackers.push_back(otherUnit2);
					}						
					else
					{
						if(!canFightOther(node->nodeEdges[j]->lowerNode, dfndr) &&
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
		if(node->type==rough||node->type==roughForest||node->town)
		{
			defenderPower+=(defenders.at(i)->getPower()*IH::Instance()->gameRules->roughDefBonus);
		}
		else
		{
			defenderPower+=defenders.at(i)->getPower();
		}
	}
	result = IH::Instance()->gameRules->calculator.doBattle(attackerPower,defenderPower);
	//in results, if attack results in unit losses, vector will be
	//cleared. if attack results in a retreat, then vector of victorious will be cleared, retreater will be
	//cleared after all retreats have gone through
	defenderPower = 0;
	switch(result)
	{
	case attackRetreat:
		{
			cout << "Attacker retreat\n";
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
			showRetreater(IH::Instance()->map, attkr, dfndr);
			break;
		}
	case attackElim:
		{
			cout << "Attacker eliminated\n";
			for(int i=0; i<attackers.size(); i++)
			{
				for(int k=0; k<attkr->currentSize; k++)
				{
					if(attkr->armyArray[k]==attackers.at(i))
					{
						if(IH::Instance()->playingLAN)
							MessageHandler::Instance()->sendMessage(attkr->armyArray[k]->getName(), KILLUNIT);
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
			cout << "Defender Retreat\n";
			IH::Instance()->retreatCalled = true;
			if(IH::Instance()->playingLAN)
			{
				IH::Instance()->keysOff = true;
				for(int i=0; i < defenders.size(); i++)
				{
					MessageHandler::Instance()->sendMessage(defenders.at(i)->getName(), DEFENDERRETREAT);
				}
				MessageHandler::Instance()->sendMessage("ready", DEFENDERRETREAT);
			}
			else
			{
				for(int i=0; i<defenders.size(); i++)
				{
					defenders.at(i)->retreat=true;
					defenders.at(i)->setCompleteCombat();
				}
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
			cout << "Defender Eliminated\n";
			for(int i=0; i<defenders.size(); i++)
			{
				for(int k=0; k<dfndr->currentSize; k++)
				{
					if(dfndr->armyArray[k]==defenders.at(i))
					{
						if(IH::Instance()->playingLAN)
							MessageHandler::Instance()->sendMessage(dfndr->armyArray[k]->getName(), KILLUNIT);
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
			cout << "Exchange\n";
			for(int i=0; i<defenders.size(); i++)
			{
				defenderPower+=defenders.at(i)->getPower();
				for(int k=0; k<dfndr->currentSize; k++)
				{
					if(dfndr->armyArray[k]==defenders.at(i))
					{
						MessageHandler::Instance()->sendMessage(dfndr->armyArray[k]->getName(), KILLUNIT);
						dfndr->moveUnit(dfndr->armyArray[k],MUFField,MUTKilled);
					}
				}
			}
			defenders.clear();
			while(defenderPower>0)
			{
				for(int i=0; i<attackers.size(); i++)
				{
					//attackers will remain at greater than 1
					//defender power=0
					if(attackers.at(i)->getPower()==defenderPower)
					{
						for(int k=0; k<attkr->currentSize; k++)
						{
							if(attkr->armyArray[k]==attackers.at(i))
							{
								MessageHandler::Instance()->sendMessage(attkr->armyArray[k]->getName(), KILLUNIT);
								attkr->moveUnit(attkr->armyArray[k],MUFField,MUTKilled);
							}
						}
						defenderPower=0;
						break;
					}
					else if(attackers.at(i)->getPower()>defenderPower)
					{
						attackers.at(i)->setPower(attackers.at(i)->getPower() - defenderPower);
						defenderPower=0;
						break;
					}
					else if(attackers.at(i)->getPower() < defenderPower)
					{
						defenderPower -= attackers.at(i)->getPower();
						for(int k=0; k<attkr->currentSize; k++)
						{
							if(attkr->armyArray[k]==attackers.at(i))
							{
								MessageHandler::Instance()->sendMessage(attkr->armyArray[k]->getName(), KILLUNIT);
								attkr->moveUnit(attkr->armyArray[k],MUFField,MUTKilled);
							}
						}
					}
				}
			}
			for(int i=0; i<attackers.size(); i++)
			{
				attackers.at(i)->setCompleteCombat();
			}
			attackers.clear();
			break;
		}
	}
	IH::Instance()->preppingCombat = false;
	return 0;
}

//push the unit clarified in the message into the defender buffer so they 
//can be retreated
void networkRetreat(string unitName)
{
	armyClass *tempArmy;
	tempArmy=&IH::Instance()->players[IH::Instance()->playerIam].playerArmy;
	for(int i=0; i<tempArmy->currentSize; i++)
	{
		if(tempArmy->armyArray[i]->getName()==unitName)
		{
			cout << unitName << " added to retreat buffer.\n";
			tempArmy->armyArray[i]->retreat = true;
			tempArmy->armyArray[i]->needCombat();
			IH::Instance()->currentBattle.defenders.push_back(tempArmy->armyArray[i]);
		}
	}
}