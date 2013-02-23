/*
 * RCRObject.cpp
 *
 *  Created on: Dec 29, 2009
 *      Author: Alireza Kondori
 *      Modifier: Aryaz Eghbali & Navid Saleh
 */

#include <cmath>
#include "RCRObject.h"
#include <iostream>
#include <assert.h>

using namespace std;
using namespace Types;
using namespace Geometry;

RCRObject::RCRObject(unsigned int id)
{
	this->id = id;
	lastTimeSeen = -1;
	setLastCycleSentInRadar(0);
	setLastCycleUpdated(0);
	setLastCycleUpdatedBySense(0);
    flag = 0;
}

RCRObject::~RCRObject()
{
	lastTimeSeen = -1;
	setLastCycleSentInRadar(0);
	setLastCycleUpdated(0);
	setLastCycleUpdatedBySense(0);
    flag = 0;
}

unsigned int RCRObject::getId() const
{
	return id;
}

unsigned int RCRObject::getX() const
{
	return x;
}

unsigned int RCRObject::getY() const
{
	return y;
}

Point RCRObject::getPos() const
{
	Point point(x, y);
	return point;
}

void RCRObject::setX(unsigned int x)
{
	this->x = x;
}

void RCRObject::setY(unsigned int y)
{
	this->y = y;
}

void RCRObject::setLastCycleUpdated(int value)
{
	this->lastCycleUpdated = value;
}

int RCRObject::getLastCycleUpdated()
{
	return this->lastCycleUpdated;
}

void RCRObject::setLastCycleSentInRadar(int value)
{
	this->lastCycleSentInRadar = value;
}

int RCRObject::getLastCycleSentInRadar()
{
	return this->lastCycleSentInRadar;
}

void RCRObject::setLastCycleUpdatedBySense(int value)
{
	this->lastCycleUpdatedBySense = value;
}

int RCRObject::getLastCycleUpdatedBySense()
{
	return this->lastCycleUpdatedBySense;
}

void RCRObject::setLastCycleRecieveData(int value)
{
    this->lastCylceRecieveData = value;
}

int RCRObject::getLastCycleRecieveData()
{
    return this->lastCylceRecieveData;
}

bool RCRObject::operator<(RCRObject &obj)
{
	return obj.getId() > this->getId();
}

bool RCRObject::operator>(RCRObject &obj)
{
	return obj.getId() < this->getId();
}

void RCRObject::setProperties(string type, int value)
{
	if(type == Encodings::getPropertyType(PT_X))
	{
		//		cout<<"x: "<<value<<endl;
		this->setX(value);
	}
	else if(type == Encodings::getPropertyType(PT_Y))
	{
		//		cout<<"y: "<<value<<endl;
		this->setY(value);
	}
	else
	{
		cerr << "setProperties: Recieve an unvalid type: " << type << endl;
		cerr << "value: " << value << endl;
		//TODO erase this assert
		assert(false);
	}
}

bool RCRObject::operator ==(const RCRObject &r1)
{
	if(r1.getId() == this->getId())
		return true;
	else
		return false;
}

bool RCRObject::isHuman()
{
	return (flag & 1);
}

bool RCRObject::isMotionlessObject()
{
	return !(flag & 1);
}

bool RCRObject::isCivilian()
{
	return isHuman() && (flag & 2);
}

bool RCRObject::isPlatoon()
{
	return isHuman() && !(flag & 2);
}

bool RCRObject::isFireBrigade()
{
	return isPlatoon() && (flag & 4) && (flag & 8);
}

bool RCRObject::isAmbulanceTeam()
{
	return isPlatoon() && !(flag & 4) && !(flag & 8);
}

bool RCRObject::isPoliceForce()
{
	return isPlatoon() && (flag & 4) && !(flag & 8);
}

bool RCRObject::isBuilding()
{
	return isMotionlessObject() && (flag & 2);
}

bool RCRObject::isRoad()
{
	return isMotionlessObject() && !(flag & 2);
}

bool RCRObject::isRefuge()
{
	return isBuilding() && (flag & 4);
}