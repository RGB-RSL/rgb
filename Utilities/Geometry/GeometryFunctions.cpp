/*
 * File:   GeometryFunctions.cpp
 * Author: Eqbal Sarjami
 *
 * Created on January 3, 2011, 9:28 PM
 */

#include "GeometryFunctions.h"
#include "Line.h"
#include "../Debugger.h"
#include "Defines.h"
#include "Circle.h"
#include <cmath>
#include <assert.h>
#define LOGLEVEL 1

using namespace Geometry;
using namespace std;

double Geometry::distanceBetweenPoints(Point a, Point b)
{
	return sqrt((a.getX() - b.getX()) * (a.getX() - b.getX()) + (a.getY() - b.getY()) * (a.getY() - b.getY()));
}

double Geometry::crossProduct(Vector a, Vector b)
{
	return a.getX() * b.getY() - a.getY() * b.getX();
}

double Geometry::dotProduct(Vector a, Vector b)
{
	return a.getX() * b.getX() + a.getY() * b.getY();
}

bool Geometry::isParallel(Line l1, Line l2)
{
	if(l1.isHorizontal() && l2.isHorizontal())
	{
		return true;
	}
	if(l1.isHorizontal() || l2.isHorizontal())
	{
		return false;
	}
	return isEqual(l1.getM(), l2.getM());
}

bool Geometry::isParallel(Segment s1, Segment s2)
{
	return isEqual(crossProduct(s1.asVector(), s2.asVector()), 0.0);
}

double Geometry::distanceToLine(Line l, Point p)
{
	return distanceToSegment(l.asSegment(), p);
}

double Geometry::distanceToSegment(Segment s, Point p)
{
	return abs(crossProduct(s.asVector(), Segment(s.getFirstPoint(), p).asVector())) / s.getLength();
}

bool Geometry::isInSameSide(Segment s, Point p1, Point p2)
{
	return hasSameSign(crossProduct(s.asVector(), Segment(s.getFirstPoint(), p1).asVector()), crossProduct(s.asVector(), Segment(s.getFirstPoint(), p2).asVector()));
}

bool Geometry::isInSameSide(Line l, Point p1, Point p2)
{
	return isInSameSide(l.asSegment(), p1, p2);
}

bool Geometry::isIntersect(Segment s1, Segment s2)
{
	return !isInSameSide(s1, s2.getFirstPoint(), s2.getSecondPoint()) && !isInSameSide(s2, s1.getFirstPoint(), s1.getSecondPoint());
}

bool Geometry::isIntersect(Line l, Segment s)
{
	return !isInSameSide(l, s.getFirstPoint(), s.getSecondPoint());
}

Point Geometry::getIntersectPoint(Line l1, Line l2)
{
	if(l2.isHorizontal())
	{
		swap(l1, l2);
	}
	if(l1.isHorizontal())
	{
		return Point(l1.getH(), l2.getY(l1.getH()));
	}
	assert(!isEqual(l1.getM(), l2.getM()));
	double x = (l2.getH() - l1.getH()) / (l1.getM() - l2.getM());
	double y = l1.getY(x);
	return Point(x, y);
}

Line Geometry::getPerpendicularLine(Line l, Point p)
{
	if(l.isHorizontal())
	{
		return Line(0, p.getY());
	}
	if(isEqual(l.getM(), 0))
	{
		Line tmp;
		tmp.setHorizontal();
		tmp.setH(p.getX());
		return tmp;
	}
	double perM = -1.0 / (l.getM());
	double perH = p.getY() - perM * p.getX();
	return Line(perM, perH);
}

Point Geometry::getPerpendicularPoint(Line l, Point p)
{
	return getIntersectPoint(l, getPerpendicularLine(l, p));
}

Line Geometry::getParallelLine(Line l, Point p)
{
	if(l.isHorizontal())
	{
		Line tmp;
		tmp.setHorizontal();
		tmp.setH(p.getX());
		return tmp;
	}
	double parH = p.getY() - l.getM() * p.getX();
	return Line(l.getM(), parH);
}

bool Geometry::isOnSegment(Segment s, Point p)
{
	return isEqual(distanceToSegment(s, p), 0)
		&& isLessEqual(min(s.getFirstPoint().getX(), s.getSecondPoint().getX()), p.getX())
		&& isLessEqual(p.getX(), max(s.getFirstPoint().getX(), s.getSecondPoint().getX()))
		&& isLessEqual(min(s.getFirstPoint().getY(), s.getSecondPoint().getY()), p.getY())
		&& isLessEqual(p.getY(), max(s.getFirstPoint().getY(), s.getSecondPoint().getY()));
}

Vector Geometry::getVectorBylength(Vector v, double l)
{
	double m = l / v.getLength();
	return Vector(v.getX() * m, v.getY() * m);
}

Polygon Geometry::increasePolygon(Polygon p, double d)
{
	Point mid = p.getMidPoint();
	//	LOG(Main, 1) << Circle(mid, 300) << ' ';
	double minD = 1e10, mPr;
	for(int i = 0; i < p.size(); i++)
	{
		Segment s = p.getSegment(i);
		Point per = getPerpendicularPoint(s.asLine(), mid);
		double d = distanceToSegment(s, mid);
		if(isOnSegment(s, per) && isLess(d, minD))
		{
			minD = d;
			mPr = crossProduct(Segment(mid, s.getFirstPoint()).asVector(), Segment(mid, s.getSecondPoint()).asVector());
		}
	}
	vector<Line> lines;
	for(int i = 0; i < p.size(); i++)
	{
		Segment seg = p.getSegment(i);
		Segment s(mid, getPerpendicularPoint(seg.asLine(), mid));
		double pr = crossProduct(Segment(mid, seg.getFirstPoint()).asVector(), Segment(mid, seg.getSecondPoint()).asVector());
		Point disP = (s.getSecondPoint().asVector() + getVectorBylength(s.asRVector(), (isLess(pr * mPr, 0) ? d : -d))).asPoint();
		//		LOG(Main, 1) << s << ' ' << Circle(disP, 30) << ' ';
		lines.push_back(getParallelLine(p.getSegment(i).asLine(), disP));
	}
	Polygon tmp;
	if(!isParallel(lines[p.size() - 1], lines[0]))
	{
		tmp.addVertex(getIntersectPoint(lines[p.size() - 1], lines[0]));
	}
	else
	{
		tmp.addVertex(getPerpendicularPoint(lines[0], p.getVertex(0)));
	}
	for(int i = 0; i < p.size() - 1; i++)
	{
		if(!isParallel(lines[i], lines[i + 1]))
		{
			tmp.addVertex(getIntersectPoint(lines[i], lines[i + 1]));
		}
		else
		{
			tmp.addVertex(getPerpendicularPoint(lines[i], p.getVertex(i + 1)));
		}
	}
	return tmp;
}

void Geometry::setColor(int c)
{
	__color = c;
}

void Geometry::setComparePoint(Point p)
{
	__comparePoint = p;
}

bool Geometry::angleCompare(Geometry::Point p1, Geometry::Point p2)
{
	if(p1 == p2)
	{
		return false;
	}
	return isLessEqual(crossProduct(Segment(__comparePoint, p1).asVector(), Segment(__comparePoint, p2).asVector()), 0.0);
}