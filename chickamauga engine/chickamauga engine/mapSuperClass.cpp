#include "graphicsloader.h"
#include "mapSuperClass.h"

void mapSuperClass::exportMap()
{
	int typeOut;
	int nodeOut;
	ofstream outfile;
	outfile.open("mapData/mapData/customMapData.txt",fstream::out);
	outfile << "Custom Map#" << width << " " << height << " ";
	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
			typeOut = 0;
			typeOut = mapPointer[j][i].type;
			if(mapPointer[j][i].town)
			{
				typeOut += 8;
			}
			if(mapPointer[j][i].control)
			{
				typeOut += 16;
			}
			if(mapPointer[j][i].controlBlue)
			{
				typeOut += 32;
			}
			outfile << typeOut << " ";
			for(int k = 0; k < 3; k++)
			{
				nodeOut = 0;
				if(mapPointer[j][i].nodeEdges[k] != NULL)
				{
					if(mapPointer[j][i].nodeEdges[k]->passable)
						nodeOut += 32;
					if(mapPointer[j][i].nodeEdges[k]->road_edge)
						nodeOut += 16;
					if(mapPointer[j][i].nodeEdges[k]->ford_edge)
						nodeOut += 8;
					if(mapPointer[j][i].nodeEdges[k]->trail_edge)
						nodeOut += 4;
					if(mapPointer[j][i].nodeEdges[k]->creek_edge)
						nodeOut += 2;
					if(mapPointer[j][i].nodeEdges[k]->bridge_edge)
						nodeOut += 1;
				}
				outfile << nodeOut << " ";
			}
		}
	}
	outfile.close();
}
mapSuperClass::~mapSuperClass()
{
	deleteMap();
}

void mapSuperClass::deleteMap()
{
	for(int i = 0; i < width; i++)
	{
		delete [] mapPointer[i];
	}
	delete [] mapPointer;
}

