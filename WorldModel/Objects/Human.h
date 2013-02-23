/*
 * AmbulanceTeam.cpp
 *
 *  Created on: Dec 29, 2009
 *      Author: Alireza Kondori
 *      Modifier: Navid Saleh
 */

#ifndef __HUMAN_H_
#define __HUMAN_H_

#include "RCRObject.h"
#include "MotionlessObject.h"

class Human: public RCRObject
{
public:
	Human(unsigned int Id);
	Human();
	virtual ~Human();
	unsigned int getBuriedness() const;
	unsigned int getDamage() const;
	unsigned int getHp() const;
	void setBuriedness(unsigned int buriedness);
	void setDamage(unsigned int damage);
	void setHp(unsigned int hp);
	MotionlessObject *getMotionlessObject() const;
	void setMotionlessObject(MotionlessObject *motionlessObject);
	void setProperties(std::string type, int value);
	unsigned int getDirection() const;
	unsigned int getPosition() const;
	unsigned int getPositionExtra() const;
	unsigned int getPositionHistory() const;
	unsigned int getStamina() const;
	unsigned long long int getTravelDistaance() const;
	void setDirection(unsigned int direction);
	void setPosition(unsigned int position);
	void setPositionExtra(unsigned int positionExtra);
	void setPositionHistory(unsigned int positionHistory);
	void setStamina(unsigned int stamina);
	void setTravelDistance(int value);
	int getRepresentiveNodeIndex();
	int getWithoutBlockadeRepresentiveNodeIndex();
	void setRepresentiveNodeIndex(int);
	void setWithoutBlockadeRepresentiveNodeIndex(int);
	int humanIndex;
        bool isAvailable;
        int value;
private:
	unsigned int buriedness;
	unsigned int damage;
	unsigned int damageEst;
	unsigned int hp;
	unsigned int hpEst;
	unsigned int direction;
	unsigned int stamina;
	unsigned int position;
	unsigned int positionExtra;
	unsigned int positionHistory;
	unsigned long long int travelDistance;
	MotionlessObject *motionlessObject;
	int representiveNodeIndex;
	int withoutBlockadeRepresentiveNodeIndex;
};

#endif
