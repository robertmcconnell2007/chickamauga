#include "draw tools.h"
#include "../infoLog.h"
#include "../Game Data Handler.h"

#include <string>
#include <sstream>
using namespace std;

void setEnemyNodes(armyClass, mapSuperClass*);

void drawBasicRules()
{
	ostringstream oss;
	oss << "Welcome to Blue vs. Gray: Chickamauga\n\n";
	oss << "Here are some of the basic rules to get you going:\n\n";
	oss << "Each player has units that they move across the map.\nThe numbers on the units denote their strength points.\n";
	oss << "The different unit types are Infantry, Cavalry, Artillary, and Trains.\n";
	oss << "Different map tiles have different movement costs associated with them\n";
	oss << "Look for roads on spaces, there is no movement penalty for moving from\na space with a road to a space with a connecting road.\n";
	oss << "Paths are similar, and reduce the penalty for moving.\n";
	oss << "Some spaces offer a defensive bonus, while others can't be crossed.\n";
	oss << "Watch out for enemy zones of control,\nIf one of your units enters one, it will have to do combat.\n";
	oss << "Complete combat by clicking on the units you want to participate in that combat.\n";
	oss << "Be careful though, all units able to, must do combat each round.\n";
	oss << "The game ends after a certain number of turns, or if one player runs out of units.\n";
	oss << "The score is based on the rules for that individual map.\n";
	oss << "To move a unit, select it, then click on a highlighted space.\n";
	oss << "You can select any space to see what type it is and what units are on it at the bottom.\n";
	oss << "To begin a match in Hotseat, or as a Host, you must know the name of the game\nyou want to play.\n";
	oss << "The basic game is called \"chickamauga.txt\"\n";
	oss << "To join another player's game online, you must know the IP address of the host.\n";
	oss << "Press the Tab key to open the menu.\n";
	oss << "There are several pages of scoring rules, click the mouse button on the rules to continue to next page.\n";
	oss << "The top left cell of the map is 1,1 (X,Y).\n";
	oss << "X increases as you move to the right, and Y increases as you move down.\n";

	printStrings(oss.str(), IH::Instance()->basicRulesBox, IH::Instance()->screen, IH::Instance()->textColor, IH::Instance()->font1);

}

void drawReinforce(SDL_Surface* screen)
{
	int j = 0;
	armyClass* currentArmy = &IH::Instance()->players[IH::Instance()->playerIam].playerArmy;
	for(int i = 0; i < currentArmy->reinforcementSize;i++)
	{
		if(currentArmy->reinforcements[i]->getReinforceTurn() <= IH::Instance()->getCurrentTurn())
		{
			j++;
		}
	}
	if(j == 0)
	{
		IH::Instance()->menuUp = false;
		IH::Instance()->canReinforce = false;
		return;
	}
	j = 0;
	SDL_FillRect(screen, &IH::Instance()->reinforceBox, 0x000000);
	printStrings("Select a unit to reinforce.", IH::Instance()->reinforceBox, screen, IH::Instance()->textColor, IH::Instance()->font1);
	drawATile(IH::Instance()->utilityTiles5050,&IH::Instance()->okBox, 8, screen, IH::Instance()->okBox.x,IH::Instance()->okBox.y);
	for(int i = 0; i < currentArmy->reinforcementSize;i++)
	{
		if(currentArmy->reinforcements[i]->getReinforceTurn() <= IH::Instance()->getCurrentTurn())
		{
			drawATile(currentArmy->getArmyColor(),currentArmy->reinforcements[i]->getUnitRect(),currentArmy->reinforcements[i]->getPower()-1,screen,IH::Instance()->reinforceBox.x + 25 +(currentArmy->reinforcements[i]->getUnitRect()->w*(j%10)),IH::Instance()->reinforceBox.y + 50 + IH::Instance()->okBox.h + (currentArmy->reinforcements[i]->getUnitRect()->h*(j/10)));
			switch(currentArmy->reinforcements[i]->getType())
			{
				case 0:
				{
					drawATile(currentArmy->getArmyColor(),currentArmy->reinforcements[i]->getUnitRect(),8,screen, IH::Instance()->reinforceBox.x + 25 +(currentArmy->reinforcements[i]->getUnitRect()->w*(j%10)),IH::Instance()->reinforceBox.y + 50 + IH::Instance()->okBox.h + (currentArmy->reinforcements[i]->getUnitRect()->h*(j/10)));
					break;
				}
				case 1:
				{
					drawATile(currentArmy->getUnitType(),currentArmy->reinforcements[i]->getTypeRect(),0,screen, IH::Instance()->reinforceBox.x + 34 +(currentArmy->reinforcements[i]->getUnitRect()->w*(j%10)),IH::Instance()->reinforceBox.y + 65 + IH::Instance()->okBox.h + (currentArmy->reinforcements[i]->getUnitRect()->h*(j/10)));
					break;
				}
				case 2:
				{
					drawATile(currentArmy->getUnitType(),currentArmy->reinforcements[i]->getTypeRect(),1,screen, IH::Instance()->reinforceBox.x + 34 +(currentArmy->reinforcements[i]->getUnitRect()->w*(j%10)),IH::Instance()->reinforceBox.y + 65 + IH::Instance()->okBox.h + (currentArmy->reinforcements[i]->getUnitRect()->h*(j/10)));
					break;
				}
				case 3:
				{
					drawATile(currentArmy->getUnitType(),currentArmy->reinforcements[i]->getTypeRect(),2,screen, IH::Instance()->reinforceBox.x + 34 +(currentArmy->reinforcements[i]->getUnitRect()->w*(j%10)),IH::Instance()->reinforceBox.y + 65 + IH::Instance()->okBox.h + (currentArmy->reinforcements[i]->getUnitRect()->h*(j/10)));
					break;
				}
			}
			j++;
		}
	}
}

