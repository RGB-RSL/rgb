/*
 * Road.h
 *
 *  Created on: Dec 29, 2009
 *      Author: alireza
 */

#ifndef __ROAD_H_
#define __ROAD_H_

#include "MotionlessObject.h"
#include "Blockade.h"

class Road : public MotionlessObject
{
public:
	Road(unsigned int Id);
	virtual ~Road();
	unsigned short int getlength() const;
	unsigned short int getLines() const;
	float getRepairCost() const;
	unsigned int getTailId() const;
	Types::WayMode getWayMode() const;
	unsigned short int getWidth() const;
	std::vector<int>& getBlockadeIds();
	void clearBlockadeIds();
	std::vector<Blockade*>& getBlockades();
	void clearBlockades();
	std::vector<Geometry::Point>& getInnerPoints();
	int getHasToBeSent();
	void setlength(unsigned short int length);
	void setLines(unsigned short int lines);
	void setRepairCost(float repairCost);
	void setWayMode(Types::WayMode wayMode);
	void setWidth(unsigned short int width);
	void addBlockadeId(int value);
	void addBlockade(Blockade *);
	void addInnerPoint(Geometry::Point);
	void setProperties(std::string type, int value);
	void setHasToBeSent(int);
	int roadIndex;
private:
	unsigned short int length;
	unsigned short int lines;
	unsigned short int width;
	std::vector<int> blockadeIds;
	std::vector<Blockade*> blockades;
	float repairCost;
	Types::WayMode wayMode;
	std::vector<Geometry::Point> innerPoints;
	int hsaToBeSent;
};

#endif
