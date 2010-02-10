#include <fstream>
#include <string>
#include <sstream>
using namespace std;
#include "Game Data Handler.h"
#include "player.h"
#include "unitClass.h"
#include "mapSuperClass.h"
#include "rules.h"
#include "tools/interface tools.h"
#include "tools.h"

void rules::deleteRules()
{
	if(DPD)
		delete DPD;
	if(RCR)
		delete RCR;
	if(NCR)
		delete NCR;
	if(UKR)
		delete UKR;
	if(UER)
		delete UER;
	if(VPR)
		delete VPR;
	if(ACR)
		delete ACR;
	if(RER)
		delete RER;
}

rules::rules()
{
	numExitNodes = 0;
	numBRNodes = 0;
	numGRNodes = 0;
	numRules = numDependancies = 0;
	RCRrules = UKRrules = UERrules = 0;
	VPRrules = ACRrules = NCRrules = 0;
	RERrules = 0;
	DPD = NULL;
	RCR = NULL;
	NCR = NULL;
	UKR = NULL;
	UER = NULL;
	VPR = NULL;
	ACR = NULL;
	RER = NULL;
	rulesWindow.w = 400;
	rulesWindow.h = 500;
	rulesWindow.x = (IH::Instance()->screen->w / 2) - 150;
	rulesWindow.y = 50;
}

