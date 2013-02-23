/*
 * fireStation.h
 *
 *  Created on: Dec 29, 2009
 *      Author: Alireza Kondori
 *      Modifier: Aryaz Eghbali & Navid Saleh
 */

#ifndef __FIRE_STATION_H_
#define __FIRE_STATION_H_

#include "Building.h"

class FireStation: public Building
{
public:
	FireStation(unsigned int Id);
	virtual ~FireStation();
	void setProperties(std::string type, int value);
	int fireStationIndex;
private:
};

#endif
