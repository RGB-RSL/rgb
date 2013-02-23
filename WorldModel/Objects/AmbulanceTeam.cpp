/*
 * AmbulanceTeam.cpp
 *
 *  Created on: Dec 29, 2009
 *      Author: Alireza Kondori
 *      Modifier: Navid Saleh
 */

#include "AmbulanceTeam.h"

using namespace std;

AmbulanceTeam::AmbulanceTeam(unsigned int Id) : Platoon(Id)
{
    unloadedTime = -1;
    loadedHuman = NULL;
    humanTarget = NULL;
    buildingTarget = NULL;
    lastRescueTime = 0;
    targetIndex = -1;
    flag &= ~12;
}

AmbulanceTeam::~AmbulanceTeam()
{
}

bool AmbulanceTeam::isLoadingInjured()
{
    if (this->loadedHuman != NULL)
        if (this->loadedHuman->getPosition() == this->id)
            return true;
    return false;
    //	return loadInjured != NULL;
}

void AmbulanceTeam::setProperties(string type, int value)
{
    Platoon::setProperties(type, value);
}