void rules::loadRules(string fileName)
{
	fstream infile;
	infile.open(fileName.c_str());
	if(!infile)
		return;
	string tester;
	bool nextLoopIsControl = false;
	int dump1 = 0, dump2 = 0, dump3 = 0;
	int totDepend, totRCR, totUKR, totUER, totVPR, totACR, totNCR, totRER;
	totDepend = totRCR = totUKR = totUER = totVPR = totACR = totNCR = totRER = 0;
	getline(infile, tester, '\n');
	calculator.loadOdds((char*)tester.c_str());
	
	infile >> numGameTurns;
	infile >> roughDefBonus;
	infile >> unitMovePoints;
	infile >> clearMovePenalty;
	infile >> forestMovePenalty;
	infile >> roughMovePenalty;
	infile >> forestroughMovePenalty;
	infile >> fordMovePenalty;
	infile >> roadCost;
	infile >> trailCost;
	infile >> dump1;
	for(int i = 0; i < dump1; ++i)
	{
		infile >> dump2;
		infile >> dump3;
		exitNodes.push_back(&IH::Instance()->map->getMap()[dump2][dump3]);
		IH::Instance()->map->getMap()[dump2][dump3].exit = true;
		numExitNodes++;
	}
	infile >> dump1;
	for(int i = 0; i < dump1; ++i)
	{
		infile >> dump2;
		infile >> dump3;
		blueEntry.push_back(&IH::Instance()->map->getMap()[dump2][dump3]);
		IH::Instance()->map->getMap()[dump2][dump3].reinforceBlue = true;
		numBRNodes++;
	}
	infile >> dump1;
	for(int i = 0; i < dump1; ++i)
	{
		infile >> dump2;
		infile >> dump3;
		grayEntry.push_back(&IH::Instance()->map->getMap()[dump2][dump3]);
		IH::Instance()->map->getMap()[dump2][dump3].reinforceGrey = true;
		numGRNodes++;
	}


	infile >> numRules;
	infile >> numDependancies;
	infile >> RCRrules;
	infile >> UKRrules;
	infile >> UERrules;
	infile >> VPRrules;
	infile >> ACRrules;
	infile >> NCRrules;
	infile >> RERrules;
	if(numDependancies)
		DPD = new dependancy[numDependancies];
	if(RCRrules)
		RCR = new roadControlRule[RCRrules];
	if(UKRrules)
		UKR = new unitKillRule[UKRrules];
	if(UERrules)
		UER = new unitExitRule[UERrules];
	if(VPRrules)
		VPR = new VIPRule[VPRrules];
	if(ACRrules)
		ACR = new areaControlRule[ACRrules];
	if(NCRrules)
		NCR	= new nodeControlRule[NCRrules];
	if(RERrules)
		RER = new roadExitRule[RERrules];

	infile.ignore(1);
	for(int i = 0; i < numRules; ++i)
	{
		infile.ignore(1);
		getline(infile, tester);
		if(tester == "RER")
		{
			if(nextLoopIsControl)
			{
				DPD[totDepend].controlRule = &RER[totRER];
				RER[totRER].controlRule = true;
				totDepend++;
				nextLoopIsControl = false;
			}
			int tempX, tempY;
			infile >> RER[totRER].playerSpecific;
			infile >> RER[totRER].howManySpacesToRoad;
			infile >> RER[totRER].howManyEntryPoints;
			RER[totRER].entryNodes = new map_node*[RER[totRER].howManyEntryPoints];
			for(int j = 0; j < RER[totRER].howManyEntryPoints; ++j)
			{
				infile >> tempX;
				infile >> tempY;
				RER[totRER].entryNodes[j] = &IH::Instance()->map->getMap()[tempX][tempY];
			}
			infile.ignore(1);
			getline(infile, tester, '\n');
			if(tester == "if")
			{
				DPD[totDepend].dependantRule = &RER[totRER];
				nextLoopIsControl = true;
			}
			totRER++;
		}
		if(tester == "UER")
		{
			if(nextLoopIsControl)
			{
				DPD[totDepend].controlRule = &UER[totUER];
				UER[totUER].controlRule = true;
				totDepend++;
				nextLoopIsControl = false;
			}
			infile >> UER[totUER].playerSpecific;
			infile >> UER[totUER].pointValue;
			infile.ignore(1);
			getline(infile, tester, '\n');
			if(tester == "if")
			{
				DPD[totDepend].dependantRule = &UER[totUER];
				nextLoopIsControl = true;
			}
			totUER++;
		}
		if(tester == "VPR")
		{
			if(nextLoopIsControl)
			{
				DPD[totDepend].controlRule = &VPR[totVPR];
				VPR[totVPR].controlRule = true;
				totDepend++;
				nextLoopIsControl = false;
			}
			infile >> VPR[totVPR].playerSpecific;
			infile >> VPR[totVPR].unitOwner;
			infile >> VPR[totVPR].requisiteEffect1;
			infile >> VPR[totVPR].requisiteEffect2;
			infile >> VPR[totVPR].requisiteEffect3;
			infile >> VPR[totVPR].pointValue;
			string temp;
			infile.ignore(1);
			getline(infile, temp, '\n');
			//ask the army for that unit and set the pointer to it
			VPR[totVPR].specialUnit = IH::Instance()->players[VPR[totVPR].unitOwner].playerArmy.findUnit(temp);			
			getline(infile, tester, '\n');
			if(tester == "if")
			{
				DPD[totDepend].dependantRule = &VPR[totVPR];
				nextLoopIsControl = true;
			}
			totVPR++;
		}
		if(tester == "ACR")
		{
			if(nextLoopIsControl)
			{
				DPD[totDepend].controlRule = &ACR[totACR];
				ACR[totACR].controlRule = true;
				totDepend++;
				nextLoopIsControl = false;
			}
			int tempX, tempY;
			infile >> ACR[totACR].numNodes;
			ACR[totACR].nodesToControl = new map_node*[ACR[totACR].numNodes];
			for(int k = 0; k < ACR[totACR].numNodes; ++k)
			{
				infile >> tempX;
				infile >> tempY;
				ACR[totACR].nodesToControl[k] = &IH::Instance()->returnMap()[tempX][tempY];
			}
			infile.ignore(1);
			getline(infile, tester, '\n');
			if(tester == "if")
			{
				DPD[totDepend].dependantRule = &ACR[totACR];
			}
			totACR++;
		}
		if(tester == "NCR")
		{
			if(nextLoopIsControl)
			{
				DPD[totDepend].controlRule = &NCR[totNCR];
				NCR[totNCR].controlRule = true;
				totDepend++;
				nextLoopIsControl = false;
			}
			int tempX, tempY;
			infile >> NCR[totNCR].playerSpecific;
			infile >> NCR[totNCR].pointValue;
			infile >> NCR[totNCR].lastController;
			infile >> (bool)NCR[totNCR].needUnitOn;
			infile >> tempX;
			infile >> tempY;
			NCR[totNCR].nodeToControl = &IH::Instance()->returnMap()[tempX][tempY];
			if(NCR[totNCR].lastController == blue)
			{
				NCR[totNCR].nodeToControl->control = true;
				NCR[totNCR].nodeToControl->controlBlue = true;
			}
			if(NCR[totNCR].lastController == gray)
			{
				NCR[totNCR].nodeToControl->control = true;
				NCR[totNCR].nodeToControl->controlBlue = false;
			}
			infile.ignore(1);
			getline(infile, tester, '\n');
			if(tester == "if")
			{
				DPD[totDepend].dependantRule = &NCR[totNCR];
				nextLoopIsControl = true;
			}
			totNCR++;
		}
		if(tester == "RCR")
		{
			if(nextLoopIsControl)
			{
				DPD[totDepend].controlRule = &RCR[totRCR];
				RCR[totRCR].controlRule = true;
				totDepend++;
				nextLoopIsControl = false;
			}
			int tempX, tempY;
			infile >> RCR[totRCR].playerSpecific;
			infile >> RCR[totRCR].numEnterNodes;
			infile >> RCR[totRCR].numExitNodes;
			RCR[totRCR].enterNodes = new map_node*[RCR[totRCR].numEnterNodes];
			RCR[totRCR].exitNodes  = new map_node*[RCR[totRCR].numExitNodes];
			for(int k = 0; k < RCR[totRCR].numEnterNodes; ++k)
			{
				infile >> tempX;
				infile >> tempY;
				RCR[totRCR].enterNodes[k] = &IH::Instance()->returnMap()[tempX][tempY];
			}
			for(int k = 0; k < RCR[totRCR].numExitNodes; ++k)
			{
				infile >> tempX;
				infile >> tempY;
				RCR[totRCR].exitNodes[k] = &IH::Instance()->returnMap()[tempX][tempY];
			}
			infile.ignore(1);
			getline(infile, tester, '\n');
			if(tester == "if")
			{
				DPD[totDepend].dependantRule = &RCR[totRCR];
				nextLoopIsControl = true;
			}
			totRCR++;
		}
		if(tester == "UKR")
		{
			if(nextLoopIsControl)
			{
				DPD[totDepend].controlRule = &UKR[totUKR];
				UKR[totUKR].controlRule = true;
				totDepend++;
				nextLoopIsControl = false;
			}
			infile >> UKR[totUKR].playerSpecific;
			infile >> UKR[totUKR].pointValue;
			infile.ignore(1);
			getline(infile, tester, '\n');
			if(tester == "if")
			{
				DPD[totDepend].dependantRule = &UKR[totUKR];
				nextLoopIsControl = true;
			}
			totUKR++;
		}
	}
//	printStrings("TESTING TESTING TESTING", rulesWindow, rulesDisplay, IH::Instance()->textColor, IH::Instance()->font1);
}

