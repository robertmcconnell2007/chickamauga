//using namespace std;

enum terrainTypes
{
	clear = 0,
	forest = 1,
	rough = 2,
	roughForest = 3,
	river = 4
};
#include <string>
#include <windows.h>
#include "SDL.h"
#include "GraphicsLoader.h"
#include "mapSuperClass.h"
#include "unitClass.h"

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;
int SCREEN_BPP = 32;
int screenShiftx = 0;
int screenShifty = 0;


//void drawMap(map_node ** map, int width, int height, SDL_Surface * tile, SDL_Rect size, SDL_Surface * screen);

int main(int argc, char ** argv)
{
	if(SDL_Init( SDL_INIT_EVERYTHING) == -1)
	{
		return 1;
	}
	int xMove = 0;
	int yMove = 0;
	SCREEN_WIDTH = GetSystemMetrics(SM_CXSCREEN);
	SCREEN_HEIGHT = GetSystemMetrics(SM_CYSCREEN);

	SDL_Event event;
	SDL_Surface * screen;
	SDL_Surface * Legend;
	bool mouseDown = false;
	bool edgeTool = false;
	bool brushTool = false;
	bool firstEdge = false;
	int actualX = 0;
	int actualY = 0;
	int firstX = 0;
	int firstY = 0;
	int firstCoordX = 0;
	int firstCoordY = 0;
	int brushType = 0;
	int clear = 0;
	int trees = 1;
	int rough = 2;
	int roughTrees = 3;
	int river = 4;
	int town = 8;
	int pointUnion = 48;
	int pointConfed = 16;
	int edgeType = 0;
	int roadEdge = 48;
	int fordEdge = 40;
	int trailEdge = 36;
	int creekEdge = 34;
	int bridgeEdge = 33;

	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_FULLSCREEN | SDL_HWSURFACE);//

	mapSuperClass MapClass("mapData/mapData/customMapData.txt");
	//mapSuperClass MapClass("mapData/mapData/chickamaugaMapData.txt");
	//mapSuperClass MapClass(25,28);

	armyClass unionArmy("unionArmyUnits.txt","unionArmy.bmp");
	armyClass rebelArmy("rebelArmyUnits.txt","rebelArmy.bmp");
	

	//Legend = load_my_image("Tile Legend.bmp");
	SDL_Rect screenSize;
	screenSize.x = screenSize.y = 0;
	screenSize.x = SCREEN_WIDTH;
	screenSize.y = SCREEN_HEIGHT;

	bool running = true;
	bool legendUp = false;
	while(running)
	{
		screenShiftx += xMove*5;
		screenShifty += yMove*5;
		screenSize.x = screenSize.y = 0;
		SDL_FillRect(screen,&screenSize,0x000000);
		MapClass.drawMap(screenShiftx, screenShifty, screen);
		unionArmy.drawArmy(screenShiftx,screenShifty,MapClass.width,MapClass.height,screen);
		rebelArmy.drawArmy(screenShiftx,screenShifty,MapClass.width,MapClass.height,screen);
		if(legendUp)
		{
			apply_surface((SCREEN_WIDTH/2 - 440),(SCREEN_HEIGHT/2 - 120),Legend,screen);
		}
		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT)
				running = false;
			if(event.type == SDL_MOUSEBUTTONDOWN)// && event.button.button == 0)
			{
				mouseDown = true;
				firstX = actualX;
				firstY = actualY;
			}
			if(event.type == SDL_MOUSEBUTTONUP)
			{
				mouseDown = false;
				if(firstX == actualX && firstY == actualY)
				{
					if(actualX >= 0 && actualX < MapClass.width && actualY >= 0 && actualY < MapClass.height && !edgeTool && !brushTool)
					{
						if(firstClick(&MapClass, &MapClass.getMap()[actualX][actualY], unionArmy, rebelArmy))
						{
							//first click was good
						}
					}
				}
			}
			if(event.type == SDL_MOUSEMOTION)
			{
				actualX = event.motion.x - screenShiftx;
				actualY = event.motion.y - screenShifty;
				actualX = (actualX-6)/38;
				if(((actualX+1)%2) == 1)
				{
					actualY = (actualY)/44;
				}
				else
					actualY = (actualY-22)/44;
				if(event.motion.x < 15)
				{
					xMove = 1;
				}
				else if(event.motion.x >= SCREEN_WIDTH - 15)
				{
					xMove = -1;
				}
				else
				{
					xMove = 0;
				}
				if(event.motion.y < 15)
				{
					yMove = 1;
				}
				else if(event.motion.y >= SCREEN_HEIGHT - 15)
				{
					yMove = -1;
				}
				else
				{
					yMove = 0;
				}
			}
			if(event.type == SDL_KEYDOWN)
			{
				switch(event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					running = false;
					break;
				case SDLK_SPACE:
					//legendUp = !legendUp;
					brushTool = false;
					edgeTool = false;
					break;
				case SDLK_1:
					edgeTool = false;
					brushTool = true;
					brushType = clear;
					break;
				case SDLK_2:
					edgeTool = false;
					brushTool = true;
					brushType = trees;
					break;
				case SDLK_3:
					edgeTool = false;
					brushTool = true;
					brushType = rough;
					break;
				case SDLK_4:
					edgeTool = false;
					brushTool = true;
					brushType = roughTrees;
					break;
				case SDLK_5:
					edgeTool = false;
					brushTool = true;
					brushType = river;
					break;
				case SDLK_6:
					edgeTool = false;
					brushTool = true;
					brushType = town;
					break;
				case SDLK_7:
					edgeTool = false;
					brushTool = true;
					brushType = pointUnion;
					break;
				case SDLK_8:
					edgeTool = false;
					brushTool = true;
					brushType = pointConfed;
					break;
				case SDLK_q:
					edgeTool = true;
					brushTool = false;
					edgeType = roadEdge;
					break;
				case SDLK_w:
					edgeTool = true;
					brushTool = false;
					edgeType = fordEdge;
					break;
				case SDLK_e:
					edgeTool = true;
					brushTool = false;
					edgeType = trailEdge;
					break;
				case SDLK_r:
					edgeTool = true;
					brushTool = false;
					edgeType = creekEdge;
					break;
				case SDLK_t:
					edgeTool = true;
					brushTool = false;
					edgeType = bridgeEdge;
					break;
				case SDLK_RETURN:
					brushTool = false;
					edgeTool = false;
					MapClass.exportMap();
					break;
				}
			}
		}
		if(actualX >= 0 && actualX < MapClass.width && actualY >= 0 && actualY < MapClass.height)
		{
			MapClass.hilightHex(actualX,actualY);
		}
		if(mouseDown)
		{
			if(brushType > 4 || edgeTool)
			{
				mouseDown = false;
			}
			if(actualX >= 0 && actualX < MapClass.width && actualY >= 0 && actualY < MapClass.height && !edgeTool && brushTool)
			{
				MapClass.setNodeType(brushType,actualX,actualY);
			}
			if(actualX >= 0 && actualX < MapClass.width && actualY >= 0 && actualY < MapClass.height && edgeTool && !brushTool)
			{
				if(!firstEdge)
				{
					firstEdge = true;
					firstCoordX = actualX;
					firstCoordY = actualY;
				}
				else
				{
					firstEdge = false;
					if(MapClass.setConnecterType(edgeType,firstCoordX,firstCoordY,actualX,actualY))
					{
						//firstEdge = true;
						//firstCoordX = actualX;
						//firstCoordY = actualY;
					}
				}
			}
		}
	
		
		if(SDL_Flip(screen) == -1)
			return 1;
	}
	return 0;
}

