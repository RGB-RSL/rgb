/*
 * PoliceForce.cpp
 *
 *  Created on: Dec 29, 2009
 *      Author: Alireza Kondori
 *      Modifier: Aryaz Eghbali
 */

#include "PoliceForce.h"

using namespace std;

PoliceForce::PoliceForce(unsigned int Id): Platoon(Id)
{
	lastTimeCleared = 0;
	flag |= 4;
	flag &= ~8;
}

PoliceForce::~PoliceForce()
{
}

void PoliceForce::setProperties(string type, int value)
{
	Platoon::setProperties(type, value);
}
