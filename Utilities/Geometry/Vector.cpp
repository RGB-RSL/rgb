/*
 * File:   Vector.cpp
 * Author: Eqbal Sarjami
 *
 * Created on February 21, 2011, 5:00 PM
 */

#include "Vector.h"
#include "../DoubleUtilities.h"

using namespace Geometry;
using namespace std;

Vector::Vector()
{
}

Vector::Vector(double x, double y)
{
	this->x = x;
	this->y = y;
}

Vector::~Vector()
{
}

void Vector::setX(double x)
{
	this->x = x;
}

void Vector::setY(double y)
{
	this->y = y;
}

double Vector::getX()
{
	return x;
}

double Vector::getY()
{
	return y;
}

void Vector::setFromPoint(Point p)
{
	x = p.getX();
	y = p.getY();
}

Point Vector::asPoint()
{
	return Point(x, y);
}

double Vector::getLength()
{
	return sqrt(x * x + y * y);
}

Vector Vector::operator +(Vector v)
{
	return Vector(x + v.x, y + v.y);
}

Vector Vector::operator -(Vector v)
{
	return Vector(x - v.x, y - v.y);
}

Vector Vector::operator *(double d)
{
	return Vector(x * d, y * d);
}

bool Vector::operator ==(Vector a)
{
	return isEqual(x, a.x) && isEqual(y, a.y);
}

bool Vector::operator<(Vector a)
{
	if(a == *this)
		return false;

	if(!isEqual(a.x, x))
		return a.x < x;

	return a.y < y;
}

ostream & operator <<(ostream &out, Vector a)
{
	out << "(" << a.getX() << ", " << a.getY() << ")";
	return out;
}