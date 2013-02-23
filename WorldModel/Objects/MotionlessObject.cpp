/*
 * MotionlessObject.cpp
 *
 *  Created on: Dec 29, 2009
 *      Author: Alireza Kondori
 *      Modifier: Aryaz Eghbali
 */

#include "MotionlessObject.h"
#include "../../Utilities/Debugger.h"
#include <iostream>
#define LOGLEVEL 1

using namespace std;
using namespace Types;
using namespace Geometry;

MotionlessObject::MotionlessObject(unsigned int id): RCRObject(id)
{
	flag &= ~1;
	representiveNodeIndex = -1;
    iHaveBeenHere = false;
}

MotionlessObject::MotionlessObject(): RCRObject(0)
{
}

MotionlessObject::~MotionlessObject()
{
}

void MotionlessObject::addEdge(int value)
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
		case 2:
			break;
		case 3:
			break;
		case 4:
			ids.push_back(value);
			break;
	}
	tmp = (tmp + 1) % 5;
}

void MotionlessObject::setProperties(string type, int value)
{
	if(type == Encodings::getPropertyType(PT_EDGES))
	{
		this->addEdge(value);
	}
	else
	{
		RCRObject::setProperties(type, value);
	}
}

Polygon& MotionlessObject::getShape()
{
	return shape;
}

vector <int> & MotionlessObject::getIDs()
{
	return ids;
}

void MotionlessObject::setID(int index, int val)
{
	ids[index] = val;
}

vector<int>& MotionlessObject::getRelativeNodesIndexes()
{
	return relativeNodesIndexes;
}

void MotionlessObject::addRelativeNodeIndex(int index)
{
	relativeNodesIndexes.push_back(index);
}

vector<int>& MotionlessObject::getRelativeEdgesIndexes()
{
	return relativeEdgesIndexes;
}

void MotionlessObject::addRelativeEdgeIndex(int index)
{
	relativeEdgesIndexes.push_back(index);
}

vector<int>& MotionlessObject::getNeighbours()
{
	return neighbours;
}

void MotionlessObject::addNeighbour(int index)
{
	neighbours.push_back(index);
}

vector<int>& MotionlessObject::getInnerHumansIndexes()
{
	return innerHumansIndexes;
}

void MotionlessObject::addInnerHumanIndex(int index)
{
	innerHumansIndexes.push_back(index);
}

void MotionlessObject::clearInnerHumans()
{
	innerHumansIndexes.clear();
}

int MotionlessObject::getRepresentiveNodeIndex()
{
	return representiveNodeIndex;
}

int MotionlessObject::getWithoutBlockadeRepresentiveNodeIndex()
{
	return withoutBlockadeRepresentiveNodeIndex;
}

void MotionlessObject::setRepresentiveNodeIndex(int index)
{
	representiveNodeIndex = index;
}

void MotionlessObject::setWithoutBlockadeRepresentiveNodeIndex(int index)
{
	withoutBlockadeRepresentiveNodeIndex = index;
}