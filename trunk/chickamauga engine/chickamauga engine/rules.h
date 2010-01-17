#pragma once

enum playerSpecificity
{
	none,
	blue,
	gray
};

enum effect
{
	no,
	killed,
	exited,
	alive
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
	virtual  string returnRule() = 0;
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
	int numExitNodes;
	map_node ** enterNodes;
	map_node ** exitNodes;
	string returnRule();
};

struct unitKillRule : public masterRule
{
	string returnRule();
};

struct unitExitRule : public masterRule
{
	string returnRule();
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
};

struct areaControlRule : public masterRule
{
	int numNodes;
	map_node ** nodesToControl;
	string returnRule();
};

struct nodeControlRule : public masterRule
{
	int lastController;
	bool needUnitOn;
	map_node * nodeToControl;
	string returnRule();
};

//this rule will declare that any units of "playerSpecific''s
//that are not within 'howManySpacesToRoad' to a clear road
//at end of game will be killed and added to the other players
//score. this can work for both players simultaniously
struct roadExitRule : public masterRule
{
	int howManySpacesToRoad;
	string returnRule();
};

struct rules
{
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
	void coutRules();
	void deleteRules();
};




/*
sections of rules:

units already on map arrays

units to be reinforced and when arrays

victory conditions / how to get points

numerical rules (i.e. how many spaces a unit can move, battle calculator shit, etc)

*/