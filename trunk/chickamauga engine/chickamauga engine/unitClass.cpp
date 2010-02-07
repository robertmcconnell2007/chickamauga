#include "unitClass.h"
#include <string>
unitClass::unitClass()
{
	movedThisTurn = false;
	alreadyCompletedCombat = false;
	inCombatPrep = false;
	needsCombat = false;
	//power=3;
	unitRect.x=unitRect.y=0;
	unitRect.h=unitRect.w=25;
	typeRect.x=typeRect.y=0;
	typeRect.h=typeRect.w=8;
	position.x=position.y= -1;	
}

unitClass::~unitClass()
{
	printf("fml\n");
}

bool unitClass::hasMoved()  {return movedThisTurn;}
void unitClass::setMoved()  {movedThisTurn = true;}
void unitClass::resetMove() {movedThisTurn = false; needsCombat = false; inCombatPrep = false; alreadyCompletedCombat = false;}
void unitClass::setPosition(int nX,int nY)
{
	position.x=nX;
	position.y=nY;
}
SDL_Rect unitClass::getSize()
{
	return unitRect;
}
void unitClass::drawUnit(int screenShiftx,int screenShifty,int mapWidth,int mapHeight,SDL_Surface *a_screen,SDL_Surface *armyColors,SDL_Surface *unitType)
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
					switch(type)
					{
						case 0:
						{
							drawATile(armyColors,&unitRect,8,a_screen, (i*50) -(i*12) + screenShiftx+12, k*44 + screenShifty+10);
							break;
						}
					case 1:
						{
							drawATile(unitType,&typeRect,0,a_screen, (i*50) - (i*12) +screenShiftx+21, k*44+screenShifty+25);	
							break;
						}
					case 2:
						{
							drawATile(unitType,&typeRect,1,a_screen, (i*50) - (i*12) +screenShiftx+22, k*44+screenShifty+25);	
							break;
						}
					case 3:
						{
							drawATile(unitType,&typeRect,2,a_screen, (i*50) - (i*12) +screenShiftx+21, k*44+screenShifty+25);	
							break;
						}
					}

				}
				else
				{
					drawATile(armyColors,&unitRect,power-1,a_screen, (i * 50) - (i*12) + screenShiftx+12, k * 44 + screenShifty+30);
					switch(type)
					{
						
					case 0:
						{
							drawATile(armyColors,&unitRect,8,a_screen, (i*50) -(i*12) + screenShiftx+12, k*44 + screenShifty+30);
							break;
						}
					case 1:
						{
							drawATile(unitType,&typeRect,0,a_screen, (i*50) - (i*12) +screenShiftx+21, k*44+screenShifty+45);
							break;
						}
					case 2:
						{
							drawATile(unitType,&typeRect,1,a_screen, (i*50) - (i*12) +screenShiftx+22, k*44+screenShifty+45);
							break;
						}
					case 3:
						{
							drawATile(unitType,&typeRect,2,a_screen, (i*50) - (i*12) +screenShiftx+21, k*44+screenShifty+45);
							break;
						}
						
					}
				}
			}
		}
	}
}

void unitClass::resetCombat()
{
	inCombatPrep = false;
}
/*
MUTKilled,
	MUTExited,
	MUTField,
	MUFField,
	MUFReinforce,
*/
bool armyClass::moveUnit(unitClass* unit, int moveFrom, int moveTo)
{
	bool found = false;
	int i = 0;
	switch(moveFrom)
	{
	case MUFField:
		for(i = 0; i < currentSize && !found; ++i)
		{
			if(armyArray[i] == unit)
			{
				armyArray[i] = NULL;
				found = true;
			}
		}
		if(found)
		{
			i--;
			currentSize--;
			for(i; i < currentSize; ++i)
				armyArray[i] = armyArray[i+1];
		}
		else
			return false;
		break;
	case MUFReinforce:
		for(i = 0; i < reinforcementSize && !found; ++i)
		{
			if(reinforcements[i] == unit)
			{
				reinforcements[i] = NULL;
				found = true;
			}
		}
		if(found)
		{
			i--;
			reinforcementSize--;
			for(i; i < reinforcementSize; ++i)
				reinforcements[i] = reinforcements[i+1];
		}
		else
			return false;
		break;
	}
	i = 0;
	switch(moveTo)
	{
	case MUTKilled:
		deadUnits[deadSize] = unit;
		deadSize++;
		break;
	case MUTField:
		armyArray[currentSize] = unit;
		currentSize++;
		break;
	case MUTExited:
		exitedUnits[exitedSize] = unit;
		exitedSize++;
		break;
	}
	return true;
}

void armyClass::resetAllCombat()
{
	for(int i = 0; i < currentSize; ++i)
		armyArray[i]->resetCombat();
}

void armyClass::loadArmy(char * fileName, char * armyColorFile)
{
	int px,py,pow,turn,type;
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
		infile>>type;
		getline(infile,name,'#');
		temp->setPosition(px,py);
		temp->setPower(pow);
		temp->setReinforceTurn(-1);
		temp->setName(name);
		temp->retreat=false;
		temp->setType(type);
		armyArray[i] = temp;
	}
	infile>>reinforcementSize;
	reinforcements=new unitClass*[reinforcementSize];
	for(int i=0; i<reinforcementSize; i++)
	{
		temp= new unitClass;
		infile>>pow;
		infile>>turn;
		infile>>type;
		getline(infile,name,'#');
		temp->setPosition(-1,-1);
		temp->setPower(pow);
		temp->setReinforceTurn(turn);
		temp->setName(name);
		temp->retreat=false;
		temp->setType(type);
		reinforcements[i]=temp;
	}
	exitedUnits=new unitClass*[fullSize];
	deadUnits=new unitClass*[fullSize];
	infile.close();
	armyColors=load_my_image(armyColorFile);
	unitType=load_my_image("Init Data/unitTypes.bmp");
	SDL_SetColorKey(unitType, SDL_SRCCOLORKEY, 0xff00ff);

}
void armyClass::drawArmy(int xShift,int yShift,int mapWidth,int mapHeight, SDL_Surface* a_screen)
{
	for(int i=0; i<currentSize; i++)
	{
		armyArray[i]->drawUnit(xShift,yShift,mapWidth,mapHeight,a_screen,armyColors,unitType);
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

bool armyClass::haveLiveUnitAt(int xcord, int ycord)
{
	for(int i = 0; i < currentSize; ++i)
	{
		if(armyArray[i]->getX() == xcord && armyArray[i]->getY() == ycord)
			return true;
	}
	return false;
}

int armyClass::currentStrength()
{
	int calc = 0;
	for(int i = 0; i < currentSize; ++i)
	{
		calc += armyArray[i]->getPower();
	}
	return calc;
}

int armyClass::exitedStrength()
{
	int calc = 0;
	for(int i = 0; i < exitedSize; ++i)
	{
		calc += exitedUnits[i]->getPower();
	}
	return calc;
}

int armyClass::deadStrength()
{
	int calc = 0;
	for(int i = 0; i < deadSize; ++i)
	{
		calc += deadUnits[i]->getPower();
	}
	return calc;
}

int armyClass::reinforcingStrength()
{
	int calc = 0;
	for(int i = 0; i < reinforcementSize; ++i)
	{
		calc += reinforcements[i]->getPower();
	}
	return calc;
}
