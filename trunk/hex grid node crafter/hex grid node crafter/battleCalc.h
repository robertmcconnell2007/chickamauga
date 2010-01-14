#pragma once

#include <iostream>
#include <fstream>
#include <ctime>


using namespace std;

enum battleResults
{
	attackRetreat,//0
	attackElim,//1
	defendRetreat,//2
	defendElim,//3
	exchange//4
};

class battleCalculator
{
private:
	int oddsArray[6][10];
public:
	battleCalculator();
	battleCalculator(char*);
	int doBattle(int,int);
};

battleCalculator::battleCalculator()
{
	ifstream file;
	file.open("odds.txt");
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
	roll=rand()%6+1;

	if(attackerAdvantage)
	{
		return oddsArray[roll][odds+3];//odds is offset
	}
	else
	{
		return oddsArray[roll][5-odds];
	}
}
