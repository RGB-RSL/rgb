/* 
 * File:   Platoon.cpp
 * Author: mehran
 * 
 * Created on October 7, 2010, 5:42 PM
 */

#include "Platoon.h"

using namespace std;

Platoon::Platoon()
{
}

Platoon::Platoon(unsigned int Id): Human(Id)
{
	flag &= ~2;
}

Platoon::~Platoon()
{
}

void Platoon::setProperties(string type, int value)
{
	Human::setProperties(type, value);
}