void rules::printRules(int page)
{
	ostringstream masterstring;
	masterstring << "Game Rules:\n\n";
	for(int i = 0; i < 	RCRrules; ++i)
	{
		if(!RCR[i].controlRule)
		{
			masterstring << RCR[i].returnRule() << "\n";
			for(int k = 0; k < numDependancies; ++k)
			{
				if(&RCR[i] == DPD[k].dependantRule)
				{
					masterstring << "And is dependant on this rule:\n" << DPD[k].controlRule->returnRule() << "\n";
				}
			}
		}
	}
	for(int i = 0; i <  UKRrules; ++i)
	{
		if(!UKR[i].controlRule)
		{
			masterstring << UKR[i].returnRule() << "\n";
			for(int k = 0; k < numDependancies; ++k)
			{
				if(&UKR[i] == DPD[k].dependantRule)
				{
					masterstring << "And is dependant on this rule:\n" << DPD[k].controlRule->returnRule() << "\n";
				}
			}
		}
	}
	for(int i = 0; i <  RERrules; ++i)
	{
		if(!RER[i].controlRule)
		{
			masterstring << RER[i].returnRule() << "\n";
			for(int k = 0; k < numDependancies; ++k)
			{
				if(&RER[i] == DPD[k].dependantRule)
				{
					masterstring << "And is dependant on this rule:\n" << DPD[k].controlRule->returnRule() << "\n";
				}
			}
		}
	}
	for(int i = 0; i < 	UERrules; ++i)
	{
		if(!UER[i].controlRule)
		{
			masterstring << UER[i].returnRule() << "\n";
			for(int k = 0; k < numDependancies; ++k)
			{
				if(&UER[i] == DPD[k].dependantRule)
				{
					masterstring << "And is dependant on this rule:\n" << DPD[k].controlRule->returnRule() << "\n";
				}
			}
		}
	}
	for(int i = 0; i < 	VPRrules; ++i)
	{
		if(!VPR[i].controlRule)
		{
			masterstring << VPR[i].returnRule() << "\n";
			for(int k = 0; k < numDependancies; ++k)
			{
				if(&VPR[i] == DPD[k].dependantRule)
				{
					masterstring << "And is dependant on this rule:\n" << DPD[k].controlRule->returnRule() << "\n";
				}
			}
		}
	}
	for(int i = 0; i < 	ACRrules; ++i)
	{
		if(!ACR[i].controlRule)
		{
			masterstring << ACR[i].returnRule() << "\n";
			for(int k = 0; k < numDependancies; ++k)
			{
				if(&ACR[i] == DPD[k].dependantRule)
				{
					masterstring << "And is dependant on this rule:\n" << DPD[k].controlRule->returnRule() << "\n";
				}
			}
		}
	}
	for(int i = 0; i < 	NCRrules; ++i)
	{
		if(!NCR[i].controlRule)
		{
			masterstring << NCR[i].returnRule() << "\n";
			for(int k = 0; k < numDependancies; ++k)
			{
				if(&NCR[i] == DPD[k].dependantRule)
				{
					masterstring << "And is dependant on this rule:\n" << DPD[k].controlRule->returnRule() << "\n";
				}
			}
		}
	}
	//29
	string piece = "", tempTotal = masterstring.str();
	//int place = 0, end = 0;
	//for(int i = 0; i < 29 && place != -1; ++i)
	//{
	//	place = tempTotal.find('\n', 0);
	//	end += place;
	//	if(place != -1)
	//		tempTotal = tempTotal.substr(place + 1, -1);
	//}
	int counter = 0, end = 0;
	for(int i = 0; i < page; ++i)
	{
		end = 0;
		counter = 0;
		while(counter < 29 && end < tempTotal.length())
		{
			if(tempTotal[end] == '\n')
				counter++;
			end++;
		}
		if(end == tempTotal.length())
		{
			IH::Instance()->maxPages = IH::Instance()->currentPage;
			end--;
		}
		piece = tempTotal.substr(0, end);
		tempTotal = tempTotal.substr(end + 1, -1);		
	}	
	printStrings(piece, rulesWindow, IH::Instance()->screen, IH::Instance()->textColor, IH::Instance()->font1);
}

