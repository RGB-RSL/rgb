/*
 * AmbulanceTeam.cpp
 *
 *  Created on: Dec 29, 2009
 *      Author: Alireza Kondori
 *      Modifier: Navid Saleh
 */

#include "Human.h"
#include <iostream>
#define LOGLEVEL 1

using namespace std;
using namespace Types;

Human::Human(unsigned int Id): RCRObject(Id)
{
	motionlessObject = NULL;
	flag |= 1;
	representiveNodeIndex = -1;
    isAvailable = true;
    value = 0;
}

Human::Human(): RCRObject(0)
{
}

Human::~Human()
{
}

unsigned int Human::getBuriedness() const
{
	return buriedness;
}

unsigned int Human::getDamage() const
{
	return damage;
}

unsigned int Human::getHp() const
{
	return hp;
}

unsigned int Human::getDirection() const
{
	return direction;
}

unsigned int Human::getPosition() const
{
	return position;
}

unsigned int Human::getPositionExtra() const
{
	return positionExtra;
}

unsigned int Human::getPositionHistory() const
{
	return positionHistory;
}

unsigned int Human::getStamina() const
{
	return stamina;
}

void Human::setBuriedness(unsigned int buriedness)
{
//	LOG(Main, 1) << "human with id: " << id << " has buriedness: " << buriedness << endl;
	this->buriedness = buriedness;
}

void Human::setDamage(unsigned int damage)
{
	this->damage = damage;
}

void Human::setHp(unsigned int hp)
{
	this->hp = hp;
}

MotionlessObject* Human::getMotionlessObject() const
{
	return motionlessObject;
}

unsigned long long int Human::getTravelDistaance() const
{
	return this->travelDistance;
}

void Human::setTravelDistance(int value)
{
	this->travelDistance = value;
}

void Human::setMotionlessObject(MotionlessObject *motionlessObject)
{
	this->motionlessObject = motionlessObject;
}

void Human::setDirection(unsigned int direction)
{
	this->direction = direction;
}

void Human::setPosition(unsigned int position)
{
	this->position = position;
}

void Human::setPositionExtra(unsigned int positionExtra)
{
	this->positionExtra = positionExtra;
}

void Human::setPositionHistory(unsigned int positionHistory)
{
	this->positionHistory = positionHistory;
}

void Human::setStamina(unsigned int stamina)
{
	this->stamina = stamina;
}

void Human::setProperties(string type, int value)
{
	if(type == Encodings::getPropertyType(PT_BURIEDNESS))
	{
		this->setBuriedness(value);
		//		cout<<"Buriedness: "<<value<<endl;
	}
	else if(type == Encodings::getPropertyType(PT_DAMAGE))
	{
		this->setDamage(value);
		//		cout<<"damage: "<<value<<endl;
	}
	else if(type == Encodings::getPropertyType(PT_HP))
	{
		this->setHp(value);
		//		cout<<"HP: "<<value<<endl;
	}
	else if(type == Encodings::getPropertyType(PT_DIRECTION))
	{
		this->setDirection(value);
		//		cout<<"direction: "<<value<<endl;
	}
	else if(type == Encodings::getPropertyType(PT_POSITION))
	{
		this->setPosition(value);
		//		cout << "position: " << value << endl;
	}
		//	case Librescue::PROPERTY_POSITION_EXTRA:
		//		this->setPositionExtra(value);
		////		cout<<"Position extra: "<<value<<endl;
		//		break;
	else if(type == Encodings::getPropertyType(PT_POSITION_HISTORY))
	{
		this->setPositionHistory(value);
		//		cout<<"Position history: "<<value<<endl;
	}
	else if(type == Encodings::getPropertyType(PT_STAMINA))
	{
		this->setStamina(value);
		//		cout<<"stamina: "<<value<<endl;
	}
	else if(type == Encodings::getPropertyType(PT_TRAVEL_DISTANCE))
	{
		this->setTravelDistance(value);
		//        cout<<"travel distance: "<<value<<endl;
	}
	else
		RCRObject::setProperties(type, value);
}

int Human::getRepresentiveNodeIndex()
{
	return representiveNodeIndex;
}

int Human::getWithoutBlockadeRepresentiveNodeIndex()
{
	return withoutBlockadeRepresentiveNodeIndex;
}

void Human::setRepresentiveNodeIndex(int index)
{
	representiveNodeIndex = index;
}

void Human::setWithoutBlockadeRepresentiveNodeIndex(int index)
{
	withoutBlockadeRepresentiveNodeIndex = index;
}