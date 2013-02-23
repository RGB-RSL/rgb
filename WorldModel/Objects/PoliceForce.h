/*
 * PoliceForce.h
 *
 *  Created on: Dec 29, 2009
 *      Author: Alireza Kondori
 *      Modifier: Aryaz Eghbali
 */

#ifndef __POLICE_FORCE_H_
#define __POLICE_FORCE_H_

#include "Human.h"
#include "Platoon.h"

class PoliceForce: public Platoon
{
public:
	PoliceForce(unsigned int Id);
	virtual ~PoliceForce();
	void setProperties(std::string type, int value);
	int lastTimeCleared;
	bool isWorking;
	int policeForceIndex;
private:
};

#endif