void rules::coutRules()
{
	cout << "Game Rules:\n\n";
	for(int i = 0; i < 	RCRrules; ++i)
	{
		if(!RCR[i].controlRule)
		{
			cout << RCR[i].returnRule() << "\n";
			for(int k = 0; k < numDependancies; ++k)
			{
				if(&RCR[i] == DPD[k].dependantRule)
				{
					cout << "And is dependant on this rule:\n" << DPD[k].controlRule->returnRule() << "\n";
				}
			}
		}
	}
	for(int i = 0; i <  UKRrules; ++i)
	{
		if(!UKR[i].controlRule)
		{
			cout << UKR[i].returnRule() << "\n";
			for(int k = 0; k < numDependancies; ++k)
			{
				if(&UKR[i] == DPD[k].dependantRule)
				{
					cout << "And is dependant on this rule:\n" << DPD[k].controlRule->returnRule() << "\n";
				}
			}
		}
	}
	for(int i = 0; i <  RERrules; ++i)
	{
		if(!RER[i].controlRule)
		{
			cout << RER[i].returnRule() << "\n";
			for(int k = 0; k < numDependancies; ++k)
			{
				if(&RER[i] == DPD[k].dependantRule)
				{
					cout << "And is dependant on this rule:\n" << DPD[k].controlRule->returnRule() << "\n";
				}
			}
		}
	}
	for(int i = 0; i < 	UERrules; ++i)
	{
		if(!UER[i].controlRule)
		{
			cout << UER[i].returnRule() << "\n";
			for(int k = 0; k < numDependancies; ++k)
			{
				if(&UER[i] == DPD[k].dependantRule)
				{
					cout << "And is dependant on this rule:\n" << DPD[k].controlRule->returnRule() << "\n";
				}
			}
		}
	}
	for(int i = 0; i < 	VPRrules; ++i)
	{
		if(!VPR[i].controlRule)
		{
			cout << VPR[i].returnRule() << "\n";
			for(int k = 0; k < numDependancies; ++k)
			{
				if(&VPR[i] == DPD[k].dependantRule)
				{
					cout << "And is dependant on this rule:\n" << DPD[k].controlRule->returnRule() << "\n";
				}
			}
		}
	}
	for(int i = 0; i < 	ACRrules; ++i)
	{
		if(!ACR[i].controlRule)
		{
			cout << ACR[i].returnRule() << "\n";
			for(int k = 0; k < numDependancies; ++k)
			{
				if(&ACR[i] == DPD[k].dependantRule)
				{
					cout << "And is dependant on this rule:\n" << DPD[k].controlRule->returnRule() << "\n";
				}
			}
		}
	}
	for(int i = 0; i < 	NCRrules; ++i)
	{
		if(!NCR[i].controlRule)
		{
			cout << NCR[i].returnRule() << "\n";
			for(int k = 0; k < numDependancies; ++k)
			{
				if(&NCR[i] == DPD[k].dependantRule)
				{
					cout << "And is dependant on this rule:\n" << DPD[k].controlRule->returnRule() << "\n";
				}
			}
		}
	}
}


