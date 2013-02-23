/*
 * AmbulanceCenter.h
 *
 *  Created on: Dec 29, 2009
 *      Author: Alireza Kondori
 *      Modifier: Navid Saleh
 */

#ifndef __AMBULANCE_CENTER_H_
#define __AMBULANCE_CENTER_H_

#include "Building.h"

class AmbulanceCenter: public Building
{
public:
	AmbulanceCenter(unsigned int Id);
	virtual ~AmbulanceCenter();
	void setProperties(std::string type, int value);
	int ambulanceCenterIndex;
private:
};

#endif
