/*
 * File:   Point.cpp
 * 
 * Modifed by Eqbal Sarjami
 * on February 21, 2011
 */

#include "Point.h"
#include <math.h>
#include "../Utilities/Debugger.h"
#include "../DoubleUtilities.h"
#include "GeometryFunctions.h"
#define LOGLEVEL 1

using namespace std;
using namespace Geometry;

Point::Point()
{
}

Point::Point(double x, double y)
{
	this->x = x;
	this->y = y;
}

Point::~Point()
{
}

void Point::setX(double x)
{
	this->x = x;
}

void Point::setY(double y)
{
	this->y = y;
}

double Point::getX()
{
	return x;
}

double Point::getY()
{
	return y;
}

void Point::setFromVector(Vector v)
{
	x = v.getX();
	y = v.getY();
}

Vector Point::asVector()
{
	return Vector(x, y);
}

bool Point::operator ==(Point a)
{
	return isEqual(x, a.x) && isEqual(y, a.y);
}

bool Point::operator<(Point a)
{
	if(a == *this)
		return false;

	if(!isEqual(a.x, x))
		return a.x < x;

	return a.y < y;
}

ostream & operator <<(ostream &out, Point a)
{
	out << fixed << setprecision(0) << "(" << a.getX() << ", " << a.getY() << ")";
	return out;
}

double Point::distFromPoint(Point point)
{
	return distanceBetweenPoints(*this, point);
}