string roadControlRule::returnRule()
{
	ostringstream oss;
	oss << "This rule applies to ";
	switch(playerSpecific)
	{
	case none:
		oss << "all players.\n";
		break;
	case blue:
		oss << "the union player.\n";
		break;
	case gray:
		oss << "the confederate player.\n";
	}
	if(controlRule)
	{
		oss << "Player must control an unblocked road between start nodes:\n";
		for(int i = 0; i < numEnterNodes; ++i)
			oss << "X: " << enterNodes[i]->col << ", Y: " << enterNodes[i]->row << ", ";
		oss << "\nAnd these end nodes:\n";
		for(int i = 0; i < numExitNodes; ++i)
			oss << "X: " << exitNodes[i]->col << ", Y: " << exitNodes[i]->row << ", ";
	}
	else
	{
		oss << pointValue << " points will be awarded for control of an\nunblocked road between start nodes:\n";
		for(int i = 0; i < numEnterNodes; ++i)
			oss << "X: " << enterNodes[i]->col << ", Y: " << enterNodes[i]->row << ", ";
		oss << "\nAnd these end nodes:\n";
		for(int i = 0; i < numExitNodes; ++i)
			oss << "X: " << exitNodes[i]->col << ", Y: " << exitNodes[i]->row << ", ";
	}
	oss << "\n";
	return oss.str();
}

