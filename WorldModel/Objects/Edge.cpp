/*
 * Edge.cpp
 *
 *  Created on: Nov 28, 2010
 *      Author: Eqbal Sarjami
 */

#include "Edge.h"

using namespace Types;

Edge::Edge(int firstNodeIndex, int secondNodeIndex)
{
	this->firstNodeIndex = firstNodeIndex;
	this->secondNodeIndex = secondNodeIndex;
	this->passingMode = PM_UNKNOWN;
}

Edge::~Edge()
{
}

int Edge::getFirstNodeIndex()
{
	return firstNodeIndex;
}

int Edge::getSecondNodeIndex()
{
	return secondNodeIndex;
}

int Edge::getLength()
{
	return length;
}

int Edge::getMotionlessIndex()
{
	return motionlessIndex;
}

void Edge::setLength(int length)
{
	this->length = length;
}

PassingMode Edge::getPassingMode()
{
	return passingMode;
}

void Edge::setPassingMode(Types::PassingMode passingMode)
{
	this->passingMode = passingMode;
}

void Edge::setMotionlessIndex(int motionlessIndex)
{
	this->motionlessIndex = motionlessIndex;
}