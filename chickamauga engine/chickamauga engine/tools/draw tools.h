#pragma once
#include "SDL.h"
#include "../Game Data Handler.h"
#include "../tools.h"

void drawGui(map_node * node, armyClass * unionArmy, armyClass * confedArmy, unitClass *currentUnits[2], SDL_Surface * screen);
void showCombat();
bool showRetreater(mapSuperClass *map,armyClass * attkrs,armyClass *dfndr);