int roadControlRule::calculateRule(int player)
{
	bool canCalc = true;
	for(int i = 0; i < IH::Instance()->gameRules->numDependancies; ++i)
	{
		if(this == IH::Instance()->gameRules->DPD[i].dependantRule)
			canCalc = (bool)IH::Instance()->gameRules->DPD[i].controlRule->calculateRule(player);
	}
	if(!canCalc)
		return 0;
	bool foundViableRoad = false;
	for(int i = 0; i < numEnterNodes; ++i)
	{
		cancelClick(IH::Instance()->map);
		roadScore(enterNodes[i], IH::Instance()->players[player].playerArmy);
		for(int j = 0; j < numExitNodes; ++j)
		{
			if(exitNodes[j]->movement)
				foundViableRoad = true;
		}
	}
	cancelClick(IH::Instance()->map);
	if(controlRule)
		return (int)foundViableRoad;
	if(foundViableRoad)
		return pointValue;
	return 0;
}

string unitKillRule::returnRule()
{
	ostringstream oss;
	oss << "This rule applies to ";
	switch(playerSpecific)
	{
	case none:
		oss << "all players\n";
		break;
	case blue:
		oss << "the union player\n";
		break;
	case gray:
		oss << "the confederate player\n";
	}
	oss << "It confers " << pointValue << " points per enemy strength point killed.\n";
	return oss.str();
}

int unitKillRule::calculateRule(int player)
{
	bool canCalc = true;
	for(int i = 0; i < IH::Instance()->gameRules->numDependancies; ++i)
	{
		if(this == IH::Instance()->gameRules->DPD[i].dependantRule)
			canCalc = (bool)IH::Instance()->gameRules->DPD[i].controlRule->calculateRule(player);
	}
	if(canCalc && ((player == 0 && (playerSpecific == none || playerSpecific == blue)) ||
		(player == 1 && (playerSpecific == none || playerSpecific == gray))))
	{
		return (IH::Instance()->players[!player].playerArmy.deadStrength() * pointValue);
	}
	return 0;
}

string unitExitRule::returnRule()
{
	ostringstream oss;
	oss << "This rule applies to ";
	switch(playerSpecific)
	{
	case none:
		oss << "all players\n";
		break;
	case blue:
		oss << "the union player\n";
		break;
	case gray:
		oss << "the confederate player\n";
	}
	oss << pointValue << " points will be awarded for each strength point\nexited from the map.\n";
	return oss.str();
}

int unitExitRule::calculateRule(int player)
{
	bool canCalc = true;
	for(int i = 0; i < IH::Instance()->gameRules->numDependancies; ++i)
	{
		if(this == IH::Instance()->gameRules->DPD[i].dependantRule)
			canCalc = (bool)IH::Instance()->gameRules->DPD[i].controlRule->calculateRule(player);
	}
	if(canCalc && ((player == 0 && (playerSpecific == none || playerSpecific == blue)) ||
		(player == 1 && (playerSpecific == none || playerSpecific == gray))))
	{
		return IH::Instance()->players[player].playerArmy.exitedStrength() * pointValue;
	}
	return 0;
}

