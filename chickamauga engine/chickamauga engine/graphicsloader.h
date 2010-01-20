#ifndef GRAPHICS_LOADER_H
#define GRAPHICS_LOADER_H
#include <iostream>
#include "SDL.h"
#include "SDL_ttf.h"
using namespace std;

SDL_Surface *load_my_image( string filename );
void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination );
void drawATile(SDL_Surface * toDraw, SDL_Rect * drawRect, int index, SDL_Surface * destSurf, int posX, int posY);
int getRandomNum();
void writeText(SDL_Surface * ascii, SDL_Rect * asciiSize, SDL_Surface * screen, string output, int positionX, int positionY);
void writeToConsole(SDL_Surface * ascii, SDL_Rect * asciiSize, SDL_Surface * screen, string output, SDL_Rect * console);
void printStrings(string text, SDL_Rect destRect, SDL_Surface *screen, SDL_Color text_color, TTF_Font *font);

#endif