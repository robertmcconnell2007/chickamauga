#include <fstream>
#include <string>
using namespace std;
#include "Game Data Handler.h"
#include "mapSuperClass.h"
#include "rules.h"

void rules::deleteRules()
{
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
	numRules = 0;
	RCRrules = UKRrules = UERrules = 0;
	VPRrules = ACRrules = NCRrules = 0;
	RERrules = 0;
	RCR = NULL;
	NCR = NULL;
	UKR = NULL;
	UER = NULL;
	VPR = NULL;
	ACR = NULL;
	RER = NULL;
}

void rules::loadRules(string fileName)
{
	fstream infile;
	infile.open(fileName.c_str());
	if(!infile)
		return;
	string tester;
	bool nextLoopIsControl = false;
	int dump1 = 0, dump2 = 0;
	int totRules, totDepend, totRCR, totUKR, totUER, totVPR, totACR, totNCR, totRER;
	totRules = totDepend = totRCR = totUKR = totUER = totVPR = totACR = totNCR = totRER = 0;
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

	
	for(int i = 0; i < totRules; ++i)
	{
		getline(infile, tester, '\n');
		if(tester == "RER")
		{
			if(nextLoopIsControl)
			{
				DPD[totDepend].checkRuleType = 6;
				DPD[totDepend].checkRule = totRER;
				totDepend++;
				nextLoopIsControl = false;
			}
			infile >> RER[totRER].playerSpecific;
			infile >> RER[totRER].howManySpacesToRoad;
			getline(infile, tester, '\n');
			if(tester == "if")
			{
				DPD[totDepend].dependantRule = totRER;
				DPD[totDepend].dependantRuleType = 6;
				nextLoopIsControl = true;
			}
			totRER++;
		}
		if(tester == "UER")
		{
			if(nextLoopIsControl)
			{
				DPD[totDepend].checkRuleType = 2;
				DPD[totDepend].checkRule = totUER;
				totDepend++;
				nextLoopIsControl = false;
			}
			infile >> UER[totUER].playerSpecific;
			infile >> UER[totUER].pointPerStrExited;
			getline(infile, tester, '\n');
			if(tester == "if")
			{
				DPD[totDepend].dependantRule = totUER;
				DPD[totDepend].dependantRuleType = 2;
				nextLoopIsControl = true;
			}
			totUER++;
		}
		if(tester == "VPR")
		{
			if(nextLoopIsControl)
			{
				DPD[totDepend].checkRuleType = 3;
				DPD[totDepend].checkRule = totVPR;
				totDepend++;
				nextLoopIsControl = false;
			}
			infile >> VPR[totVPR].playerSpecific;
			infile >> VPR[totVPR].requisiteEffect1;
			infile >> VPR[totVPR].requisiteEffect2;
			infile >> VPR[totVPR].requisiteEffect3;
			infile >> VPR[totVPR].pointValue;
			infile >> dump1;
			//ask the army for that unit and set the pointer to it
			getline(infile, tester, '\n');
			if(tester == "if")
			{
				DPD[totDepend].dependantRule = totVPR;
				DPD[totDepend].dependantRuleType = 3;
				nextLoopIsControl = true;
			}
			totVPR++;
		}
		if(tester == "ACR")
		{
			if(nextLoopIsControl)
			{
				DPD[totDepend].checkRuleType = 4;
				DPD[totDepend].checkRule = totACR;
				totDepend++;
				nextLoopIsControl = false;
			}
			getline(infile, tester, '\n');
			if(tester == "if")
			{
				DPD[totDepend].dependantRule = totACR;
				DPD[totDepend].dependantRuleType = 4;
				nextLoopIsControl = true;
			}
			totACR++;
		}
		if(tester == "NCR")
		{
			if(nextLoopIsControl)
			{
				DPD[totDepend].checkRuleType = 5;
				DPD[totDepend].checkRule = totNCR;
				totDepend++;
				nextLoopIsControl = false;
			}
			infile >> NCR[totNCR].playerSpecific;
			infile >> NCR[totNCR].pointValue;
			infile >> dump1;
			getline(infile, tester, '\n');
			if(tester == "if")
			{
				DPD[totDepend].dependantRule = totNCR;
				DPD[totDepend].dependantRuleType = 5;
				nextLoopIsControl = true;
			}
			totNCR++;
		}
		if(tester == "RCR")
		{
			if(nextLoopIsControl)
			{
				DPD[totDepend].checkRuleType = 0;
				DPD[totDepend].checkRule = totRCR;
				totDepend++;
				nextLoopIsControl = false;
			}

			getline(infile, tester, '\n');
			if(tester == "if")
			{
				DPD[totDepend].dependantRule = totRCR;
				DPD[totDepend].dependantRuleType = 0;
				nextLoopIsControl = true;
			}
			totRCR++;
		}
		if(tester == "UKR")
		{
			if(nextLoopIsControl)
			{
				DPD[totDepend].checkRuleType = 1;
				DPD[totDepend].checkRule = totUKR;
				totDepend++;
				nextLoopIsControl = false;
			}

			getline(infile, tester, '\n');
			if(tester == "if")
			{
				DPD[totDepend].dependantRule = totUKR;
				DPD[totDepend].dependantRuleType = 1;
				nextLoopIsControl = true;
			}
			totUKR++;
		}
	}
}