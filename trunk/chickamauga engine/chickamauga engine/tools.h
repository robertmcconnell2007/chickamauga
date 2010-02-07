#pragma once


int stringToInt(string str);
bool isUnits(map_node * node, armyClass * unionArmy, armyClass * confedArmy);
void setEnemyNodes(armyClass enemyArmy, mapSuperClass* map);
void checkUnitStacks(mapSuperClass* map, armyClass first, armyClass second);
void updatePowers();
