#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include "Game Data Handler.h"
#include "tools.h"
#include "unitClass.h"
#include "mapSuperClass.h"
#include "tools.h"
#include "rules.h"
#include "graphicsloader.h"
#include "battleCalc.h"
#include "infoLog.h"
#include "sound.h"
#include "tools/combat tools.h"

#include "messageHandler.h"
#include "UDP.h"
///<<<<<<< .mine
#include "SDL.h"		// SDL library
#include "SDL_ttf.h"	// true-type font library for SDL
#include <stdio.h>

#include "tools/interface tools.h"
#include "tools/combat tools.h"
#include "tools/draw tools.h"

using namespace std;
enum terrainTypes;

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
///primary map and unit control functions
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

void roadScore(map_node* node, armyClass enemyUnits)
{
	for(int i = 0; i < enemyUnits.currentSize; i++)
	{
		if(node->col == enemyUnits.armyArray[i]->getX() && node->row == enemyUnits.armyArray[i]->getY())
			return;
	}
	node->movement = 1;
	for(int i = 0; i < 6; i++)
	{
		if(i < 3)
		{
			if(node->nodeEdges[i] != NULL && node->nodeEdges[i]->road_edge && node->nodeEdges[i]->upperNode->movement != 1)
				roadScore(node->nodeEdges[i]->upperNode,enemyUnits);
		}
		else
		{
			if(node->nodeEdges[i] != NULL && node->nodeEdges[i]->road_edge && node->nodeEdges[i]->lowerNode->movement != 1)
				roadScore(node->nodeEdges[i]->lowerNode,enemyUnits);
		}
	}
}

bool closeToRoad(map_node* node, int distanceLeft)
{
	if(distanceLeft < 0)
		return false;
	if(node->movement == 1)
	{
		node->selected = true;
		return true;
	}
	for(int i = 0; i < 6; i++)
	{
		if(i < 3)
		{
			if(node->nodeEdges[i] != NULL && node->nodeEdges[i]->upperNode->selected != true && !node->nodeEdges[i]->creek_edge)
				if(distanceLeft-1 >= 0 && closeToRoad(node->nodeEdges[i]->upperNode, distanceLeft-1))
				{
					node->selected = true;
					return true;
				}
		}
		else
		{
			if(node->nodeEdges[i] != NULL && node->nodeEdges[i]->lowerNode->selected != true && !node->nodeEdges[i]->creek_edge)
				if(distanceLeft-1 >= 0 && closeToRoad(node->nodeEdges[i]->lowerNode, distanceLeft-1))
				{
					node->selected = true;
					return true;
				}
		}
	}
	return false;
}

void updatePowers()
{
	ostringstream oss;
	IH::Instance()->attackerTotalPower=0;
	IH::Instance()->defenderTotalPower=0;
	battle *tempBattle;
	map_node *tempNode;
	tempBattle=&IH::Instance()->currentBattle;
	if(tempBattle->attackers.size()>0)
	{
		for(int i=0; i<tempBattle->attackers.size(); i++)
		{
			IH::Instance()->attackerTotalPower+=tempBattle->attackers.at(i)->getPower();
		}
	}
	if(tempBattle->defenders.size()>0)
	{
		for(int i=0; i<tempBattle->defenders.size(); i++)
		{
			tempNode=&IH::Instance()->map->getMap()[tempBattle->defenders.at(i)->getY()-1][tempBattle->defenders.at(i)->getX()-1];
			if(tempNode->type==rough||tempNode->type==roughForest)
			{
				IH::Instance()->defenderTotalPower+=tempBattle->defenders.at(i)->getPower()*IH::Instance()->gameRules->roughDefBonus;
			}
			else
			{
				IH::Instance()->defenderTotalPower+=tempBattle->defenders.at(i)->getPower();
			}
		}
	}
	oss << IH::Instance()->attackerTotalPower << "#" << IH::Instance()->defenderTotalPower;
	MessageHandler::Instance()->sendMessage(oss.str(),COMBATUPDATE);
}
int stringToInt(string str)
{
	int newInt = 0;
	int length = 0;

	length = str.length();
	for(int i = length - 1, j = 0; i >= 0; --i, ++j)
	{
		newInt += (int)((int)(str.c_str()[i] - '0') * pow(10.0, (double)j));
	}

	return newInt;
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

void setEnemyNodes(armyClass enemyArmy, mapSuperClass* map)
{
	for(int k = 0; k < enemyArmy.currentSize; k++)
	{
		map->setEnemy(enemyArmy.armyArray[k]->getX()-1,enemyArmy.armyArray[k]->getY()-1);
	}
}

void checkUnitStacks(mapSuperClass* map, armyClass first, armyClass second)
{
	map->cleanStacks();
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
					if(first.armyArray[f]->getType() == 0)
					{
						mapPointer[i][j].numOfUnits += 1;
					}
				}
			}
			for(int s = 0; s < second.currentSize; s++)
			{
				if(second.armyArray[s]->getX()-1 == j && second.armyArray[s]->getY()-1 == i)
				{
					mapPointer[i][j].numOfUnits += 1;
					if(second.armyArray[s]->getType() == 0)
					{
						mapPointer[i][j].numOfUnits += 1;
					}
				}
			}
		}
	}
}

