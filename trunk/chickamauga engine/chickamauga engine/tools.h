#pragma once


int stringToInt(string str);
bool isUnits(map_node * node, armyClass * unionArmy, armyClass * confedArmy);
int checkEdge(node_edge* edge, int pos);
void moveTo(map_node* node,int movement);
void setEnemyNodes(armyClass enemyArmy, mapSuperClass* map);
void checkUnitStacks(mapSuperClass* map, armyClass first, armyClass second);
