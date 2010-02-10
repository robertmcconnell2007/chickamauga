#pragma once
#include <fstream>
#include <string>
using namespace std;

struct FileInfo
{
	string title;
	string logo;
	string rules;
	string ttf;
	string map;
	string mapnode;
	string maproads;
	string mapcreeks;
	string mapstatus;
	string maptowns;
	string army1;
	string army1colors;
	string army2;
	string army2colors;
	string u5050tiles;
	string menu;
	string menuOptions;
	string turnCounter;
	string blackBox;
	void loadFileInfo(string filename);
};