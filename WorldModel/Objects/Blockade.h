/* 
 * File:   Blockade.h
 * Author: mehran
 *
 * Created on September 24, 2010, 11:32 AM
 */

#ifndef _BLOCKADE_H
#define	_BLOCKADE_H

#include "RCRObject.h"
#include "../../Utilities/Geometry/Polygon.h"

class Blockade : public RCRObject
{
public:
	Blockade(unsigned int id);
	Blockade();
	virtual ~Blockade();
	//	std::vector<Apex>& getApexes();
	unsigned int getPosition() const;
	unsigned int getRepairCost() const;
	unsigned short int getLastClearTime() const;
	void addVertex(int value);
	void setPosition(unsigned int value);
	void setRepairCost(unsigned int value);
	void setProperties(std::string type, int value);
	void setLastClearTime(unsigned short int value);
	Geometry::Polygon& getShape();
	void clearShape();

private:
	unsigned int repairCost;
	unsigned int position;
	unsigned short int lastClearTime;
	Geometry::Polygon shape;
};

#endif	/* _BLOCKADE_H */

