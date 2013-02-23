/*
 * River.h
 *
 *  Created on: Jan 27, 2010
 *      Author: Alireza Kondori
 */

#ifndef RIVER_H_
#define RIVER_H_

#include "MotionlessObject.h"

class River: public MotionlessObject
{
public:
	River(unsigned int Id);
	virtual ~River();
	void setProperties(std::string type, int value);
	int riverIndex;
private:
};

#endif /* RIVER_H_ */
