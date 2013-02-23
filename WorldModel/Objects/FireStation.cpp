/*
 * FireStation.cpp
 *
 *  Created on: Dec 29, 2009
 *      Author: Alireza Kondori
 *      Modifier: Aryaz Eghbali
 */

#include "FireStation.h"

using namespace std;

FireStation::FireStation(unsigned int Id): Building(Id)
{
}

FireStation::~FireStation()
{
}

void FireStation::setProperties(string type, int value)
{
	Building::setProperties(type, value);
}
