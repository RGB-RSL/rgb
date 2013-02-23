/*
 * Node.cpp
 *
 *  Created on: Nov 28, 2010
 *      Author: Eqbal Sarjami
 */

#include <vector>
#include <memory>

#include "Node.h"
#include "MotionlessObject.h"
#include "Config.h"

using namespace std;
using namespace Geometry;

Node::Node(int firstMotionlessIndex, int secondMotionlessIndex)
{
	this->firstMotionlessIndex = firstMotionlessIndex;
	this->secondMotionlessIndex = secondMotionlessIndex;
}

Node::~Node()
{
}

int Node::getFirstMotionlessIndex()
{
	return firstMotionlessIndex;
}

int Node::getSecondMotionlessIndex()
{
	return secondMotionlessIndex;
}

vector<int>& Node::getEdgeIndexes()
{
	return edgeIndexes;
}

void Node::addEdgeIndex(int edgeID)
{
	edgeIndexes.push_back(edgeID);
}

int Node::getEdgeIndexInFirstMotionless()
{
	return edgeIndexInFirstMotionless;
}

void Node::setEdgeIndexInFirstMotionless(int index)
{
	edgeIndexInFirstMotionless = index;
}

int Node::getEdgeIndexInSecondMotionless()
{
	return edgeIndexInSecondMotionless;
}

void Node::setEdgeIndexInSecondMotionless(int index)
{
	edgeIndexInSecondMotionless = index;
}

vector<int>& Node::getNeighbours()
{
	return neighbours;
}

void Node::addNeighbour(int index)
{
	neighbours.push_back(index);
}

Point& Node::getRepresentivePoint()
{
	return representivePoint;
}

void Node::setRepresentivePoint(Point point)
{
	representivePoint = point;
}