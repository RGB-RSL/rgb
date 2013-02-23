/*
 * Civilian.cpp
 *
 *  Created on: Dec 29, 2009
 *      Author: Alireza Kondori
 *      Modifier: Aryaz Eghbali
 */

#include "Civilian.h"
#include "Geometry/Circle.h"

using namespace std;

Civilian::Civilian(unsigned int Id) : Human(Id)
{
    timeToDeath = id;
    estimateDamage = 0;
    estimateHp = 10000;
    flag |= 2;
}

Civilian::~Civilian()
{
}

void Civilian::setEstimateHp(int value)
{
	this->estimateHp = value;
}

int Civilian::getEstimateHp()
{
	return this->estimateHp;
}

void Civilian::setEstimateDamage(int value)
{
	this->estimateDamage = value;
}

int Civilian::getEstimateDamage()
{
	return this->estimateDamage;
}

void Civilian::setTimeToDeath(int value)
{
	this->timeToDeath = value;
}

int Civilian::getTimeToDeath()
{
	return this->timeToDeath;
}

void Civilian::setSpotedTime(int value)
{
    this->spotedTime = value;
}

int Civilian::getSpotedTime()
{
    return this->spotedTime;
}

void Civilian::setProperties(string type, int value)
{
	Human::setProperties(type, value);
}