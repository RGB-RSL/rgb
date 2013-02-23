/*
 * Road.cpp
 *
 *  Created on: Dec 29, 2009
 *      Author: alireza
 */

#include <iostream>
#include "Road.h"
#define LOGLEVEL 1
using namespace std;
using namespace Types;
using namespace Geometry;

Road::Road(unsigned int Id) : MotionlessObject(Id)
{
	//	LOG(Main, 1) << "new road with id: " << Id << endl;
	blockadeIds.clear();
	flag &= ~2;
	hsaToBeSent = -1;
}

Road::~Road()
{
}

unsigned short int Road::getlength() const
{
	return length;
}

unsigned short int Road::getLines() const
{
	return lines;
}

float Road::getRepairCost() const
{
	return repairCost;
}

WayMode Road::getWayMode() const
{
	return wayMode;
}

unsigned short int Road::getWidth() const
{
	return width;
}

std::vector<int>& Road::getBlockadeIds()
{
	return blockadeIds;
}

std::vector<Blockade*>& Road::getBlockades()
{
	return blockades;
}

void Road::clearBlockadeIds()
{
	blockadeIds.clear();
}

void Road::clearBlockades()
{
	blockades.clear();
}

vector<Point>& Road::getInnerPoints()
{
	return innerPoints;
}

int Road::getHasToBeSent()
{
	return hsaToBeSent;
}

void Road::addBlockadeId(int value)
{
	if(value == 0)
	{
//		LOG(Main, 1) << "HEYYYYYYYYYYYYYYYYYYYYYYYYYY!" << endl;
		return;
	}
	blockadeIds.push_back(value);
}

void Road::addBlockade(Blockade *b)
{
	blockades.push_back(b);
}

void Road::setlength(unsigned short int length)
{
	this->length = length;
}

void Road::setLines(unsigned short int lines)
{
	this->lines = lines;
}

void Road::setRepairCost(float repairCost)
{
	this->repairCost = repairCost;
}

void Road::setWayMode(WayMode wayMode)
{
	this->wayMode = wayMode;
}

void Road::setWidth(unsigned short int width)
{
	this->width = width;
}

void Road::addInnerPoint(Point p)
{
	innerPoints.push_back(p);
}

void Road::setProperties(string type, int value)
{
	if(type == Encodings::getPropertyType(PT_LENGTH))
	{
		this->setlength(value);
		//		cout<<"Length: "<<value<<endl;
	}
	else if(type == Encodings::getPropertyType(PT_WIDTH))
	{
		this->setWidth(value);
		//		cout<<"width: "<<value<<endl;
	}
	else if(type == Encodings::getPropertyType(PT_REPAIR_COST))
	{
		this->setRepairCost((float)value);
		//		cout<<"repair cost: "<<value<<endl;
	}
	else if(type == Encodings::getPropertyType(PT_BLOCKADES))
	{
		this->addBlockadeId(value);
		//        cout<<"add new edge: "<<value <<endl;
	}
	else
		MotionlessObject::setProperties(type, value);
}

void Road::setHasToBeSent(int a)
{
	hsaToBeSent = a;
}
