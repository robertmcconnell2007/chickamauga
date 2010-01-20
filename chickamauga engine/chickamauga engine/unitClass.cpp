#include "unitClass.h"
#include <string>
unitClass::unitClass()
{
	movedThisTurn = false;
	//power=3;
	unitRect.x=unitRect.y=0;
	unitRect.h=unitRect.w=25;
	//position.x=position.y=2;
	
}

unitClass::~unitClass()
{
	printf("fml\n");
}

bool unitClass::hasMoved()  {return movedThisTurn;}
void unitClass::setMoved()  {movedThisTurn = true;}
void unitClass::resetMove() {movedThisTurn = false;}
void unitClass::setPosition(int nX,int nY)
{
	position.x=nX;
	position.y=nY;
}
SDL_Rect unitClass::getSize()
{
	return unitRect;
}
void unitClass::drawUnit(int screenShiftx,int screenShifty,int mapWidth,int mapHeight,SDL_Surface *a_screen,SDL_Surface *armyColors)
{
	for(int i = 0; i < mapWidth; ++i)
	{
		for(int k = 0; k < mapHeight; ++k)
		{
			if(position.x-1==k&&position.y-1==i)
			{
				if(i%2==0)
				{
					drawATile(armyColors,&unitRect,power-1,a_screen, (i * 50) - (i*12) + screenShiftx+12, k * 44 + screenShifty+10);
				}
				else
				{
					drawATile(armyColors,&unitRect,power-1,a_screen, (i * 50) - (i*12) + screenShiftx+12, k * 44 + screenShifty+30);
				}
			}
		}
	}
}

void armyClass::loadArmy(char * fileName, char * armyColorFile)
{
	int px,py,pow,turn;
	string name;
	unitClass *temp;
	ifstream infile;
	infile.open(fileName);
	infile>>fullSize;
	infile>>currentSize;
	armyArray=new unitClass*[fullSize];
	for(int i=0; i<currentSize; i++)
	{
		temp = new unitClass;
		infile>>py;
		infile>>px;
		infile>>pow;
		getline(infile,name,'#');
		temp->setPosition(px,py);
		temp->setPower(pow);
		temp->setReinforceTurn(-1);
		temp->setName(name);
		armyArray[i] = temp;
	}
	infile>>reinforcementSize;
	reinforcements=new unitClass*[reinforcementSize];
	for(int i=0; i<reinforcementSize; i++)
	{
		temp= new unitClass;
		infile>>pow;
		infile>>turn;
		getline(infile,name,'#');
		temp->setPosition(-1,-1);
		temp->setPower(pow);
		temp->setReinforceTurn(turn);
		temp->setName(name);
		reinforcements[i]=temp;
	}
	exitedUnits=new unitClass*[fullSize];
	deadUnits=new unitClass*[fullSize];
	infile.close();
	armyColors=load_my_image(armyColorFile);

}
void armyClass::drawArmy(int xShift,int yShift,int mapWidth,int mapHeight, SDL_Surface* a_screen)
{
	for(int i=0; i<currentSize; i++)
	{
		armyArray[i]->drawUnit(xShift,yShift,mapWidth,mapHeight,a_screen,armyColors);
	}
}

void armyClass::resetMoves()
{
	for(int i = 0; i < currentSize; ++i)
	{
		armyArray[i]->resetMove();
	}
}

unitClass * armyClass::findUnit(string lookUpName)
{
	for(int i = 0; i < currentSize; ++i)
	{
		if(armyArray[i]->getName() == lookUpName)
			return armyArray[i];
	}
	for(int i = 0; i < reinforcementSize; ++i)
	{
		if(reinforcements[i]->getName() == lookUpName)
			return reinforcements[i];
	}
	for(int i = 0; i < exitedSize; ++i)
	{
		if(exitedUnits[i]->getName() == lookUpName)
			return exitedUnits[i];
	}
	for(int i = 0; i < deadSize; ++i)
	{
		if(deadUnits[i]->getName() == lookUpName)
			return deadUnits[i];
	}
	return NULL;
}

int armyClass::checkStatus(unitClass* unit)
{
	for(int i = 0; i < currentSize; ++i)
	{
		if(armyArray[i] == unit)
			return alive;
	}
	for(int i = 0; i < reinforcementSize; ++i)
	{
		if(reinforcements[i] == unit)
			return reinforcing;
	}
	for(int i = 0; i < deadSize; ++i)
	{
		if(deadUnits[i] == unit)
			return killed;
	}
	for(int i = 0; i < exitedSize; ++i)
	{
		if(exitedUnits[i] == unit)
			return exited;
	}
	return -1;
}