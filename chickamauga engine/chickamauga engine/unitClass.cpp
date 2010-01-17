#include "unitClass.h"

unitClass::unitClass()
{
	hasMovedThisTurn = false;
	//power=3;
	unitRect.x=unitRect.y=0;
	unitRect.h=unitRect.w=25;
	//position.x=position.y=2;
	
}
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
	int px,py,pow;
	unitClass *temp;
	ifstream infile;
	infile.open(fileName);
	infile>>size;
	armyArray=new unitClass*[size];
	for(int i=0; i<size; i++)
	{
		temp = new unitClass;
		infile>>py;
		infile>>px;
		infile>>pow;
		temp->setPosition(px,py);
		temp->setPower(pow);
		armyArray[i] = temp;
	}
	infile.close();
	armyColors=load_my_image(armyColorFile);

}
void armyClass::drawArmy(int xShift,int yShift,int mapWidth,int mapHeight, SDL_Surface* a_screen)
{
	for(int i=0; i<size; i++)
	{
		armyArray[i]->drawUnit(xShift,yShift,mapWidth,mapHeight,a_screen,armyColors);
	}
}

void armyClass::resetMoves()
{
	for(int i = 0; i < size; ++i)
	{
		armyArray[i]->resetMove();
	}
}