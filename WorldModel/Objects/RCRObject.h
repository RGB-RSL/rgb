/*
 * RCRObject.h
 *
 *  Created on: Dec 29, 2009
 *      Author: Alireza Kondori
 *      Modifier: Aryaz Eghbali & Navid Saleh
 */

#ifndef __RCROBJECT_H_
#define __RCROBJECT_H_

#include "../../Utilities/Types/Types.h"
//#include <common.h>
#include <string>
#include "../../Utilities/Encodings.h"
#include "../../Utilities/Geometry/Point.h"
#include "../Utilities/Debugger.h"

class RCRObject
{
public:
	RCRObject(unsigned int Id);
	virtual ~RCRObject();
	virtual void setProperties(std::string type, int value);
	bool operator<(RCRObject &obj);
	bool operator>(RCRObject &obj);
	bool operator ==(const RCRObject &r1);
	unsigned int getId() const;
	unsigned int getX() const;
	unsigned int getY() const;
	Geometry::Point getPos() const;
	//		void setId(unsigned int id);
	void setX(unsigned int x);
	void setY(unsigned int y);
	void setLastCycleUpdated(int value);
	int getLastCycleUpdated();
	void setLastCycleUpdatedBySense(int value);
	int getLastCycleUpdatedBySense();
	void setLastCycleSentInRadar(int value);
	int getLastCycleSentInRadar();
	void setLastCycleRecieveData(int value);
	int getLastCycleRecieveData();
	int lastTimeSeen;
	bool isHuman();
	bool isMotionlessObject();
	bool isCivilian();
	bool isPlatoon();
	bool isFireBrigade();
	bool isAmbulanceTeam();
	bool isPoliceForce();
	bool isBuilding();
	bool isRoad();
	bool isRefuge();
protected:
	unsigned int x;
	unsigned int y;
	unsigned int id;
	int lastCycleUpdated;
	int lastCycleUpdatedBySense;
	int lastCycleSentInRadar;
        int lastCylceRecieveData;
	char flag;
};

#endif
