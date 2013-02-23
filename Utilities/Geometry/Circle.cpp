#include <iostream>
#include <cmath>
#include "Line.h"
#include "Circle.h"
#include "GeometryFunctions.h"

using namespace std;
using namespace Geometry;

Circle::Circle(Point C_center, double C_radius)
{
	center = C_center;
	radius = C_radius;
}

Point Circle::getCenter(void)
{
	return center;
}

double Circle::getRadius(void)
{
	return radius;
}

bool Circle::isInCircle(Point point)
{
	return point.distFromPoint(center) <= radius;
}

ostream & operator <<(ostream &out, Circle a)
{
	out << "circle" << a.getCenter() << ' ' << a.getRadius() << ", " << __color << ';';
	return out;
}