void drawYesNo(SDL_Surface* screen)
{
	if(IH::Instance()->canExit)
	{
		SDL_FillRect(screen,&IH::Instance()->yesNoBox,0x000000);
		printStrings("Would you like to exit this unit from the map?", IH::Instance()->yesNoBox, screen, IH::Instance()->textColor, IH::Instance()->font1);
		drawATile(IH::Instance()->utilityTiles5050,&IH::Instance()->yesBox, 8, screen, IH::Instance()->yesBox.x,IH::Instance()->yesBox.y);
		drawATile(IH::Instance()->utilityTiles5050,&IH::Instance()->noBox, 9, screen, IH::Instance()->noBox.x,IH::Instance()->noBox.y);
	}
}

void drawChat(infoLog* chatBox, string chatString, int scroll, SDL_Surface* screen)
{
	if(chatBox->size-scroll < 10)
		scroll = chatBox->size-10;
	ostringstream linesInChatBox;
	for(int i = 0; i < 10; i++)
	{
		linesInChatBox << chatBox->at(chatBox->size-i-scroll) << "\n";
	}
	linesInChatBox << chatString;
	SDL_FillRect(screen, &IH::Instance()->chatRect, 0x000000);
	printStrings(linesInChatBox.str(), IH::Instance()->chatRect, screen, IH::Instance()->textColor, IH::Instance()->font1);
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
		switch(currentUnits[0]->getType())
		{
		case 1:
			{
				slottxt[0] << "Infantry\n";
				break;
			}
		case 2:
			{
				slottxt[0] << "Calvary\n";
				break;
			}
		case 3:
			{
				slottxt[0] << "Artillery\n";
			}

		}
	}
	if(currentUnits[1])
	{
		slottxt[1] << currentUnits[1]->getName() << "\n";
		slottxt[1] << currentUnits[1]->getPower() << "\n";
		switch(currentUnits[1]->getType())
		{
		case 1:
			{
				slottxt[1] << "Infantry\n";
				break;
			}
		case 2:
			{
				slottxt[1] << "Calvary\n";
				break;
			}
		case 3:
			{
				slottxt[1] << "Artillery\n";
			}

		}
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

void drawCombatGui(SDL_Surface *screen)
{
	
	ostringstream slottxt[3];
	for(int i = 0; i < 3; ++i)
		slottxt[i] << "";
	battle *tempBattle;
	int odds;
	double tempOdds;
	map_node *tempNode;
	SDL_FillRect(screen, &IH::Instance()->GUIFrameRect, 0x000000);
	tempBattle=&IH::Instance()->currentBattle;
	
	slottxt[0] << "Attackers Strength\n";
	slottxt[0] << IH::Instance()->attackerTotalPower << "\n";
	slottxt[1] << "Defenders Strength\n";
	slottxt[1] << IH::Instance()->defenderTotalPower<< "\n";

	slottxt[2] << "odds\n";
	//attacker advantage
	if(IH::Instance()->attackerTotalPower > IH::Instance()->defenderTotalPower)
	{
		if(IH::Instance()->defenderTotalPower>0)
		{
			odds=IH::Instance()->attackerTotalPower/IH::Instance()->defenderTotalPower;
			slottxt[2] << odds << " : 1\n";
		}
	}
	else if(IH::Instance()->defenderTotalPower> IH::Instance()->attackerTotalPower)
	{
		if(IH::Instance()->attackerTotalPower)
		{
			tempOdds= (double)IH::Instance()->defenderTotalPower/(double)IH::Instance()->attackerTotalPower;
			odds=tempOdds+0.9999;
			slottxt[2] << "1 : "<< odds << "\n";
		}
	}
	else
	{
		slottxt[2] << "1 : 1 \n";
	}
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
		while(tempBattle->attackers.back()->getType()==3)
		{
			node=&map->getMap()[tempBattle->attackers.back()->getY()-1][tempBattle->attackers.back()->getX()-1];
			if(!node->enemy)
			{
				tempBattle->attackers.pop_back();
			}
			else
			{
				break;
			}
		}
		node=&map->getMap()[tempBattle->attackers.back()->getY()-1][tempBattle->attackers.back()->getX()-1];
		if(tempBattle->attackers.back()->getType()==0)
		{
			for(int i=0; i<6; i++)
			{
				if(i<3)
				{
					if(!node->nodeEdges[i]->upperNode->enemy&&!node->nodeEdges[i]->creek_edge&&node->nodeEdges[i]->upperNode->numOfUnits<1&&(node->nodeEdges[i]->road_edge||node->nodeEdges[i]->trail_edge))
					{
						node->nodeEdges[i]->upperNode->selected=true;
						nodeFound=true;
					}
				}
				else
				{
					if(!node->nodeEdges[i]->lowerNode->enemy&&!node->nodeEdges[i]->creek_edge&&node->nodeEdges[i]->lowerNode->numOfUnits<1&&(node->nodeEdges[i]->road_edge||node->nodeEdges[i]->trail_edge))
					{
						node->nodeEdges[i]->lowerNode->selected=true;
						nodeFound=true;
					}
				}
			}
		}
		else
		{
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
	}
	else if(tempBattle->defenders.size() > 0)
	{
		setEnemyNodes(*attkrs,map);
		node=&map->getMap()[tempBattle->defenders.back()->getY()-1][tempBattle->defenders.back()->getX()-1];
		if(tempBattle->defenders.back()->getType()==0)
		{
			for(int i=0; i<6; i++)
			{
				if(i<3)
				{
					if(!node->nodeEdges[i]->upperNode->enemy&&!node->nodeEdges[i]->creek_edge&&node->nodeEdges[i]->upperNode->numOfUnits<1&&(node->nodeEdges[i]->road_edge||node->nodeEdges[i]->trail_edge))
					{
						node->nodeEdges[i]->upperNode->selected=true;
						nodeFound=true;
					}
				}
				else
				{
					if(!node->nodeEdges[i]->lowerNode->enemy&&!node->nodeEdges[i]->creek_edge&&node->nodeEdges[i]->lowerNode->numOfUnits<1&&(node->nodeEdges[i]->road_edge||node->nodeEdges[i]->trail_edge))
					{
						node->nodeEdges[i]->lowerNode->selected=true;
						nodeFound=true;
					}
				}
			}
		}
		else
		{
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
	else//funky crap about to go down //AKA Bumping 
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
							if(attkrs->armyArray[k]->getType()!=3&&attkrs->armyArray[k]->getX()==node->nodeEdges[i]->upperNode->col&&attkrs->armyArray[k]->getY()==node->nodeEdges[i]->upperNode->row)
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
							if(attkrs->armyArray[k]->getType()!=3&&attkrs->armyArray[k]->getX()==node->nodeEdges[i]->lowerNode->col&&attkrs->armyArray[k]->getY()==node->nodeEdges[i]->lowerNode->row)
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