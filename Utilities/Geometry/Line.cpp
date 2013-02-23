/*
 * File:   Line.cpp
 * Author: Alimohammad Forutannezhad
 * Modified by Eqbal Sarjami
 *
 * Created on September 17, 2010, 11:03 AM
 */

#include <cmath>
#include <assert.h>
#include "Line.h"
#include "Point.h"
#include "../DoubleUtilities.h"
#include "Segment.h"
#include "../Debugger.h"

#define LOGLEVEL 1

using namespace std;
using namespace Geometry;

Line::Line()
{
	isHoriz = false;
}

Line::Line(double m, double h)
{
	this->m = m;
	this->h = h;
	isHoriz = false;
}

Line::Line(Point p, double degree)
{
	double degDivPi = (degree - M_PI_2) / M_PI, fracPart;
	modf(degDivPi, &fracPart);
	if(isEqual(fracPart, 0.0) || isEqual(fracPart, 1.0))
	{
		isHoriz = true;
		m = 0;
		h = p.getX();
		return;
	}
	isHoriz = false;
	m = tan(degree);
	h = p.getY() - m * p.getX();
}

Line::Line(Point p1, Point p2)
{
//	LOG(Main, 1) << "in constructor: " << p1 << p2 << endl;
	if(isEqual(p1.getX(), p2.getX()))
	{
		isHoriz = true;
		m = 0;
		h = p1.getX();
		return;
	}
	isHoriz = false;
	m = (p1.getY() - p2.getY()) / (p1.getX() - p2.getX());
	h = p1.getY() - m * p1.getX();
}

Line::~Line()
{
}

void Line::setM(double m)
{
	this->m = m;
}

void Line::setH(double h)
{
	this->h = h;
}

void Line::setHorizontal(bool isHorizontal)
{
	isHoriz = isHorizontal;
}

double Line::getM()
{
	return m;
}

double Line::getH()
{
	return h;
}

bool Line::isHorizontal()
{
	return isHoriz;
}

double Line::getX(double y)
{
	if(isHoriz)
	{
		return h;
	}
	if(isEqual(m, 0.0) && isEqual(y, h))
	{
		return 0.0;
	}
	assert(!isEqual(m, 0.0));
	return (y - h) / m;
}

double Line::getY(double x)
{
	if(isHoriz && isEqual(x, h))
	{
		return 0.0;
	}
	assert(!isHoriz);
	return m * x + h;
}

Segment Line::asSegment()
{
	if(isHoriz)
	{
		return Segment(Point(h, 0.0), Point(h, 100.0));
	}
	return Segment(Point(0.0, getY(0.0)), Point(100.0, getY(100.0)));
}