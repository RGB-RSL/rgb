/*
 * AmbulanceCenter.cpp
 *
 *  Created on: Dec 29, 2009
 *      Author: Alireza Kondori
 *      Modifier: Navid Saleh
 */

#include "AmbulanceCenter.h"

using namespace std;

AmbulanceCenter::AmbulanceCenter(unsigned int Id): Building(Id)
{
}

AmbulanceCenter::~AmbulanceCenter()
{
}

void AmbulanceCenter::setProperties(string type, int value)
{
	Building::setProperties(type, value);
}
