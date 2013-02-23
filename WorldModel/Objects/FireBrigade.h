/*
 * FireBrigade.h
 *
 *  Created on: Dec 29, 2009
 *      Author: Alireza Kondori
 *      Modifier: Aryaz Eghbali
 */

#ifndef __FIRE_BRIGADE_H_
#define __FIRE_BRIGADE_H_

#include "Human.h"
#include "Building.h"
#include "Platoon.h"

class FireBrigade: public Platoon
{
public:
    FireBrigade(unsigned int Id);
    virtual ~FireBrigade();
    unsigned short int getWaterQuantity() const;
    void setWaterQuantity(unsigned short int waterQuantity);
    void setProperties(std::string type, int value);
    void setHasWaterValue(bool value);
    bool hasWater();
    bool hasJob;
    int lastTimeExtinguish;
    int fireBrigadeIndex;
private:
    bool hasWaterValue;
    unsigned short int waterQuantity;
};

#endif
