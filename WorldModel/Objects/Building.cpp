/*
 * Building.cpp
 *
 *  Created on: Dec 29, 2009
 *      Author: Alireza Kondori
 *      Modifier: Navid Saleh
 */

#include "Building.h"
#include "Road.h"
#include <iostream>
#include <vector>
#define LOGLEVEL 1
using namespace std;
using namespace Types;

Building::Building(unsigned int id): MotionlessObject(id)
{
	this->blockNum = -1;
	this->temperature = 0;
        this->wasBurning = false;
        value = 0;
	//	LOG(Main, 1) << "new building with id: " << id << endl;
	flag |= 2;
}

Building::~Building()
{
}

bool Building::isBurning()
{
	if(this->getFieryness() > 0 && this->getFieryness() <= 3)
		return true;
	return false;
}

bool Building::isUnburned()
{
	if(this->getFieryness() == 0)
		return true;
	return false;
}

bool Building::isPutOut()
{
	if(this->getFieryness() >= 5 && this->getFieryness() <= 7)
		return true;
	return false;
}

void Building::setValue(int value)
{
    this->value = value;
}

int Building::getValue()
{
    return this->value;
}

unsigned int Building::getAreaGround() const
{
	return AreaGround;
}

unsigned int Building::getAreaTotal() const
{
	return AreaTotal;
}

BuildingCode Building::getBuildingCode() const
{
	return buildingCode;
}

unsigned short int Building::getFieryness() const
{
	return fieryness;
}

unsigned short int Building::getFloors() const
{
	return floors;
}

unsigned int Building::getBrokenness() const
{
	return brokenness;
}

int Building::getTemperature() const
{
	return temperature;
}

int Building::getImportance() const
{
	return importance;
}

unsigned int Building::getAttributes() const
{
	return attributes;
}

vector<Building::InSightPoint>& Building::getInSightPoints()
{
	return inSightPoints;
}

void Building::setAreaGround(unsigned int AreaGround)
{
	this->AreaGround = AreaGround;
}

void Building::setAreaTotal(unsigned int AreaTotal)
{
	this->AreaTotal = AreaTotal;
}

void Building::setBuildingCode(BuildingCode buildingCode)
{
	this->buildingCode = buildingCode;
}

void Building::setFieryness(unsigned short int fieryness)
{
	this->fieryness = fieryness;
}

void Building::setFloors(unsigned short int floors)
{
	this->floors = floors;
}

void Building::setImportance(int value)
{
	this->importance = value;
}

void Building::setBrokenness(unsigned int brokenness)
{
	this->brokenness = brokenness;
}

void Building::setAttributes(unsigned int value)
{
	this->attributes = value;
}

void Building::setTemperature(unsigned int value)
{
    if(this->temperature > 40 && value < 40)
        this->wasBurning = true;
    else
        this->wasBurning = false;
    this->temperature = value;
}

void Building::setProperties(string type, int value)
{
	if(type == Encodings::getPropertyType(PT_FLOORS))
	{
		//		cout<<"floors: "<<value<<endl;
		this->setFloors(value);
	}
	else if(type == Encodings::getPropertyType(PT_BROKENNESS))
	{
		//		cout<<"brokenness: "<<value<<endl;
		this->setBrokenness(value);
	}
	else if(type == Encodings::getPropertyType(PT_BUILDING_AREA_GROUND))
	{
		//		cout<<"area ground: "<<value<<endl;
		this->setAreaGround(value);
	}
	else if(type == Encodings::getPropertyType(PT_BUILDING_AREA_TOTAL))
	{
		//		cout<<"area total: "<<value<<endl;
		this->setAreaTotal(value);
	}
	else if(type == Encodings::getPropertyType(PT_BUILDING_CODE))
	{
		//		cout<<"building code : "<<value<<endl;
		this->setBuildingCode((BuildingCode)value);
	}
	else if(type == Encodings::getPropertyType(PT_FIERYNESS))
	{
		//		cout<<"fieryness: "<<value<<endl;
		this->setFieryness(value);
	}
	else if(type == Encodings::getPropertyType(PT_IMPORTANCE))
	{
		//		cout<<"entrances: "<<value<<endl;
		this->setImportance(value);
	}
	else if(type == Encodings::getPropertyType(PT_TEMPERATURE))
	{
		//        cout<<"add edge: "<<value<<endl;
		this->setTemperature(value);
	}
	else if(type == Encodings::getPropertyType(PT_BUILDING_ATTRIBUTES))
	{
		//        cout<<"add edge: "<<value<<endl;
		this->setAttributes(value);
	}
	else
		MotionlessObject::setProperties(type, value);
}

void Building::addInSightPoint(int roadIndex, int index)
{
	inSightPoints.push_back(InSightPoint(roadIndex, index));
}

Building::InSightPoint::InSightPoint()
{
}

Building::InSightPoint::InSightPoint(int roadIndex, int index)
{
	this->roadIndex = roadIndex;
	this->index = index;
}