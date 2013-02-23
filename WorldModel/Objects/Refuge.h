/*
 * Refuge.h
 *
 *  Created on: Dec 29, 2009
 *      Author: Alireza Kondori
 *      Modifier: Aryaz Eghbali
 */

#ifndef __REFUGE_H_
#define __REFUGE_H_

#include "Building.h"

class Refuge: public Building
{
public:
	Refuge(unsigned int Id);
	virtual ~Refuge();
	void setProperties(std::string type, int value);
	int refugeIndex;
private:
};

#endif
