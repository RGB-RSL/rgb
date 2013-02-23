/*
 * PoliceOffice.h
 *
 *  Created on: Dec 29, 2009
 *      Author: Alireza Kondori
 *      Modifier: Aryaz Eghbali
 */

#ifndef __POLICE_OFFICE_H_
#define __POLICE_OFFICE_H_

#include "Building.h"

class PoliceOffice: public Building
{
public:
	PoliceOffice(unsigned int Id);
	virtual ~PoliceOffice();
	void setProperties(std::string type, int value);
	int policeOfficeIndex;
private:
};

#endif
