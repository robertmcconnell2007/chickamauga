#include "draw tools.h"

#include <string>
#include <sstream>
using namespace std;

void setEnemyNodes(armyClass, mapSuperClass*);

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

void showCombat()
{
	unitClass *unit;
	//IH::Instance()->map->clearEnemy();
	IH::Instance()->map->showEnemyControl = true;
	if(IH::Instance()->currentBattle.attackers.size() == 0 && IH::Instance()->currentBattle.defenders.size() == 0)
		return;
	for(int i=0; i< IH::Instance()->currentBattle.attackers.size(); i++)
	{
		unit = IH::Instance()->currentBattle.attackers.at(i);
		IH::Instance()->map->getMap()[unit->getY()-1][unit->getX()-1].selected = true;
	}
	for(int i = 0; i < IH::Instance()->currentBattle.defenders.size(); i++)
	{
		unit=IH::Instance()->currentBattle.defenders.at(i);
		IH::Instance()->map->getMap()[unit->getY()-1][unit->getX()-1].enemy = true;
	}
}

bool showRetreater(mapSuperClass *map, armyClass * attkrs, armyClass *dfndr)
{
	battle * tempBattle;	
	map_node *node;
	bool nodeFound=false;
	map->clearEnemy();
	map->clearMovement();
	checkUnitStacks(map,*attkrs,*dfndr);
 	tempBattle=&IH::Instance()->currentBattle;
	if(tempBattle->attackers.size() > 0)//still attackers
	{
		setEnemyNodes(*dfndr,map);
		node=&map->getMap()[tempBattle->attackers.back()->getY()-1][tempBattle->attackers.back()->getX()-1];
		for(int i=0; i<6; i++)
		{
			if(i<3)
			{
				if(!node->nodeEdges[i]->upperNode->enemy && !node->nodeEdges[i]->creek_edge && node->nodeEdges[i]->upperNode->numOfUnits < 2)
				{
					node->nodeEdges[i]->upperNode->selected=true;
					nodeFound=true;
				}
			}
			else
			{
				if(!node->nodeEdges[i]->lowerNode->enemy&&!node->nodeEdges[i]->creek_edge&&node->nodeEdges[i]->lowerNode->numOfUnits<2)
				{
					node->nodeEdges[i]->lowerNode->selected=true;
					nodeFound=true;
				}
			}
		}
	}
	else if(tempBattle->defenders.size() > 0)
	{
		setEnemyNodes(*attkrs,map);
		node=&map->getMap()[tempBattle->defenders.back()->getY()-1][tempBattle->defenders.back()->getX()-1];
		for(int i=0; i<6; i++)
		{
			if(i<3)
			{
				if(!node->nodeEdges[i]->upperNode->enemy&&!node->nodeEdges[i]->creek_edge&&node->nodeEdges[i]->upperNode->numOfUnits<2)
				{
					node->nodeEdges[i]->upperNode->selected=true;
					nodeFound=true;
				}
			}
			else
			{
				if(!node->nodeEdges[i]->lowerNode->enemy&&!node->nodeEdges[i]->creek_edge&&node->nodeEdges[i]->lowerNode->numOfUnits<2)
				{
					node->nodeEdges[i]->lowerNode->selected=true;
					nodeFound=true;
				}
			}
		}
	}
	else
	{
		IH::Instance()->retreatCalled=false;
		return false;
	}

	if(nodeFound)
	{
		map->clearEnemy();
		return true;
	}
	else//funky crap about to go down
	{
		if(tempBattle->attackers.size()>0)
		{
			for(int i=0; i<6; i++)
			{
				if(i<3)
				{
					if(!node->nodeEdges[i]->upperNode->enemy&&node->nodeEdges[i]->upperNode->numOfUnits==2)
					{
						for(int k=0; k<attkrs->currentSize; k++)
						{
							if(attkrs->armyArray[k]->getX()==node->nodeEdges[i]->upperNode->col&&attkrs->armyArray[k]->getY()==node->nodeEdges[i]->upperNode->row)
							{
								nodeFound=true;
								tempBattle->attackers.push_back(attkrs->armyArray[k]);
							}
						}
					}
					if(nodeFound)
					{
						break;
					}
				}
				else
				{
					if(!node->nodeEdges[i]->lowerNode->enemy&&node->nodeEdges[i]->lowerNode->numOfUnits==2)
					{
						for(int k=0; k<attkrs->currentSize; k++)
						{
							if(attkrs->armyArray[k]->getX()==node->nodeEdges[i]->lowerNode->col&&attkrs->armyArray[k]->getY()==node->nodeEdges[i]->lowerNode->row)
							{
								nodeFound=true;
								tempBattle->attackers.push_back(attkrs->armyArray[k]);
							}
						}
					}
					if(nodeFound)
					{
						break;
					}
				}
			}
		}
		else if(tempBattle->defenders.size()>0)
		{
			for(int i=0; i<6; i++)
			{
				if(i<3)
				{
					if(!node->nodeEdges[i]->upperNode->enemy&&node->nodeEdges[i]->upperNode->numOfUnits==2)
					{
						for(int k=0; k<dfndr->currentSize; k++)
						{
							if(dfndr->armyArray[k]->getX()==node->nodeEdges[i]->upperNode->col&&dfndr->armyArray[k]->getY()==node->nodeEdges[i]->upperNode->row)
							{
								nodeFound=true;
								tempBattle->defenders.push_back(dfndr->armyArray[k]);
							}
						}
					}
					if(nodeFound)
					{
						break;
					}
				}
				else
				{
					if(!node->nodeEdges[i]->lowerNode->enemy&&node->nodeEdges[i]->lowerNode->numOfUnits==2)
					{
						for(int k=0; k<dfndr->currentSize; k++)
						{
							if(dfndr->armyArray[k]->getX()==node->nodeEdges[i]->lowerNode->col&&dfndr->armyArray[k]->getY()==node->nodeEdges[i]->lowerNode->row)
							{
								nodeFound=true;
								tempBattle->defenders.push_back(dfndr->armyArray[k]);
							}
						}
					}
					if(nodeFound)
					{
						break;
					}
				}
			}
		}
	}
	if(nodeFound)
	{
		map->clearEnemy();
		return true;
	}
	else//if all else fails...unit says FML
	{
		if(tempBattle->attackers.size() > 0)
		{
			for(int k=0; k<attkrs->currentSize; k++)
			{
				if(attkrs->armyArray[k]==tempBattle->attackers.back())
				{
					attkrs->moveUnit(attkrs->armyArray[k],MUFField,MUTKilled);
					tempBattle->attackers.pop_back();
					break;
				}
			}
		}
		else if(tempBattle->defenders.size() > 0)
		{
			for(int k=0; k<dfndr->currentSize; k++)
			{
				if(dfndr->armyArray[k]==tempBattle->defenders.back())
				{
					dfndr->moveUnit(dfndr->armyArray[k],MUFField,MUTKilled);
					tempBattle->defenders.pop_back();
					break;
				}
			}
		}

	}
	return true;
}