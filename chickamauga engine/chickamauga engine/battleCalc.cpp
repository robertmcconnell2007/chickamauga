#include "battleCalc.h"
#include "graphicsloader.h"

battleCalculator::battleCalculator()
{
	ifstream file;
	file.open("Init Data/odds.txt");
	for(int i=0; i<6; i++)
	{
		for(int k=0; k<10; k++)
		{
			file>>oddsArray[i][k];	
		}	
	}
	file.close();
}
battleCalculator::battleCalculator(char* fileName)
{
	loadOdds(fileName);
}

void battleCalculator::loadOdds(char* fileName)
{
	ifstream file;
	file.open(fileName);
	for(int i=0; i<6; i++)
	{
		for(int k=0; k<10; k++)
		{
			file>>oddsArray[i][k];	
		}	
	}
	file.close();
}

int battleCalculator::doBattle(int attackerPower,int defenderPower)
{
	srand(time(0));
	bool attackerAdvantage;
	int odds;
	int roll;
	if(defenderPower == 0 || attackerPower == 0)
		return 5;
	if(attackerPower==defenderPower)
	{
		odds=1;
		attackerAdvantage=true;
	}
	else if(attackerPower>defenderPower)
	{
		attackerAdvantage=true;
		odds=attackerPower/defenderPower;
	}
	else if(attackerPower<defenderPower)
	{
		attackerAdvantage=false;
		odds=defenderPower/attackerPower;
	}
	roll=getRandomNum()%6;

	if(odds > 5)
		odds = 5;
	if(odds < -3)
		odds = -3;
	
	if(attackerAdvantage)
	{
		return oddsArray[roll][odds+3];//odds is offset
	}
	else
	{
		return oddsArray[roll][5-odds];
	}
}