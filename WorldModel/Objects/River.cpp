/*
 * River.cpp
 *
 *  Created on: Jan 27, 2010
 *      Author: Alireza Kondori
 */

#include "River.h"

using namespace std;

River::River(unsigned int Id): MotionlessObject(Id)
{
}

River::~River()
{
}

void River::setProperties(string type, int value)
{
	MotionlessObject::setProperties(type, value);
}