void mapSuperClass::selectHex(int nodeX, int nodeY)
{
	for(int i = 0; i < height; i++)
		for(int j = 0; j < width; j++)
			if(mapPointer[j][i].selected)
				mapPointer[j][i].selected = false;
	mapPointer[nodeX][nodeY].selected = !mapPointer[nodeX][nodeY].selected;
}
void mapSuperClass::setNodeType(int type, int nodeX, int nodeY)
{
	if(mapEdit)
	{
		if(type <= 4)
			mapPointer[nodeX][nodeY].type = type;
		if(type == 8)
			mapPointer[nodeX][nodeY].town = !mapPointer[nodeX][nodeY].town;
		if(type == 16)
		{
			if(mapPointer[nodeX][nodeY].control == false)
			{
				mapPointer[nodeX][nodeY].control = true;
			}
			else
				mapPointer[nodeX][nodeY].control = false;
			mapPointer[nodeX][nodeY].controlBlue = false;
		}
		if(type == 48)
		{
			if(mapPointer[nodeX][nodeY].control == false)
			{
				mapPointer[nodeX][nodeY].control = true;
				mapPointer[nodeX][nodeY].controlBlue = true;
			}
			else
			{
				mapPointer[nodeX][nodeY].control = false;
				mapPointer[nodeX][nodeY].controlBlue = false;
			}
		}
	}
}
void mapSuperClass::setConnecterType(int type, int node1X, int node1Y, int node2X, int node2Y)
{
	if(mapEdit)
	{
		int connecter = 9;
		for(int i = 0; i < 6; i++)
		{
			if(mapPointer[node1X][node1Y].nodeEdges[i] != NULL)
				if(mapPointer[node1X][node1Y].nodeEdges[i]->upperNode == &mapPointer[node2X][node2Y] || mapPointer[node1X][node1Y].nodeEdges[i]->lowerNode == &mapPointer[node2X][node2Y])
					connecter = i;
			if(connecter != 9)
			{
				i = 6;
			}
		}
		if(connecter != 9)
		{
			if((type&32) == 32)
				mapPointer[node1X][node1Y].nodeEdges[connecter]->passable = true;
			if((type&16) == 16)
				mapPointer[node1X][node1Y].nodeEdges[connecter]->road_edge = !mapPointer[node1X][node1Y].nodeEdges[connecter]->road_edge;
			if((type&8) == 8)
				mapPointer[node1X][node1Y].nodeEdges[connecter]->ford_edge = !mapPointer[node1X][node1Y].nodeEdges[connecter]->ford_edge;
			if((type&4) == 4)
				mapPointer[node1X][node1Y].nodeEdges[connecter]->trail_edge = !mapPointer[node1X][node1Y].nodeEdges[connecter]->trail_edge;
			if((type&2) == 2)
				mapPointer[node1X][node1Y].nodeEdges[connecter]->creek_edge = !mapPointer[node1X][node1Y].nodeEdges[connecter]->creek_edge;
			if((type&1) == 1)
				mapPointer[node1X][node1Y].nodeEdges[connecter]->bridge_edge = !mapPointer[node1X][node1Y].nodeEdges[connecter]->bridge_edge;
		}
	}
}
mapSuperClass::mapSuperClass(int sizeX, int sizeY)
{
	mapSuperClassIni(sizeX,sizeY);
}
bool mapSuperClass::mapSuperClassIni(int sizeX, int sizeY)
{
	loadData();
	width = sizeX;
	height = sizeY;
	createBlankMap(sizeX,sizeY);
	for(int i = 0; i < sizeY; i++)
	{
		for(int j = 0; j < sizeX; j++)
		{
			mapPointer[j][i].type = 0;
		}
	}
	mapEdit = true;
	return true;
}
mapSuperClass::mapSuperClass(const char* nameOfInputFile)
{
	mapSuperClassIni(nameOfInputFile);
}
bool mapSuperClass::mapSuperClassIni(const char* nameOfInputFile)
{
	loadData();
	mapEdit = false;
	ifstream infile;
	infile.open(nameOfInputFile);
	if(!infile.is_open())
	{
		return false;
	}
	getline(infile,mapName,'#');
	infile >> width >> height;
	infile.ignore();
	createBlankMap(width,height);
	int nodeData = 0;
	while(!infile.eof())
	{
		for(int j = 0; j < height; j++)
		{
			for(int i = 0; i < width; i++)
			{
				mapPointer[i][j].col = i+1;
				mapPointer[i][j].row = j+1;
				infile >> nodeData;
				mapPointer[i][j].type = nodeData&7;
				if((nodeData&8) == 8)
					mapPointer[i][j].town = true;
				if((nodeData&16) == 16)
					mapPointer[i][j].control = true;
				if((nodeData&32) == 32)
					mapPointer[i][j].controlBlue = true;
				else
					mapPointer[i][j].controlBlue = false;
				for(int k = 0; k < 3; k++)
				{
					infile >> nodeData;
					if(mapPointer[i][j].nodeEdges[k] != NULL)
					{
						if((nodeData&32) == 32)
							mapPointer[i][j].nodeEdges[k]->passable = true;
						if((nodeData&16) == 16)
							mapPointer[i][j].nodeEdges[k]->road_edge = true;
						if((nodeData&8) == 8)
							mapPointer[i][j].nodeEdges[k]->ford_edge = true;
						if((nodeData&4) == 4)
							mapPointer[i][j].nodeEdges[k]->trail_edge = true;
						if((nodeData&2) == 2)
							mapPointer[i][j].nodeEdges[k]->creek_edge = true;
						if((nodeData&1) == 1)
							mapPointer[i][j].nodeEdges[k]->bridge_edge = true;
					}
				}
			}
		}
		infile.ignore();
	}
	infile.close();
	return true;
}

