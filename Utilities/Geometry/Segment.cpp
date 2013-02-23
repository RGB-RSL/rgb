/*
 * File:   Segment.cpp
 * Author: Alimohammad Foroutannezhad
 * Modified by Eqbal Sarjami
 *
 * Created on September 17, 2010, 11:03 AM
 */

#include <Segment.h>
#include "../Utilities/Debugger.h"
#include "GeometryFunctions.h"
#define LOGLEVEL 1
using namespace std;
using namespace Geometry;

Segment::Segment()
{
}

Segment::Segment(Point a, Point b)
{
	first = a;
	second = b;
}

Segment::~Segment()
{
}

void Segment::setFirstPoint(Point a)
{
	first = a;
}

void Segment::setSecondPoint(Point a)
{
	second = a;
}

Point Segment::getFirstPoint(void)
{
	return first;
}

Point Segment::getSecondPoint(void)
{
	return second;
}

double Segment::getLength(void)
{
	return asVector().getLength();
}

Point Segment::getMiddlePoint()
{
	return Point((first.getX() + second.getX()) / 2, (first.getY() + second.getY()) / 2);
}

Vector Segment::asVector()
{
	return second.asVector() - first.asVector();
}

Vector Segment::asRVector()
{
	return first.asVector() - second.asVector();
}

Line Segment::asLine()
{
	return Line(first, second);
}

ostream & operator <<(ostream &out, Segment a)
{
	out << "line" << a.getFirstPoint() << a.getSecondPoint() << ' ' << __color << ';';
	return out;
}