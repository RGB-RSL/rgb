/* 
 * File:   AmbulanceCenterAgent.cpp
 * Author: mehran
 * 
 * Created on September 24, 2010, 3:56 PM
 */

#include "AmbulanceCenterAgent.h"
#include "Agent.h"
#include "../Utilities/Debugger.h"
#include "AmbulanceTeamAgent.h"
#define LOGLEVEL 1

AmbulanceCenterAgent::AmbulanceCenterAgent()
{
}

AmbulanceCenterAgent::~AmbulanceCenterAgent()
{
}

void AmbulanceCenterAgent::act()
{
}

void AmbulanceCenterAgent::actBeforeRadar()
{
}

void AmbulanceCenterAgent::precomputation()
{
	Agent::precomputation();
}

AmbulanceCenter* AmbulanceCenterAgent::self()
{
	return (AmbulanceCenter*)world->self;
}