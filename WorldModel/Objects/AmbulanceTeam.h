/*
 * AmbulanceTeam.cpp
 *
 *  Created on: Dec 29, 2009
 *      Author: Alireza Kondori
 *      Modifier: Navid Saleh
 */

#ifndef __AMBULANCE_TEAM_H_
#define __AMBULANCE_TEAM_H_

#include "Human.h"
#include "Platoon.h"
#include "Civilian.h"
#include "Building.h"

class AmbulanceTeam: public Platoon
{
public:
	AmbulanceTeam(unsigned int Id);
	virtual ~AmbulanceTeam();
	bool isLoadingInjured();
	void setProperties(std::string type, int value);
	int unloadedTime;
	//    Civilian *loadedCivilian;
	bool hasJob;
	int lastRescueTime;
	int ambulanceTeamIndex;
	Human *loadedHuman;
        Human *humanTarget;
        Building* buildingTarget;
        int targetIndex;
private:
};

#endif
