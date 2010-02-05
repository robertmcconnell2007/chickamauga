#pragma once
#include "SDL.h"
#include "../Game Data Handler.h"
#include "../tools.h"

bool getUnitsAroundNode(map_node * node, int path, armyClass * army, unitClass * &unit1, unitClass * &unit2);
bool getUnitsOnNode(map_node * node, armyClass * army, unitClass * &unit1, unitClass * &unit2);
bool canFightOther(map_node * node, armyClass * army);
bool alreadyInAttkDef(unitClass * unit);
void networkRetreat(string unitName);
void resetCombat();
bool foundDistantCombat(map_node * currentNode, int distance, armyClass * otherArmy);