///will show the first retreater of the retreating battle force

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
		if(event.type == SDL_QUIT)
		{
			endGame();
			break;
		}
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
			if(initialRulesDisplay && clickedIn(event, basicRulesBox))
				initialRulesDisplay = !initialRulesDisplay;
			if(clickedIn(event, basicRulesButton))
				initialRulesDisplay = !initialRulesDisplay;
			
		}
		break;
	case atMatchPrep:
		if(event.type == SDL_QUIT)
		{
			endGame();
			break;
		}
		if(event.type == SDL_MOUSEBUTTONUP)
		{
			if(!waiting && clickedIn(event, GameHotseatButton))
			{
				waiting = true;
				amHost = true;
				playingLAN = false;
				//gameState = atMatchPrepSecond;
				
				//matchFileNames.setGame("chickamauga.txt");
				//if(matchFileNames.checkFileNames())
				//{
				//	matchFileNames.setFiles();
				//	createMatch();
				//	gameState = matchMainPhase;					
				//}
				//else
				//{
				//	gameState = atTitleScreen;
				//	cout << "FAILED TO LOAD FILES\n";
				//}
			}
			if(!waiting && clickedIn(event, GameHostButton))
			{
				waiting = true;
				cout << "I'm prepping the match now\n";
				playingLAN = true;
				amHost = true;
				//matchFileNames.setGame("chickamauga.txt");
				//if(matchFileNames.checkFileNames())
				//{
				//	matchFileNames.setFiles();
				MessageHandler::Instance()->setupHost();
				//	cout << MessageHandler::Instance()->getLastUDPError() << "\n";
				//}
			}
			if(!waiting && clickedIn(event, GameJoinButton))
			{
				waiting = true;
				amHost = false;
				playingLAN = true;			
			}
		}
		else if(event.type == SDL_KEYDOWN && waiting /*&& playingLAN*/ && !keysOff)
		{
			switch(event.key.keysym.sym)
			{
			case SDLK_RETURN:
				keysOff = true;
				if(!amHost)
				{
					if(MessageHandler::Instance()->setupClient(output.c_str()))
					{
						MessageHandler::Instance()->sendMessage(output, GETIP);
						beginWait = SDL_GetTicks();
					}
					else
					{
						cout << MessageHandler::Instance()->getLastUDPError() << endl;
						output.clear();
						keysOff = false;
					}
				}
				else
				{
					matchFileNames.setGame(output);
					if(matchFileNames.checkFileNames())
					{
						matchFileNames.setFiles();
						canPickFaction = true;
						if(!playingLAN)
						{
							createMatch();
							keysOff = false;
							gameState = matchMainPhase;
						}
						if(connected)
						{
							MessageHandler::Instance()->sendMessage(output, GAMEFILENAME);
							MessageHandler::Instance()->sendMessage("OK", GETIP);
							gameState = atMatchPrepSecond;
						}
					}
					else
						keysOff = false;
				}
				output = "";
				break;
			case SDLK_BACKSPACE:
				if(output.length() > 0)
					output = output.substr(0, output.length() - 1);
				break;
			default:
				if((event.key.keysym.sym >= 'a' && event.key.keysym.sym <= 'z') ||
					(event.key.keysym.sym >= 'A' && event.key.keysym.sym <= 'Z') || ((event.key.keysym.sym >= '0' && event.key.keysym.sym <= '9') || event.key.keysym.sym == '.'))
				{
					output += event.key.keysym.sym;
				}
				break;
			}
		}
		break;
	case atMatchPrepSecond:
		if(event.type == SDL_QUIT)
		{
			endGame();
			break;
		}
		if(event.type == SDL_MOUSEBUTTONUP && prefferedFaction == 10)
		{
			if(clickedIn(event, BlueOptionBox))
			{
				prefferedFaction = 0;
				if(!amHost)
					MessageHandler::Instance()->sendMessage("0", PICKFACTION); 
			}
			if(clickedIn(event, GrayOptionBox))
			{
				prefferedFaction = 1;
				if(!amHost)
					MessageHandler::Instance()->sendMessage("1", PICKFACTION); 
			}
			if(clickedIn(event, RandomOptionBox))
			{
				prefferedFaction = 2;
				if(!amHost)
					MessageHandler::Instance()->sendMessage("2", PICKFACTION); 
			}
			if(otherPrefferedFaction != 10)
			{
				if(prefferedFaction == otherPrefferedFaction)
				{
					randomizefactions();
				}
				ostringstream oss;
				oss << otherPrefferedFaction;
				playerIam = prefferedFaction;
				MessageHandler::Instance()->sendMessage(oss.str(), PICKFACTION);
				createMatch();
				gameState = matchMainPhase;
				keysOff = false;
			}
		}
		
		break;
	case matchMainPhase:
		switch(event.type)
		{
		case SDL_KEYDOWN:
			switch(event.key.keysym.sym)
			{
			case SDLK_TAB:
				escapeMenu = !escapeMenu;
				break;
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
					if(startTyping)
					{
						MessageHandler::Instance()->sendMessage(chatString,CHATMESSAGE);
						chatBox->addString(chatString);
						chatString = "";
						startTyping = false;
					}
					else
					{
						startTyping = true;
					}
					break;
				case SDLK_BACKSPACE:
					{
						chatString = chatString.substr(0,chatString.length()-1);
					}
					break;
				default:
					if(startTyping)
					{
						char c = event.key.keysym.sym;
						if(c >= ' ' && c <= '~')
						{
							chatString += c;
						}
					}
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
			if(event.motion.x < 30)
				xMove = 1;
			else if(event.motion.x >= screenSize.x - 30)
				xMove = -1;
			else
				xMove = 0;
			if(event.motion.y < 30)
				yMove = 1;
			else if(event.motion.y >= screenSize.y - 30)
				yMove = -1;
			else
				yMove = 0;
			if(actualX < map->width && actualX >= 0 && actualY < map->height && actualY >= 0)
			{
				map->hilightHex(actualX,actualY);
			}
			if(clickedIn(event, chatRect))
			{
				if(chatRect.x == 0)
					chatRect.x = screenSize.x-chatRect.w;
				else
					chatRect.x = 0;
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			mouseDown = true;
			firstX = actualX;
			firstY = actualY;
			break;
		case SDL_MOUSEBUTTONUP:
			mouseDown = false;
			//check for open menu
			if(specificRulesDisplay)
			{
				if(clickedIn(event, IH::Instance()->gameRules->rulesWindow))
				{
					if(currentPage == maxPages)
					{
						specificRulesDisplay = false;
						currentPage = 1;
					}
					else
					{
						currentPage++;
					}
				}
			}
			else if(escapeMenu)
			{					
				if(clickedIn(event,menuOptions))
				{
					escapeMenu = false;
				}
				if(clickedIn(event,menuVolume))
				{
					specificRulesDisplay = true;
				}
				if(clickedIn(event,menuMain))
				{
					//IH::_resetAll();
					escapeMenu = false;
				}
				if(clickedIn(event,menuClose))
				{
					escapeMenu = false;
				}
			}
			if(!menuUp)
			{
				if(clickedIn(event, GUIFrameRect))
				{
					if(clickedIn(event, GUIEndTurnBox) && playersTurn == playerIam)
					{
						map->cleanReinforce();
						gameState = matchCombatPhase;
						cancelClick(map);
						if(playingLAN)
						{
							MessageHandler::Instance()->sendMessage("I'mma goin to combat!", COMBATPHASE);
							chatBox->addString("You have entered combat!");
						}
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
					if(firstX < map->width && firstY < map->height && firstX >= 0 && firstY >= 0)
					{
						selectedNode = &map->getMap()[firstX][firstY];
						//if(event.button.button == 1)
						//	roadScore(selectedNode, players[0].playerArmy); // TESTROADSCORE (left click on a road)
						//else
						//	closeToRoad(selectedNode, 10); // TESTCLOSETOROAD (any other click on a node currently without a unit cause it'll clear the selection)
						if(currentUnits[0] || currentUnits[1] && !menuUp)
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
							if(!canExit)
							{
								unit1Selected = unit2Selected = false;
								nodeGui = false;
								cancelClick(map);
							}
						}
						else if(isUnits(&map->getMap()[firstX][firstY],&players[0].playerArmy,&players[1].playerArmy) && !menuUp)
						{
							nodeGui = true;
							if(playerIam == 0)
								firstClick(map, &map->getMap()[actualX][actualY], players[0].playerArmy, players[1].playerArmy);
							else
								firstClick(map, &map->getMap()[actualX][actualY], players[1].playerArmy, players[0].playerArmy);
							selectedX = actualX;
							selectedY = actualY;
						}
						else if(selectedNode->reinforceBlue && playersTurn == 0 && playerIam == playersTurn && 
							selectedNode->reinforce < gameRules->unitMovePoints)
						{
							if(!canReinforce)
							{
								canReinforce = true;
								menuUp = true;
							}
						}
						else if(selectedNode->reinforceGrey && playersTurn == 1 && playerIam == playersTurn && 
							selectedNode->reinforce < gameRules->unitMovePoints)
						{
							canReinforce = true;
							menuUp = true;
						}
					}
				}
			}
			else
			{
				exitDialog(event);
				reinforceDialog(event);
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
				case SDLK_TAB:
					escapeMenu = !escapeMenu;
					break;
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
					if(startTyping)
					{
						MessageHandler::Instance()->sendMessage(chatString, CHATMESSAGE);
						chatBox->addString(chatString);
						chatString = "";
						startTyping = false;
					}
					else
					{
						startTyping = true;
					}
					break;
				case SDLK_DELETE:
					{
						chatString = chatString.substr(0,chatString.length()-1);
					}
					break;
				default:
					if(startTyping)
					{
						char c = event.key.keysym.sym;
						if(c >= ' ' && c <= '~')
						{
							chatString += c;
						}
					}
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
				if(clickedIn(event, chatRect))
				{
					if(chatRect.x == 0)
						chatRect.x = screenSize.x-chatRect.w;
					else
						chatRect.x = 0;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				mouseDown = true;
				firstX = actualX;
				firstY = actualY;
				break;
			case SDL_MOUSEBUTTONUP:
				mouseDown = false;
			
				//check for open menu
				if(specificRulesDisplay)
				{
					if(clickedIn(event, IH::Instance()->gameRules->rulesWindow))
						specificRulesDisplay = false;
				}
				else if(escapeMenu)
				{
					if(clickedIn(event,menuOptions))
					{
						escapeMenu = false;
					}
					if(clickedIn(event,menuVolume))
					{
						escapeMenu = false;
					}
					if(clickedIn(event,menuMain))
					{
						escapeMenu = false;
					}
					if(clickedIn(event,menuClose))
					{
						escapeMenu = false;
					}
				}
				if(!menuUp)
				{
					if(clickedIn(event, GUIFrameRect))
					{
						if(clickedIn(event, GUICalcCombatBox) && playerIam == playersTurn && !keysOff)
						{
							currentBattle.calcBattle();
						}
						if(clickedIn(event, GUIResetCombatBox) && playerIam==playersTurn && !keysOff)
						{
							resetCombat();
						}
					}
					else if(firstX == actualX && firstY == actualY)
					{
						if(firstX < map->width && firstY < map->height)
						{
							selectedNode = &map->getMap()[firstX][firstY];
						}
						if(retreatCalled)
						{
							if((playingLAN && playersTurn != playerIam) || !playingLAN)
								doRetreat(map,selectedNode,&players[playersTurn].playerArmy,&players[!playersTurn].playerArmy);
						}
						else if(playersTurn == playerIam && !keysOff)
						{
							clickAttacker(selectedNode, &players[playersTurn].playerArmy, &players[!playersTurn].playerArmy);
							clickDefender(selectedNode, &players[playersTurn].playerArmy, &players[!playersTurn].playerArmy);
							updatePowers();
						}
					}
					if(!retreatCalled)
					{
						cancelClick(map);
					}
				}
				else
				{
					//MENUUPCOMBATPHASE
				}
			}
			break;
		}
	case reviewingMatch:
		{
			
			switch(event.type)
			{
			case SDL_MOUSEBUTTONUP:
				if(clickedIn(event, ReturnToMenuBox))
				{
					_resetAll();
				}
				break;
			}
		}
		break;		
	}	
}

void IH::update(int mspassed)
{
//	int bob = 0;
	//handle map borders
	bool switchState = false;
	switch(gameState)
	{
	case matchMainPhase:
	case matchCombatPhase:
	case reviewingMatch:
		if(map)
		{
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
		}
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
		if(waiting && playingLAN && beginWait > 0)
		{
			if(!amHost && timeOut < SDL_GetTicks()-beginWait)
			{
				beginWait = 0;
				waiting = false;
				playingLAN = false;
				amHost = false;
				keysOff = false;
			}
			if(clickCancel)
			{
				beginWait = 0;
				waiting = false;
				playingLAN = false;
				amHost = false;
			}
		}
		break;
	case atMatchPrepSecond:
		break;
	case matchMainPhase:
		if(canExit && menuOption != -1)
		{
			if(menuOption == 0)
			{
				if(currentUnits[0] != NULL)
					players[playersTurn].playerArmy.moveUnit(currentUnits[0],MUFField, MUTExited);
				if(currentUnits[1] != NULL)
					players[playersTurn].playerArmy.moveUnit(currentUnits[1],MUFField, MUTExited);
			}
			if(menuOption == 1)
			{
			}
			unit1Selected = unit2Selected = false;
			nodeGui = false;
			cancelClick(map);
			canExit = false;
			menuOption = -1;
		}
		if(players[0].playerArmy.currentSize == 0 || players[1].playerArmy.currentSize == 0 || currentTurn > gameRules->numGameTurns)
		{
			//TEMPTEMP flash the screen to a wait message
			//this is a temp version
			SDL_FillRect(screen, &screen->clip_rect, 0x00000);
			SDL_Flip(screen);
			gameRules->calcAllRules();
			IH::gameSound->stopMusic(prevSong);
			IH::gameSound->playWAV(endGameMusic);
			IH::Instance()->prevSong = endGameMusic;
			gameState = reviewingMatch;
		}
		break;
	case matchCombatPhase:
		if(playerIam == playersTurn)
		{
			switchState = true;
			for(int i = 0; i < players[playerIam].playerArmy.currentSize; ++i)
			{
				if(players[playerIam].playerArmy.armyArray[i]->needCombat() && !(players[playerIam].playerArmy.armyArray[i]->completedCombat()))
				{
					switchState = false;
					break;
				}
			}
		}
		if(switchState && !retreatCalled)
		{
			attackerTotalPower=0;
			defenderTotalPower=0;
			if(playersTurn == 1)
				currentTurn++;
			playersTurn = !playersTurn;
			players[0].startTurn();
			players[1].startTurn();
			keysOff = false;
			gameState = matchMainPhase;
			if(!playingLAN)
			{
				playerIam = !playerIam;
				IH::Instance()->gameSound->stopMusic(prevSong);
				if(IH::Instance()->playersTurn == 0)
				{
					IH::Instance()->gameSound->playWAV(unionMusic);
					IH::Instance()->prevSong = unionMusic;
				}
				else
				{
					IH::Instance()->gameSound->playWAV(confederateMusic);
					IH::Instance()->prevSong = confederateMusic;
				}
			}
			else
			{
				MessageHandler::Instance()->sendMessage("Your turn!", STARTTURN);
				chatBox->addString("You have ended your turn.");
			}
			switchState = false;			
		}
		else
		{
			if(retreatCalled && !playingLAN)
			{
				showRetreater(map,&players[playersTurn].playerArmy,&players[!playersTurn].playerArmy);
			}
		}
		break;
	case reviewingMatch:
		//call something to calc the gameRules
		//gameRules->calcAllRules();
		break;
	}
	MessageHandler::Instance()->checkMessages();
	MessageHandler::Instance()->sendNextMessage();
	if(MessageHandler::Instance()->getMessage(&IH::Instance()->currentMessage, &IH::Instance()->currentMessageFlag))
		IH::Instance()->handleMessage();
	if(playingLAN && connected && (SDL_GetTicks()/1000)%2)
	{
		MessageHandler::Instance()->sendMessage("Yo!", PING);
	}
	if(playingLAN && connected && pingTime < SDL_GetTicks()-beginWait)
	{
		beginWait = SDL_GetTicks();
		if(connection == true)
		{
			connection = false;
		}
		else
		{
			int i = 0;
			//OMGQUITNOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO!!!!!!!!!
		}
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
		drawATile(utilityTiles5050, &u5050, 1, screen, GameStartButton.x, GameStartButton.y);
		drawATile(utilityTiles5050, &u5050, 2, screen, GameQuitButton.x, GameQuitButton.y);
		drawATile(utilityTiles5050, &u5050, 13, screen, basicRulesButton.x, basicRulesButton.y);
		if(initialRulesDisplay)
		{
			SDL_FillRect(screen, &basicRulesBox, 0x000000);
			drawBasicRules();
		}
		break;
	case atMatchPrep:
		if(!waiting)
		{
			printStrings("Please choose an option.\n", GameMessageBox, screen, textColor, font1);
			drawATile(utilityTiles5050, &u5050, 3, screen, GameHostButton.x, GameHostButton.y);
			drawATile(utilityTiles5050, &u5050, 4, screen, GameJoinButton.x, GameJoinButton.y);
			drawATile(utilityTiles5050, &u5050, 5, screen, GameHotseatButton.x, GameHotseatButton.y);
		}
		else
		{
			if(amHost && canPickFaction)
				printStrings("Waiting for other player to join\n", GameMessageBox, screen, textColor, font1);
			else if(amHost && !canPickFaction)
			{
				printStrings("Please type in the name of the game to play\n", GameMessageBox, screen, textColor, font1);
				printStrings("\n" + output, GameMessageBox, screen, textColor, font1);
			}
			else
			{
				printStrings("Please enter the IP number of the host.", GameMessageBox, screen, textColor, font1);
				printStrings("\n" + output, GameMessageBox, screen, textColor, font1);
			}
		}
		break;
	case atMatchPrepSecond:
		if(!canPickFaction && amHost)
		{
			printStrings("Please type in the name of the game to play.", GameMessageBox, screen, textColor, font1);
			printStrings("\n" + output, GameMessageBox, screen, textColor, font1);
		}
		else if(prefferedFaction == 10)
			printStrings("Please choose your preffered faction.\nIf both players choose the same faction, it will be randomized.\n", GameMessageBox, screen, textColor, font1);
		else
			printStrings("Waiting on other player to pick their preffered faction.\n", GameMessageBox, screen, textColor, font1);
		if(canPickFaction)
		{
			drawATile(utilityTiles5050, &u5050, 10, screen, BlueOptionBox.x, BlueOptionBox.y);
			drawATile(utilityTiles5050, &u5050, 11, screen, GrayOptionBox.x, GrayOptionBox.y);
			drawATile(utilityTiles5050, &u5050, 12, screen, RandomOptionBox.x, RandomOptionBox.y);
		}
		break;
	case matchMainPhase:
	case matchCombatPhase:
		//drawATile(utilityTiles5050, &u5050, 6, GUIGameFrame, 0,0);
		map->drawMap(screenShiftX, screenShiftY, screen);
		players[0].playerArmy.drawArmy(screenShiftX,screenShiftY,map->width,map->height,screen);
		players[1].playerArmy.drawArmy(screenShiftX,screenShiftY,map->width,map->height,screen);
		showCombat();
		if(gameState==matchMainPhase)
		{
			drawGui(selectedNode,&players[0].playerArmy,&players[1].playerArmy, currentUnits, screen);
			drawATile(utilityTiles5050, &u5050, 14, screen, GUIEndTurnBox.x, GUIEndTurnBox.y);
			//creates a black rec
			SDL_FillRect(screen,&turnRect,0x000000);
			ostringstream oss;
			//loads turn: # of # into oss
			oss << "Turn: " << IH::Instance()->getCurrentTurn() << " of " << IH::Instance()->gameRules->numGameTurns;
			//prints the oss onto the rectangle
			printStrings(oss.str(),turnRect,screen,IH::textColor,IH::font1);
		}
		else
		{
			drawCombatGui(screen);
			if(playerIam == playersTurn)
			{
				drawATile(utilityTiles5050, &u5050, 6, screen, GUICalcCombatBox.x, GUICalcCombatBox.y);
				drawATile(utilityTiles5050, &u5050, 7, screen, GUIResetCombatBox.x, GUIResetCombatBox.y);
			}
		}

		if(playingLAN)
			drawChat(chatBox,chatString,1,screen);
		if(escapeMenu)
		{
			drawMenu();			
		}
		if(menuUp)
		{
			if(canExit)
				drawYesNo(screen);
			if(canReinforce)
				drawReinforce(screen);
		}
		//drawATile(utilityTiles5050, &u5050, 0, screen, GUIEndTurnBox.x, GUIEndTurnBox.y);
		if(specificRulesDisplay)
		{
			SDL_FillRect(screen, &gameRules->rulesWindow, 0x0000000);
			//drawATile(gameRules->rulesDisplay, &gameRules->rulesWindow, 0, screen, gameRules->rulesWindow.x, gameRules->rulesWindow.y);
			gameRules->printRules(currentPage);
			//SDL_BlitSurface(gameRules->rulesDisplay, &gameRules->rulesWindow, screen, &screen->clip_rect);
		}
		break;
	case reviewingMatch:
		ostringstream oss;
		oss << "Blue player scored ";
		oss << players[0].pointsEarned;
		oss << " points\n\n";
		oss << "Gray player scored ";
		oss << players[1].pointsEarned;
		oss << " points\n\n";
		if(players[0].pointsEarned == players[1].pointsEarned)
			oss << "The game was a tie\n\n";
		else if(players[0].pointsEarned > players[1].pointsEarned)
			oss << "Blue player has won\n\n";
		else
			oss << "Gray player has won\n\n";
		printStrings(oss.str(), matchEndOutputBox, screen, textColor, font1);
		drawATile(menuTiles, &u20060, 4, screen, ReturnToMenuBox.x, ReturnToMenuBox.y);
		break;
	}
	if(SDL_Flip(screen) == -1)
		return;
}

bool IH::handleMessage()
{
	string unitName, stringX, stringY;
	string temp;
	unitClass * unitToHandle;
	cout << currentMessage << "\n";
	int newX, newY, n = 0;
	switch(currentMessageFlag)
	{
	case GAMEFILENAME:
		matchFileNames.setGame(currentMessage);
		if(matchFileNames.checkFileNames())
		{
			matchFileNames.setFiles();
			//createMatch();
			//gameState = matchMainPhase;
		}
		else
		{
			cout << "\n\nERROR, failure to load files\n\n";
			MessageHandler::Instance()->sendMessage("Failed to load files", QUIT);
		}
		return true;
		break;
	case GETIP:
		if(amHost)
		{
			//MessageHandler::Instance()->sendMessage("1", PICKFACTION);
			//MessageHandler::Instance()->sendMessage(matchFileNames.gameName, GAMEFILENAME);
			connected = true;
			beginWait = SDL_GetTicks();
			connection = true;
			MessageHandler::Instance()->sendMessage("Yo!",PING);
			if(canPickFaction)
			{
				MessageHandler::Instance()->sendMessage(matchFileNames.gameName, GAMEFILENAME);
				MessageHandler::Instance()->sendMessage("OK", GETIP);
				gameState = atMatchPrepSecond;
			}
		}
		else
		{
			if(currentMessage == "OK")
			{
				connected = true;
				beginWait = SDL_GetTicks();
				connection = true;
				gameState = atMatchPrepSecond;
				canPickFaction = true;
			}
		}
		return true;
		break;
	case PICKFACTION:
		if(amHost && prefferedFaction != 10)
		{
			otherPrefferedFaction = (int)(currentMessage.c_str()[0] - '0');
			if(prefferedFaction == otherPrefferedFaction)
			{
				randomizefactions();
			}
			ostringstream oss;
			oss << otherPrefferedFaction;
			playerIam = prefferedFaction;
			MessageHandler::Instance()->sendMessage(oss.str(), PICKFACTION);
			createMatch();
			gameState = matchMainPhase;
			keysOff = false;
		}
		else if(amHost)
		{
			otherPrefferedFaction = (int)(currentMessage.c_str()[0] - '0');
		}
		else
		{
			playerIam = (int)(currentMessage.c_str()[0] - '0');
			createMatch();
			gameState = matchMainPhase;
			keysOff = false;
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
		{
			moveUnit(unitToHandle, map, newX, newY);
			temp = unitName + " has moved to (" + stringX + ", " + stringY + ")";
			chatBox->addString(temp);
		}
		else
			return false;
		return true;
		break;
	case KILLUNIT:
		if(unitToHandle = players[!playerIam].playerArmy.findUnit(currentMessage))
		{
			players[!playerIam].playerArmy.moveUnit(unitToHandle, MUFField, MUTKilled);
			temp = "Enemy unit, " + currentMessage + " has been killed.";
			chatBox->addString(temp);
		}
		else if(unitToHandle = players[playerIam].playerArmy.findUnit(currentMessage))
		{
			players[playerIam].playerArmy.moveUnit(unitToHandle, MUFField, MUTKilled);
			temp = "Your unit, " + currentMessage + " has been killed.";
			chatBox->addString(temp);
		}
		else
			return false;
		return true;
		break;
	case REINFORCEUNIT:
		if(unitToHandle = players[!playerIam].playerArmy.findUnit(currentMessage))
		{
			players[!playerIam].playerArmy.moveUnit(unitToHandle, MUFReinforce, MUTField);
			temp = currentMessage + " has entered the battlefield.";
			chatBox->addString(temp);
		}
		else
			return false;
		return true;
		break;
	case EXITUNIT:
		if(unitToHandle = players[!playerIam].playerArmy.findUnit(currentMessage))
		{
			players[!playerIam].playerArmy.moveUnit(unitToHandle, MUFField, MUTExited);
			temp = currentMessage + " has left the battlefield.";
			chatBox->addString(temp);
		}
		else
			return false;
		return true;
		break;
	case COMBATPHASE:
		chatBox->addString(currentMessage);
		gameState = matchCombatPhase;
		return true;
		break;
	case STARTTURN:
		if(playersTurn == 1)
			currentTurn++;
		playersTurn = !playersTurn;
		keysOff = false;
		players[0].startTurn();
		players[1].startTurn();
		chatBox->addString(currentMessage);
		gameState = matchMainPhase;
		keysOff = false;
		return true;
		break;
	case ENDTURN:
		return true;
		break;
	case DEFENDERRETREAT:
		if(currentMessage == "done")
		{
			keysOff = false;
			retreatCalled = false;
			currentBattle.attackers.clear();
			currentBattle.defenders.clear();
		}
		else if(currentMessage == "ready")
		{
			retreatCalled = true;
			chatBox->addString("Your units lost a battle and need to retreat!");
			showRetreater(map,&players[playersTurn].playerArmy,&players[!playersTurn].playerArmy);
		}
		else
		{
			networkRetreat(currentMessage);
			chatBox->addString("Your unit, " + currentMessage + ", needs to retreat.");
		}
		return true;
		break;
	case CHATMESSAGE:
		chatBox->addString(currentMessage);
		return true;
		break;
	case QUIT:
		cout << currentMessage << "\n";
		chatBox->addString(currentMessage);
		gameRules->calcAllRules();
		IH::gameSound->stopMusic(prevSong);
			IH::gameSound->playWAV(endGameMusic);
			IH::Instance()->prevSong = endGameMusic;
		gameState = reviewingMatch;
		return true;
		break;
	case COMBATUPDATE:
		n = currentMessage.find('#', 0);
		newX = stringToInt(currentMessage.substr(0,n));
		currentMessage = currentMessage.substr(n+1, -1);
		newY = stringToInt(currentMessage);
		break;
	case PING:
		connection = true;
		break;
	}
	return false;
}

