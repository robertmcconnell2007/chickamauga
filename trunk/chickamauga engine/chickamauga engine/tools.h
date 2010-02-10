#pragma once


int stringToInt(string str);
bool isUnits(map_node * node, armyClass * unionArmy, armyClass * confedArmy);
void setEnemyNodes(armyClass enemyArmy, mapSuperClass* map);
void checkUnitStacks(mapSuperClass* map, armyClass first, armyClass second);
void updatePowers();
void roadScore(map_node* node, armyClass enemyUnits);
bool closeToRoad(map_node* node, int distanceLeft);