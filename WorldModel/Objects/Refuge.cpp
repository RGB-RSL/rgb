/*
 * Refuge.cpp
 *
 *  Created on: Dec 29, 2009
 *      Author: Alireza Kondori
 *      Modifier: Aryaz Eghbali
 */

#include "Refuge.h"

using namespace std;

Refuge::Refuge(unsigned int Id): Building(Id)
{
	flag |= 4;
}

Refuge::~Refuge()
{
}

void Refuge::setProperties(string type, int value)
{
	Building::setProperties(type, value);
}
