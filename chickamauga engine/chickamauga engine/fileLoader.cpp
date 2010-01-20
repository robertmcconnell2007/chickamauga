#include "fileLoader.h"
#include <iostream>

void FileInfo::loadFileInfo(string filename)
{
	std::cout << "Loading file names\n";
	ifstream infile;
	infile.open((const char*)filename.c_str(),ifstream::in);
	if(!infile.is_open())
	{
		return;
	}
	string name;
	getline(infile,name,'#');
	while(!infile.eof())
	{
		if(name == "title")
		{
			getline(infile,title);
		}
		else if(name == "logo")
		{
			getline(infile,logo);
		}
		else if(name == "rules")
		{
			getline(infile,rules);
		}
		else if(name == "ttf")
		{
			getline(infile,ttf);
		}
		else if(name == "map")
		{
			getline(infile,map);
		}
		else if(name == "mapnode")
		{
			getline(infile,mapnode);
		}
		else if(name == "maproads")
		{
			getline(infile,maproads);
		}
		else if(name == "mapcreeks")
		{
			getline(infile,mapcreeks);
		}
		else if(name == "mapstatus")
		{
			getline(infile,mapstatus);
		}
		else if(name == "maptowns")
		{
			getline(infile,maptowns);
		}
		else if(name == "army1")
		{
			getline(infile,army1);
		}
		else if(name == "army1colors")
		{
			getline(infile,army1colors);
		}
		else if(name == "army2")
		{
			getline(infile,army2);
		}
		else if(name == "army2colors")
		{
			getline(infile,army2colors);
		}
		else if(name == "u5050tiles")
		{
			getline(infile, u5050tiles);
		}
		//infile.ignore();
		getline(infile,name,'#');
	}
}