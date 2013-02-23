/*
 * Main.cpp
 *
 *  Created on: Dec 17, 2009
 *      Author: Mehran Akhavan
 *      Modifier: Eqbal Sarjami
 */

#define LOGLEVEL 1

#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <cstring>
#include <map>
#include <vector>

#include "Utilities/Defines.h"
#include "Decision/Agent.h"
#include "Utilities/Basics.h"
#include "Utilities/Types/Types.h"
#include "Decision/AmbulanceTeamAgent.h"
#include "Decision/FireBrigadeAgent.h"
#include "Decision/FireStationAgent.h"
#include "Decision/PoliceForceAgent.h"
#include "Decision/PoliceOfficeAgent.h"
#include "Decision/AmbulanceCenterAgent.h"
#include "Utilities/Geometry/OneTimeSettings.h"

using namespace std;
using namespace basics;
using namespace Types;
const char FB = '1', FS = '2', AT = '5', AC = '6', PF = '3', PO = '4';

int runAgent(EntityType et, string ip, int port, int offlineId)
{
	Agent* agent;
	switch(et)
	{
		case ET_AMBULANCE_TEAM:
			agent = new AmbulanceTeamAgent();
			break;
		case ET_AMBULANCE_CENTER:
			agent = new AmbulanceCenterAgent();
			break;
		case ET_FIRE_BRIGADE:
			agent = new FireBrigadeAgent();
			break;
		case ET_FIRE_STATION:
			agent = new FireStationAgent();
			break;
		case ET_POLICE_FORCE:
			agent = new PoliceForceAgent();
			break;
		case ET_POLICE_OFFICE:
			agent = new PoliceForceAgent();
			break;
	}
	int retVal;
	retVal = agent->initData(et);
	if(retVal < 0)
		return retVal;
	if(ip != "offline")
		agent->setConnectionParameters(ip, port);
	retVal = agent->initConnection(et, offlineId);
	if(retVal < 0)
		return retVal;
	agent->run();
	return 0;
}

EntityType parseArgs(char arg)
{
	switch(arg)
	{
		case FB:
			return ET_FIRE_BRIGADE;
			break;
		case FS:
			return ET_FIRE_STATION;
			break;
		case AT:
			return ET_AMBULANCE_TEAM;
			break;
		case AC:
			return ET_AMBULANCE_CENTER;
			break;
		case PF:
			return ET_POLICE_FORCE;
			break;
		case PO:
			return ET_POLICE_OFFICE;
			break;
	}
}

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		cout << "Usage: ./ type{FB=1 FS=2 AT=3 AC=4 PF=5 PO=6} [ip{default=0}]" << endl;
		return 0;
	}
	EntityType type = parseArgs(argv[1][0]);
	string ip = "0";
	int offlineId = -1;
	if(argc >= 3)
	{
		ip = argv[2];
		if(ip == "offline")
		{
			if(argc >= 4)
			{
				sscanf(argv[3], "%d", &offlineId);
				cout << "Offline playing for agent with id: " << offlineId << endl;
			}
			else
			{
				cerr << "OFFLINE USAGE: ./type{FB=1 FS=2 AT=3 AC=4 PF=5 PO=6} offline id" << endl;
				return 0;
			}

		}
	}
	int retVal;
	retVal = runAgent(type, ip, 0, offlineId);
	return retVal;
}