string VIPRule::returnRule()
{
	ostringstream oss;
	if(specialUnit == NULL)
	{
		oss << "This VIP rule couldn't find its unit, it is not applicable.\n";
		return oss.str();
	}
	oss << "This rule applies to ";
	switch(playerSpecific)
	{
	case none:
		oss << "all players\n";
		break;
	case blue:
		oss << "the union player\n";
		break;
	case gray:
		oss << "the confederate player\n";
	}
	oss << pointValue << " points will be awarded if the unit " << specialUnit->getName() << " is:\n";
	switch(requisiteEffect1)
	{
	case killed:
		oss << "Killed";
		break;
	case exited:
		oss << "Exited";
		break;
	case alive:
		oss << "Alive";
		break;
	}
	switch(requisiteEffect2)
	{
	case killed:
		oss << " and/or killed";
		break;
	case exited:
		oss << " and/or exited";
		break;
	case alive:
		oss << " and/or alive";
		break;
	}
	switch(requisiteEffect3)
	{
	case killed:
		oss << " and/or killed";
		break;
	case exited:
		oss << " and/or exited";
		break;
	case alive:
		oss << " and/or alive";
		break;
	}
	oss << " at end of match.\n";
	return oss.str();
}
//	no,
//	killed,
//	exited,
//	alive,
//	reinforcing
int VIPRule::calculateRule(int player)
{
	bool canCalc = true;
	for(int i = 0; i < IH::Instance()->gameRules->numDependancies; ++i)
	{
		if(this == IH::Instance()->gameRules->DPD[i].dependantRule)
			canCalc = (bool)IH::Instance()->gameRules->DPD[i].controlRule->calculateRule(player);
	}
	if(canCalc && ((player == blue && (playerSpecific == none || playerSpecific == blue)) ||
		(player == gray && (playerSpecific == none || playerSpecific == gray))))
	{
		if( (requisiteEffect1 != no || requisiteEffect1 == IH::Instance()->players[player].playerArmy.checkStatus(specialUnit)) ||
			(requisiteEffect2 != no || requisiteEffect2 == IH::Instance()->players[player].playerArmy.checkStatus(specialUnit)) ||
			(requisiteEffect3 != no || requisiteEffect3 == IH::Instance()->players[player].playerArmy.checkStatus(specialUnit)))
			return pointValue;
	}
	return 0;
}

string areaControlRule::returnRule()
{
	ostringstream oss;
	oss << "This rule applies to ";
	switch(playerSpecific)
	{
	case none:
		oss << "all players\n";
		break;
	case blue:
		oss << "the union player\n";
		break;
	case gray:
		oss << "the confederate player\n";
	}
	oss << pointValue << " points will be awarded if each of the following\n";
	oss << "nodes have one of your units on it:\n";
	for(int i = 0; i < numNodes; ++i)
		oss << "X: " << nodesToControl[i]->col << ", Y: " << nodesToControl[i]->row << "; ";
	oss << "\n";
	return oss.str();
}

int areaControlRule::calculateRule(int player)
{
	bool canCalc = true;
	for(int i = 0; i < IH::Instance()->gameRules->numDependancies; ++i)
	{
		if(this == IH::Instance()->gameRules->DPD[i].dependantRule)
			canCalc = (bool)IH::Instance()->gameRules->DPD[i].controlRule->calculateRule(player);
	}
	if(!canCalc)
		return 0;
	bool holdAll = true;
	for(int i = 0; i < numNodes; ++i)
	{
		if(!IH::Instance()->players[player].playerArmy.haveLiveUnitAt(nodesToControl[i]->col, nodesToControl[i]->row))
			holdAll = false;
	}
	if(holdAll)
		return pointValue;
	return 0;
}

string nodeControlRule::returnRule()
{
	ostringstream oss;
	oss << "This rule applies to ";
	switch(playerSpecific)
	{
	case none:
		oss << "all players\n";
		break;
	case blue:
		oss << "the union player\n";
		break;
	case gray:
		oss << "the confederate player\n";
	}
	oss << pointValue << " points will be awarded if space\n";
	oss << nodeToControl->col << ", " << nodeToControl->row << " is controlled at end of game.\n";
	if(needUnitOn)
		oss << "A unit is required to be on this space at end of game.\n";
	else
		oss << "A unit is not required to be on this space at end of game.\n";
	return oss.str();
}

int nodeControlRule::calculateRule(int player)
{
	bool canCalc = true;
	for(int i = 0; i < IH::Instance()->gameRules->numDependancies; ++i)
	{
		if(this == IH::Instance()->gameRules->DPD[i].dependantRule)
			canCalc = (bool)IH::Instance()->gameRules->DPD[i].controlRule->calculateRule(player);
	}
	if(canCalc && ((player == blue &&  nodeToControl->controlBlue && (playerSpecific == blue || playerSpecific == none)) ||
		(player == gray && !nodeToControl->controlBlue && (playerSpecific == gray || playerSpecific == none))))
		return pointValue;
	return 0;
}

