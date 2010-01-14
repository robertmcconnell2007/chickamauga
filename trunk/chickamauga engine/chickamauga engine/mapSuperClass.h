#pragma once

#include <fstream>
#include <string>
using namespace std;
#include "SDL.h"
#include "GraphicsLoader.h"


struct node_edge;

struct map_node
{
	node_edge *nodeEdges[6];
	int col, row;
	int type;			//0-4
	bool selected;
	bool enemy;
	bool town;			//8
	bool control;		//16
	bool controlBlue;	//32
	void newNode(int x, int y)
	{
		col = x;
		row = y;
		type = 1;
		enemy = false;
		selected = false;
		town = false;
		control = false;
		controlBlue = false;
		for(int j = 0; j < 6; ++j)
		{
			nodeEdges[j] = NULL;
		}
	}
};
//controlblue control town type type type
//32 16 8 4 2 1

//32 16 8 4 2 1
struct node_edge
{
	map_node * upperNode;
	map_node * lowerNode;
	bool passable;		//32
	bool road_edge;		//16
	bool ford_edge;		//8
	bool trail_edge;	//4
	bool creek_edge;	//2
	bool bridge_edge;	//1
	void newNode()
	{
		passable = true;
		road_edge = false;
		ford_edge = false;
		trail_edge = false;
		creek_edge = false;
		bridge_edge = false;
	}
};


class mapSuperClass
{
private:
	int width;
	int height;
	string mapName;
	bool showEnemyControl;
	bool mapEdit;
	map_node** mapPointer;
	SDL_Surface * nodeTypes;
	SDL_Surface * roadsTrails;
	SDL_Surface * creeksBridgesFords;
	SDL_Surface * statusTiles;
	SDL_Surface * townNstratPoint;
	SDL_Rect hexSize;
	//array of units
	void createBlankMap(int width, int height); //for the specific use of this class
	void loadData()
	{
		nodeTypes = load_my_image("mapData/mapImages/hexSheet.bmp");
		roadsTrails = load_my_image("mapData/mapImages/roadsNtrails.bmp");
		creeksBridgesFords = load_my_image("mapData/mapImages/creeksNbridgesNfords.bmp");
		statusTiles = load_my_image("mapData/mapImages/statusTiles.bmp");
		townNstratPoint = load_my_image("mapData/mapImages/townNstratPoint.bmp");
		SDL_SetColorKey(nodeTypes, SDL_SRCCOLORKEY, 0xff00ff);
		SDL_SetColorKey(roadsTrails, SDL_SRCCOLORKEY, 0xff00ff);
		SDL_SetColorKey(creeksBridgesFords, SDL_SRCCOLORKEY, 0xff00ff);
		SDL_SetColorKey(statusTiles, SDL_SRCCOLORKEY, 0xff00ff);
		SDL_SetColorKey(townNstratPoint, SDL_SRCCOLORKEY, 0xff00ff);
		showEnemyControl = false;
		hexSize.x = hexSize.y = 0;
		hexSize.h = 44;
		hexSize.w = 50;
	}
public:
	map_node** getMap() { return mapPointer; }
	mapSuperClass(const char* nameOfInputFile); //fixed map generation
	bool mapSuperClassIni(const char* nameOfInputFile); //fixed map generation
	//void mapDraw(SDL_Surface screen,int offsetX, int offsetY); //draws map and all units on the map
	void selectHex(int nodeX, int nodeY); //highlights the selected hex, if a unit is present, will show the available movement and enemy control area
	//bool unitMotion(); //will return false if units aren't in the process of moving to show movement rather than instantaneous
	void drawMap(int screenShiftx, int screenShifty, SDL_Surface * screen);
	mapSuperClass(int sizeX, int sizeY); //random map generation
	mapSuperClass() {mapPointer = NULL; width = height = 0;}
	void exportMap();
	bool mapSuperClassIni(int sizeX, int sizeY);
	void setNodeType(int type, int nodeX, int nodeY);
	void setConnecterType(int type, int node1X, int node1Y, int node2X, int node2Y);
	~mapSuperClass();
	void deleteMap();
};


