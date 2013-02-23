/*
 * File:   Polygon.cpp
 * Author: Hamed Saleh
 *
 * Created on November 1, 2010, 7:03 PM
 */

#include "GeometryFunctions.h"


#include "Polygon.h"
#include "Point.h"
#include "Line.h"
#include "../DoubleUtilities.h"
#include "Segment.h"
#include "../Debugger.h"
#include <cmath>
#include <memory>

#define LOGLEVEL 1

using namespace std;
using namespace Geometry;

Polygon::Polygon()
{
    vertices.clear();
}

Polygon::Polygon(vector <Point> p)
{
    vertices = p;
}

Polygon::~Polygon()
{
}

void Polygon::setVertices(vector <Point> p)
{
    vertices = p;
}

void Polygon::addVertex(Point p)
{
    vertices.push_back(p);
}

vector <Point>& Polygon::getVertices()
{
    return vertices;
}

void Polygon::clear()
{
    vertices.clear();
}

Point Polygon::getVertex(int i)
{
    return vertices[i];
}

Segment Polygon::getSegment(int i)
{
    return Segment(vertices[i], vertices[(i + 1) % size()]);
}

Point Polygon::getMidPoint()
{
	double midX = 0, midY = 0;
	for(int i = 0; i < size(); i++)
	{
		midX += vertices[i].getX();
		midY += vertices[i].getY();
	}
	midX /= size();
	midY /= size();
	return Point(midX, midY);
}

bool Polygon::isInPolygon(Point p)
{
	Segment seg(p, Point(p.getX(), 1e9));
	int t = 0;
	for(int i = 0; i < (int)vertices.size(); i++)
	{
		if(isOnSegment(getSegment(i), p))
		{
			return true;
		}
		if(isIntersect(seg, getSegment(i)))
		{
			t++;
		}
	}
	return t % 2 == 1;
}

int Polygon::size()
{
    return vertices.size();
}

ostream & operator <<(ostream &out, Polygon a)
{
//	setColor(4);
    for(int i = 0; i < (int)a.size() - 1; i++)
    {
        out << a.getSegment(i) << ' ';
//		setColor(i % 5);
    }
	out << a.getSegment(a.size() - 1);
    return out;
}
