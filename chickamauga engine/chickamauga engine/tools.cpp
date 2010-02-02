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
				MessageHandler::Instance()->setupClient(output.c_str());
				MessageHandler::Instance()->sendMessage(output, GETIP);
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
					if(firstX < map->width && firstY < map->height)
					{
						selectedNode = &map->getMap()[firstX][firstY];
						if(selectedNode->reinforceBlue && playersTurn == 0 && playerIam == playersTurn)
							canReinforce = true;
						if(selectedNode->reinforceGrey && playersTurn == 1 && playerIam == playersTurn)
							canReinforce = true;
					}
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
				break;
			case SDL_MOUSEBUTTONDOWN:
				mouseDown = true;
				firstX = actualX;
				firstY = actualY;
				break;
			case SDL_MOUSEBUTTONUP:
				mouseDown = false;
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
							doRetreat(map,selectedNode,&players[playersTurn].playerArmy,&players[!playersTurn].playerArmy);
						}
						else if(playersTurn == playerIam && !keysOff)
						{
							clickAttacker(selectedNode, &players[playersTurn].playerArmy, &players[!playersTurn].playerArmy);
							clickDefender(selectedNode, &players[playersTurn].playerArmy, &players[!playersTurn].playerArmy);
						}

					}
					if(!retreatCalled)
					{
						cancelClick(map);
					}
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
				if(players[playerIam].playerArmy.armyArray[i]->needCombat() && !(players[playerIam].playerArmy.armyArray[i]->completedCombat()))
				{
					switchState = false;
					break;
				}
			}
		}
		if(switchState && !retreatCalled)
		{
			if(playersTurn == 1)
				currentTurn++;
			playersTurn = !playersTurn;
			players[0].startTurn();
			players[1].startTurn();
			gameState = matchMainPhase;			
			if(!playingLAN)
			{
				playerIam = !playerIam;
				//IH::Instance()->gameSound->stopMusic(prevSong);
				//if(IH::Instance()->playersTurn == 0)
				//{
				//	IH::Instance()->gameSound->playWAV(unionMusic);
				//	IH::Instance()->prevSong = unionMusic;
				//}
				//else
				//{
				//	IH::Instance()->gameSound->playWAV(confederateMusic);
				//	IH::Instance()->prevSong = confederateMusic;
				//}
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
		{
			if(amHost)
				printStrings("Waiting for other player to join\n", GameMessageBox, screen, textColor, font1);
			else
				printStrings("Please enter the IP number of the host.", GameMessageBox, screen, textColor, font1);
		}
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
		showCombat();
		if(gameState==matchMainPhase)
		{
			drawGui(selectedNode,&players[0].playerArmy,&players[1].playerArmy, currentUnits, screen);
			drawATile(utilityTiles5050, &u5050, 0, screen, GUIEndTurnBox.x, GUIEndTurnBox.y);
		}
		else if(!IH::Instance()->retreatCalled)
		{
			drawCombatGui(screen);
			drawATile(utilityTiles5050, &u5050, 6, screen, GUICalcCombatBox.x, GUICalcCombatBox.y);
			drawATile(utilityTiles5050, &u5050, 7, screen, GUIResetCombatBox.x, GUIResetCombatBox.y);
		}
		drawChat(chatBox,chatString,1,screen);
		
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
			createMatch();
			gameState = matchMainPhase;
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
			cout << "OMG I GOT A PING\n";
			MessageHandler::Instance()->sendMessage("1", PICKFACTION);
			MessageHandler::Instance()->sendMessage(matchFileNames.gameName, GAMEFILENAME);
			createMatch();
			gameState = matchMainPhase;
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
			MessageHandler::Instance()->sendMessage(oss.str(), PICKFACTION);

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
		gameState = reviewingMatch;
		return true;
		break;
	}
	return false;
}

