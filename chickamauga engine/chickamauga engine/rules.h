#pragma once
#include "battleCalc.h"
#include "mapSuperClass.h"
#include <vector>
#include "SDL.h"

enum playerSpecificity
{
	blue,
	gray,
	none
};

enum effect
{
	no,
	killed,
	exited,
	alive,
	reinforcing
};

enum ruleTypes
{
	RCRr,
	UKRr,
	UERr,
	VPRr,
	ACRr,
	NCRr,
	RERr,
	NON
};

struct masterRule
{
	bool controlRule;
	int playerSpecific;
	int pointValue;
	virtual string returnRule() = 0;
	virtual int calculateRule(int player) = 0;
	masterRule() {controlRule = false;}
};

struct dependancy
{
	masterRule *dependantRule;
	masterRule *controlRule;
};

struct roadControlRule : public masterRule
{
	int numEnterNodes;
	map_node ** enterNodes;
	string returnRule();
	int calculateRule(int player);
};

struct unitKillRule : public masterRule
{
	string returnRule();
	int calculateRule(int player);
};

struct unitExitRule : public masterRule
{
	string returnRule();
	int calculateRule(int player);
};

struct VIPRule : public masterRule
{
	int unitOwner;
	int requisiteEffect1;
	int requisiteEffect2;
	int requisiteEffect3;
	//unit * to the unit
	unitClass * specialUnit;
	string returnRule();
	int calculateRule(int player);
};

struct areaControlRule : public masterRule
{
	int numNodes;
	map_node ** nodesToControl;
	string returnRule();
	int calculateRule(int player);
};

struct nodeControlRule : public masterRule
{
	int lastController;
	bool needUnitOn;
	map_node * nodeToControl;
	string returnRule();
	int calculateRule(int player);
};

//this rule will declare that any units of "playerSpecific''s
//that are not within 'howManySpacesToRoad' to a clear road
//at end of game will be killed and added to the other players
//score. this can work for both players simultaniously
struct roadExitRule : public masterRule
{
	int howManySpacesToRoad;
	int howManyEntryPoints;
	map_node ** entryNodes;
	string returnRule();
	int calculateRule(int player);
};

struct rules
{
	SDL_Rect      rulesWindow;
	int numExitNodes;
	int numBRNodes;//blue reinforcement nodes
	int numGRNodes;//gray reinforcement nodes
	int numGameTurns;
	int roughDefBonus;
	int unitMovePoints;
	int clearMovePenalty;
	int forestMovePenalty;
	int roughMovePenalty;
	int forestroughMovePenalty;
	int fordMovePenalty;
	int roadCost;
	int trailCost;
	battleCalculator calculator;
	vector<map_node*> exitNodes;
	vector<map_node*> blueEntry;
	vector<map_node*> grayEntry;

	int numRules;
	int numDependancies;

	int RCRrules;
	int UKRrules;
	int UERrules;
	int VPRrules;
	int ACRrules;
	int NCRrules;
	int RERrules;

	dependancy      * DPD;
	roadControlRule * RCR;
	unitKillRule    * UKR;
	unitExitRule    * UER;
	VIPRule         * VPR;
	areaControlRule * ACR;
	nodeControlRule * NCR;
	roadExitRule    * RER;

	//functionality
	rules();
	void loadRules(string fileName);
	void printRules(int page);
	void coutRules();
	void calcAllRules();
	void deleteRules();
};




/*
sections of rules:

units already on map arrays

units to be reinforced and when arrays

victory conditions / how to get points

numerical rules (i.e. how many spaces a unit can move, battle calculator shit, etc)

*/