/*
 * FireBrigade.cpp
 *
 *  Created on: Dec 29, 2009
 *      Author: Alireza Kondori
 *      Modifier: Aryaz Eghbali
 */

#include "FireBrigade.h"
#include <iostream>
using namespace std;
using namespace Types;

FireBrigade::FireBrigade(unsigned int Id): Platoon(Id)
{
	lastTimeExtinguish = 0;
    hasWaterValue = true;
	flag |= 12;
}

FireBrigade::~FireBrigade()
{
}

void FireBrigade::setWaterQuantity(unsigned short int waterQuantity)
{
	this->waterQuantity = waterQuantity;
}

unsigned short int FireBrigade::getWaterQuantity() const
{
	return waterQuantity;
}

void FireBrigade::setHasWaterValue(bool value)
{
    this->hasWaterValue = value;
}

bool FireBrigade::hasWater()
{
    return this->hasWaterValue;
}

void FireBrigade::setProperties(string type, int value)
{
	if(type == Encodings::getPropertyType(PT_WATER_QUANTITY))
	{
		this->setWaterQuantity(value);
	}
	else
		Platoon::setProperties(type, value);

}
