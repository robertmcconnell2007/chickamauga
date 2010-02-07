#pragma once
#include "SDL.h"
#include "../Game Data Handler.h"
#include "../tools.h"

bool firstClick(mapSuperClass* map, map_node* node, armyClass currentArmy, armyClass enemyArmy);\
bool secondClick(mapSuperClass* map, map_node* node,int newX,int newY, armyClass currentArmy, armyClass enemyArmy, unitClass * unitMoving);
bool clickedIn(SDL_Event event, SDL_Rect rect);
bool clickAttacker(map_node * node, armyClass * attackerArmy, armyClass * defenderArmy);
bool clickDefender(map_node * node, armyClass * attackerArmy, armyClass * defenderArmy);
void cancelClick(mapSuperClass * map);
void doRetreat(mapSuperClass *map , map_node *node, armyClass *attkrs,armyClass * dfndrs);
void moveUnit(unitClass * unitToMove, mapSuperClass * map, int newX, int newY);
void exitDialog(SDL_Event event);
void reinforceDialog(SDL_Event event);
void moveTrain(map_node* node,int movement);
void moveTo(map_node* node,int movement);
int checkEdge(node_edge* edge, int pos);
int checkEdgeTrain(node_edge* edge, int pos);

