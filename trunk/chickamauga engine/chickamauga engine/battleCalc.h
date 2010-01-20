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
	void loadOdds(char*);
	int doBattle(int atkpwr, int defpwr);
};


