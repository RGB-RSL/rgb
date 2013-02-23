/*
 * Civilian.h
 *
 *  Created on: Dec 29, 2009
 *      Author: Alireza Kondori
 *      Modifier: Aryaz Eghbali
 */

#ifndef __CIVILIAN_H_
#define __CIVILIAN_H_

#include "Human.h"

class Civilian: public Human
{
public:
	Civilian(unsigned int Id);
	virtual ~Civilian();
	void setProperties(std::string type, int value);
        void setTimeToDeath(int value);
        int getTimeToDeath();
        void setEstimateHp(int value);
        int getEstimateHp();
        void setEstimateDamage(int value);
        int getEstimateDamage();
        void setSpotedTime(int value);
        int getSpotedTime();
        
        int civilianIndex;
        
private:
        int timeToDeath;
        int estimateHp;
        int estimateDamage;
        int spotedTime;
};

#endif
