/* 
 * File:   Blockade.cpp
 * Author: Mehran Akhavan
 * 
 * Created on September 24, 2010, 11:32 AM
 */

#include "Blockade.h"
#define LOGLEVEL 1

using namespace std;
using namespace Types;
using namespace Geometry;

Blockade::Blockade(unsigned int id) : RCRObject(id)
{
	//	LOG(Main, 1) << "new blockade with id: " << id << endl;
}

Blockade::Blockade() : RCRObject(0)
{
}

Blockade::~Blockade()
{
}

unsigned int Blockade::getRepairCost() const
{
	return this->repairCost;
}

unsigned int Blockade::getPosition() const
{
	return this->position;
}

unsigned short int Blockade::getLastClearTime() const
{
	return this->lastClearTime;
}

void Blockade::setRepairCost(unsigned int value)
{
	repairCost = value;
}

void Blockade::setLastClearTime(unsigned short int value)
{
	this->lastClearTime = value;
}

//void Blockade::addApex(Apex apex)
//{
//    this->apexes.push_back(apex);
//}

void Blockade::addVertex(int value)
{
	static int tmp = 0;
	static Point p;
	switch(tmp)
	{
	case 0:
	{
		p.setX(value);
		break;
	}
	case 1:
	{
		p.setY(value);
		shape.addVertex(p);
		break;
	}
	}
	tmp = (tmp + 1) % 2;
}

void Blockade::setPosition(unsigned int value)
{
	this->position = value;
}

void Blockade::setProperties(string type, int value)
{
	if(type == Encodings::getPropertyType(PT_APEXES))
	{
		this->addVertex(value);
	}
	else if(type == Encodings::getPropertyType(PT_POSITION))
	{
		this->setPosition(value);
	}
	else if(type == Encodings::getPropertyType(PT_REPAIR_COST))
	{
		this->setRepairCost(value);
	}
	else
	{
		RCRObject::setProperties(type, value);
	}
}

Polygon& Blockade::getShape()
{
	return shape;
}

void Blockade::clearShape()
{
	shape.clear();
}