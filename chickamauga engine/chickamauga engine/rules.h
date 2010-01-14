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

struct dependancy
{
	int dependantRuleType;
	int dependantRule;
	int checkRuleType;
	int checkRule;
};

struct roadControlRule
{
	int playerSpecific;
	map_node ** enterNodes;
	map_node ** exitNodes;
	int pointValue;
};

struct unitKillRule
{
	int playerSpecific;
	int pointPerStrKill;
};

struct unitExitRule
{
	int playerSpecific;
	int pointPerStrExited;
};

struct VIPRule
{
	int playerSpecific;
	int requisiteEffect1;
	int requisiteEffect2;
	int requisiteEffect3;
	int pointValue;
	//unit * to the unit
};

struct areaControlRule
{
	int playerSpecific;
	int pointValue;
	int numNodes;
	map_node ** nodesToControl;
};

struct nodeControlRule
{
	int playerSpecific;
	int pointValue;
	int lastController;
	bool needUnitOn;
	map_node * nodeToControl;
};

//this rule will declare that any units of "playerSpecific''s
//that are not within 'howManySpacesToRoad' to a clear road
//at end of game will be killed and added to the other players
//score. this can work for both players simultaniously
struct roadExitRule
{
	int playerSpecific;
	int howManySpacesToRoad;
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
	void deleteRules();
};




/*
sections of rules:

units already on map arrays

units to be reinforced and when arrays

victory conditions / how to get points

numerical rules (i.e. how many spaces a unit can move, battle calculator shit, etc)

*/