string roadExitRule::returnRule()
{
	ostringstream oss;
	oss << "This rule applies to ";
	switch(playerSpecific)
	{
	case none:
		oss << "all players\n";
		break;
	case blue:
		oss << "the union player\n";
		break;
	case gray:
		oss << "the confederate player\n";
	}
	oss << "If units are not within ";
	oss << howManySpacesToRoad;
	oss << " spaces of an unblocked road, \nthey will be killed at match end.\n";
	return oss.str();
}

int roadExitRule::calculateRule(int player)
{
	bool canCalc = true;
	for(int i = 0; i < IH::Instance()->gameRules->numDependancies; ++i)
	{
		if(this == IH::Instance()->gameRules->DPD[i].dependantRule)
			canCalc = (bool)IH::Instance()->gameRules->DPD[i].controlRule->calculateRule(player);
	}
	if(!((player == blue && (playerSpecific == blue || playerSpecific == none)) || (player == gray && (playerSpecific == gray || playerSpecific == none))))
		return 0;
	bool foundAPath;
	bool endNodeSelected;
	for(int i = 0; i < IH::Instance()->players[player].playerArmy.currentSize; ++i)
	{
		foundAPath = false;
		for(int j = 0; j < howManyEntryPoints; ++j)
		{
			endNodeSelected = false;
			cancelClick(IH::Instance()->map);
			roadScore(entryNodes[j], IH::Instance()->players[!player].playerArmy);
			for(int k = 0; k < IH::Instance()->gameRules->numExitNodes; ++k)
			{
				if(IH::Instance()->gameRules->exitNodes[k]->movement == 1)
					endNodeSelected = true;
			}
			if(endNodeSelected && closeToRoad(&IH::Instance()->map->getMap()[IH::Instance()->players[player].playerArmy.armyArray[i]->getY() - 1][IH::Instance()->players[player].playerArmy.armyArray[i]->getX() - 1], howManySpacesToRoad))
				foundAPath = true;
		}
		if(!foundAPath)
		{
			cout << "I had to kill unit " << IH::Instance()->players[player].playerArmy.armyArray[i]->getName() << " cause he wasn't close enough to a road.\n";
			IH::Instance()->players[player].playerArmy.moveUnit(IH::Instance()->players[player].playerArmy.armyArray[i], MUFField, MUTKilled);
			i--;
		}
	}
	return 0;
}




void rules::calcAllRules()
{
	int points[2];
	points[0] = points[1] = 0;
	for(int j = 0; j < RERrules; ++j)
	{
		if(RERrules && !RER[j].controlRule)
			points[0] += RER[j].calculateRule(0);
	}
	for(int j = 0; j < RERrules; ++j)
	{
		if(RERrules && !RER[j].controlRule)
			points[1] += RER[j].calculateRule(1);
	}
	for(int i = 0; i < 2; ++i)
	{
		for(int j = 0; j < RCRrules; ++j)
		{
			if(RCRrules && !RCR[j].controlRule)
				points[i] += RCR[j].calculateRule(i);
		}
		for(int j = 0; j < UKRrules; ++j)
		{
			if(UKRrules && !UKR[j].controlRule)
				points[i] += UKR[j].calculateRule(i);
		}
		for(int j = 0; j < UERrules; ++j)
		{
			if(UERrules && !UER[j].controlRule)
				points[i] += UER[j].calculateRule(i);
		}
		for(int j = 0; j < VPRrules; ++j)
		{
			if(VPRrules && !VPR[j].controlRule)
				points[i] += VPR[j].calculateRule(i);
		}
		for(int j = 0; j < ACRrules; ++j)
		{
			if(ACRrules && !ACR[j].controlRule)
				points[i] += ACR[j].calculateRule(i);
		}
		for(int j = 0; j < NCRrules; ++j)
		{
			if(NCRrules && !NCR[j].controlRule)
				points[i] += NCR[j].calculateRule(i);
		}
	}
	IH::Instance()->players[0].pointsEarned = points[0];
	IH::Instance()->players[1].pointsEarned = points[1];
	//loop through each rule, check if it's a dependant rule and if it's applicable
	//to player one, accumulate totals
	//do the same for player 2
}