void mapSuperClass::createBlankMap(int width, int height)
{
	mapPointer = new map_node*[width];
	for(int i = 0; i < width; ++i)
		mapPointer[i] = new map_node[height];

	for(int i = 0; i < width; ++i)
	{
		for(int k = 0; k < height; ++k)
		{
			mapPointer[i][k].newNode(i,k);
		}
	}

	node_edge * newEdge;


	for(int k = 0; k < height; ++k)
	{
		for(int i = 0; i < width; ++i)
		{
			if(i % 2 == 0)
			{
				if(k > 0) //mapPointer[i][k - 1] exists					
				{
					newEdge = new node_edge;
					newEdge->newNode();
					mapPointer[i][k - 1].nodeEdges[4] = newEdge;
					mapPointer[i][k].nodeEdges[1] = newEdge;
					newEdge->upperNode = &mapPointer[i][k - 1];
					newEdge->lowerNode = &mapPointer[i][k];
				}
				if(i < width - 1 && k > 0) //mapPointer[i + 1][k - 1] exists
				{
					newEdge = new node_edge;
					newEdge->newNode();
					mapPointer[i + 1][k - 1].nodeEdges[5] = newEdge;
					mapPointer[i][k].nodeEdges[2] = newEdge;
					newEdge->upperNode = &mapPointer[i + 1][k -1];
					newEdge->lowerNode = &mapPointer[i][k];
				}
				if(i > 0 && k > 0) //mapPointer[i - 1][k - 1] exists
				{
					newEdge = new node_edge;
					newEdge->newNode();
					mapPointer[i - 1][k - 1].nodeEdges[3] = newEdge;
					mapPointer[i][k].nodeEdges[0] = newEdge;
					newEdge->upperNode = &mapPointer[i - 1][k - 1];
					newEdge->lowerNode = &mapPointer[i][k];
				}
			}
			else
			{
				if(k > 0) //mapPointer[i][k - 1] exists
				{
					newEdge = new node_edge;
					newEdge->newNode();
					mapPointer[i][k - 1].nodeEdges[4] = newEdge;
					mapPointer[i][k].nodeEdges[1] = newEdge;
					newEdge->upperNode = &mapPointer[i][k - 1];
					newEdge->lowerNode = &mapPointer[i][k];
				}
				if(i < width - 1)// mapPointer[i + 1][k] exists
				{
					newEdge = new node_edge;
					newEdge->newNode();
					mapPointer[i + 1][k].nodeEdges[5] = newEdge;
					mapPointer[i][k].nodeEdges[2] = newEdge;
					newEdge->upperNode = &mapPointer[i + 1][k];
					newEdge->lowerNode = &mapPointer[i][k];
				}
				if(i > 0)// mapPointer[i - 1][k] exists
				{
					newEdge = new node_edge;
					newEdge->newNode();
					mapPointer[i - 1][k].nodeEdges[3] = newEdge;
					mapPointer[i][k].nodeEdges[0] = newEdge;
					newEdge->upperNode = &mapPointer[i - 1][k];
					newEdge->lowerNode = &mapPointer[i][k];
				}
			}
		}
	}
}
void mapSuperClass::drawMap(int screenShiftx, int screenShifty, SDL_Surface * screen)
{
	for(int i = 0; i < width; ++i)
	{
		for(int k = 0; k < height; ++k)
		{
			if(i % 2 == 1)
			{
				drawATile(nodeTypes, &hexSize, mapPointer[i][k].type, screen, (i * 50) - (i*12) + screenShiftx, (k * 44) + 21 + screenShifty);
				if(mapPointer[i][k].town)
				{
					drawATile(townNstratPoint, &hexSize, 0, screen, (i * 50) - (i*12) + screenShiftx, (k * 44) + 21 + screenShifty);
				}
				if(mapPointer[i][k].control)
				{
					if(mapPointer[i][k].controlBlue)
						drawATile(townNstratPoint, &hexSize, 1, screen, (i * 50) - (i*12) + screenShiftx, (k * 44) + 21 + screenShifty);
					else
						drawATile(townNstratPoint, &hexSize, 2, screen, (i * 50) - (i*12) + screenShiftx, (k * 44) + 21 + screenShifty);
				}
				for(int j = 0; j < 6; j++)
				{
					if(mapPointer[i][k].nodeEdges[j] != NULL)
					{
						if(mapPointer[i][k].nodeEdges[j]->bridge_edge)
						{
							drawATile(creeksBridgesFords, &hexSize, 6+j, screen, (i * 50) - (i*12) + screenShiftx, (k * 44) + 21 + screenShifty);
						}
						else if(mapPointer[i][k].nodeEdges[j]->ford_edge)
						{
							drawATile(creeksBridgesFords, &hexSize, 12+j, screen, (i * 50) - (i*12) + screenShiftx, (k * 44) + 21 + screenShifty);
						}
						else if(mapPointer[i][k].nodeEdges[j]->creek_edge)
						{
							drawATile(creeksBridgesFords, &hexSize, j, screen, (i * 50) - (i*12) + screenShiftx, (k * 44) + 21 + screenShifty);
						}
						if(mapPointer[i][k].nodeEdges[j]->road_edge)
						{
							drawATile(roadsTrails, &hexSize, j, screen, (i * 50) - (i*12) + screenShiftx, (k * 44) + 21 + screenShifty);
						}
						else if(mapPointer[i][k].nodeEdges[j]->trail_edge)
						{
							drawATile(roadsTrails, &hexSize, 6+j, screen, (i * 50) - (i*12) + screenShiftx, (k * 44) + 21 + screenShifty);
						}
					}
				}
				if(mapPointer[i][k].selected)
				{
					drawATile(statusTiles, &hexSize, 0, screen, (i * 50) - (i*12) + screenShiftx, (k * 44) + 21 + screenShifty);
				}
				if(mapPointer[i][k].enemy && (mapPointer[i][k].selected || showEnemyControl))
				{
					drawATile(statusTiles, &hexSize, 1, screen, (i * 50) - (i*12) + screenShiftx, (k * 44) + 21 + screenShifty);
				}
			}
			else
			{
				drawATile(nodeTypes, &hexSize, mapPointer[i][k].type, screen, (i * 50) - (i*12) + screenShiftx, k * 44 + screenShifty);
				if(mapPointer[i][k].town)
				{
					drawATile(townNstratPoint, &hexSize, 0, screen, (i * 50) - (i*12) + screenShiftx, k * 44 + screenShifty);
				}
				if(mapPointer[i][k].control)
				{
					if(mapPointer[i][k].controlBlue)
						drawATile(townNstratPoint, &hexSize, 1, screen, (i * 50) - (i*12) + screenShiftx, k * 44 + screenShifty);
					else
						drawATile(townNstratPoint, &hexSize, 2, screen, (i * 50) - (i*12) + screenShiftx, k * 44 + screenShifty);
				}
				for(int j = 0; j < 6; j++)
				{
					if(mapPointer[i][k].nodeEdges[j] != NULL)
					{
						if(mapPointer[i][k].nodeEdges[j]->bridge_edge)
						{
							drawATile(creeksBridgesFords, &hexSize, 6+j, screen, (i * 50) - (i*12) + screenShiftx, k * 44 + screenShifty);
						}
						else if(mapPointer[i][k].nodeEdges[j]->ford_edge)
						{
							drawATile(creeksBridgesFords, &hexSize, 12+j, screen, (i * 50) - (i*12) + screenShiftx, k * 44 + screenShifty);
						}
						else if(mapPointer[i][k].nodeEdges[j]->creek_edge)
						{
							drawATile(creeksBridgesFords, &hexSize, j, screen, (i * 50) - (i*12) + screenShiftx, k * 44 + screenShifty);
						}
						if(mapPointer[i][k].nodeEdges[j]->road_edge)
						{
							drawATile(roadsTrails, &hexSize, j, screen, (i * 50) - (i*12) + screenShiftx, k * 44 + screenShifty);
						}
						else if(mapPointer[i][k].nodeEdges[j]->trail_edge)
						{
							drawATile(roadsTrails, &hexSize, 6+j, screen, (i * 50) - (i*12) + screenShiftx, k * 44 + screenShifty);
						}
					}
				}
				if(mapPointer[i][k].selected)
				{
					drawATile(statusTiles, &hexSize, 0, screen, (i * 50) - (i*12) + screenShiftx, k * 44 + screenShifty);
				}
				if(mapPointer[i][k].enemy && (mapPointer[i][k].selected || showEnemyControl))
				{
					drawATile(statusTiles, &hexSize, 1, screen, (i * 50) - (i*12) + screenShiftx, k * 44 + screenShifty);
				}
			}
		}
	}
}
