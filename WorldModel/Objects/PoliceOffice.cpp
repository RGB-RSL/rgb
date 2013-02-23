/*
 * PoliceOffice.cpp
 *
 *  Created on: Dec 29, 2009
 *      Author: Alireza Kondori
 *      Modifier: Aryaz Eghbali
 */

#include "PoliceOffice.h"

using namespace std;

PoliceOffice::PoliceOffice(unsigned int Id): Building(Id)
{
}

PoliceOffice::~PoliceOffice()
{
}

void PoliceOffice::setProperties(string type, int value)
{
	Building::setProperties(type, value);
}
